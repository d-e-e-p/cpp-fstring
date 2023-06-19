#!/usr/bin/env python3
import os
import subprocess
import sys
from glob import glob
from pathlib import Path
from unittest.mock import patch

import pytest  # noqa: F401

from cpp_fstring.cpp_fstring import run

# cd to test dir to find testcase files
abspath = os.path.abspath(__file__)
dname = os.path.dirname(abspath)
os.chdir(dname)
actual_dir = "actual"
expect_dir = "expect"
input_dir = "input"


def run_external_routine(actual_file, exebin_file):
    with open(actual_file, "w") as f:
        subprocess.run([exebin_file], stdout=f)


def filter_lines(input):
    """
    remove out space in lines
    """
    output = []
    for line in input:
        line = line.strip()
        if line.isspace():
            continue
        if line == "":
            continue
        if "unnamed struct at " in line:
            continue
        if "unnamed union at " in line:
            continue
        output.append(line)

    return output


def compare_output(capsys, actual_output, expect_output):
    # Compare the actual output with the expected output using pytest
    with open(actual_output) as factual:
        with open(expect_output) as fexpect:
            lactual = factual.read().split("\n")
            lexpect = fexpect.read().split("\n")
            lactual = filter_lines(lactual)
            lexpect = filter_lines(lexpect)
            assert lexpect == lactual


def run_routine(capsys, input_file, actual_output):
    """
    execute cpp-fstring in input_file and capture output
    """
    with patch.object(sys, "argv", ["cpp_fstring", input_file]):
        run()
        captured = capsys.readouterr()
        with open(actual_output, "w") as file:
            file.write(captured.out)


def test_compare(capsys):
    """
    test series of files
    """
    for input_file in glob(f"{input_dir}/*.cpp"):
        stem = Path(input_file).stem
        actual_output = os.path.join(actual_dir, stem + ".cpp")
        expect_output = os.path.join(expect_dir, stem + ".cpp")
        if not os.path.exists(actual_dir):
            os.makedirs(actual_dir)
        with capsys.disabled():
            print(f"{input_file=} {actual_output=} {expect_output=}")
        run_routine(capsys, input_file, actual_output)
        compare_output(capsys, actual_output, expect_output)

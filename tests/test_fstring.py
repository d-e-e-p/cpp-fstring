#!/usr/bin/env python3
import os
import sys
from glob import glob
from pathlib import Path
from unittest.mock import patch

from cpp_fstring.cpp_fstring import run

# cd to test dir to find testcase files
abspath = os.path.abspath(__file__)
dname = os.path.dirname(abspath)
os.chdir(dname)
actual_dir = "actual"
expected_dir = "expected"


def test_files(capsys):
    """
    test series of files
    """
    for infile in glob("input/*.cc"):
        stem = Path(infile).stem
        actual_file = os.path.join(actual_dir, stem + ".cpp")
        expected_file = os.path.join(expected_dir, stem + ".cpp")

        with capsys.disabled():
            print(f" in={infile} expected={expected_file} actual={actual_file} ")

        with patch.object(sys, "argv", ["cpp_fstring", infile]):
            run()
            captured = capsys.readouterr()
            actual = captured.out.splitlines()
            if not os.path.exists(actual_dir):
                os.makedirs(actual_dir)
            with open(actual_file, "w") as file:
                file.write(captured.out)

            with capsys.disabled():
                with open(expected_file) as f:
                    expected = f.read().splitlines()
                    # remove empty lines
                    actual = list(filter(None, actual))
                    expected = list(filter(None, expected))
                    # print(f" actual = {actual}")
                    # print(f" expected = {expected}")
                    assert expected == actual

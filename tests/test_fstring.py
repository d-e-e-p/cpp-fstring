#!/usr/bin/env python3
import os
import sys
from glob import glob
from unittest.mock import patch

from cpp_fstring.cpp_fstring import run

# import pytest


# cd to test dir to find in/out dirs
abspath = os.path.abspath(__file__)
dname = os.path.dirname(abspath)
os.chdir(dname)


def test_files(capsys):
    """
    test series of files
    """
    for infile in glob("input/*.cpp"):
        outfile = os.path.join("output", os.path.basename(infile))
        with capsys.disabled():
            print(f" in={infile} out={outfile}")

        with patch.object(sys, "argv", ["cpp_fstring", infile]):
            run()
            captured = capsys.readouterr()
            actual = captured.out.splitlines()
            with capsys.disabled():
                # print(f"cap= {captured}")
                with open(outfile) as f:
                    expected = f.read().splitlines()
                    # remove empty lines
                    actual = list(filter(None, actual))
                    expected = list(filter(None, expected))
                    assert actual == expected

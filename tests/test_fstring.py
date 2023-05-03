#!/usr/bin/env python3
# -*- coding: utf-8 -*-
import sys
import os
from glob import glob
import pytest
from unittest.mock import patch
from cpp_fstring.cpp_fstring import run

__author__ = "d-e-e-p"
__copyright__ = "d-e-e-p"
__license__ = "MIT"

# cd to test dir
abspath = os.path.abspath(__file__)
dname = os.path.dirname(abspath)
os.chdir(dname)

def test_files(capsys):
    """
    test series of files
    """
    with capsys.disabled():
        print(f" running test_files")
    for infile in glob("input/*.cpp"):
        outfile = os.path.join("output", os.path.basename(infile))
        with capsys.disabled():
            print(f" in={infile} out={outfile}")

        with patch.object(sys, 'argv', ['cpp_fstring', infile]):
            run()
            captured = capsys.readouterr()
            actual = captured.out.splitlines()
            with capsys.disabled():
                print(f"cap= {captured}")
                with open(outfile) as f:
                    expected = f.read().splitlines()
                    assert actual == expected

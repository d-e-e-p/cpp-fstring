#
# Makefile to document tox
# https://github.com/d-e-e-p/cpp-fstring
# Copyright (c) 2023 Sandeep <deep@tensorfield.ag>
#

.PHONY: *
.DEFAULT_GOAL := help
INSTALL_LOCATION := ~/.local
MAKEFLAGS += -j4
#SHELL=/bin/bash -vx # for debug



all help:
	tox -av

#lint:       Perform static analysis and style checks
#build:      Build the package in isolation according to PEP517, see https://github.com/pypa/build
#clean:      Remove old distribution files and temporary build artifacts (./build and ./dist)
#docs:       Invoke sphinx-build to build the docs
#doctests:   Invoke sphinx-build to run doctests
#linkcheck:  Check for broken links in the documentation
#publish:    Publish the package you have been developing to a package index server. By default, it uses testpypi. If you really w

lint build clean docs doctests linkcheck:
	tox -e $@

publish: build
	tox -e $@

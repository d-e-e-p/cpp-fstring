"""
cpp-fstring is a c++ pre-processor that adds python f-string support

cpp-fstring modifies c++ string statements with {var} into calls to fmt::format.
After processing the args and setting up logging, the actual conversion is done
in 3 phases:

The 3 phases of conversion are:
        - phase1: parse code and create records of string, classes, and enums.
        - phase2: decide what lines need to be modified and what file appends need to be made
        - phase3: execute these changes in the input file and write modified code to stdout


"""

import argparse
import logging
import sys

from cpp_fstring.GenerateOutput import GenerateOutput

# from cpp_fstring import __version__
from cpp_fstring.ParseCPP import ParseCPP
from cpp_fstring.Processor import Processor

__version__ = "0.1.1"
__author__ = "d-e-e-p"
__copyright__ = "d-e-e-p"
__license__ = "MIT"

log = logging.getLogger(__name__)


def parse_args(args):
    """Parse command line parameters

    Args:

    Returns:
      :obj:`argparse.Namespace`: command line parameters namespace
    """
    parser = argparse.ArgumentParser(description="Preprocess CPP to expand f-string to format")
    parser.add_argument(
        "--version",
        action="version",
        version=f"cpp_fstring {__version__}",
    )
    parser.add_argument(
        "-v",
        "--verbose",
        dest="loglevel",
        help="set loglevel to INFO",
        action="store_const",
        const=logging.INFO,
    )
    parser.add_argument(
        "-vv",
        "--very-verbose",
        dest="loglevel",
        help="set loglevel to DEBUG",
        action="store_const",
        const=logging.DEBUG,
    )
    parser.add_argument(
        "filename",
        help="name of file to process",
    )

    return parser.parse_known_args(args)


def setup_logging(loglevel):
    """Setup basic logging

    Args:
      loglevel (int): minimum loglevel for emitting messages
    """
    logformat = "%(levelname)s:%(name)s:%(message)s"
    logging.basicConfig(level=loglevel, stream=sys.stderr, format=logformat, datefmt="%Y-%m-%d %H:%M:%S")


def main(args):
    """
    main entry point for cpp-fstring.

    Parameters
    ----------
    args : argparse.Namespace
        Arguments parsed from the command line.

    3 Phases:
        - parse code and pick out string tokens, classes, enum etc
        - decide what lines need to be modified and what file appends need to be made
        - execute these changes in the input file and write modified code to stdout

    """
    args, extraargs = parse_args(args)
    setup_logging(args.loglevel)
    log.debug(f"args = {args}")

    with open(args.filename, encoding='utf8', errors='ignore') as f:
        code = f.read()

    # record all interesting snippets in source
    parser = ParseCPP(code, args.filename, extraargs)
    string_records, enum_records, class_records = parser.extract_interesting_records()

    # batch up changes and additions:
    #   changes: in line edits to existing code
    #   addition: can be appended to the end of file
    processor = Processor()
    # changes
    string_changes = processor.gen_fstring_changes(string_records)
    class_changes = processor.gen_class_changes(class_records)
    enum_changes = processor.gen_enum_changes(enum_records)
    # addition
    enum_addition = processor.gen_enum_format(enum_records)
    # class_addition = processor.gen_class_format(class_records)

    # execute changes
    go = GenerateOutput(code)
    go.write_changes(string_changes, class_changes, enum_changes)
    go.append(enum_addition)
    # go.append(class_addition)

    log.info("end")


def run():
    # entry point to create console scripts with setuptools.
    main(sys.argv[1:])


if __name__ == "__main__":
    run()

"""
"""

import argparse
import logging
import sys

#from cpp_fstring import __version__
from cpp_fstring.FormatFstring import FormatFstring
from cpp_fstring.GenerateOutput import GenerateOutput
from cpp_fstring.ParseCPP import ParseCPP

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

    return parser.parse_args(args)


def setup_logging(loglevel):
    """Setup basic logging

    Args:
      loglevel (int): minimum loglevel for emitting messages
    """
    logformat = "%(levelname)s:%(name)s:%(message)s"
    logging.basicConfig(level=loglevel, stream=sys.stderr, format=logformat, datefmt="%Y-%m-%d %H:%M:%S")


def main(args):
    """
    Args:
    """
    args = parse_args(args)
    setup_logging(args.loglevel)
    log.debug(f"args = {args}")

    with open(args.filename) as f:
        code = f.read()

    string_tokens, enum_tokens = ParseCPP().find_tokens(args.filename)

    pc = FormatFstring()
    string_changes = pc.get_changes(string_tokens)
    enum_changes = pc.gen_enum_format(enum_tokens)

    go = GenerateOutput(code)
    go.write_changes(string_changes)
    go.gen_enum_format(enum_changes)

    log.info("end")


def run():
    # entry point to create console scripts with setuptools.
    main(sys.argv[1:])


if __name__ == "__main__":
    run()

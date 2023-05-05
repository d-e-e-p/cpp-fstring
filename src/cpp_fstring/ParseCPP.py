"""
"""

import logging
import pdb  # noqa: F401

from cpp_fstring.Preprocessor import Preprocessor

_logger = logging.getLogger(__name__)


class ParseCPP:
    """
    parse cpp file
    """

    def __init__(self, args=None, **kwargs):
        self.pp = Preprocessor()

    def find_fstrings(self, code, filename):
        self.pp.parse(code, filename)
        tokens = []
        for tok in iter(self.pp.token, None):
            if (tok.type == "CPP_STRING") or (tok.type == "CPP_RAWSTRING"):
                if tok.value.rfind("{") > 0 or tok.value.rfind("}") > 0:
                    tokens.append(tok)
        return tokens

"""
"""

import pdb
import sys
import logging

from cpp_fstring.Preprocessor import Preprocessor

_logger = logging.getLogger(__name__)


class ParseCPP():
    """
    parse cpp file
    """
    def __init__(self, args=None, **kwargs):
        self.pp = Preprocessor()

    def find_fstrings(self, code, filename):
        self.pp.parse(code, filename)
        tokens = []
        for tok in iter(self.pp.token, None):
            if tok.type == 'CPP_STRING':
                if tok.value.rfind('{') > 0 and tok.value.rfind('}') > 0:
                    tokens.append(tok)
        return tokens

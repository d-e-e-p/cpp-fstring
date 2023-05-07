"""
"""

import pdb
import sys
import logging

from cpp_fstring.clang.cindex import Index, Config, CursorKind, TokenKind

log = logging.getLogger(__name__)

# TODO: locate and check lib path
library_path = "/opt/homebrew/Cellar/cling/0.9/libexec/lib"
library_path = "/opt/homebrew/Cellar/llvm/16.0.1/lib/"
log.debug(f"using cling library from: {library_path}")
Config.set_library_path(library_path)

class ParseCPP():
    """
    parse cpp file
    """
    def __init__(self, args=None, **kwargs):

        self.string_tokens = []
        self.enum_tokens = []
        self.class_tokens = []

    def find_fstrings(self, filename):
        args = [filename]
        args.extend(['-xc++', '--std=c++17', "-nobuiltininc", "--no-standard-includes",])

        index = Index.create()
        tu = index.parse(None, args)
        if not tu:
            log.error(f"unable to load input using args = {args}")

        #self.find_string_tokens(tu.cursor)
        self.get_info(tu.cursor)
        return self.string_tokens

    def get_info(self, node, depth=0):
            children = [self.get_info(c, depth+1)
                        for c in node.get_children()]
            # log.debug(f" node={node.kind} {node.spelling}")
            res = { 'node' : node,
                     'kind' : node.kind,
                     'usr' : node.get_usr(),
                     'spelling' : node.spelling,
                     'location' : node.location,
                     'extent.start' : node.extent.start,
                     'extent.end' : node.extent.end,
                     'is_definition' : node.is_definition(),
                     'children' : children }
            #log.debug(res)
            if node.kind == CursorKind.ENUM_DECL:
                print(f" enum {node.spelling}")
                for token in node.get_tokens():
                    if token.kind == TokenKind.IDENTIFIER:
                        #pu.db
                        print(f" enum {token.cursor.displayname} -> {token.cursor.enum_value} {token.spelling} {token.cursor.type.kind}/{token.cursor.type.get_declaration().enum_type.kind}")

            if node.kind == CursorKind.COMPOUND_STMT:
                for token in node.get_tokens():
                    if token.kind == TokenKind.LITERAL:
                        self.string_tokens.append(token)
                        #pu.db

    def find_string_tokens(self, node):
        for child in node.get_children():
            self.find_string_tokens(child)
            print(f" ktk={node.type.kind} ")
            print(f" nk = {node.kind}")
            if (node.kind == CursorKind.COMPOUND_STMT):
                for token in node.get_tokens():
                    if token.kind == TokenKind.LITERAL:
                        self.string_tokens.append(token)


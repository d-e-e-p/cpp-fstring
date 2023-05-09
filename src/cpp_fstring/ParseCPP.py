"""
"""

import bpdb
import logging
import types

from cpp_fstring.clang.cindex import Index, Config, Cursor, Token
from cpp_fstring.clang.cindex import CursorKind, TokenKind, TypeKind
from dataclasses import dataclass, field

log = logging.getLogger(__name__)

# TODO: locate and check lib path
library_path = "/opt/homebrew/Cellar/cling/0.9/libexec/lib"
library_path = "/opt/homebrew/Cellar/llvm/16.0.1/lib/"
log.debug(f"using cling library from: {library_path}")
Config.set_library_path(library_path)


@dataclass
class EnumConstantDecl:
    name: str
    index: str


@dataclass
class EnumToken:
    """
    store enum definition
    """
    name: str
    is_scoped: bool = False
    is_anonymous: bool = False
    enum_type: TypeKind = TypeKind.INT
    values: list[EnumConstantDecl] = field(default_factory=list)


@dataclass
class MarkedTokens:
    """
    store lists of str/enum/class tokens
    """
    tstring: list[Token]
    tenum:   list[EnumToken]
    tclass:  list[Token]


def dump(obj, name="obj"):
    for attribute in dir(obj):
        # if isinstance(getattr(obj, attribute), str):
        try:
            val = getattr(obj, attribute)
            # if isinstance(val, type(lambda: None))
            if type(val) is types.MethodType:
                if not val.__name__.startswith("_"):
                    res = val()
                    print(f"{name}[{attribute}]: {res}")
            else:
                if not val.startswith("_"):
                    print(f"{name}[{attribute}]: {val}")
        except:
            pass


class ParseCPP():
    """
    parse cpp file
    """
    def __init__(self, args=None, **kwargs):
        self.string_tokens = []
        self.enum_tokens = []
        self.class_tokens = []
        self.enum_decl_nodes = []
        self.comp_stmt_nodes = []
        self.class_decl_nodes = []

        self.INDENT = 4

    def find_tokens(self, filename):
        args = [filename]
        args.extend(['-xc++', '--std=c++17', "-nobuiltininc", "--no-standard-includes",])

        index = Index.create()
        tu = index.parse(None, args)
        if not tu:
            log.error(f"unable to load input using args = {args}")

        # self.find_string_tokens(tu.cursor)
        # self.get_info(tu.cursor)
        self.visit(tu.cursor, 0, set())
        self.extract_string_tokens()
        self.extract_enum_tokens()
        return self.string_tokens, self.enum_tokens

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
            log.debug(res)
            match node.kind:
                case CursorKind.STRUCT_DECL:
                    dump(node, "struct")
                    for token in node.get_tokens():
                        dump(token, token.cursor.displayname)
            if node.kind == CursorKind.ENUM_DECL:
                enum_token = EnumToken(node.spelling)
                for token in node.get_tokens():
                    if token.kind == TokenKind.KEYWORD:
                        if token.spelling == "class":
                            enum_token.is_scoped = True;
                    if token.kind == TokenKind.IDENTIFIER:
                        if token.cursor.kind == CursorKind.ENUM_DECL:
                            enum_token.index_type=token.cursor.type.get_declaration().enum_type.kind
                            # log.debug(f" {token.spelling} {token.cursor.type.kind}/{token.cursor.type.get_declaration().enum_type.kind}")
                        else:
                            if token.cursor.kind == CursorKind.ENUM_CONSTANT_DECL:
                                # log.debug(f" enum {token.cursor.kind} {token.cursor.displayname} -> {token.cursor.enum_value}")
                                enum_constant_decl = EnumConstantDecl(token.cursor.displayname, str(token.cursor.enum_value))
                                enum_token.values.append(enum_constant_decl)
                            else:
                                log.error(f" unknown enum kind {token.cursor.kind} for enum {enum_token}")
                self.enum_tokens.append(enum_token)
                log.debug(f" enum {enum_token}")

            if node.kind == CursorKind.COMPOUND_STMT:
                for token in node.get_tokens():
                    if token.kind == TokenKind.LITERAL:
                        self.string_tokens.append(token)
                        #pu.db

    # from https://gist.github.com/scturtle/a7b5349028c249f2e9eeb5688d3e0c5e
    def visit(self, node: Cursor, indent: int, saw):
        prefix = ' ' * indent
        kind = node.kind
        # skip printting UNEXPOSED_*
        if not kind.is_unexposed():
            line = f"{prefix}{kind.name} "
            if kind == CursorKind.ENUM_DECL:
                self.enum_decl_nodes.append(node)
            if kind == CursorKind.COMPOUND_STMT:
                self.comp_stmt_nodes.append(node)
            if node.spelling:
                line += f"s: {node.spelling} "
                if node.type.spelling:
                    line += f"s: {node.spelling} t: {node.type.spelling}  "
            log.debug(line)
        saw.add(node.hash)
        if node.referenced is not None and node.referenced.hash not in saw:
            self.visit(node.referenced, indent + self.INDENT, saw)
        skip = False
        for c in node.get_children():
            self.visit(c, indent + self.INDENT, saw)
        saw.remove(node.hash)

    def extract_enum_tokens(self):
        """
        look at nodes for enum decl and definitions
        """
        for node in self.enum_decl_nodes:

            # TODO: find a more robust solution for anon namespace

            if "(anonymous namespace)::" in node.type.spelling:
                enum_token = EnumToken(node.spelling)
            else:
                enum_token = EnumToken(node.type.spelling)
                enum_token.is_anonymous = node.is_anonymous();
                # bpdb.set_trace()
            for token in node.get_tokens():
                # either in decl or const_decl
                match (token.cursor.kind, token.kind):
                    case (CursorKind.ENUM_DECL, TokenKind.KEYWORD):
                        # if token.spelling == "class":
                        if token.cursor.is_scoped_enum():
                            enum_token.is_scoped = True;
                    case (CursorKind.ENUM_DECL, TokenKind.IDENTIFIER):
                        enum_token.index_type=token.cursor.type.get_declaration().enum_type.kind
                    case (CursorKind.ENUM_CONSTANT_DECL, TokenKind.IDENTIFIER):
                        enum_constant_decl = EnumConstantDecl(token.cursor.displayname, str(token.cursor.enum_value))
                        enum_token.values.append(enum_constant_decl)
            log.debug(f" enum {enum_token}")
            self.enum_tokens.append(enum_token)

    def extract_string_tokens(self):
        for node in self.comp_stmt_nodes:
            for token in node.get_tokens():
                if token.kind == TokenKind.LITERAL:
                    in_str = token.spelling
                    log.debug(f"{token.cursor.kind}  str: {token.spelling}")
                    # bpdb.set_trace()
                    if in_str.find('{') > 0 or in_str.find('}') > 0:
                        self.string_tokens.append(token)


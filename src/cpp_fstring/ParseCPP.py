"""
"""

import bpdb  # noqa: F401
import logging
import types

from cpp_fstring.clang.cindex import Index, Config, Cursor, Token, TranslationUnit
from cpp_fstring.clang.cindex import CursorKind, TokenKind, TypeKind
from dataclasses import dataclass, field

log = logging.getLogger(__name__)

# TODO: locate and check lib path
library_path = "/opt/homebrew/Cellar/cling/0.9/libexec/lib"
library_path = "/opt/homebrew/Cellar/llvm/16.0.1/lib/"
log.debug(f"using cling library from: {library_path}")
Config.set_library_path(library_path)

"""
storage for enum/class tokens
"""


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
class ClassVarToken:
    """
    store class/struct variables
    """
    name: str
    vartype: str
    objc_type_encoding: str
    access_specifier: str = "PUBLIC"

@dataclass
class ClassTemplateVarToken:
    """
    store template params for class
    """
    name: str
    displayname: str
    vartype: str
    is_template_type: bool



@dataclass
class ClassToken:
    """
    store class/struct definition
    """
    name: str
    displayname: str
    class_kind: str = "STRUCT_DECL"
    last_tok: Token = Token()
    vars: list[ClassVarToken] = field(default_factory=list)
    tvars: list[ClassTemplateVarToken] = field(default_factory=list)


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
            if type(val) is types.MethodType:   # noqa: E721
                if not val.__name__.startswith("_"):
                    print(f"{name}[{attribute}]", end=" ")
                    print(val())
            else:
                if not val.startswith("_"):
                    print(f"{name}[{attribute}]", end=" ")
                    print(val)
        except:  # noqa: E722
            pass


class ParseCPP():
    """
    parse cpp file
    """
    def __init__(self, filename, code, args=None, **kwargs):
        self.string_tokens = []
        self.enum_tokens = []
        self.class_tokens = []
        self.enum_decl_nodes = []
        self.comp_stmt_nodes = []
        self.class_decl_nodes = []

        self.filename = filename
        self.code = code

        self.INDENT = 4

    def find_tokens(self):
        args = [self.filename]
        args.extend(['-xc++', '--std=c++17', "-nobuiltininc", "--no-standard-includes",])
        # from https://cwoodall.com/posts/2018-02-24-using-clang-and-python-to-generate-cpp-struct-serde-fns/
        # Add the include files for the standard library.
        # syspath = ccsyspath.system_include_paths('clang++')
        # incargs = [b'-I' + inc for inc in syspath]
        unsaved_files = [(self.filename, self.code)]

        index = Index.create()
        tu = index.parse(path=None, args=args, unsaved_files=unsaved_files, options=TranslationUnit.PARSE_INCOMPLETE)
        if not tu:
            log.error(f"unable to load input using args = {args}")

        # self.find_string_tokens(tu.cursor)
        # self.get_info(tu.cursor)
        self.visit(tu.cursor, 0, set())
        self.remove_duplicate_tokens()
        self.extract_string_tokens()
        self.extract_enum_tokens()
        self.extract_class_tokens()
        return self.string_tokens, self.enum_tokens, self.class_tokens

    def dup_remover(self, nodes):
        seen = set()
        res = []
        for node in nodes:
            if node.hash not in seen:
                res.append(node)
                seen.add(node.hash)
        return res


    def remove_duplicate_tokens(self):
        """
        remove copies of visited nodes
        """

        self.class_decl_nodes = self.dup_remover(self.class_decl_nodes)
        self.comp_stmt_nodes = self.dup_remover(self.comp_stmt_nodes)
        self.enum_decl_nodes = self.dup_remover(self.enum_decl_nodes)

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
            if kind == CursorKind.STRUCT_DECL:
                self.class_decl_nodes.append(node)
            if kind == CursorKind.CLASS_DECL:
                self.class_decl_nodes.append(node)
            if kind == CursorKind.CLASS_TEMPLATE:
                self.class_decl_nodes.append(node)
            if node.spelling:
                line += f"s: {node.spelling} "
                if node.type.spelling:
                    line += f" t: {node.type.spelling}  "
            log.debug(line)
        saw.add(node.hash)
        if node.referenced is not None and node.referenced.hash not in saw:
            self.visit(node.referenced, indent + self.INDENT, saw)
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
                enum_token.is_anonymous = node.is_anonymous()
            for token in node.get_tokens():
                # either in decl or const_decl
                match (token.cursor.kind, token.kind):
                    case (CursorKind.ENUM_DECL, TokenKind.KEYWORD):
                        # if token.spelling == "class":
                        if token.cursor.is_scoped_enum():
                            enum_token.is_scoped = True
                    case (CursorKind.ENUM_DECL, TokenKind.IDENTIFIER):
                        enum_token.index_type = token.cursor.type.get_declaration().enum_type.kind
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
                    if in_str.find('{') > 0 or in_str.find('}') > 0:
                        self.string_tokens.append(token)

    def extract_class_tokens(self):
        for node in self.class_decl_nodes:
            if node.is_anonymous():
                continue
            class_token = ClassToken(node.type.spelling, node.displayname)
            # now find closing brace
            *_, last_tok = node.get_tokens()
            class_token.last_tok = last_tok
            if last_tok.kind != TokenKind.PUNCTUATION or last_tok.spelling != "}":
                log.warning(f" last tok of class {node.type.spelling} is : {last_tok.kind} {last_tok.spelling}")
            class_token.class_kind = node.kind.name
            # if template then also get template params
            if node.kind == CursorKind.CLASS_TEMPLATE:
                """
                skip templates of templates and other weird creatures
                """
                for child in node.get_children():
                    is_template_type_param = (child.kind == CursorKind.TEMPLATE_TYPE_PARAMETER)
                    is_template_non_type_param = (child.kind == CursorKind.TEMPLATE_NON_TYPE_PARAMETER)
                    if is_template_type_param or is_template_non_type_param:
                        tvar_token = ClassTemplateVarToken(child.spelling, child.displayname, child.type.spelling, is_template_type_param)
                        class_token.tvars.append(tvar_token)

                    if child.kind == CursorKind.FIELD_DECL:
                        var_token = ClassVarToken(child.spelling, child.type.spelling, child.type.spelling)
                        class_token.vars.append(var_token)
                    if child.kind == CursorKind.VAR_DECL:
                        var_token = ClassVarToken(child.spelling, child.type.spelling, child.objc_type_encoding)
                        var_token.access_specifier = child.access_specifier.name
                        class_token.vars.append(var_token)

                #bpdb.set_trace()


            log.debug(f" nts={node.type.spelling} {node.spelling} of {node.kind} : {node.objc_type_encoding}")
            # also works for get_children()
            for child in node.get_children():
                if child.kind == CursorKind.CXX_BASE_SPECIFIER:
                    log.debug(f" ck={child.kind} base={child.spelling}") 
                    bpdb.set_trace()
            for fd in node.type.get_fields():
                if fd.is_definition():
                    var_token = ClassVarToken(fd.spelling, fd.type.spelling, fd.objc_type_encoding)
                    var_token.access_specifier = fd.access_specifier.name
                    class_token.vars.append(var_token)
            log.debug(f"class_token = {class_token}")
            if len(class_token.vars) > 0:
                self.class_tokens.append(class_token)

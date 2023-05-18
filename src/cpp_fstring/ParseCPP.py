"""
"""

import bpdb  # noqa: F401
import logging
import types
from dataclasses import dataclass, field
from typing import Callable


from cpp_fstring.clang.cindex import Index, Config, Cursor, Token, TranslationUnit
from cpp_fstring.clang.cindex import CursorKind, TokenKind, TypeKind

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
    enum_type: str = ""
    is_scoped: bool = False
    is_anonymous: bool = False
    values: list[EnumConstantDecl] = field(default_factory=list)


@dataclass
class ClassVarToken:
    """
    store class/struct variables
    """
    name: str
    displayname: str
    vartype: str
    access_specifier: str = "PUBLIC"
    indent: int = 0
    is_template_type: bool = False


@dataclass
class ClassToken:
    """
    store class/struct definition
    """
    name: str
    displayname: str
    hash: int
    class_kind: str = "STRUCT_DECL"
    last_tok: Token = Token()
    bases: list[int] = field(default_factory=list)
    vars: list[ClassVarToken] = field(default_factory=list)
    tvars: list[ClassVarToken] = field(default_factory=list)


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
        if attribute.startswith("_"):
            continue

        try:
            val = getattr(obj, attribute)
        except:  # noqa: E722
            val = None

        if val is None:
            print(f"{name}[{attribute}] = ERR")
            continue
        
        print(f"{name}[{attribute}]", end=" ")
        # if isinstance(val, type(lambda: None))
        if type(val) is types.MethodType:   # noqa: E721
            try:
                print("()", end=" ")
                print(val())
            except:  # noqa: E722
                print("ERR")
        else:
                print(val)

    attr_list = "lexical_parent type".split()
    for attr in attr_list:
        if getattr(obj, attr):
            val = getattr(obj, attr).spelling
            print(f"{name}[{attr}] = {val}")

    if getattr(obj, "get_tokens"):
        tokens = ""
        for fd in obj.get_tokens():
            tokens += " " + fd.spelling
        print(f"{name}[tokens] = {tokens}")

    print("------")

class ParseCPP():
    """
    parse cpp file
    """
    def __init__(self, code, filename, extraargs, **kwargs):
        self.string_tokens = []
        self.enum_tokens = []
        self.class_tokens = []
        self.enum_decl_nodes = []
        self.comp_stmt_nodes = []
        self.class_decl_nodes = []

        # used by callback
        self.enum_token = None
        self.class_token = None
        self.cxx_base_specifier = set()

        self.code = code
        self.filename = filename
        self.extraargs = extraargs
        self.interesting_kinds = [
            CursorKind.COMPOUND_STMT,   # for strings
            CursorKind.ENUM_DECL,       # for enum
            # for structs+classes
            CursorKind.STRUCT_DECL,  CursorKind.CLASS_DECL, CursorKind.CLASS_TEMPLATE, 
        ]
        self.nodelist = {key: [] for key in self.interesting_kinds}
        self.INDENT = 4


    def find_tokens(self):
        args = [self.filename]
        args.extend(['-xc++', '--std=c++17', "-nobuiltininc", "--no-standard-includes",])
        args.extend(self.extraargs)
        # from https://cwoodall.com/posts/2018-02-24-using-clang-and-python-to-generate-cpp-struct-serde-fns/
        # Add the include files for the standard library.
        # syspath = ccsyspath.system_include_paths('clang++')
        # incargs = [b'-I' + inc for inc in syspath]
        unsaved_files = [(self.filename, self.code)]

        index = Index.create()
        log.debug(f"clang args = {args}")
        tu = index.parse(path=None, args=args, unsaved_files=unsaved_files, options=TranslationUnit.PARSE_INCOMPLETE)
        if not tu:
            log.error(f"unable to load input using args = {args}")

        # self.find_string_tokens(tu.cursor)
        # self.get_info(tu.cursor)
        self.visit(tu.cursor, 0, set(), self.cb_store_if_interesting)
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
        for type, nodes in self.nodelist.items():
            self.nodelist[type] = self.dup_remover(self.nodelist[type])

    # from https://gist.github.com/scturtle/a7b5349028c249f2e9eeb5688d3e0c5e
    def visit(self, node: Cursor, indent: int, saw: set, callback: Callable[[], str]):
        kind = node.kind
        # skip printting UNEXPOSED_*
        if not kind.is_unexposed():
            callback(node, indent)
        saw.add(node.hash)
        if node.referenced is not None and node.referenced.hash not in saw:
            self.visit(node.referenced, indent + self.INDENT, saw, callback)
        for c in node.get_children():
            self.visit(c, indent + self.INDENT, saw, callback)
        # saw.remove(node.hash) # <-- to see expanded tree

    def cb_store_if_interesting(self, node, indent):
        kind = node.kind
        prefix = ' ' * indent
        line = f"{prefix}{kind.name} "
        if kind in self.interesting_kinds:
            self.nodelist[kind].append(node)
        if node.spelling:
            line += f"s: {node.spelling} "
        if node.type.spelling:
            line += f" t: {node.type.spelling}"
            num_temp_args = node.type.get_num_template_arguments()
            if num_temp_args > 0:
                line += f"  ta={num_temp_args}"

        log.debug(line)

    def extract_enum_tokens(self):
        """
        look at nodes for enum decl and definitions
        """
        for node in self.nodelist[CursorKind.ENUM_DECL]:

            # TODO: find a more robust solution for anon namespace

            if "(anonymous namespace)::" in node.type.spelling:
                self.enum_token = EnumToken(node.spelling)
            else:
                self.enum_token = EnumToken(node.type.spelling)
                self.enum_token.is_anonymous = node.is_anonymous()

            self.enum_token.is_scoped = node.is_scoped_enum()
            self.enum_token.enum_type = node.type.get_declaration().enum_type.kind.name
            
            self.visit(node, 0, set(), self.cb_extract_enum_tokens)
            self.enum_tokens.append(self.enum_token)

    def cb_extract_enum_tokens(self, node, indent):
        """
        for each visited note, look for constant declarations
        """
        if node.kind == CursorKind.ENUM_CONSTANT_DECL:
            enum_constant_decl = EnumConstantDecl(node.displayname, str(node.enum_value))
            self.enum_token.values.append(enum_constant_decl)

    def extract_string_tokens(self):
        """
        just create a list of string object tokens
        """
        for node in self.nodelist[CursorKind.COMPOUND_STMT]:
            for token in node.get_tokens():
                if token.kind == TokenKind.LITERAL:
                    in_str = token.spelling
                    log.debug(f"{token.cursor.kind.name}  str: {token.spelling}")
                    if in_str.find('{') > 0 or in_str.find('}') > 0:
                        self.string_tokens.append(token)

    def get_qualified_name(self, node):

        if node is None:
            return ''
        elif node.kind == CursorKind.TRANSLATION_UNIT or node.kind == CursorKind.FILE:
            return ''
        else:
            res = self.get_qualified_name(node.semantic_parent)
            if res != '':
                return res + '::' + node.spelling
        return node.spelling


    def cb_extract_class_tokens(self, node, indent):
        """
        for each node expand tree looking for variables
        """
        prefix = ' ' * indent
        if node.kind == CursorKind.CXX_BASE_SPECIFIER:
            self.cxx_base_specifier.add(node.get_definition().hash)
        if node.kind == CursorKind.FIELD_DECL or node.kind == CursorKind.VAR_DECL:
            is_base_class = node.hash in self.cxx_base_specifier
            print(f" {prefix} {indent} {node.kind.name} {node.spelling} {node.access_specifier.name} {node.displayname} base={is_base_class}")
            var_token = ClassVarToken(
                node.spelling, node.displayname, node.type.spelling, node.access_specifier.name)
            var_token.indent = indent
            self.class_token.vars.append(var_token)

    def extract_vars_from_class(self, node, indent):
        """
        if the class definition node has base class, then add the vars to list to print
        """
        var_tokens = []

        for fd in node.get_children():
            if fd.kind == CursorKind.FIELD_DECL:
                if node.kind == CursorKind.CLASS_TEMPLATE:
                    name = self.get_qualified_name(fd)
                else:
                    name = fd.spelling

                var_token = ClassVarToken(
                        name, fd.displayname, fd.type.spelling, fd.access_specifier.name, indent)
                var_tokens.append(var_token)


        # both cases need to deal with inheritance
        # see https://stackoverflow.com/questions/42795408/can-libclang-parse-the-crtp-pattern
        # TODO: decide between visiting all downstream nodes with fd.walk_preorder() or just 
        # immediate children with fd.get_children()
        for fd in node.walk_preorder():
            if fd.kind == CursorKind.CXX_BASE_SPECIFIER:
                has_template_args = fd.type.get_num_template_arguments() > 0
                for fm in fd.walk_preorder():
                    log.debug(f" fm ref = {fm.displayname} {fm.type.spelling} {has_template_args}")
                    #if fm.kind == CursorKind.TYPE_REF:
                    #if fm.kind == CursorKind.CLASS_DECL:
                    #if fm.kind == CursorKind.TEMPLATE_REF:

                # gather more variables from base classes
                base_node = fd.get_definition()
                derived_var_tokens = self.extract_vars_from_class(base_node, indent + 1)
                var_tokens.extend(derived_var_tokens)

        #for fd in node.walk_preorder():
        #    print(f" {fd.spelling} {fd.type.spelling} {fd.kind} ")
        #    dump(fd, fd.spelling)
        #bpdb.set_trace()
        return var_tokens


    def extract_class_tokens(self):
        """
        look at nodes for enum decl and definitions
        """
        for kind in [CursorKind.CLASS_DECL, CursorKind.CLASS_TEMPLATE, CursorKind.STRUCT_DECL,]:
            for node in self.nodelist[kind]:
                # skip anon classes for now
                if node.is_anonymous():
                    continue
                # create record
                name = node.type.spelling
                if not name:
                    qname = self.get_qualified_name(node)
                    if "::" in qname:
                        # 'A::Base' + 'Base<T>' => 'A::Base<T>'
                        path = qname.rsplit("::", 1)[0]
                        name = path + "::" + node.displayname
                    else:
                        name = node.displayname

                class_token = ClassToken(name, node.displayname, node.hash, node.kind.name)
                # now find closing brace so we can inject 'friend' type statements
                *_, last_tok = node.get_tokens()
                class_token.last_tok = last_tok
                if last_tok.kind != TokenKind.PUNCTUATION or last_tok.spelling != "}":
                    log.warning(f" can't find closing brace of {class_token}")

                if node.kind == CursorKind.CLASS_TEMPLATE:
                    for fd in node.walk_preorder():
                        is_template_type_param =        (fd.kind == CursorKind.TEMPLATE_TYPE_PARAMETER)
                        is_template_non_type_param =    (fd.kind == CursorKind.TEMPLATE_NON_TYPE_PARAMETER)
                        is_dield_declaration =          (fd.kind == CursorKind.FIELD_DECL)
                        # print(f" {fd.kind} {fd.spelling} type:{fd.type.spelling} is_def:{fd.is_definition()} as:{fd.access_specifier.name} dn:{fd.displayname} ")
                        if is_template_type_param or is_template_non_type_param:
                            tvar_token = ClassVarToken(
                                    fd.spelling, fd.displayname, fd.type.spelling, fd.access_specifier.name)
                            tvar_token.indent = 0
                            tvar_token.is_template_type = is_template_type_param;
                            class_token.tvars.append(tvar_token)

                            
                class_token.vars = self.extract_vars_from_class(node, indent=0)

                log.debug(f"class_token = {class_token}")
                if len(class_token.vars) > 0 or len(class_token.tvars) > 0:
                    self.class_tokens.append(class_token)


                #self.class_token = class_token
                #self.class_token.vars = []
                #self.class_token.tvars = []
                #self.visit(node, 0, set(), self.cb_extract_class_tokens)
                #self.class_tokens.append(self.class_token)



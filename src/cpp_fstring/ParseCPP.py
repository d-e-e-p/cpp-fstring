"""

translation unit (TU)
    single file including all #included code.
    root cursor for traversing the contents of a translation unit

cursor:
    abstraction that represents some node in the AST (Abstract Syntax Tree) of a TU

token:
    smallest unit of a program that is meaningful to the compiler
    keyword, identifier, literal, operator or punctuation symbol
"""

import bpdb  # noqa: F401
import logging
import types
from dataclasses import dataclass, field
from typing import Callable


from cpp_fstring.clang.cindex import Index, Config, Cursor, Token, TranslationUnit
from cpp_fstring.clang.cindex import CursorKind, TokenKind

log = logging.getLogger(__name__)

# TODO: locate and check lib path
library_path = "/opt/homebrew/Cellar/llvm/16.0.1/lib/"
log.debug(f"using cling library from: {library_path}")
Config.set_library_path(library_path)

"""
storage for string/enum/class records
"""


@dataclass
class EnumConstantDecl:
    name: str
    index: str


@dataclass
class EnumRecord:
    """
    store enum definition
    """
    name: str
    enum_type: str = ""
    is_scoped: bool = False
    is_anonymous: bool = False
    values: list[EnumConstantDecl] = field(default_factory=list)


@dataclass
class ClassVar:
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
class ClassRecord:
    """
    store class/struct definition
    """
    name: str
    displayname: str
    hash: int
    class_kind: str = "STRUCT_DECL"
    last_tok: Token = Token()
    bases: list[int] = field(default_factory=list)
    vars: list[ClassVar] = field(default_factory=list)
    tvars: list[ClassVar] = field(default_factory=list)


@dataclass
class SelectedRecords:
    """
    store lists of str/enum/class tokens
    """
    tstring: list[Token]
    tenum:   list[EnumRecord]
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
        self.string_records = []
        self.enum_records = []
        self.class_records = []
        self.enum_decl_nodes = []
        self.comp_stmt_nodes = []
        self.class_decl_nodes = []

        # used by callback
        self.enum_record = None
        self.class_record = None
        self.cxx_base_specifier = set()

        self.code = code
        self.filename = filename
        self.extraargs = extraargs
        self.interesting_kinds = [
            CursorKind.COMPOUND_STMT,   # for strings
            CursorKind.ENUM_DECL,       # for enum
            # for structs+classes
            CursorKind.STRUCT_DECL,  CursorKind.CLASS_DECL, CursorKind.CLASS_TEMPLATE,
            CursorKind.UNION_DECL
        ]
        self.nodelist = {key: [] for key in self.interesting_kinds}
        self.INDENT = 4

    def find_records(self):
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

        # self.find_string_records(tu.cursor)
        # self.get_info(tu.cursor)
        self.visit(tu.cursor, 0, set(), self.cb_store_if_interesting)
        self.remove_duplicate_records()
        self.extract_string_records()
        self.extract_enum_records()
        self.extract_class_records()
        return self.string_records, self.enum_records, self.class_records

    def dup_remover(self, nodes):
        seen = set()
        res = []
        for node in nodes:
            if node.hash not in seen:
                res.append(node)
                seen.add(node.hash)
        return res

    def remove_duplicate_records(self):
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

    def extract_enum_records(self):
        """
        look at nodes for enum decl and definitions
        """
        for node in self.nodelist[CursorKind.ENUM_DECL]:

            # TODO: find a more robust solution for anon namespace

            if "(anonymous namespace)::" in node.type.spelling:
                self.enum_record = EnumRecord(node.spelling)
            else:
                self.enum_record = EnumRecord(node.type.spelling)
                self.enum_record.is_anonymous = node.is_anonymous()

            self.enum_record.is_scoped = node.is_scoped_enum()
            self.enum_record.enum_type = node.type.get_declaration().enum_type.kind.name

            self.visit(node, 0, set(), self.cb_extract_enum_records)
            self.enum_records.append(self.enum_record)

    def cb_extract_enum_records(self, node, indent):
        """
        for each visited note, look for constant declarations
        """
        if node.kind == CursorKind.ENUM_CONSTANT_DECL:
            enum_constant_decl = EnumConstantDecl(node.displayname, str(node.enum_value))
            self.enum_record.values.append(enum_constant_decl)

    def extract_string_records(self):
        """
        just create a list of string object tokens
        """
        for node in self.nodelist[CursorKind.COMPOUND_STMT]:
            for token in node.get_tokens():
                if token.kind == TokenKind.LITERAL:
                    in_str = token.spelling
                    log.debug(f"{token.cursor.kind.name}  str: {token.spelling}")
                    if in_str.find('{') > 0 or in_str.find('}') > 0:
                        self.string_records.append(token)

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

    def extract_vars_from_class(self, node, indent):
        """
        if the class definition node has base class, then add the vars to list to print
        """
        var_records = []

        for fd in node.get_children():
            if fd.kind == CursorKind.FIELD_DECL:
                if node.kind == CursorKind.CLASS_TEMPLATE:
                    name = self.get_qualified_name(fd)
                else:
                    name = fd.spelling

                var_record = ClassVar(
                        name, fd.displayname, fd.type.spelling, fd.access_specifier.name, indent)
                var_records.append(var_record)

        # both cases need to deal with inheritance
        # see https://stackoverflow.com/questions/42795408/can-libclang-parse-the-crtp-pattern
        # TODO: decide between visiting all downstream nodes with fd.walk_preorder() or just
        # immediate children with fd.get_children()
        for fd in node.walk_preorder():
            if fd.kind == CursorKind.CXX_BASE_SPECIFIER:
                # has_template_args = fd.type.get_num_template_arguments() > 0
                # for fm in fd.walk_preorder():
                #   log.debug(f" fm ref = {fm.displayname} {fm.type.spelling} {has_template_args}")
                #   if fm.kind == CursorKind.TYPE_REF:
                #   if fm.kind == CursorKind.CLASS_DECL:
                #   if fm.kind == CursorKind.TEMPLATE_REF:

                # gather more variables from base classes
                base_node = fd.get_definition()
                derived_var_records = self.extract_vars_from_class(base_node, indent + 1)
                var_records.extend(derived_var_records)

        # for fd in node.walk_preorder():
        #    print(f" {fd.spelling} {fd.type.spelling} {fd.kind} ")
        #    dump(fd, fd.spelling)
        # bpdb.set_trace()
        return var_records

    def extract_class_records(self):
        """
        look at nodes for enum decl and definitions
        """
        CK = CursorKind
        for kind in [CK.CLASS_DECL, CK.CLASS_TEMPLATE, CK.STRUCT_DECL, CK.UNION_DECL]:
            for node in self.nodelist[kind]:
                # skip anon classes for now
                # TODO: allow union in class/struct
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

                class_record = ClassRecord(name, node.displayname, node.hash, node.kind.name)
                # now find closing brace so we can inject 'friend' type statements
                *_, last_tok = node.get_tokens()
                class_record.last_tok = last_tok
                if last_tok.kind != TokenKind.PUNCTUATION or last_tok.spelling != "}":
                    log.warning(f" can't find closing brace of {class_record}")

                if node.kind == CK.CLASS_TEMPLATE:
                    for fd in node.walk_preorder():
                        is_template_type_param = (fd.kind == CK.TEMPLATE_TYPE_PARAMETER)
                        is_template_non_type_param = (fd.kind == CK.TEMPLATE_NON_TYPE_PARAMETER)
                        # print(f" {fd.kind} {fd.spelling} type:{fd.type.spelling} is_def:{fd.is_definition()}
                        # as:{fd.access_specifier.name} dn:{fd.displayname} ")
                        if is_template_type_param or is_template_non_type_param:
                            tvar_record = ClassVar(
                                    fd.spelling, fd.displayname, fd.type.spelling, fd.access_specifier.name, 0)
                            tvar_record.is_template_type = is_template_type_param
                            class_record.tvars.append(tvar_record)

                class_record.vars = self.extract_vars_from_class(node, indent=0)

                log.debug(f"class_record = {class_record}")
                if len(class_record.vars) > 0 or len(class_record.tvars) > 0:
                    self.class_records.append(class_record)

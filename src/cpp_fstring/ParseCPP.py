"""
    @file  ParseCPP.py
    @author  Sandeep <deep@tensorfield.ag>
    @version 1.0

    @section LICENSE

    MIT License <http://opensource.org/licenses/MIT>

    @section DESCRIPTION

    https://github.com/d-e-e-p/cpp-fstring
    Copyright (c) 2023 Sandeep <deep@tensorfield.ag>

translation unit (TU)
    single file including all #included code.
    root cursor for traversing the contents of a translation unit

cursor:
    abstraction that represents some node in the AST (Abstract Syntax Tree) of a TU

token:
    smallest unit of a program that is meaningful to the compiler
    keyword, identifier, literal, operator or punctuation symbol
"""

import logging
import os
import re
from typing import Callable

from clang.cindex import AccessSpecifier, Config, Cursor
from clang.cindex import CursorKind as CK
from clang.cindex import Index, Token, TokenKind, TranslationUnit, TypeKind

# import bpdb  # noqa: F401
from cpp_fstring.DataClass import BaseClassRecord, ClassRecord, ClassVar, EnumConstantDecl, EnumRecord, dump

# from cpp_fstring.clang.cindex import AccessSpecifier, Config, Cursor
# from cpp_fstring.clang.cindex import CursorKind as CK
# from cpp_fstring.clang.cindex import Index, Token, TokenKind, TranslationUnit

log = logging.getLogger(__name__)


class ParseCPP:
    """
    parse cpp file
    """

    def __init__(self, code, filename, extraargs, **kwargs):
        self.string_records = []
        self.enum_records = []
        self.class_records = []

        # used by callback
        self.enum_record = None
        self.class_record = None
        self.cxx_base_specifier = set()

        self.code = code
        self.filename = filename
        self.file = None
        self.extraargs = extraargs
        self.interesting_kinds = [
            CK.COMPOUND_STMT,  # for strings
            CK.ENUM_DECL,  # for enum
            # for structs+classes
            CK.STRUCT_DECL,
            CK.CLASS_DECL,
            CK.UNION_DECL,
            CK.CLASS_TEMPLATE,
        ]
        self.nodelist = {key: [] for key in self.interesting_kinds}
        self.INDENT = 4
        self.file_has_existing_formatters = set()

    def extract_interesting_records(self):
        """
        pick out classes, structs, unions, enums, strings

        parse the c++ file using libclang and visit every node, extracting interesting objects
        """
        self.set_filename_for_parsing()
        args = [self.filename]
        args.extend(
            [
                "--std=c++17",
                "-nobuiltininc",
                "--no-standard-includes",
            ]
        )
        args.extend(self.extraargs)
        # from https://cwoodall.com/posts/2018-02-24-using-clang-and-python-to-generate-cpp-struct-serde-fns/
        # Add the include files for the standard library?
        # syspath = ccsyspath.system_include_paths('clang++')
        # incargs = [b'-I' + inc for inc in syspath]

        unsaved_files = [(self.filename, self.code)]

        if not Config.loaded:
            self.set_libclang_from_lib()
        index = Index.create()
        log.debug(f"clang args = {args}")
        tu = index.parse(path=None, args=args, unsaved_files=unsaved_files, options=TranslationUnit.PARSE_INCOMPLETE)
        if not tu:
            log.error(f"unable to load input using args = {args}")
        for diagnostic in tu.diagnostics:
            log.debug(diagnostic.format())

        self.file = tu.get_file(self.filename)  # to compare against external included files

        # self.find_string_records(tu.cursor)
        # self.get_info(tu.cursor)
        self.visit(tu.cursor, 0, set(), self.cb_store_if_interesting)
        self.remove_duplicate_records()
        self.extract_string_records()
        self.find_existing_formatters()
        self.extract_enum_records()
        self.extract_class_records()

        return self.string_records, self.enum_records, self.class_records

    def find_libclang_lib(self):
        file_name, dirs = self.get_libclang_file_dirs()
        file = self.find_first_file(file_name, dirs)
        if file is None:
            log.error(f"can't find pre-built lib {file_name} under dirs {dirs}")
            exit()
        log.info(f"using library file {file}")
        if not Config.loaded:
            Config.set_library_file(file)

    def set_filename_for_parsing(self):
        """
        deal with include files

        libclang parsing has problems if the file to process is an include file
        so append a fake .cpp for these cases.
        tools/clang-format/git-clang-format looks for: h hh hpp hxx
        we just blindly append all files with with .h* type extension with .fake.cpp
        """
        if re.match(r".*\.h[^.]*", self.filename):
            self.filename += ".fake.cpp"

    def get_libclang_file(self):
        """
        copied from clang cindex
        """
        import platform

        name = platform.system()

        if name == "Darwin":
            file = "libclang.dylib"
        elif name == "Windows":
            file = "libclang.dll"
        else:
            file = "libclang.so"
        return file

    def find_first_file(self, file_name, dirs):
        """
        find first file matching file_name in list of dirs
        """
        for dir_path in dirs:
            for root, dirs, files in os.walk(dir_path):
                if file_name in files:
                    first_file = os.path.join(root, file_name)
                    return first_file
        return None

    def find_newest_file(self, file_name, dirs):
        """
        find newest file matching file_name in list of dirs
        """
        newest_file = None
        newest_time = 0
        for dir_path in dirs:
            for root, dirs, files in os.walk(dir_path):
                if file_name in files:
                    file_path = os.path.join(root, file_name)
                    file_time = os.path.getmtime(file_path)
                    if file_time > newest_time:
                        newest_file = file_path
                        newest_time = file_time
        return newest_file

    def set_libclang_from_lib(self):
        """
        assume libclang.dylib/dll/so is under "native" dir of clang lib
        """
        import clang  # noqa: E402

        dir = clang.__path__[0]
        library_path = os.path.join(os.path.realpath(dir), "native")
        file = os.path.join(library_path, self.get_libclang_file())
        if os.path.isfile(file):
            log.debug(f"using libclang library : {file}")
        else:
            log.error(f"libclang file not found: {file}")
        Config.set_library_file(file)

    def dup_nodes_remover(self, nodes):
        seen = set()
        res = []
        for node in nodes:
            if node.hash not in seen:
                res.append(node)
                seen.add(node.hash)
        return res

    def dup_vars_remover(self, vars):
        seen = set()
        res = []
        for var in vars:
            if var.name not in seen:
                res.append(var)
                seen.add(var.name)
        return res

    def remove_duplicate_records(self):
        """
        remove copies of visited nodes
        """
        for type, nodes in self.nodelist.items():
            self.nodelist[type] = self.dup_nodes_remover(self.nodelist[type])

    def remove_duplicate_class_vars(self):
        """
        it's possible to end up with same var because of overloading, eg:
        (name='Target', qualified_name='clipp::detail::action_provider<Derived>::set(Target &)::Target',
        (name='Target', qualified_name='clipp::detail::action_provider<Derived>::set(Target &, Value &&)::Target',
        """

        for rec in self.class_records:
            rec.vars = self.dup_vars_remover(rec.vars)
            rec.tvars = self.dup_vars_remover(rec.tvars)

    def strip_location_from_unnamed_struct(self):
        """
        unnamed struct and class and union get a long description of location
        make it short!
        """
        for rec in self.class_records:
            rec.name = self.remove_loc_from_name(rec.name)
            rec.displayname = self.remove_loc_from_name(rec.displayname)
            for var in rec.vars:
                var.name = self.remove_loc_from_name(var.name)
                var.displayname = self.remove_loc_from_name(var.displayname)
                var.qualified_name = self.remove_loc_from_name(var.qualified_name)
                var.vartype = self.remove_loc_from_name(var.vartype)

    def remove_loc_from_name(self, name):
        """
        strip out location information in unnamed class/struct

        .. code-block:: CPP

            //convert:
             FrogClass::(unnamed struct at /Us/c/clde.h.fake.cpp:219:5)
            //to:
             FrogClass::(unnamed struct)
        """
        return re.sub(r"\(unnamed (\w+) at .*\)", r"(unnamed \1)", name)

    # from https://gist.github.com/scturtle/a7b5349028c249f2e9eeb5688d3e0c5e
    def visit(self, node: Cursor, indent: int, saw: set, callback: Callable[[], str]):
        # skip printting UNEXPOSED_* ?
        # kind = node.kind
        # if not kind.is_unexposed():
        callback(node, indent)
        saw.add(node.hash)
        if node.referenced is not None and node.referenced.hash not in saw:
            self.visit(node.referenced, indent + self.INDENT, saw, callback)
        for c in node.get_children():
            self.visit(c, indent + self.INDENT, saw, callback)
        # saw.remove(node.hash) # <-- to see expanded tree

    def cb_store_if_interesting(self, node, indent):
        kind = node.kind
        prefix = " " * indent
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

        if hasattr(node, "get_tokens"):
            tokens = ""
            for fd in node.get_tokens():
                tokens += " " + fd.spelling
            line += f"  tok={tokens}"

            log.debug(line)

    def extract_enum_records(self):
        """
        look at nodes for enum decl and definitions
        """
        for node in self.nodelist[CK.ENUM_DECL]:
            # ignore unnamed enum
            if node.is_anonymous():
                continue

            log.debug(f" extract_enum_records {node.spelling}")

            # skip if file has pre-existing fmt::formatter statements
            *_, last_tok = node.get_tokens()
            if last_tok.kind != TokenKind.PUNCTUATION or last_tok.spelling != "}":
                log.debug(f" can't find closing brace of {node.spelling}")
                continue

            # skip this enum because file has some pre-existing formatters
            if last_tok.location.file.name in self.file_has_existing_formatters:
                continue

            # skip this enum because it is external--only internal enums are supported
            is_external = last_tok.location.file.name != self.filename
            if is_external:
                continue

            # TODO: find a more robust solution for anon namespace
            if "(anonymous namespace)::" in node.type.spelling:
                enum_record = EnumRecord(node.spelling)
            else:
                name = self.get_qualified_name(node)
                enum_record = EnumRecord(name)
                enum_record.is_anonymous = node.is_anonymous()

            enum_record.is_scoped = node.is_scoped_enum()
            kind = node.type.get_declaration().kind
            # CK.NO_DECL_FOUND when struct S { using enum Fruit; };
            if kind == CK.NO_DECL_FOUND:
                enum_record.enum_type = None
            else:
                enum_record.enum_type = node.type.get_declaration().enum_type.kind.name
            if node.access_specifier == AccessSpecifier.INVALID:
                enum_record.access_specifier = "PUBLIC"
            else:
                enum_record.access_specifier = node.access_specifier.name

            # is any parent a namespace?
            namespacelist = self.get_parent_namespaces(node)
            if namespacelist:
                enum_record.namespace = "::".join(namespacelist)

            # is parent a class?
            is_in_class, last_tok = self.get_enclosing_class(node)
            enum_record.is_in_class = is_in_class
            enum_record.class_last_tok = last_tok

            enum_record.is_in_function = self.get_enclosing_function(node)

            self.enum_record = enum_record
            self.visit(node, 0, set(), self.cb_extract_enum_records)
            self.enum_records.append(self.enum_record)

    def cb_extract_enum_records(self, node, indent):
        """
        for each visited note, look for constant declarations
        """
        if node.kind == CK.ENUM_CONSTANT_DECL:
            enum_constant_decl = EnumConstantDecl(node.displayname, str(node.enum_value))
            self.enum_record.values.append(enum_constant_decl)

    def extract_string_records(self):
        """
        just create a list of string object tokens
        """
        for node in self.nodelist[CK.COMPOUND_STMT]:
            # skip if external
            is_external = node.location.file.name != self.filename
            if is_external:
                continue
            for token in node.get_tokens():
                if token.kind == TokenKind.LITERAL:
                    in_str = token.spelling
                    log.debug(f"{token.cursor.kind.name}  str: {token.spelling}")
                    if in_str.find("{") > 0 or in_str.find("}") > 0:
                        self.string_records.append(token)

    def get_qualified_name(self, node):
        if node is None:
            return ""
        # elif node.kind == CK.TRANSLATION_UNIT or node.kind == CK.FILE:
        elif node.kind == CK.TRANSLATION_UNIT:
            return ""
        else:
            res = self.get_qualified_name(node.semantic_parent)
            if res != "":
                return res + "::" + node.displayname
        return node.displayname

    def get_parent_namespaces(self, node):
        res = []
        if node is None:
            return res
        elif node.kind == CK.TRANSLATION_UNIT:
            return res
        else:
            res = self.get_parent_namespaces(node.semantic_parent)
            if node.semantic_parent.kind == CK.NAMESPACE:
                res.append(node.semantic_parent.displayname)
        return res

    def get_enclosing_class(self, node):
        """
        for enum we need to potentially insert friend statement for protected enum
        at class level
        """
        kind = node.semantic_parent.kind
        if kind != CK.CLASS_DECL and kind != CK.STRUCT_DECL and kind != CK.CLASS_TEMPLATE:
            return False, Token()

        *_, last_tok = node.semantic_parent.get_tokens()
        return True, last_tok

    def get_enclosing_function(self, node):
        """
        for enum we need to potentially insert to_string function inside function
        """
        kind = node.semantic_parent.kind
        is_in_function = kind == CK.CXX_METHOD
        return is_in_function

    def extract_vars_from_class(self, node, prefix, indent):
        """
        associate vars with nested classes

        .. code-block::

        - add immediate variables
            - for templates need to calculate specialization name
        - then add variables from base classes
        """
        var_records = []
        if node is None:
            return var_records

        # if "Map" in node.spelling:
        #    for fd in node.get_children():
        #        print(f" {self.get_qualified_name(fd)} {fd.kind.name} {fd.type.spelling}")
        #    bpdb.set_trace()
        for fd in node.get_children():
            if fd.kind == CK.FIELD_DECL or fd.kind == CK.VAR_DECL:
                name = fd.spelling
                qualified_name = self.get_qualified_name(fd)

                var_record = ClassVar(
                    prefix + name,
                    prefix + qualified_name,
                    fd.displayname,
                    fd.type.spelling,
                    fd.access_specifier.name,
                    indent,
                )
                var_record.is_anonymous = fd.is_anonymous()
                var_record.is_pointer = fd.type.kind == TypeKind.POINTER
                var_record.parent_node = node
                var_records.append(var_record)

        # need to deal with inheritance
        # see https://stackoverflow.com/questions/42795408/can-libclang-parse-the-crtp-pattern
        # TODO: decide between visiting all downstream nodes with fd.walk_preorder() or just
        # immediate children with fd.get_children()
        for fd in node.walk_preorder():
            if fd.kind == CK.CXX_BASE_SPECIFIER:
                # has_template_args = fd.type.get_num_template_arguments() > 0
                # for fm in fd.walk_preorder():
                #   log.debug(f" fm ref = {fm.displayname} {fm.type.spelling} {has_template_args}")
                #   if fm.kind == CK.TYPE_REF:
                #   if fm.kind == CK.CLASS_DECL:
                #   if fm.kind == CK.TEMPLATE_REF:

                # gather more variables from base classes
                base_node = fd.get_definition()
                derived_var_records = self.extract_vars_from_class(base_node, prefix, indent + 1)
                for rec in derived_var_records:
                    if fd.access_specifier != AccessSpecifier.PUBLIC:
                        rec.access_specifier = fd.access_specifier.name
                    var_records.append(rec)

        # for fd in node.walk_preorder():
        #    print(f" {fd.spelling} {fd.type.spelling} {fd.kind} ")
        #    dump(fd, fd.spelling)
        return var_records

    def extract_one_class_record(self, node):
        """create ClassRecord for suitable nodes"""

        # skip anon classes for now
        # TODO: allow union in class/struct
        # if node.is_anonymous():
        #    return

        # skip Local class/struct, ie defined inside main() for example
        if node.lexical_parent.kind == CK.FUNCTION_DECL:
            return

        # create record
        name = self.get_qualified_name(node)
        # if ">::" in name:
        #    #  template class is parent...skip for now
        #    return
        if not name:
            pass
            """
            name = self.get_qualified_name(node)
            if "::" in qname:
                # 'A::Base' + 'Base<T>' => 'A::Base<T>'
                path = qname.rsplit("::", 1)[0]
                name = path + "::" + node.displayname
            else:
                name = node.displayname
            """

        class_record = ClassRecord(name, node.displayname, node.hash, node.kind.name)
        if node.access_specifier is AccessSpecifier.INVALID:
            class_record.access_specifier == "PUBLIC"
        else:
            class_record.access_specifier = node.access_specifier.name

        # now find closing brace so we can inject 'friend' or 'to_string()'
        *_, last_tok = node.get_tokens()
        class_record.last_tok = last_tok
        if last_tok.kind != TokenKind.PUNCTUATION or last_tok.spelling != "}":
            log.debug(f" can't find closing brace of {class_record}")
            class_record.last_tok = None
            return
        # skip this definition because file has some pre-existing formatters
        if last_tok.location.file.name in self.file_has_existing_formatters:
            return

        # skip if this class already has a pre-existing to_string function
        for fd in node.get_children():
            if fd.kind == CK.CXX_METHOD and fd.spelling == "to_string":
                return

        if node.kind == CK.CLASS_TEMPLATE:
            """
            see https://en.cppreference.com/w/cpp/language/template_parameters
            // non-type template parameter type is char
            B<'a'> b2;
            // type param has class or typename
            template<typename... Ts> class My_tuple;
            // Template template parameter
            template<template<typename> typename C> class Map;
            """
            for fd in node.walk_preorder():
                is_template_type_param = fd.kind == CK.TEMPLATE_TYPE_PARAMETER
                is_template_non_type_param = fd.kind == CK.TEMPLATE_NON_TYPE_PARAMETER
                is_template_template_param = fd.kind == CK.TEMPLATE_TEMPLATE_PARAMETER
                template_type_map = {
                    is_template_type_param: "type",
                    is_template_non_type_param: "non_type",
                    is_template_template_param: "template",
                }

                # print(f" {fd.kind} {fd.spelling} type:{fd.type.spelling} is_def:{fd.is_definition()}
                # as:{fd.access_specifier.name} dn:{fd.displayname} ")
                if is_template_type_param or is_template_non_type_param or is_template_template_param:
                    qualified_name = self.get_qualified_name(fd)
                    tvar_record = ClassVar(
                        fd.spelling, qualified_name, fd.displayname, fd.type.spelling, fd.access_specifier.name, 0
                    )
                    tvar_record.template_type = template_type_map[True]
                    # detect param packs like in template < auto ... Values >
                    for tok in fd.get_tokens():
                        if tok.kind == TokenKind.PUNCTUATION and tok.spelling == "...":
                            tvar_record.is_param_pack = True

                    class_record.tvars.append(tvar_record)

        class_record.vars = self.extract_vars_from_class(node, prefix="", indent=0)
        self.mark_base_classes_with_protected_vars(class_record)

        if len(class_record.vars) > 0 or len(class_record.tvars) > 0:
            self.class_records.append(class_record)

    def mark_base_classes_with_protected_vars(self, class_record):
        """
        derived classes might need to print variables marked private in base classes
        mark them for future friend statements
        """
        base_class = {}
        for var in class_record.vars:
            if var.access_specifier != "PUBLIC" and var.parent_node.hash != class_record.hash:
                base_class[var.parent_node.hash] = var.parent_node

        for node in base_class.values():
            name = self.get_qualified_name(node)
            base_record = BaseClassRecord(name, node.displayname, node.hash, node.kind.name)
            *_, last_tok = node.get_tokens()
            base_record.last_tok = last_tok
            class_record.bases.append(base_record)

    def extract_class_records(self):
        """
        look at nodes for enum decl and definitions
        """
        for kind in [CK.CLASS_DECL, CK.CLASS_TEMPLATE, CK.STRUCT_DECL, CK.UNION_DECL]:
            for node in self.nodelist[kind]:
                self.extract_one_class_record(node)

        """
         OK: PUBLIC
         NOT OK: INVALID PROTECTED PRIVATE NONE?
        """
        for rec in self.class_records:
            if rec.access_specifier != "PUBLIC":
                rec.wants_to_be_friends = True
            private_vars = [var for var in rec.vars if var.access_specifier != "PUBLIC"]
            if len(private_vars) > 0:
                rec.wants_to_be_friends = True

        """
        mark external definitions in include files
        """
        for rec in self.class_records:
            rec.is_external = rec.last_tok.location.file.name != self.filename

        """
        remove records if last_tok is None, ie just forward declarations
        """
        self.class_records = list(filter(lambda rec: rec.last_tok is not None, self.class_records))

        """
        remove duplicate vars and tvars
        """
        self.remove_duplicate_class_vars()

        """
        get rid of (unnamed struct at examples/psrc/class_namespace1.cpp:16:1) in names
        """
        self.strip_location_from_unnamed_struct()

        for rec in self.class_records:
            log.debug(f"class_record = {rec}")

    def add_one_existing_formatter(self, node):
        """
        determine name and args about formatter
        """
        pass

    def find_existing_formatters(self):
        """
        we would like to find existing functions of the type

        template <>
        struct fmt::formatter<Bar> {

        unfortunately libclang doesn't expose explicit specialization
        of a template like this one. so we could parse this like
        ourselves or use just assume any existing specialization means
        all classes and enums have fmt:formatter in this file.

        """
        for kind in [CK.STRUCT_DECL]:
            for node in self.nodelist[kind]:
                if "unnamed struct" in node.spelling:
                    if getattr(node, "get_tokens"):
                        tokens = ""
                        for fd in node.get_tokens():
                            tokens += " " + fd.spelling
                        if "struct fmt :: formatter" in tokens:
                            if fd.location.file.name not in self.file_has_existing_formatters:
                                self.file_has_existing_formatters.add(fd.location.file.name)

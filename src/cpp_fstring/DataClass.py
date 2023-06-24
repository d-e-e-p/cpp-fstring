"""
    @file  DataClass.py
    @author  Sandeep <deep@tensorfield.ag>
    @version 1.0

    @section LICENSE

    MIT License <http://opensource.org/licenses/MIT>

    @section DESCRIPTION

    https://github.com/d-e-e-p/cpp-fstring
    Copyright (c) 2023 Sandeep <deep@tensorfield.ag>

"""

from dataclasses import dataclass, field
from typing import Callable
import types

from clang.cindex import Token

"""
storage structures for string/enum/class records
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
    is_external: bool = False
    last_tok: Token = Token()
    access_specifier: str = "PUBLIC"
    namespace: str = None
    is_in_class: bool = False
    is_in_function: bool = False
    class_last_tok = Token()
    values: list[EnumConstantDecl] = field(default_factory=list)


@dataclass
class ClassVar:
    """
    store class/struct variables
    """

    name: str
    qualified_name: str
    displayname: str
    vartype: str
    access_specifier: str = "PUBLIC"
    indent: int = 0
    template_type: str = ""  # TODO: use enum for valid values type, non_type, template ?
    is_pointer: bool = False
    is_param_pack: bool = False
    parent_node = None
    out: str = ""


@dataclass
class BaseClassRecord:
    """
    store base class/struct
    """

    name: str
    displayname: str
    hash: int
    class_kind: str = "STRUCT_DECL"
    last_tok: Token = Token()


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
    wants_to_be_friends: bool = False
    is_external: bool = False
    needs_to_string: bool = False
    access_specifier: str = "PUBLIC"
    is_anonymous: bool = False
    bases: list[BaseClassRecord] = field(default_factory=list)
    vars: list[ClassVar] = field(default_factory=list)
    tvars: list[ClassVar] = field(default_factory=list)


@dataclass
class SelectedRecords:
    """
    store lists of str/enum/class tokens
    """

    tstring: list[Token]
    tenum: list[EnumRecord]
    tclass: list[Token]


def dump(obj, name="obj"):
    """
    dump any key value obj, especially clang cursor objects
    """
    for attribute in dir(obj):
        # if isinstance(getattr(obj, attribute), str):
        if attribute.startswith("_"):
            continue
        if attribute.startswith("objc_type_encoding"):  # often makes process segfault
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
        if type(val) is types.MethodType:  # noqa: E721
            try:
                print("()", end=" ")
                print(val())
            except:  # noqa: E722
                print("ERR")
        else:
            print(val)

    attr_list = "lexical_parent type".split()
    for attr in attr_list:
        if hasattr(obj, attr):
            val = getattr(obj, attr)
            if val is not None:
                print(f"{name}[{attr}] = {val.spelling}")

    if hasattr(obj, "get_tokens"):
        tokens = ""
        for fd in obj.get_tokens():
            tokens += " " + fd.spelling
        print(f"{name}[tokens] = {tokens}")

    print("------")

import logging
import re
from collections import defaultdict

import bpdb  # noqa: F401

log = logging.getLogger(__name__)


class Processor:
    """
    from: https://docs.python.org/3.11/reference/lexical_analysis.html#formatted-string-literals

    f_string          ::=  (literal_char | "{{" | "}}" | replacement_field)*
    replacement_field ::=  "{" f_expression ["="] ["!" conversion] [":" format_spec] "}"
    f_expression      ::=  (conditional_expression | "*" or_expr)
                             ("," conditional_expression | "," "*" or_expr)* [","]
                           | yield_expression
    conversion        ::=  "s" | "r" | "a"
    format_spec       ::=  (literal_char | NULL | replacement_field)*
    literal_char      ::=  <any code point except "{", "}" or NULL>
    """

    def __init__(self, args=None, **kwargs):
        # https://regex101.com/r/5cY7CW/1
        self.pattern = r"(\{)([^}:]+)(?=(:[^}]+)?(\}))"
        self.vars = []

    def get_char_replacements(self, in_str):
        """
        find something to replace double brackets and :: that isn't in the in_str
        also see https://clang.llvm.org/doxygen/LiteralSupport_8cpp_source.html
        """
        lbracket = self.get_unique(in_str, "⟪")
        rbracket = self.get_unique(in_str, "⟫")
        doublecolon = self.get_unique(in_str, "__DOUBLECOLON__")

        return lbracket, rbracket, doublecolon

    def get_unique(self, in_str, target):
        while in_str.find(target) > 0:
            target += target
        return target

    def fstring_elem_callback(self, match):
        """
        ok now look for { in string and assume each one has a corresponding var
        """
        var = match[2].strip()
        if not var:
            log.warning(f" no var found in fstring: {match}")

        ends_with_equal = var.endswith("=")

        if ends_with_equal:
            var = var.rstrip("=")
        self.vars.append(var)

        if ends_with_equal:
            repl = f"{var}={{"
        else:
            repl = "{"
        return repl

    def gen_fstring_changes(self, records):
        changes = []
        for rec in records:
            """
            convert:
                "this is a {foo_bar} test"
            to:
                fmt::format("this is a {} test", foo_bar)
                            ------ f_str ------  -v_str-
            """
            # in_str = repr(rec.value)[1:-1]  # escape backslash
            in_str = rec.spelling

            (lbracket, rbracket, doublecolon) = self.get_char_replacements(in_str)
            rbacket_rev = rbracket[::-1]

            # replace protected patterns like :: {{ }} so regex doesn't have to real with them
            in_str = in_str.replace("::", doublecolon)
            in_str = in_str.replace("{{", lbracket)
            # right-to-left replace
            in_str = in_str[::-1].replace("}}", rbacket_rev)[::-1]

            # implement a version of :
            # https://docs.python.org/3/whatsnew/3.8.html#f-strings-support-for-self-documenting-expressions-and-debugging # noqa: E501

            self.vars = []
            f_str = re.sub(self.pattern, self.fstring_elem_callback, in_str)

            f_str = f_str.replace(lbracket, "{{")
            f_str = f_str.replace(rbracket, "}}")
            f_str = f_str.replace(doublecolon, "::")

            # are there any vars or const inside brackets?
            if self.vars:
                v_str = ", ".join(self.vars)
                v_str = v_str.replace(doublecolon, "::")
                replacement_str = f"fmt::format({f_str}, {v_str})"
            else:
                replacement_str = f_str

            changes.append([rec, replacement_str])

        return changes

    def gen_class_changes_old(self, records):
        """
        add a friend format statement to classes with private vars
             OK: PUBLIC
             NOT OK: INVALID PROTECTED PRIVATE NONE?
        """
        changes = []
        for rec in records:
            ok_to_be_friends = rec.wants_to_be_friends and not rec.is_external
            if ok_to_be_friends:
                replacement_str = f"  friend struct fmt::formatter<{rec.name}>;\n"
                replacement_str += rec.last_tok.spelling
                changes.append([rec.last_tok, replacement_str])
        return changes

    def remove_duplicate_class_instances(self, records):
        """
        sometimes a class ends up in AST with template and non-template version
        """
        rec_by_loc = defaultdict(list)
        for rec in records:
            if not rec.is_external:
                loc = rec.last_tok.location
                key = tuple([loc.line, loc.column])
                rec_by_loc[key].append(rec)

        #
        # ok now remove duplicates, giving CLASS_TEMPLATE priority over CLASS_DECL
        #                                 --->   -                        --> -
        # hack: T comes after D so...
        for key, recs in rec_by_loc.items():
            recs.sort(key=lambda rec: rec.class_kind)
            recs[-1].needs_to_string = True

    def gen_class_changes(self, records):
        """
        inject to_string entry into every class/struct
        """
        self.remove_duplicate_class_instances(records)

        changes = []
        for rec in records:
            if not rec.is_external and rec.needs_to_string:
                replacement_str = self.gen_to_string(rec)
                replacement_str += rec.last_tok.spelling
                changes.append([rec.last_tok, replacement_str])
        """
        inject friend entry for all derived classes
        """
        for rec in records:
            if not rec.is_external:
                for base in rec.bases:
                    replacement_str = self.gen_class_derived_friend_string(base, rec)
                    replacement_str += base.last_tok.spelling
                    changes.append([base.last_tok, replacement_str])

        return changes

    def gen_enum_changes(self, records):
        """
        inject friend entry into every class/struct that have projected enum
        """
        changes = []
        for rec in records:
            if not rec.is_external and rec.is_in_class:
                replacement_str = self.gen_enum_friend_statement(rec)
                replacement_str += rec.class_last_tok.spelling
                changes.append([rec.class_last_tok, replacement_str])
        return changes

    def gen_class_derived_friend_string(self, base, rec):
        """
        base classes need friend statement to access private vars, so
        remove common prefix from base.name and rec.name
        """
        derived_name = self.remove_common_namespace(base.name, rec.name)
        return f"\n  friend class {derived_name};\n"

    def remove_common_namespace(self, source: str, target: str) -> str:
        """
        if both derived and base class have exactly the same prefix, then leave it out
        """
        source_list = source.split("::")
        target_list = target.split("::")

        tail = target_list.pop()
        source_list.pop()

        if target_list == source_list:
            return tail
        else:
            return target

    def gen_enum_friend_statement(self, rec):
        """
        generate a way to stringify any function
        """
        # skip if anon
        if rec.is_anonymous:
            return ""
        out = self.gen_enum_header_comment(rec)
        out += " friend "
        out += self.gen_enum_switch_statement(rec)
        return out

    def gen_to_string(self, rec):
        """
        generate a way to stringify any function
        """
        vars = self.get_all_class_vars(rec)
        log.debug(f"{rec.name} : {vars}")

        # if len(vars) == 0:
        #    return f"{rec.name}"

        template_decl_str, ttvars = self.get_template_decl(rec)

        decl = rec.name

        out = f"""// Generated to_string for {rec.access_specifier} {rec.class_kind} {decl}
  public:
  auto to_string() const {{
    return fstr::format(R"( {decl}: """

        last_vartype = None
        for var in vars:
            prefix = " " * var.indent
            # vartype='T *' should still end up with description
            vlist = var.vartype.split(None, 1)
            vtype = vlist[0]
            decoration = "" if (len(vlist) == 1) else vlist[1]
            if vtype in ttvars:
                # vartype = f"{vtype}={{}}{decoration}"
                vartype = f"<{{}}{decoration}>"
            else:
                vartype = var.vartype
            if vartype == last_vartype:
                var.out = f"{var.name}={{}}"
            else:
                var.out = f"{vartype} {var.name}={{}}"
            last_vartype = vartype

        out += ', '.join([var.out for var in vars])
        out += '\n)"'
        #bpdb.set_trace()

        # deal with pointers using fmt::ptr
        # deal with special cases of derived variables in class templates using this->
        # TODO: only use this-> for class templates
        varlist = []
        last_vtype = None
        for var in vars:
            vtype = var.vartype.split()[0]
            if vtype in ttvars:
                if vtype != last_vtype:
                    varlist.append(f"typeid({vtype}).name()")
            last_vtype = vtype

            if var.indent > 0:
                name = f"this->{var.name}"
            else:
                name = var.name

            if var.is_pointer:
                varlist.append(f"fmt::ptr({name})")
            else:
                varlist.append(name)

        if varlist:
            out += ", " + ", ".join(varlist)

        out += """);
  }
"""
        return out

    def gen_enum_format(self, records):
        """
        given list of enum generate fmt: statements or format_as statements
        """
        changes = ""
        for rec in records:
            log.debug(f" enum = {rec}")
            changes += self.gen_one_enum(rec)
        # for enum in namespaces add alias command to refer to top level
        # version of format_as
        changes += self.gen_enum_namespace_alias(records)
        return changes

    def gen_enum_namespace_alias(self, records):
        nslist = set()
        for rec in records:
            if rec.is_in_class or rec.is_in_function or rec.namespace is None:
                continue
            nslist.add(rec.namespace)

        out = "\n"
        for ns in nslist:
            out += f"namespace {ns} {{using ::format_as;}}\n"

        return out

    def gen_one_enum(self, rec):
        """ """

        # skip enum with no entries
        if len(rec.values) == 0:
            return ""

        # skip if anon
        if rec.is_anonymous:
            return ""

        # skip if already a friend statement
        if rec.is_in_class or rec.is_in_function:
            return ""

        # comment out private enums..
        out = ""
        comment_out = (rec.access_specifier != "PUBLIC" and rec.is_external) or (rec.access_specifier == "PROTECTED")

        if comment_out:
            out += f"\n/******************* {rec.access_specifier} **\n"

        out += self.gen_enum_header_comment(rec)
        out += self.gen_enum_switch_statement(rec)

        if comment_out:
            out += f"\n******************** {rec.access_specifier} */\n"

        return out

    def gen_enum_header_comment(self, rec):
        """
        comment for enum
        """
        scoped_str = "scoped" if rec.is_scoped else ""
        return (
            f"// Generated formatter for {rec.access_specifier} enum {rec.name} of type {rec.enum_type} {scoped_str}\n"
        )

    def gen_enum_switch_statement(self, rec):
        """
          follow example in fmt:: documentation, ie:

        enum class color {red, green, blue};
        auto format_as(const color c) {
          fmt::string_view name = "unknown";
          switch (c) {
              case color::red:   name = "red";   break;
              case color::green: name = "green"; break;
              case color::blue:  name = "blue";  break;
          }
          return name;
        };
        """
        out = ""

        decl = rec.name
        out += f"""constexpr auto format_as(const {decl} obj) {{
  fmt::string_view name = "<missing>";
  switch (obj) {{
"""
        out2 = ""
        out2 += f"""
// Generated formatter for {rec.access_specifier} enum {decl} of type {rec.enum_type} scoped {rec.is_scoped}
template <>
struct fmt::formatter<{decl}>: formatter<string_view> {{
  template <typename FormatContext>
  auto format({decl} val, FormatContext& ctx) const {{
    string_view name = "<unknown>";
    switch (val) {{
"""
        # if scoped and name of enum decl is A::B::my_enum then inherit the whole name
        # if not scoped, leave out the my_enum part
        if rec.is_scoped:
            prefix = f"{decl}::"
        else:
            separator = "::"
            prefix = separator.join(decl.rsplit(separator, 1)[:-1]) + separator
            if prefix == separator:
                prefix = ""

        # if all index values are zero, its probably a problem
        seen_index = set()
        for elem in rec.values:
            seen_index.add(elem.index)
        is_valid_index = len(seen_index) > 1

        seen_index = set()
        for elem in rec.values:
            is_duplicate = elem.index in seen_index
            seen_index.add(elem.index)

            width = max([len(x.name) for x in rec.values])
            name_in_quotes = f'"{elem.name}"'
            line = (
                f"case {prefix}{elem.name:<{width}}: name = {name_in_quotes:<{width+2}}; break;  // index={elem.index}"
            )
            if is_duplicate and is_valid_index:
                out += f"//  {line} <-- index is duplicate\n"
            else:
                out += f"    {line}\n"

        out += """  }
  return name;
}
"""

        out2 += """    }
    return formatter<string_view>::format(name, ctx);
  }
};"""
        return out

    def gen_class_format(self, records):
        """
        look at simple class/struct and produce to_string statement
        """
        changes = ""
        for rec in records:
            changes += self.gen_one_class(rec)
        return changes

    def get_template_decl(self, rec):
        """
        look thru definitions and produce list that goes inside template <>, eg
                template <typename T, T Min, T Max>

        for template typename arguments, emit extra statement showing type of template param,
        eg, for example above:
            typeid(T).name()
        """
        if rec.class_kind != "CLASS_TEMPLATE":
            return "", []

        tvarlist = []
        ttypelist = []
        for tvar in rec.tvars:
            if tvar.name:
                if tvar.is_template_type:
                    tvarlist.append(f"typename {tvar.name}")
                    ttypelist.append(tvar.name)
                else:
                    tvarlist.append(f"{tvar.vartype} {tvar.name}")

        template_decl_str = ", ".join(tvarlist)
        log.debug(f" template_decl_str = {template_decl_str}")
        return template_decl_str, ttypelist

    def get_typeid_calls(self, rec):
        """ """
        if rec.class_kind != "CLASS_TEMPLATE":
            return ""

        tvarlist = []
        ttypelist = []
        for tvar in rec.tvars:
            if tvar.is_template_type:
                tvarlist.append(f"typename {tvar.name}")
                ttypelist.append(tvar.name)
            else:
                tvarlist.append(f"{tvar.vartype} {tvar.name}")

        template_decl_str = ", ".join(tvarlist)
        print(f" template_decl_str={template_decl_str}, ttypelist={ttypelist}")
        return template_decl_str, ttypelist

    def get_all_class_vars(self, rec):
        """
        deal with inherited classes having multiple echos of same var
        """
        seen = set()
        vars = []
        # everthing is blocked for private classes
        if rec.access_specifier != "PUBLIC" and rec.is_external:
            return vars

        # assume everything internal is going to be friended
        for var in rec.vars:
            if var.displayname not in seen:
                if var.access_specifier == "PUBLIC" or not rec.is_external:
                    vars.append(var)
                    seen.add(var.displayname)
        return vars

    def gen_one_class(self, rec):
        """
        follow example in fmt:: documentation
        """
        vars = self.get_all_class_vars(rec)
        log.debug(f"{rec.name} : {vars}")
        # TODO: dump out empty class/struct ?
        if len(vars) == 0:
            return ""

        template_decl_str, tvars = self.get_template_decl(rec)

        decl = rec.name

        out = f"""// Generated formatter for {rec.access_specifier} {rec.class_kind} {decl}
template <{template_decl_str}>
struct fmt::formatter<{decl}> {{
    constexpr auto parse(format_parse_context& ctx) {{
        return ctx.begin();
    }}

    template <typename FormatContext>
    auto format(const {decl}& obj, FormatContext& ctx) {{
        return format_to(ctx.out(),
R"({rec.class_kind} {decl}:
"""
        for tvar in tvars:
            out += f"   type({tvar}): {{}} \n"

        for var in vars:
            prefix = " " * var.indent
            out += f" {prefix}   {var.access_specifier} {var.vartype} {var.name}: {{}} \n"

        out += ')"'

        tvarlist = [f"typeid({tvar}).name() " for tvar in tvars]
        if tvarlist:
            out += ", " + ", ".join(tvarlist)

        varlist = [f"obj.{var.name}" for var in vars]
        if varlist:
            out += ", " + ", ".join(varlist)

        out += """);
    }
};
"""
        return out

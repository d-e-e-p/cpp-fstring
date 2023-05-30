import logging
import re

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

    def gen_class_changes(self, records):
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

    def gen_enum_format(self, records):
        """
        given list of enum generate fmt: statements
        """
        changes = ""
        for rec in records:
            log.debug(f" enum = {rec}")
            changes += self.gen_one_enum(rec)
        return changes

    def gen_one_enum_format_as(self, rec):
        """
        follow example in fmt:: documentation, ie:

        """

        # skip enum with no entries
        if len(rec.values) == 0:
            return ""

        # skip if anon
        if rec.is_anonymous:
            return ""

        # don't care about access_specifier?

        decl = rec.name
        out = f"""
// Generated formatter for {rec.access_specifier} enum {decl} of type {rec.enum_type.spelling} scoped {rec.is_scoped}
  auto format_as(const {decl} obj) {{
    fmt::string_view name = "<unknown>";
    switch (obj) {{
"""
        # if scoped and name of enum decl is foo::bar::my_enum then inherit the whole name
        # if not scoped, leave out the my_enum part
        if rec.is_scoped:
            prefix = f"{decl}::"
        else:
            separator = "::"
            prefix = separator.join(decl.rsplit(separator, 1)[:-1]) + separator
            if prefix == separator:
                prefix = ""

        seen_index = []
        for elem in rec.values:
            is_duplicate = elem.index in seen_index
            seen_index.append(elem.index)

            width = max([len(x.name) for x in rec.values])
            name_in_quotes = f'"{elem.name}"'
            line = (
                f"case {prefix}{elem.name:<{width}}: name = {name_in_quotes:<{width+2}}; break;  // index={elem.index}"
            )
            if not is_duplicate:
                out += f"        {line}\n"
            else:
                out += f"    //  {line} <-- index is duplicate\n"

        out += """    }
    return name;
    };"""

        return out

    def gen_one_enum(self, rec):
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

        or template version...

        """

        # skip enum with no entries
        if len(rec.values) == 0:
            return ""

        # skip if anon
        if rec.is_anonymous:
            return ""

        # comment out private enums..
        out = ""
        if rec.access_specifier == "PROTECTED":
            out += "\n/******************* PROTECTED **\n"

        decl = rec.name
        out += f"""
// Generated formatter for {rec.access_specifier} enum {decl} of type {rec.enum_type} scoped {rec.is_scoped}
template <>
struct fmt::formatter<{decl}>: formatter<string_view> {{
  template <typename FormatContext>
  auto format({decl} val, FormatContext& ctx) const {{
    string_view name = "<unknown>";
    switch (val) {{
"""
        # if scoped and name of enum decl is foo::bar::my_enum then inherit the whole name
        # if not scoped, leave out the my_enum part
        if rec.is_scoped:
            prefix = f"{decl}::"
        else:
            separator = "::"
            prefix = separator.join(decl.rsplit(separator, 1)[:-1]) + separator
            if prefix == separator:
                prefix = ""

        seen_index = []
        for elem in rec.values:
            is_duplicate = elem.index in seen_index
            seen_index.append(elem.index)

            width = max([len(x.name) for x in rec.values])
            name_in_quotes = f'"{elem.name}"'
            line = (
                f"case {prefix}{elem.name:<{width}}: name = {name_in_quotes:<{width+2}}; break;  // index={elem.index}"
            )
            if not is_duplicate:
                out += f"        {line}\n"
            else:
                out += f"    //  {line} <-- index is duplicate\n"

        out += """    }
    return formatter<string_view>::format(name, ctx);
  }
};"""

        if rec.access_specifier == "PROTECTED":
            out += "\n******************** PROTECTED */\n"

        return out

    def gen_class_format(self, records):
        """
        look at simple struct and produce debug format to_string version
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
        bpdb.set_trace()
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
        if len(vars) == 0:
            return ""

        template_decl_str, tvars = self.get_template_decl(rec)

        decl = rec.name

        out = f"""// Generated formatter for {rec.class_kind} {decl}
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

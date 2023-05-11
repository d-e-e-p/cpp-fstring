import logging
import re
import pudb
import bpdb

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
        # https://regex101.com/r/lurKSR/1
        self.pattern = r"(?<=\{)([^}:]+)(?=(:[^}]+)?\})"
        self.lbracket = "__LEFT_CURLY_BRACKET__"
        self.rbracket = "__RIGHT_CURLY_BRACKET__"

    def get_char_replacements(self, in_str):
        """
        find something to replace double brackets and :: that isn't in the in_str
        """
        lbracket = "⟪"
        while in_str.find(lbracket) > 0:
            lbracket += lbracket

        rbracket = "⟫"
        while in_str.find(rbracket) > 0:
            rbracket += rbracket

        doublecolon = "__DOUBLECOLON__"
        while in_str.find(rbracket) > 0:
            rbracket += rbracket
        return lbracket, rbracket, doublecolon

    def get_literals(self, tok):
        """
        see https://clang.llvm.org/doxygen/LiteralSupport_8cpp_source.html
        """
        lpos = tok.lexpos
        rpos = lpos + len(tok.value)

        (lliteral, rliteral) = ("", "")
        #ldelim = self.code[lpos-1]
        #rdelim = self.code[rpos+1]
        log.debug(f" {tok} ld={ldelim} rd={rdelim}")



    def gen_fstring_changes(self, tokens):
        changes = []
        for tok in tokens:
            """
            convert:
                "this is a {foo_bar} test"
            to:
                fmt::format("this is a {} test", foo_bar)
                            ------ f_str ------  -v_str-
            """
            #in_str = repr(tok.value)[1:-1]  # escape backslash
            in_str = tok.spelling
            # (lliteral, rliteral) = self.get_literals(tok)

            (lbracket, rbracket, doublecolon) = self.get_char_replacements(in_str)
            rbacket_rev = rbracket[::-1]

            in_str = in_str.replace("::", doublecolon)
            in_str = in_str.replace("{{", lbracket)
            # right-to-left replace
            in_str = in_str[::-1].replace("}}", rbacket_rev)[::-1]


            log.debug(f"t2 = {tok} i={in_str}")

            matches = re.findall(self.pattern, in_str)
            f_str = re.sub(self.pattern, "", in_str)

            # are there any vars or const inside brackets?
            if matches:
                v_str = ", ".join(map(lambda x: x[0], matches))
                v_str = v_str.replace(doublecolon, "::")

                f_str = f_str.replace(lbracket, "{{")
                f_str = f_str.replace(rbracket, "}}")
                f_str = f_str.replace(doublecolon, "::")
                replacement_str = f"fmt::format({f_str}, {v_str})"
            else:
                f_str = f_str.replace(lbracket, "{")
                f_str = f_str.replace(rbracket, "}")
                f_str = f_str.replace(doublecolon, "::")
                replacement_str = f_str

            log.debug(f"t={tok} after={replacement_str}")
            changes.append([tok, replacement_str])

        return changes

    def gen_class_changes(self, tokens):
        """
        add a friend format statement to classes with private vars
             OK: PUBLIC 
             NOT OK: INVALID PROTECTED PRIVATE NONE?
        """
        changes = []
        for tok in tokens:
            public_vars = [var for var in tok.vars if var.access_specifier=='PUBLIC']
            if len(tok.vars) > len(public_vars):
                replacement_str = f"  friend struct fmt::formatter<{tok.name}>;\n"
                replacement_str += tok.last_tok.spelling;
                changes.append([tok.last_tok, replacement_str])
                # bpdb.set_trace()
        
        return changes


    def gen_enum_format(self, tokens):
        """
        given list of enum generate fmt: statements
        """
        changes = ""
        for tok in tokens:
            log.debug(f" tok = {tok}")
            changes += self.gen_one_enum(tok)
        return changes

    def gen_one_enum_format_as(self, tok):
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

        # skip enum with no entries
        if len(tok.values) == 0:
            return ""

        # skip if anon
        if tok.is_anonymous:
            return ""

        decl = tok.name
        out = f"""
// Generated formatter for enum {decl} of type {tok.enum_type.spelling} scoped {tok.is_scoped}
  auto format_as(const {decl} obj) {{
    fmt::string_view name = "<unknown>";
    switch (obj) {{
"""
        # pu.db()
        # if scoped and name of enum decl is foo::bar::my_enum then inherit the whole name
        # if not scoped, leave out the my_enum part
        if tok.is_scoped:
            prefix = f"{decl}::"
        else:
            separator = "::"
            prefix = separator.join(decl.rsplit(separator, 1)[:-1]) + separator
            if prefix == separator:
                prefix = ""

        seen_index = []
        for elem in tok.values:
            is_duplicate = (elem.index in seen_index)
            seen_index.append(elem.index)

            width = max([len(x.name) for x in tok.values])
            name_in_quotes = f'"{elem.name}"'
            line = f"""case {prefix}{elem.name:<{width}}: name = {name_in_quotes:<{width+2}}; break;  // index={elem.index}"""
            if not is_duplicate:
                out += f"        {line}\n"
            else:
                out += f"    //  {line} <-- index is duplicate\n"

        out += """    }
    return name;
    };"""

        return out

    def gen_one_enum(self, tok):
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

        # skip enum with no entries
        if len(tok.values) == 0:
            return ""

        # skip if anon
        if tok.is_anonymous:
            return ""

        decl = tok.name
        out = f"""
// Generated formatter for enum {decl} of type {tok.enum_type.spelling} scoped {tok.is_scoped}
template <> 
struct fmt::formatter<{decl}>: formatter<string_view> {{
  template <typename FormatContext>
  auto format({decl} val, FormatContext& ctx) const {{
    string_view name = "<unknown>";
    switch (val) {{
"""
        # if scoped and name of enum decl is foo::bar::my_enum then inherit the whole name
        # if not scoped, leave out the my_enum part
        if tok.is_scoped:
            prefix = f"{decl}::"
        else:
            separator = "::"
            prefix = separator.join(decl.rsplit(separator, 1)[:-1]) + separator
            if prefix == separator:
                prefix = ""

        seen_index = []
        for elem in tok.values:
            is_duplicate = (elem.index in seen_index)
            seen_index.append(elem.index)

            width = max([len(x.name) for x in tok.values])
            name_in_quotes = f'"{elem.name}"'
            line = f"""case {prefix}{elem.name:<{width}}: name = {name_in_quotes:<{width+2}}; break;  // index={elem.index}"""
            if not is_duplicate:
                out += f"        {line}\n"
            else:
                out += f"    //  {line} <-- index is duplicate\n"

        out += """    }
    return formatter<string_view>::format(name, ctx);
  }
};"""

        return out

    def gen_class_format(self, tokens):
        """
        look at simple struct and produce debug format to_string version
        """
        changes = ""
        for tok in tokens:
            changes += self.gen_one_class(tok)
        return changes

    def get_template_decl(self, tok):
        """
        look thru definitions and produce list that goes inside template <>, eg
                template <typename T, T Min, T Max>
        """
        if tok.class_kind != 'CLASS_TEMPLATE':
            return ""

        tvarlist = []
        for tvar in tok.tvars:
            if tvar.is_template_type:
                tvarlist.append(f"typename {tvar.name}")
            else:
                tvarlist.append(f"{tvar.vartype} {tvar.name}")

        return ", ".join(tvarlist)

    def gen_one_class(self, tok):
        """
        follow example in fmt:: documentation
        """
        template_decl_str = self.get_template_decl(tok) 

        decl = tok.name if tok.name else tok.displayname
        
        out = f"""// Generated formatter for {tok.class_kind} {decl}
template <{template_decl_str}>
struct fmt::formatter<{decl}> {{
    constexpr auto parse(format_parse_context& ctx) {{
        return ctx.begin();
    }}

    template <typename FormatContext>
    auto format(const {decl}& obj, FormatContext& ctx) {{
        return format_to(ctx.out(),
R"({tok.class_kind} {decl}:
"""
        for var in tok.vars:
            out += f"   {var.access_specifier} {var.vartype} {var.name}: {{}} \n"

        out += ')"'

        # bpdb.set_trace()
        varlist = [f"obj.{var.name}" for var in tok.vars]
        if varlist:
            out += ", " + ", ".join(varlist)

        out += """);
    }
};
"""
        return out

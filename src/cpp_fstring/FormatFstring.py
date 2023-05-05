import logging
import re

_logger = logging.getLogger(__name__)


class FormatFstring:
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

    def get_bracket_replacements(self, in_str):
        """
        find something to replace double brackets that isn't in the in_str
        """
        lbracket = "⟪"
        while in_str.find(lbracket) > 0:
            lbracket += lbracket

        rbracket = "⟫"
        while in_str.find(rbracket) > 0:
            rbracket += rbracket
        return lbracket, rbracket

    def get_literals(self, tok):
        """
        see https://clang.llvm.org/doxygen/LiteralSupport_8cpp_source.html
        """
        lpos = tok.lexpos
        rpos = lpos + len(tok.value)

        (lliteral, rliteral) = ("", "")
        #ldelim = self.code[lpos-1]
        #rdelim = self.code[rpos+1]
        print(f" {tok} ld={ldelim} rd={rdelim}")



    def get_changes(self, tokens):
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
            in_str = tok.value
            # (lliteral, rliteral) = self.get_literals(tok)

            (lbracket, rbracket) = self.get_bracket_replacements(in_str)
            rbacket_rev = rbracket[::-1]

            in_str = in_str.replace("{{", lbracket)
            # right-to-left replace
            in_str = in_str[::-1].replace("}}", rbacket_rev)[::-1]
            _logger.debug(f"t2 = {tok} i={in_str}")
            matches = re.findall(self.pattern, in_str)
            f_str = re.sub(self.pattern, "", in_str)

            # are there any vars or const inside brackets?
            if matches:
                v_str = ", ".join(map(lambda x: x[0], matches))
                f_str = f_str.replace(lbracket, "{{")
                f_str = f_str.replace(rbracket, "}}")
                replacement_str = f"fmt::format({f_str}, {v_str})"
            else:
                f_str = f_str.replace(lbracket, "{")
                f_str = f_str.replace(rbracket, "}")
                replacement_str = f_str

            _logger.debug(f"t={tok} after={replacement_str}")
            changes.append([tok, replacement_str])

        return changes

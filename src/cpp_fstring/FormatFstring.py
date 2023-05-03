
import ast
import logging

_logger = logging.getLogger(__name__)

class FormatFstring():
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
        pass

    def get_changes(self, tokens):
        changes = []
        for tok in tokens:
            fstr = 'f' + tok.value
            p = ast.parse(fstr)
            _logger.debug(f"t2 = {tok} p={p}")
            for body in p.body:
                typ = type(body.value)
                if typ is ast.Constant:
                    continue
                #breakpoint()
                _logger.debug(f" body.value = {body.value}")
                parts = body.value.values
                replstr = "std::format(\""
                varlist = []
                for part in parts:
                    typ = type(part)
                    _logger.debug(f" {part} typ={typ} val={part.value}  ")
                    if typ is ast.Constant:
                        # don't interprit backslash
                        replstr += repr(part.value)[1:-1]
                    if typ is ast.FormattedValue:
                        col_start = part.value.col_offset
                        col_end = part.value.end_col_offset
                        string = tok.value[col_start:col_end]
                        # _logger.debug(f"{part.conversion}, fs={part.format_spec}, l={part.lineno}, v={part.value} cons={string}")
                        # varlist.append(string)
                        replstr += "{"
                        if hasattr(part.value, "id"):
                            _logger.debug(f"var id = {part.value.id}")
                            varlist.append(part.value.id)
                            if part.format_spec:
                                for val in part.format_spec.values:
                                    # 'col_offset', 'end_col_offset', 'end_lineno', 'lineno'
                                    _logger.debug(f"format = {part.format_spec} spec= {val.value}")
                                    replstr += ":" + val.value
                        replstr += "}"
                    if typ is ast.Str:
                        _logger.debug(f" str {part} typ={type(part)} val={part.value} kind={part.kind} ")
                replstr += "\", "
                replstr += ", ".join(varlist)
                replstr += ");"
                _logger.debug(f"res = {replstr}")
                changes.append([tok, replstr])

        return changes

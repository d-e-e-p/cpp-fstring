
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
            # replace {{ and }} for easier matching
            # TODO: make sure these tags don't already exist in string
            ast_parse = ast.parse(fstr)
            _logger.debug(f"t2 = {tok} p={ast_parse}")
            print(ast.dump(ast_parse, indent=4))
            for body in ast_parse.body:
                typ = type(body.value)
                if typ is ast.Constant:
                    continue
                #breakpoint()
                _logger.debug(f" body.value = {body.value}")
                parts = body.value.values
                replstr = "\""
                varlist = []
                for part in parts:
                    typ = type(part)
                    _logger.debug(f" {part} typ={typ} val={part.value}  ")
                    if typ is ast.Constant:
                        # don't interprit backslash
                        strvalue = repr(part.value)[1:-1]
                        strvalue = strvalue.replace('}','}}')
                        strvalue = strvalue.replace('{','{{')
                        replstr += strvalue
                    if typ is ast.FormattedValue:
                        # either value=Name(id='foo', ctx=Load()),
                        # or value=Constant(value=42),
                        replstr += "{"
                        #breakpoint()
                        vtyp = type(part.value)
                        if vtyp is ast.Name:
                            _logger.debug(f"var id = {part.value.id}")
                            varlist.append(part.value.id)
                            if part.format_spec:
                                for val in part.format_spec.values:
                                    # 'col_offset', 'end_col_offset', 'end_lineno', 'lineno'
                                    _logger.debug(f"format = {part.format_spec} spec= {val.value}")
                                    replstr += ":" + val.value
                        
                        if vtyp is ast.Constant:
                            _logger.debug(f"var value = {part.value.value}")
                            breakpoint()
                            # can't use part.value.value because 0xa -> 65 etc
                            strconst = tok.value[part.value.col_offset:part.value.end_col_offset]
                            varlist.append(strconst)
                            if part.format_spec:
                                for val in part.format_spec.values:
                                    # 'col_offset', 'end_col_offset', 'end_lineno', 'lineno'
                                    _logger.debug(f"format = {part.format_spec} spec= {val.value}")
                                    replstr += ":" + val.value
                        
                        replstr += "}"
                    if typ is ast.Str:
                        _logger.debug(f" str {part} typ={type(part)} val={part.value} kind={part.kind} ")
                replstr += "\""
                if varlist:
                    varstr = ", ".join(varlist)
                    replstr = f"std::format({replstr}, {varstr})"
                _logger.debug(f"res = {replstr}")
                changes.append([tok, replstr])

        return changes

import logging

import bpdb  # noqa: F401

log = logging.getLogger(__name__)

"""
should really be using
 https://clang.llvm.org/doxygen/classclang_1_1Lexer.html#a7e7f08993261441a8d83d9253ca53859
 ◆ makeFileCharRange() - Accepts a range and returns a character range with file locations.
 ◆ MeasureTokenLength - Relex the token at the specified location and return its length in bytes in the input file.


https://clang.llvm.org/doxygen/classclang_1_1tooling_1_1Replacement.html
◆ Replacement() Creates a replacement of the range [Offset, Offset+Length) in FilePath with ReplacementText.



"""


class GenerateOutput:
    def __init__(self, code, args=None, **kwargs):
        self.code = code
        self.offset = 0

    def get_absolute_position(self, line_number, column_number):
        lines = self.code.split("\n")
        characters_per_line = [len(line) for line in lines]
        # Add one for the newline character
        characters_per_line = [1 + i for i in characters_per_line]
        characters_per_line = [0] + characters_per_line  # Add a zero at the beginning
        absolute_position = sum(characters_per_line[:line_number]) + column_number - 1
        return absolute_position

    def write_changes(self, *args):
        """
        write code changes to stdout
        changes is a list of [token, replacement_string]
        """
        changes = []
        for arg in args:
            if isinstance(arg, list):
                changes += arg

        pos_changes = []
        for [tok, replstr] in changes:
            log.debug(f" tok={tok} r={replstr}")
            pos_start = self.get_absolute_position(tok.extent.start.line, tok.extent.start.column)
            pos_end = self.get_absolute_position(tok.extent.end.line, tok.extent.end.column)
            if len(tok.spelling) != pos_end - pos_start:
                log.error(
                    f"""
                tok : {tok.spelling}
                extent: {tok.extent}
                len: {len(tok.spelling)}
                pos: {pos_start} -> {pos_end} = {pos_end - pos_start}
                """
                )
                j = 0
                for i in range(pos_start, pos_end):
                    log.error(f" i={i} j={j} tokenstring={tok.spelling[j]} code={self.code[i]}")
                    j += 1
            pos_changes.append([pos_start, tok.spelling, replstr])

        pos_changes.sort()  # sort by pos_start
        for [pos_start, before, after] in pos_changes:
            self.replace_str(pos_start, before, after)

        print(self.code)
        return self.code

    def replace_str(self, pos, before, after):
        """
        replace strings in file based on position
        """
        pos_start = pos + self.offset
        pos_end = pos_start + len(before)
        self.offset += len(after) - len(before)
        self.code = after.join([self.code[:pos_start], self.code[pos_end:]])

    def append(self, addition):
        """
        just append changes to code for now
        """
        self.code += addition
        print(addition)

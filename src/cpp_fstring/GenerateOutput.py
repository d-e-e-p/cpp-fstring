import logging
import pdb  # noqa: F401

_logger = logging.getLogger(__name__)


class GenerateOutput:
    def __init__(self, args=None, **kwargs):
        self.code = None
        self.offset = 0

    def write_changes(self, code, changes):
        """
        write code changes to stdout
        changes is a list of [token, replacement_string]
        """
        self.code = code
        for [tok, replstr] in changes:
            _logger.debug(f" tok={tok} r={replstr}")
            self.replace_str(tok.lexpos, tok.value, replstr)

        print(self.code)

    def replace_str(self, pos, before, after):
        """
        replace strings in file based on position
        """
        pos_start = pos + self.offset
        pos_end = pos_start + len(before)
        self.offset += len(after) - len(before)
        code = after.join([self.code[:pos_start], self.code[pos_end:]])
        print("-----------------------------------------")
        print(f"LEFT   part = '{self.code[pos_start-2:pos_start]}'")
        print(f"MID    part = '{self.code[pos_start:pos_end]}' len={len(before)}")
        print(f"MID    part = '{before}' len={len(before)}")
        print(f"RIGHT  part = '{self.code[pos_end:pos_end+2]}'")
        ll = 0
        for i in range(len(before)):
            match = before[i] == self.code[pos_start+i]
            print(f" i={i} l={len(before)} b={before[i]} m={self.code[pos_start+i]} MATCH={match} b={ord(before[i])} m={ord(self.code[pos_start+i])}")
        self.code = code

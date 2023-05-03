
import pdb
import logging
_logger = logging.getLogger(__name__)


class GenerateOutput():

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
        self.code = after.join([self.code[:pos_start], self.code[pos_end:]])

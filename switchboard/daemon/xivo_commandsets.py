
__version__ = '$Revision$ $Date$'

CMD_OTHER = 1 << 0
CMD_LOGIN = 1 << 1

class Command:
        def __init__(self, commandname, commandargs):
                self.name = commandname
                self.args = commandargs
                self.type = None

class BaseCommand:
        def __init__(self):
                pass

CommandClasses = {}

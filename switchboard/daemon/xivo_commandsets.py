
__version__ = '$Revision$ $Date$'

class Command:
        def __init__(self, commandname, commandargs):
                self.name = commandname
                self.args = commandargs

class BaseCommand:
        def __init__(self):
                pass

CommandClasses = {}

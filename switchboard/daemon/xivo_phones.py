__version__   = '$Revision$'
__date__      = '$Date$'
__copyright__ = 'Copyright (C) 2007, 2008, Proformatique'
__author__    = 'Corentin Le Gall'


class Phones:
        def __init__(self):
                self.list = {}
                return

        def setcommandclass(self, commandclass):
                self.commandclass = commandclass
                return

        def update(self, astid):
                # self.list = self.commandclass.get_phonelist()
                return

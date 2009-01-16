__version__ = "$Revision$ $Date$"
__license__ = """
    Copyright (C) 2008  Proformatique <technique@proformatique.com>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
"""

from xivo_agid import agid
from xivo_agid import objects

def agent_get_options(agi, cursor, args):
    try:
        agent = objects.Agent(agi, cursor, number=args[0])
    except LookupError, e:
        agi.verbose(str(e))
        return

    options = ""

    if agent.silent:
        options += "s"

    agi.set_variable('XIVO_AGENTEXISTS', 1)
    agi.set_variable('XIVO_AGENTPASSWD', agent.passwd)
    agi.set_variable('_XIVO_AGENTID', agent.id)
    agi.set_variable('_XIVO_AGENTLANGUAGE', agent.language)
    agi.set_variable('_XIVO_AGENTOPTIONS', options)

agid.register(agent_get_options)

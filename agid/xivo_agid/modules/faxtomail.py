__version__ = "$Revision$ $Date$"
__license__ = """
    Copyright (C) 2006, 2007, 2008  Proformatique

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA..
"""

import os

from xivo_agid import agid

class FaxToMailException(Exception): pass

def faxtomail(handler, agi, cursor, args):
	dstnum = agi.get_variable('REAL_DSTNUM')

	filename = args[0]
	filepdf = None
	error_message = None

	try:
		if not filename:
			raise FaxToMailException("No fax file")

		filepdf = filename.replace(".tif",".pdf")

		email = args[1]

		if not email:
			raise FaxToMailException("No email address")

		# TODO fetch tiff2pdf path from configuration file.
		status = os.system("/usr/bin/tiff2pdf -o '%s' '%s'" % (filepdf, filename))

		if status:
			raise FaxToMailException("Unable to convert fax to PDF")

		# TODO fetch Mutt path from configuration file.
		status = os.system("echo \"Un nouveau fax est arrive. Il est joint dans ce mail.\n\nCordialement,\nService IPBX\" | /usr/bin/mutt -s \"Reception de FAX vers %s\" -a %s %s" % (dstnum, filepdf, email))

		if status:
			raise FaxToMailException("Unable to send fax as an email")
	except Exception, message:
		error_message = message

	try:
		if filename:
			os.remove(filename)

		if filepdf:
			os.remove(filepdf)
	except OSError:
		pass

	if error_message:
		agi.dp_break(error_message)

agid.register(faxtomail)

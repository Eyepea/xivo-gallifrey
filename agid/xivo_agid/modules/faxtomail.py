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
import subprocess

from xivo_agid import agid

TIFF2PDF = "/usr/bin/tiff2pdf"
MUTT = "/usr/bin/mutt"

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
		try:
			status = subprocess.call([TIFF2PDF, "-o", filepdf, filename], close_fds=True)
		except OSError:
			status = 1

		if status:
			raise FaxToMailException("Unable to convert fax to PDF")

		# TODO fetch Mutt path from configuration file.
		try:
			mutt = subprocess.Popen([ MUTT, "-s", "Reception de FAX vers %s" % dstnum, "-a", filepdf, email ],
			                        stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE,
			                        close_fds=True)
			mutt.communicate("Un nouveau fax est arrive. Il est joint dans ce mail.\n\nCordialement,\nService IPBX\n")
			status = mutt.wait()
		except OSError:
			status = 1

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

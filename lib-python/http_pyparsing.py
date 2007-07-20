"""HTTP Related Parsing using pyparsing

Copyright (C) 2007, Proformatique

WARNING: This parser is _extremely_ slow, you can expect it to consume up to about
         1 million CPU cycles per output field. Use with care or optimize it!

"""

__version__ = "$Revision$ $Date$"
__license__ = """
    Copyright (C) 2007, Proformatique

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
"""

from string import hexdigits
from pyparsing import Word, Suppress, ZeroOrMore, QuotedString, \
                      Group, LineEnd, Optional

__BYTES_VAL = ''.join(map(chr,xrange(0,256)))

HTTP_CHAR = ''.join(map(chr,xrange(128)))
HTTP_SEPARATORS = '()<>@,;:\\/[]?={} \t"'
HTTP_CTLS = ''.join(map(chr,xrange(32)))+chr(127)
HTTP_TOKCHAR = HTTP_CHAR.translate(__BYTES_VAL, HTTP_SEPARATORS + HTTP_CTLS)

HTTP_TOKEN = Word( HTTP_TOKCHAR )
HTTP_QUOTED = QuotedString( '"', escChar = '\\', multiline = True )
HTTP_VALUE = HTTP_TOKEN | HTTP_QUOTED
HTTP_PARAMETER = HTTP_TOKEN + Suppress( '=' ) + HTTP_VALUE

HTTP_TRANSFER_EXTENSION = HTTP_TOKEN + Group( ZeroOrMore( Suppress( ';' ) + Group( HTTP_PARAMETER ) ) )
HTTP_TRANSFER_CODING = Optional( Group( HTTP_TRANSFER_EXTENSION ) ) + ZeroOrMore( Suppress( ',' ) + Optional( Group( HTTP_TRANSFER_EXTENSION ) ) )

HTTP_TRANSFER_CODING_LINE = HTTP_TRANSFER_CODING + LineEnd()


HTTP_CHUNK_EXTENSION = HTTP_TOKEN + Optional( Suppress( '=' ) + HTTP_VALUE )
HTTP_CHUNK_HEADER = Word( hexdigits ) + Group( ZeroOrMore( Suppress( ';' ) + Group( HTTP_CHUNK_EXTENSION ) ) )

HTTP_CHUNK_HEADER_LINE = HTTP_CHUNK_HEADER + LineEnd()

__all__ = [ 'HTTP_CHAR', 'HTTP_SEPARATORS', 'HTTP_CTLS', 'HTTP_TOKCHAR',
            'HTTP_TOKEN', 'HTTP_QUOTED', 'HTTP_VALUE', 'HTTP_PARAMETER',
	    'HTTP_TRANSFER_EXTENSION', 'HTTP_TRANSFER_CODING',
	    'HTTP_TRANSFER_CODING_LINE', 'HTTP_CHUNK_EXTENSION',
	    'HTTP_CHUNK_HEADER', 'HTTP_CHUNK_HEADER_LINE' ]

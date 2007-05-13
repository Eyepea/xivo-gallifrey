# -*- coding: iso-8859-15 -*-
"""Supplementary functions useful to play with URI - very very close to RFC 3986

Copyright (C) 2007, Proformatique

"""

__version__ = "$Revision$ $Date$"

import uriparse, re, string

# Near RFC 3986 definitions
ALPHA = string.ascii_letters
DIGIT = string.digits
HEXDIG = string.digits + "abcdefABCDEF"
SCHEME_CHAR = ALPHA + DIGIT + '+-.'
GEN_DELIMS = ':/?#[]@'
SUB_DELIMS = "!$&'()*+,;="
RESERVED = GEN_DELIMS + SUB_DELIMS
UNRESERVED = ALPHA + DIGIT + '-._~'
PCHAR = UNRESERVED + SUB_DELIMS + '%:@/'
QUEFRAG_CHAR = PCHAR + '?'
USER_CHAR = UNRESERVED + SUB_DELIMS + '%'
PASSWD_CHAR = UNRESERVED + SUB_DELIMS + '%:'
REG_NAME_CHAR = UNRESERVED + SUB_DELIMS + '%'

IPV_FUTURE_RE = 'v[\da-fA-F]+\.[' \
                + re.escape(UNRESERVED + SUB_DELIMS + ':') \
                + ']+'

# Host type alternatives:
HOST_IP_LITERAL = 1
HOST_IPV4_ADDRESS = 2
HOST_REG_NAME = 3

def __all_in(s, charset):
	return False not in map(lambda c:c in charset, s)

def __split_sz(s, n):
	return [s[b:b+n] for b in range(0,len(s),n)]

def __valid_IPv4address(potential_ipv4):
	if potential_ipv4[0] in "0123456789xX" and __all_in(potential_ipv4[1:], "0123456789.xX"):
		s_ipv4 = potential_ipv4.split('.', 4)
		if len(s_ipv4) == 4:
			try:
				for s in s_ipv4:
					i = int(s, 0)
					if i < 0 or i > 255:
						return False
			except ValueError:
				return False
			return True
	return False

def __valid_h16(h16):
	try:
		i = int(h16, 16)
		return i >= 0 and i <= 65535
	except ValueError:
		return False

def __valid_rightIPv6(right_v6):
	if right_v6 == '':
		return 0
	array_v6 = right_v6.split(':', 8)
	if len(array_v6) > 8 \
	   or (len(array_v6) > 7 and ('.' in right_v6)) \
	   or (not __all_in(''.join(array_v6[:-1]), HEXDIG)):
		return False
	if '.' in array_v6[-1]:
		if not __valid_IPv4address(array_v6[-1]):
			return False
		h16_count = 2
		array_v6 = array_v6[:-1]
	else:
		h16_count = 0
	for h16 in array_v6:
		if not __valid_h16(h16):
			return False
	return h16_count + len(array_v6)

def __valid_leftIPv6(left_v6):
	if left_v6 == '':
		return 0
	array_v6 = left_v6.split(':', 7)
	if len(array_v6) > 7 \
	   or (not __all_in(''.join(array_v6), HEXDIG)):
		return False
	for h16 in array_v6:
		if not __valid_h16(h16):
			return False
	return len(array_v6)

def __valid_IPv6address(potential_ipv6):
	sep_pos = potential_ipv6.find('::')
	sep_count = potential_ipv6.count('::')
	if sep_pos < 0:
		return __valid_rightIPv6(potential_ipv6) == 8
	elif sep_count == 1:
		right = __valid_rightIPv6(potential_ipv6[sep_pos+2:])
		if right is False:
			return False
		left = __valid_leftIPv6(potential_ipv6[:sep_pos])
		if left is False:
			return False
		return right + left <= 7
	else:
		return False

def __valid_IPvFuture(potential_ipvf):
	return bool(re.match(IPV_FUTURE_RE+'$', potential_ipvf))

def __valid_IPLiteral(potential_ipliteral):
	if len(potential_ipliteral) < 2 or potential_ipliteral[0] != '[' \
	   or potential_ipliteral[-1] != ']':
		return False
	return __valid_IPv6address(potential_ipliteral[1:-1]) \
	       or __valid_IPvFuture(potential_ipliteral[1:-1])

def __valid_query(pquery_tuple):
	for k,v in pquery_tuple:
		if (k and (not __all_in(k, QUEFRAG_CHAR))) \
		   or (v and (not __all_in(v, QUEFRAG_CHAR))):
			return False
	return True

class InvalidURIError(ValueError):
  """Base class of all Exceptions directly raised by this module"""
class InvalidSchemeError(InvalidURIError):
    """Invalid content for the scheme part of an URI"""
class InvalidAuthorityError(InvalidURIError):
    """Invalid content for the authority part of an URI"""
class InvalidUserError(InvalidAuthorityError):
	"""Invalid content for the user part of an URI"""
class InvalidPasswdError(InvalidAuthorityError):
	"""Invalid content for the passwd part of an URI"""
class InvalidHostError(InvalidAuthorityError):
        """Invalid content for the host part of an URI"""
class InvalidIPLiteralError(InvalidHostError):
                """Invalid content for the IP-literal part of an URI"""
class InvalidRegNameError(InvalidHostError):
                """Invalid content for the reg-name part of an URI"""
class InvalidPortError(InvalidAuthorityError):
	"""Invalid content for the port part of an URI"""
class InvalidPathError(InvalidURIError):
    """Invalid content for the path part of an URI"""
class InvalidQueryError(InvalidURIError):
    """Invalid content for the query part of an URI"""
class InvalidFragmentError(InvalidURIError):
    """Invalid content for the fragment part of an URI"""

def pct_decode(s):
	"""Returns the percent-decoded version of string s
	
	>>> def __split_sz(s, n):
	...     return [s[b:b+n] for b in range(0,len(s),n)]
	... 
	>>> s = ''.join(map(lambda x: '%'+x,
	...                 __split_sz(
	...                     binascii.hexlify("Coucou, je suis un satan"),2))
	...             ).upper()
	>>> print s
	%43%6F%75%63%6F%75%2C%20%6A%65%20%73%75%69%73%20%75%6E%20%73%61%74%61%6E
	>>> pct_decode(s)
	'Coucou, je suis un satan'
	
	"""
	if s is None:
		return None
	ro = re.compile('\%[\da-fA-F][\da-fA-F]')
	m = ro.search(s)
	while m:
		s = s[:m.start()] + chr(int(m.group()[1:], 16)) + s[m.end():]
		m = ro.search(s)
	return s

def query_elt_decode(s):
	"""Returns the percent-decoded version of string s, after a
	plus-to-space substitution has been done.

	>>> query_elt_decode('query+++%2b')
	'query   +'

	"""
	if s is None:
		return None
	return pct_decode(s.replace('+', ' '))

def host_type(host):
	"""Correctly classifies correct RFC 3986 compliant hostnames, but
	don't try hard to validate compliance anyway...
	NOTE: indeed we allow a small deviation from the RFC 3986: IPv4
	addresses are allowed to contain bytes represented in hexadecimal or
	octal notation when begining respectively with '0x'/'0X' and '0'
	numbers prepended with one or more zero won't be rejected. Anyway
	representation of multiple bytes by a single decimal/octal/hexadecimal
	integer is not allowed.
	
	Returns HOST_IP_LITERAL, HOST_IPV4_ADDRESS or HOST_REG_NAME

	>>> host_type('[blablabla]')
	HOST_IP_LITERAL
	>>> host_type('')
	HOST_REG_NAME
	>>> host_type('127.0.0.1')
	HOST_IPV4_ADDRESS
	>>> host_type('0x7F.0.0.00000000000001')
	HOST_IPV4_ADDRESS
	>>> host_type('666.42.131.2')
	HOST_REG_NAME
	>>> host_type('foobar.42')
	HOST_REG_NAME

	"""
	if not host:
		return HOST_REG_NAME
	elif host[0] == '[':
		return HOST_IP_LITERAL
	elif __valid_IPv4address(host):
		return HOST_IPV4_ADDRESS
	else:
		return HOST_REG_NAME

def split_authority(authority):
	"""Splits authority into component parts. This function supports
	IP-literal as defined in RFC 3986.
	
	>>> split_authority("user:passwd@host:port")
	('user', 'passwd', 'host', 'port')
	>>> split_authority("user:@host:port")
	('user', None, 'host', 'port')
	>>> split_authority("user@host:port")
	('user', None, 'host', 'port')
	>>> split_authority("user@[host]:port")
	Traceback (most recent call last):
	  File "<stdin>", line 1, in ?
	  File "<stdin>", line 26, in split_authority
	InvalidIPLiteralError: Highly invalid IP-literal detected in URI authority "user@[host]:port"
	>>> split_authority("user@[::dead:192.168.42.131]:port")
	('user', None, '[::dead:192.168.42.131]', 'port')
	>>> split_authority("[::dead:192.168.42.131]:port")
	(None, None, '[::dead:192.168.42.131]', 'port')
	>>> split_authority(":port")
	(None, None, None, 'port')
	>>> split_authority("user@:port")
	('user', None, None, 'port')
	
	Very basic validation is done if the host part of the authority starts
	with an '[' as when this is the case, the splitting is done in a quite
	different manner than the one used by most URI parsers. As a result an
	InvalidIPLiteralError exception is raised if IP-literal is patently
	wrong, so the risk of major clashes between two deviant implementations
	is highly reduced.
	
	"""
	if '@' in authority:
        	userinfo, hostport = authority.split('@', 1)
		if userinfo and ':' in userinfo:
			user, passwd = userinfo.split(':', 1)
		else:
			user, passwd = userinfo, None
	else:
        	user, passwd, hostport = None, None, authority
	if hostport:
		if hostport[0] == '[':
			m = re.match('\[([\da-fA-F:\.]+|'+IPV_FUTURE_RE
			                     +')\](\:.*|)$', hostport)
			if m:
				host = '[' + m.group(1) + ']'
				port = m.group(2)[1:]
			else:
				raise InvalidIPLiteralError, 'Highly invalid IP-literal detected in URI authority "%s"' % authority
		elif ':' in hostport:
			host, port = hostport.split(':', 1)
		else:
			host, port = hostport, None
	else:
		host, port = None, None
	return (user and user or None, passwd and passwd or None,
		host and host or None, port and port or None)

def split_query(query):
	"""Handles the query as a WWW HTTP 1630 query, as this is how people
	usually thinks of URI queries in general. We do not decode anything
	in split operations, neither percent nor the terrible plus-to-space
	conversion. Returns:
	
	>>> split_query("k1=v1&k2=v+2%12&k3=&k4&&&k5==&=k&==")
	(('k1', 'v1'), ('k2', 'v+2%12'), ('k3', ''), ('k4', None),
		('k5', '='), ('', 'k'), ('', '='))
	
	"""
	def split_assignment(a):
		sa = a.split('=', 1)
		return len(sa) == 2 and tuple(sa) or (sa[0], None)
	assignments = query.split('&')
	return tuple([split_assignment(a) for a in assignments if a])

def uri_split_tree(uri):
	"""Returns (scheme, (user, passwd, host, port), path,
	            ((k1, v1), (k2, v2), ...), fragment) using
	uriparse.urisplit(), then split_authority() and split_query() on the
	result.
	
	>>> uri_split_tree(
	...	'http://%42%20+blabla:lol@%77ww.foobar.org/%7Exilun/' +
	...	'?query=+++%2b&=&===&a=b&&&+++aaa%3D=+%2B%2D&&&&' +
	...	'#frag+++%42')
	('http',
	 ('%42%20+blabla', 'lol', '%77ww.foobar.org', None),
	 '/%7Exilun/',
	 (('query', '+++%2b'), ('', ''), ('', '=='),
	 	('a', 'b'), ('+++aaa%3D', '+%2B%2D')),
	 'frag+++%42')
	
	"""
	scheme, authority, path, query, fragment = uriparse.urisplit(uri)
	if authority:
		authority = split_authority(authority)
	if query:
		query = split_query(query)
	return (scheme and scheme or None, authority and authority or None,
	        path and path or None, query and query or None,
	        fragment and fragment or None)

def uri_tree_normalize(uri_tree):
	"""Transforms an URI tree so that adjacent all-empty fields are
	coalesced into a single None at parent level.
	The return value can be used for validation by uri_tree_validate()
	As a result, no distinction is made between empty and absent fields.
	It is believed that this limitation is harmless because this is the
	behavior of most implementations, and even useful in the context of
	this Python module because empty strings are already not distinguished
	from None when converting to boolean, so we are only generalizing this
	concept in order to keep code small and minimize special cases.
	
	If the distinction is ever really needed, for example to support empty
	anchor special HTTP script related URI in a clean way, one will
	probably need to completely rewrite (or at least review and modify)
	this module, and special care would be taken to distinguish between '',
	(), None and others everywhere implicit boolean conversion is now
	performed. The behavior should then be checked in regards to its
	conformance with RFC 3986, especially (but this would probably not be
	sufficient) the classification switches of some URI parts according to
	the content of others.
	
	"""
	scheme, authority, path, query, fragment = uri_tree
	if authority and (filter(lambda x: bool(x), authority) == ()):
		authority = None
	if query:
		query = filter(lambda (x,y): bool(x) or bool(y), query)
	return (scheme and scheme or None, authority and authority or None,
	        path and path or None, query and query or None,
	        fragment and fragment or None)

def uri_tree_validate(uri_tree):
	"""Validate a tree splitted URI in format returned by
	uri_tree_normalize(), raising an exception in case something invalid
	is detected - that is RFC 3986 is not respected - and returning the
	unmodified uri_tree otherwise, so this function can be used in an
	helper function chaining uri_split_tree(), uri_tree_normalize(),
	uri_tree_validate() and then uri_tree_decode().
	
	This function must be called on something similar to the return value
	of uri_tree_normalize() - and not uri_tree_decode() or directly
	uri_split_tree() - to have a meaningful action.
	
	The following deviations from RFC 3986 - and also design choice - are
	allowed and no exception will be raised in these cases:
	
	- IPv4address can contain decimal / octal / hexadecimal representation
	  of individual bytes.
	- in a similar way h16 in IPv6address can be zero-prepended
	- no percent encoding validation is performed, so that non pct-encoded
	  sequences begining with an '%' will be leaved untouched later by the
	  percent decoding algorithm
	- this function will not attempt to classify path as path-absolute
	  according to the presence of a non-empty authority; paths will always
	  be allowed to begin with '//' because the implicit assertion that
	  this URI has just been splitted by the Appendix B Regular Expression
	  is made, so a (possibly empty) authority was present, and that when
	  the intent of the caller is to join the URI it will take appropriate
	  measures to guaranty RFC 3986 compliance, by unconditionally or if
	  necessary adding an empty authority.
	
	"""
	scheme, authority, path, query, fragment = uri_tree
	if scheme:
		if (scheme[0] not in ALPHA) or (not __all_in(scheme[1:], SCHEME_CHAR)):
			raise InvalidSchemeError, 'Invalid scheme "%s"' % scheme
	if authority:
		user, passwd, host, port = authority
		if user and not __all_in(user, USER_CHAR):
			raise InvalidUserError, 'Invalid user "%s"' % user
		if passwd and not __all_in(passwd, PASSWD_CHAR):
			raise InvalidPasswdError, 'Invalid passwd "%s"' % passwd
		if host:
			type_host = host_type(host)
			if type_host == HOST_REG_NAME:
				if not __all_in(host, REG_NAME_CHAR):
					raise InvalidRegNameError, 'Invalid reg-name "%s"' % host
			elif type_host == HOST_IP_LITERAL:
				if not __valid_IPLiteral(host):
					raise InvalidIPLiteralError, 'Invalid IP-literal "%s"' % host
		if port and not __all_in(port, DIGIT):
			raise InvalidPortError, 'Invalid port "%s"' % port
	if path:
		if not __all_in(path, PCHAR):
			raise InvalidPathError, 'Invalid path "%s" - invalid character detected' % path
		if authority and path[0] != '/':
			raise InvalidPathError, 'Invalid path "%s" - non-absolute path can\'t be used with an authority' % path
		if (not authority) and (not scheme) \
		   and (':' in path.split('/')[0]):
			raise InvalidPathError, 'Invalid path "%s" - path-noscheme can\'t have a \':\' if no \'/\' before' % path
	if query and (not __valid_query(query)):
		raise InvalidQueryError, 'Invalid splitted query tuple "%s"' % str(query)
	if fragment and (not __all_in(fragment, QUEFRAG_CHAR)):
		raise InvalidFragmentError, 'Invalid fragment "%s"' % fragment
	return uri_tree

def uri_tree_decode(uri_tree):
	"""Decode a tree splitted Uri in format returned by uri_split_tree() or
	uri_tree_normalize(), the returned value keeping the same layout.
	
	user, passwd, path, fragment are percent decoded, and so is host if of
	type reg-name.
	
	>>> uri_tree_decode(
	...	uri_split_tree(
	...		'http://%42%20+blabla:lol@%77ww.foobar.org/%7Exilun/' +
	...		'?query=+++%2b&=&===&a=b&&&+++aaa%3D=+%2B%2D&&&&' +
	...		'#frag+++%42'))
	('http',
	 ('B +blabla', 'lol', 'www.foobar.org', None),
	 '/~xilun/',
	 (('query', '   +'), ('', ''), ('', '=='),
		('a', 'b'), ('   aaa=', ' +-')),
	 'frag+++B')
	
	"""
	scheme, authority, path, query, fragment = uri_tree
	if authority:
		user, passwd, host, port = authority
		user, passwd = pct_decode(user), pct_decode(passwd)
		if host and host_type(host) == HOST_REG_NAME:
			host = pct_decode(host)
		authority = (user, passwd, host, port)
	path, fragment = pct_decode(path), pct_decode(fragment)
	if query:
		query = tuple(map(lambda (x,y): (query_elt_decode(x), query_elt_decode(y)), query))
	return (scheme, authority, path, query, fragment)

def uri_split_norm_valid_decode(uri):
	"""Returns uri_tree_decode(
	                uri_tree_validate(
                                uri_tree_normalize(
                                        uri_split_tree(uri))))
	"""
	return uri_tree_decode(
	                uri_tree_validate(
                                uri_tree_normalize(
                                        uri_split_tree(uri))))

# -*- coding: iso-8859-15 -*-
"""Supplementary functions useful to play with URI

Copyright (C) 2007, Proformatique

"""

__version__ = "$Revision$ $Date$"

import uriparse, re, string

# RFC3986 definitions
ALPHA = string.ascii_letters
DIGIT = string.digits
SCHEME_CHAR = ALPHA + DIGIT + '+-.'
GEN_DELIMS = ':/?#[]@'
SUB_DELIMS = "!$&'()*+,;="
RESERVED = GEN_DELIMS + SUB_DELIMS
UNRESERVED = ALPHA + DIGIT + '-._~'
PCHAR = UNRESERVED + SUB_DELIMS + ':@'
QUEFRAG_CHAR = PCHAR + '/?'

# Host type alternatives:
HOST_IP_LITERAL = 1
HOST_IPV4_ADDRESS = 2
HOST_REG_NAME = 3

def __all_in(s, charset):
	return False not in map(lambda c:c in charset, s)

def __split_sz(s, n):
	return [s[b:b+n] for b in range(0,len(s),n)]

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
	addresses do not need to be as small as possible and those with
	numbers prepended with one or more zero won't be regected.
	
	Returns HOST_IP_LITERAL, HOST_IPV4_ADDRESS or HOST_REG_NAME

	>>> host_type('[blablabla]')
	HOST_IP_LITERAL
	>>> host_type('')
	HOST_REG_NAME
	>>> host_type('127.0.0.1')
	HOST_IPV4_ADDRESS
	>>> host_type('127.0.0.00000000000001')
	HOST_IPV4_ADDRESS
	>>> host_type('666.42.131.2')
	HOST_REG_NAME
	>>> host_type('foobar.42')
	HOST_REG_NAME

	"""
	if not host:
		return HOST_REG_NAME
	if host[0] == '[':
		return HOST_IP_LITERAL
	if host[0] in "0123456789":
		sh = host.split('.', 4)
		if len(sh) == 4:
			try:
				for s in sh:
					i = int(s)
					if i < 0 or i > 255:
						return HOST_REG_NAME
			except ValueError:
				return HOST_REG_NAME
			return HOST_IPV4_ADDRESS
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
	ValueError: Highly invalid IP-literal detected in URI authority "user@[host]:port"
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
	different manner than the one used by most URI parsers. So a ValueError
	exception is raised if IP-literal is patently wrong, so the risk of
	major clashes between two deviant implementations is highly reduced.
	
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
			m = re.match('\[([\da-fA-F:\.]+|v\d+\.['
				+ re.escape(UNRESERVED + SUB_DELIMS + ':')
				+ ']+)\](\:.*|)$', hostport)
			if m:
				host = '[' + m.group(1) + ']'
				port = m.group(2)[1:]
			else:
				raise ValueError, 'Highly invalid IP-literal detected in URI authority "%s"' % authority
		elif ':' in hostport:
			host, port = hostport.split(':', 1)
		else:
			host, port = hostport, None
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
	return (scheme, authority, path, query, fragment)

def uri_tree_decode(uri_tree):
	"""Decode a tree splitted Uri in format returned by uri_slit_tree()
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

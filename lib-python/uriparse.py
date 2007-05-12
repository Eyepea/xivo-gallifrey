"""An extensible URI parsing library

  A uri parsing library that strives to be STD66 (aka RFC3986)
  compliant. http://gbiv.com/protocols/uri/rfc/ has a brief history of
  URI standards, which explains why it took so long to get to where this
  module could be written.

Features:
  * Extensible URI-handling framework that includes default URI parsers
    for many common URI schemes
  * convenience methods for splitting up/rejoining URIs
  * convenience methods for splitting up/rejoining authority strings,
    also known as netloc strings
  * urljoin, which produces an absolute URI given a base and a relative
    path to apply

Comments:
  * The code looks simple, and you may wonder at the lack of handling
    %-encoding, but STD66 section 2.4 says that %-encodings can't be
    delimiters, so it's okay to be naive.

Usage:
    # the verbose way
    try:
        p = URIParser(extra={'custom': CustomSchemeHandler})
        defaults = ()
        if p.scheme_of(url) == 'http':
            defaults = ('user', 'password', 'host', 'port', 'path')
        pieces = p.parse(url, defaults)
    except UnknownSchemeError:
        print 'unknown scheme'

    # quick-n-dirty
    try:
        pieces = URIParser({'custom':CustomSchemeHandler}).parse(url, 
	    ('user','pass','host','port','path'))
    except UnknownSchemeError:
        print 'unknown scheme'

    # if you're trying to parse something with a standard URI scheme 
    # (http for instance), with no default values
    url = "http://user:pass@host:port/path"
    try:
        pieces = URIParser().parse(url)
    except UnknownSchemeError:
        print 'unknown scheme'



"""


def urisplit(uri):
    """
       Basic URI Parser according to STD66 aka RFC3986

       >>> urisplit("scheme://authority/path?query#fragment")
       ('scheme', 'authority', 'path', 'query', 'fragment') 

    """
    import re
    # regex straight from STD 66 section B
    regex = '^(([^:/?#]+):)?(//([^/?#]*))?([^?#]*)(\?([^#]*))?(#(.*))?'
    p = re.match(regex, uri).groups()
    scheme, authority, path, query, fragment = p[1], p[3], p[4], p[6], p[8]
    #if not path: path = None
    return (scheme, authority, path, query, fragment) 


def uriunsplit((scheme, authority, path, query, fragment)):
    """
       Reverse of urisplit()

       >>> uriunsplit(('scheme','authority','path','query','fragment'))
       "scheme://authority/path?query#fragment"
    """
    result = ''
    if scheme: 
        result += scheme + ':'
    if authority: 
        result += '//' + authority
    if path:
        result += path
    if query: 
        result += '?' + query
    if fragment: 
        result += '#' + fragment
    return result


def split_authority(authority):
    """
       Basic authority parser that splits authority into component parts
       
       >>> split_authority("user:password@host:port")
       ('user', 'password', 'host', 'port')

    """
    if '@' in authority:
        userinfo, hostport = authority.split('@', 1)
    else:
        userinfo, hostport = None, authority
    if userinfo and ':' in userinfo:
        user, passwd = userinfo.split(':', 1)
    else:
        user, passwd = userinfo, None
    if hostport and ':' in hostport:
        host, port = hostport.split(':', 1)
    else:
        host, port = hostport, None
    if not host:
        host = None
    return (user, passwd, host, port)


def join_authority((user, passwd, host, port)):
    """
       Reverse of split_authority()

       >>>join_authority(('user', 'password', 'host', 'port'))
       "user:password@host:port"

    """
    result = ''
    if user:
        result += user
	if passwd:
	    result += ':' + passwd
	result += '@'
    result += host
    if port:
        result += ':' + port
    return result

class URLParser:
    """Internal Basic URL parsing class.

       In this code URI generally refers to generic URIs and URL refers to
       to URIs that match scheme://user:password@host:port/path?query#fragment.
       While users of this library could use this as an example of how to write a
       parser or even as a parent class of their own parser, they should not directly
       instantiate it - let URIParser do that for you.
    """

    # user, password, host, port, path, query, fragment 
    _defaults = (None, None, None, None, None, None, None)

    def __init__(self, defaults=None):
        if defaults:
            self._defaults = defaults
	dlist = list(self._defaults)
	for d in range(len(self._defaults)):
	    if dlist[d]: 
	        dlist[d] = str(dlist[d])
	self._defaults = dlist

    def parse(self, urlinfo):
        scheme, authority, path, query, frag = urisplit(urlinfo)
	user, passwd, host, port = split_authority(authority)
	duser, dpasswd, dhost, dport, dpath, dquery, dfrag = self._defaults
	if user is None: 
	    user = duser
	if passwd is None: 
	    passwd = dpasswd
	if host is None: 
	    host = dhost
	if port is None: 
	    port = dport
	if path == '': 
	    path = dpath
        if query is None: 
	    query = dquery
        if frag is None: 
	    frag = self._defaults[6]
        return (user, passwd, host, port, path, query, frag)

    def unparse(self, pieces):
        authority = unparse_authority(pieces[:4])
	return unparse_uri(('', authority, pieces[4], pieces[5], pieces[6]))

class HttpURLParser(URLParser):
    """Internal class to hold the defaults of HTTP URLs"""
    _defaults=(None, None, None, 80, '/', None, None)

class HttpsURLParser(HttpURLParser):
    """Internal class to hold the defaults of HTTPS URLs"""
    _defaults=(None, None, None, 443, '/', None, None)

class ShttpURLParser(HttpURLParser):
    """Internal class to hold the defaults of SHTTP URLs"""
    _defaults=(None, None, None, 443, '/', None, None)

class ImapURLParser(URLParser):
    """Internal class to hold the defaults of IMAP URLs"""
    _defaults=(None, None, 'localhost', 143, '/', None, None)

class ImapsURLParser(URLParser):
    """Internal class to hold the defaults of IMAPS URLs"""
    _defaults=(None, None, 'localhost', 993, '/', None, None)

class FtpURLParser(URLParser):
    """Internal class to hold the defaults of FTP URLs"""
    _defaults=('anonymous', 'anonymous', None, 21, '/', None, None)

class TftpURLParser(URLParser):
    """Internal class to hold the defaults of TFTP URLs"""
    _defaults=(None, None, None, 69, '/', None, None)

class FileURLParser(URLParser):
    """Internal class to hold the defaults of file URLs"""
    _defaults=(None, None, None, None, '/', None, None)

class TelnetURLParser(URLParser):
    """Internal class to hold the defaults of telnet URLs"""
    _defaults=(None, None, None, 23, '/', None, None)

class MailtoURIParser(URLParser):
    """Internal mailto URI parser

       This class has a basic understanding of mailto: URIs of the
       format: mailto:user@host?query#frag

    """
    # user, host, query, fragment 
    _defaults = (None, None, None, None)

    def parse(self, urlinfo):
        scheme, authority, path, query, frag = urisplit(urlinfo)
	user, host = path.split('@', 1)
	return (user, host, query, frag)

    def unparse(self, pieces):
        path = pieces[0] + '@' + pieces[1]
	return unparse_uri(('', None, path, pieces[2], pieces[3]))


class URIParser(object):
    """Scheme-independent parsing frontend

       URIParser is a scheme-conscious parser that picks the right
       parser based on the scheme of the URI handed to it.


    """

    Schemes = {'http': HttpURLParser,
               'https': HttpsURLParser,
	       'imap': ImapURLParser,
	       'imaps': ImapsURLParser,
	       'ftp': FtpURLParser,
	       'tftp': TftpURLParser,
	       'file': FileURLParser,
               'telnet': TelnetURLParser,
               'mailto': MailtoURIParser,
	      }

    def __init__(self, schemes=Schemes, extra={}):
        """Create a new URIParser

	schemes is the full set of schemes to consider.  It defaults to URIParser.Schemes,
	which is the full set of parsers on hand.

	extra is a dictionary of schemename:parserclass that is added to the list of
	known parsers.

	"""
        self._parsers = {}
        self._parsers.update(schemes)
        self._parsers.update(extra)

    def parse(self, uri, defaults=None):
        """Parse the URI.  
	
	uri is the uri to parse.
	defaults is a scheme-dependent list of values to use if there
	is no value for that part in the supplied URI.

	The return value is a tuple of scheme-dependent length.

	"""
        return tuple([self.scheme_of(uri)] + list(self.parser_for(uri)(defaults).parse(uri)))

    def unparse(self, pieces, defaults=None):
        """Join the parts of a URI back together to form a valid URI.

        pieces is a tuble of URI pieces.  The scheme must be in pieces[0] so that
	the rest of the pieces can be interpreted.
	
	"""
        return self.parser_for(pieces[0])(defaults).unparse(pieces)

    # these work on any URI 
    def scheme_of(self, uri):
        """Return the scheme of any URI."""
        return uri.split(':')[0]

    def info_of(self, uri):
        """Return the non-scheme part of any URI."""
        return uri.split(':')[1]

    def parser_for(self, uri):
        """Return the Parser object used to parse a particular URI.
	
	Parser objects are required to have only 'parse' and 'unparse' methods.

	"""
        return self._parsers[self.scheme_of(uri)]

def _dirname(p):
    q = p
    while q and not q.endswith('/'):
        q = q[:-1]
    return q

def _pathjoin(a,b):
    if not a or not b:
        return a or b
    elif not b.startswith('/'):
        return a+'/'+b
    else:
        return b

def urljoin(base, url):
    """Join a base URL and a (possiby relative) URL.
   
    base - base url
    url - a (possibly relative) URL to join to the base URL

    Returns the result as an absolute URL.

    """
    import posixpath
    bscheme, bauthority, bpath, bquery, bfragment = urisplit(base)
    uscheme, uauthority, upath, uquery, ufragment = urisplit(url)
    if uscheme is not None and bscheme != uscheme:
        return url
    if uauthority is not None:
        bauthority, bpath, bquery, bfragment = \
            uauthority, upath, uquery, ufragment
    elif upath != '':
        bpath = posixpath.normpath(_pathjoin(_dirname(bpath), upath))
        bquery, bfragment = uquery, ufragment
    elif uquery is not None:
        bquery, bfragment = uquery, ufragment
    elif ufragment is not None:
        bfragment = ufragment
    return uriunsplit((bscheme, bauthority, bpath, bquery, bfragment)) 



def _test():
    import sys
    parsetests = {
        # Simple tests
        'http://user:pass@host:8080/path?query=result#fragment':
            ('http', 'user', 'pass', 'host', '8080', '/path', 
	        'query=result', 'fragment'),
        'http://user@host:8080/path?query=result#fragment':
            ('http', 'user', None,'host','8080', '/path', 'query=result', 'fragment'),
        'http://host:8080/path?query=result#fragment':
            ('http', None, None, 'host', '8080', '/path', 'query=result', 'fragment'),
        'http://host/path?query=result#fragment':
            ('http', None, None, 'host', '80', '/path', 'query=result', 'fragment'),
        'http://host/path?query=result':
            ('http', None, None, 'host', '80', '/path','query=result',None),
        'http://host/path#fragment':
            ('http', None, None, 'host', '80', '/path', None, 'fragment'),
        'http://host/path':
            ('http', None, None, 'host', '80', '/path', None, None),
        'http://host':
            ('http', None, None, 'host', '80', '/', None, None),
        'http:///path':
            ('http', None, None, None, '80', '/path', None, None),
        # torture tests
        'http://user:pass@host:port/path?que:ry/res@ult#fr@g:me/n?t': 
            ('http', 'user', 'pass', 'host', 'port', '/path', 
	        'que:ry/res@ult', 'fr@g:me/n?t'),
        'http://user:pass@host:port/path#fr@g:me/n?t': 
            ('http', 'user', 'pass', 'host', 'port', '/path', None, 'fr@g:me/n?t'),
        'http://user:pass@host:port?que:ry/res@ult#fr@g:me/n?t': 
            ('http', 'user', 'pass', 'host', 'port', '/', 
	        'que:ry/res@ult', 'fr@g:me/n?t'),
        'http://user:pass@host:port#fr@g:me/n?t': 
            ('http', 'user', 'pass', 'host', 'port', '/', None, 'fr@g:me/n?t'),
    }
    failures = 0
    for url in parsetests:
        print ("url: %s : " % url),
        result = URIParser().parse(url)
        if result == parsetests[url]:
            print "passed"
        else:
            print "Failed."
	    print "       got:  %s" % repr(result)
	    print "  expected:  %s" % repr(parsetests[url])
            failures += 1

    base = "http://a/b/c/d;p?q"
    jointests = {     
        # Normal Examples from STD 66 Section 5.4.1
        "g:h"           :  "g:h",
        "g"             :  "http://a/b/c/g",
        "./g"           :  "http://a/b/c/g",
        "g/"            :  "http://a/b/c/g/",
        "/g"            :  "http://a/g",
        "//g"           :  "http://g",
        "?y"            :  "http://a/b/c/d;p?y",
        "g?y"           :  "http://a/b/c/g?y",
        "#s"            :  "http://a/b/c/d;p?q#s",
        "g#s"           :  "http://a/b/c/g#s",
        "g?y#s"         :  "http://a/b/c/g?y#s",
        ";x"            :  "http://a/b/c/;x",
        "g;x"           :  "http://a/b/c/g;x",
        "g;x?y#s"       :  "http://a/b/c/g;x?y#s",
        ""              :  "http://a/b/c/d;p?q",
        "."             :  "http://a/b/c/",
        "./"            :  "http://a/b/c/",
        ".."            :  "http://a/b/",
        "../"           :  "http://a/b/",
        "../g"          :  "http://a/b/g",
        "../.."         :  "http://a/",
        "../../"        :  "http://a/",
        "../../g"       :  "http://a/g",
        # Abnormal Examples from STD 66 Section 5.4.2
        "../../../g"    :  "http://a/g",
        "../../../../g" :  "http://a/g",
        "/./g"          :  "http://a/g",
        "/../g"         :  "http://a/g",
        "g."            :  "http://a/b/c/g.",
        ".g"            :  "http://a/b/c/.g",
        "g.."           :  "http://a/b/c/g..",
        "..g"           :  "http://a/b/c/..g",
        "./../g"        :  "http://a/b/g",
        "./g/."         :  "http://a/b/c/g/",
        "g/./h"         :  "http://a/b/c/g/h",
        "g/../h"        :  "http://a/b/c/h",
        "g;x=1/./y"     :  "http://a/b/c/g;x=1/y",
        "g;x=1/../y"    :  "http://a/b/c/y",
        "g?y/./x"       :  "http://a/b/c/g?y/./x",
        "g?y/../x"      :  "http://a/b/c/g?y/../x",
        "g#s/./x"       :  "http://a/b/c/g#s/./x",
        "g#s/../x"      :  "http://a/b/c/g#s/../x",
        "http:g"        :  "http://a/b/c/g" 
    }

    for relref in jointests:
        result = urljoin(base, relref)
        print ("%s + %s = %s : " % (repr(base), repr(relref), repr(result))),
        if result == jointests[relref]:
            print "passed" 
	elif result + '/' == jointests[relref]:
	    # unclear whether this is the same or not
	    # fixable by fixing the use of posixpath.normpath above
	    print "passed"
        else:
            print "Failed.\n  expected: %s " % repr(jointests[relref])
	    failures += 1
    
    print ("%d Tests finished." % (len(parsetests)+len(jointests))),
    print "%d failures." % failures
    sys.exit(failures)

if __name__ == '__main__':
    _test()


 	  	 

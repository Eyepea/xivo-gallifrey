"""Supplementary functions useful to play with URI - very very close to RFC 3986

Copyright (C) 2007-2009  Proformatique

"""

__version__ = "$Revision$ $Date$"
__license__ = """
    Copyright (C) 2007-2009  Proformatique

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
"""

import re

# Right from RFC 3986 section B
RFC3986_MATCHER = re.compile(r"^(([^:/?#]+):)?(//([^/?#]*))?([^?#]*)(\?([^#]*))?(#(.*))?").match

# Near RFC 3986 definitions
ALPHA = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
DIGIT = "0123456789"
HEXDIG = "0123456789abcdefABCDEF"
SCHEME_CHAR = ALPHA + DIGIT + "+-."
GEN_DELIMS = ":/?#[]@"
SUB_DELIMS = "!$&'()*+,;="
RESERVED = GEN_DELIMS + SUB_DELIMS
UNRESERVED = ALPHA + DIGIT + "-._~"
PCHAR = UNRESERVED + SUB_DELIMS + "%:@/"
QUEFRAG_CHAR = PCHAR + "?"
USER_CHAR = UNRESERVED + SUB_DELIMS + "%"
PASSWD_CHAR = UNRESERVED + SUB_DELIMS + "%:"
REG_NAME_CHAR = UNRESERVED + SUB_DELIMS + "%"

IPV_FUTURE_RE = r"v[\da-fA-F]+\.[" \
                + re.escape(UNRESERVED + SUB_DELIMS + ":") \
                + "]+"

BYTES_VAL = ''.join(map(chr, range(0, 256)))

def __allow_to_encdct(charset):
    enc_charset = set(iter(charset.replace('%','')))
    return dict(((k in enc_charset) and (k, k) or (k, "%%%02X" % ord(k)) for k in BYTES_VAL))

USER_ENCDCT = __allow_to_encdct(USER_CHAR)
PASSWD_ENCDCT = __allow_to_encdct(PASSWD_CHAR)
REG_NAME_ENCDCT = __allow_to_encdct(REG_NAME_CHAR)
P_ENCDCT = __allow_to_encdct(PCHAR)
FRAG_ENCDCT = __allow_to_encdct(QUEFRAG_CHAR)
# QUERY_ENCDCT is reserved for query encoding, that is spaces are not percent
# encoded but translated into plus ' ' -> '+'
QUERY_KEY_ENCDCT = __allow_to_encdct(QUEFRAG_CHAR.translate(BYTES_VAL, "&+=") + " ")
QUERY_VAL_ENCDCT = __allow_to_encdct(QUEFRAG_CHAR.translate(BYTES_VAL, "&+") + " ")

# Host type alternatives:
HOST_IP_LITERAL = 1
HOST_IPV4_ADDRESS = 2
HOST_REG_NAME = 3

# Indexes in a parsed/splitted URI
SCHEME = 0
AUTHORITY = 1
PATH = 2
QUERY = 3
FRAGMENT = 4

AUTHORITY_USER = 0
AUTHORITY_PASSWD = 1
AUTHORITY_HOST = 2
AUTHORITY_PORT = 3

def __all_in(s, charset):
    return not s.translate(BYTES_VAL, charset)

def __split_sz(s, n):
    return [s[b:b+n] for b in range(0, len(s), n)]

def __valid_IPv4address(potential_ipv4):
    if potential_ipv4[0] in (HEXDIG + "xX") and __all_in(potential_ipv4[1:], (HEXDIG + ".xX")):
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
    sep_pos = potential_ipv6.find("::")
    sep_count = potential_ipv6.count("::")
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
    return bool(re.match(IPV_FUTURE_RE + "$", potential_ipvf))

def __valid_IPLiteral(potential_ipliteral):
    if len(potential_ipliteral) < 2 or potential_ipliteral[0] != '[' \
       or potential_ipliteral[-1] != ']':
        return False
    return __valid_IPv6address(potential_ipliteral[1:-1]) \
           or __valid_IPvFuture(potential_ipliteral[1:-1])

def __valid_query(pquery_tuple):
    for k, v in pquery_tuple:
        if (k and (not __all_in(k, QUEFRAG_CHAR))) \
           or (v and (not __all_in(v, QUEFRAG_CHAR))):
            return False
    return True

def valid_scheme(potential_scheme):
    """
    Check whether or not the content of potential_scheme is a valid
    URI scheme
    """
    return (potential_scheme[0] in ALPHA) \
           and __all_in(potential_scheme[1:], SCHEME_CHAR)

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
class InvalidIPv4addressError(InvalidHostError):
    """Invalid content for the IPv4address part of an URI"""
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

PERCENT_CODE_SUB = re.compile(r"\%[\da-fA-F][\da-fA-F]").sub

def pct_decode(s):
    """
    Return the percent-decoded version of string s.

    >>> pct_decode('%43%6F%75%63%6F%75%2C%20%6A%65%20%73%75%69%73%20%63%6F%6E%76%69%76%69%61%6C')
    'Coucou, je suis convivial'
    >>> pct_decode('')
    ''
    >>> pct_decode('%2525')
    '%25'
    """
    if s is None:
        return None
    return PERCENT_CODE_SUB(lambda mo: chr(int(mo.group(0)[1:], 16)), s)

def pct_encode(s, encdct):
    """
    Return a translated version of s where each character is mapped to a
    string thanks to the encdct dictionary.

    Use the encdct parameter to construct a string from parameter s where
    each character k from s is replaced by the value corresponding to key k
    in encdct.  It happens that callers use dictionaries smartly
    constructed so that this function will perform percent-encoding quickly
    when called whith such a dictionary.
    """
    if s is None:
        return None
    return ''.join(map(encdct.__getitem__, s))

def query_elt_decode(s):
    """
    Return the percent-decoded version of string s, after a plus-to-space
    substitution has been done.

    >>> query_elt_decode('query+++%2b')
    'query   +'
    """
    if s is None:
        return None
    return pct_decode(s.replace('+', ' '))

def query_elt_encode(s, encdct):
    """
    Query encode a string, using the encdct parameter to do character
    conversions.  '+' must be converted (percent encoded) by pct_encode()
    with the same s and encdct parameters, while ' ' must not be converted.
    """
    if s is None:
        return None
    return pct_encode(s, encdct).replace(' ', '+')

def host_type(host):
    """
    Correctly classify correct RFC 3986 compliant hostnames, but do not try
    hard to validate compliance anyway...
    NOTE: indeed we allow a small deviation from the RFC 3986: IPv4
    addresses are allowed to contain bytes represented in hexadecimal or
    octal notation when begining respectively with '0x'/'0X' and '0'
    numbers prepended with one or more zero won't be rejected.  Anyway
    representation of multiple bytes by a single decimal/octal/hexadecimal
    integer is not allowed.

    Return 1 (HOST_IP_LITERAL), 2 (HOST_IPV4_ADDRESS) or 3 (HOST_REG_NAME)

    >>> host_type('[blablabla]')
    1
    >>> host_type('')
    3
    >>> host_type('127.0.0.1')
    2
    >>> host_type('0x7F.0.0.00000000000001')
    2
    >>> host_type('666.42.131.2')
    3
    >>> host_type('foobar.42')
    3
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
    """
    Split authority into component parts.  This function supports
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
    InvalidIPLiteralError: Highly invalid IP-literal detected in URI authority 'user@[host]:port'
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
    different manner than the one used by most URI parsers.  As a result an
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
            m = re.match(r"\[([\da-fA-F:\.]+|" + IPV_FUTURE_RE
                                 + r")\](\:.*|)$", hostport)
            if m:
                host = '[' + m.group(1) + ']'
                port = m.group(2)[1:]
            else:
                raise InvalidIPLiteralError, "Highly invalid IP-literal detected in URI authority %r" % (authority,)
        elif ':' in hostport:
            host, port = hostport.split(':', 1)
        else:
            host, port = hostport, None
    else:
        host, port = None, None
    return (user or None, passwd or None, host or None, port or None)

def split_query(query):
    """
    Handle the query as a WWW HTTP 1630 query, as this is how people
    usually thinks of URI queries in general.  We do not decode anything
    in split operations, neither percent nor the terrible plus-to-space
    conversion.  Return:

    >>> split_query("k1=v1&k2=v+2%12&k3=&k4&&&k5==&=k&==")
    (('k1', 'v1'), ('k2', 'v+2%12'), ('k3', ''), ('k4', None), ('k5', '='), ('', 'k'), ('', '='))
    """
    def split_assignment(a):
        sa = a.split('=', 1)
        return len(sa) == 2 and tuple(sa) or (sa[0], None)
    assignments = query.split('&')
    return tuple([split_assignment(a) for a in assignments if a])

def unsplit_query(query):
    """
    Create a query string using the tuple query with a format as the one
    returned by split_query()
    """
    def unsplit_assignment((x, y)):
        if (x is not None) and (y is not None):
            return x + '=' + y
        elif x is not None:
            return x
        elif y is not None:
            return '=' + y
        else:
            return ''
    return '&'.join(map(unsplit_assignment, query))

def basic_urisplit(uri):
    """
    Basic URI Parser according to RFC 3986

    >>> basic_urisplit("scheme://authority/path?query#fragment")
    ('scheme', 'authority', '/path', 'query', 'fragment')
    """
    p = RFC3986_MATCHER(uri).groups()
    return (p[1], p[3], p[4], p[6], p[8])

def uri_split_tree(uri):
    """
    Return (scheme, (user, passwd, host, port), path,
               ((k1, v1), (k2, v2), ...), fragment) using
    basic_urisplit(), then split_authority() and split_query() on the
    result.

    >>> uri_split_tree(
    ...     'http://%42%20+blabla:lol@%77ww.foobar.org/%7Exilun/' +
    ...     '?query=+++%2b&=&===&a=b&&&+++aaa%3D=+%2B%2D&&&&' +
    ...     '#frag+++%42')
    ('http', ('%42%20+blabla', 'lol', '%77ww.foobar.org', None), '/%7Exilun/', (('query', '+++%2b'), ('', ''), ('', '=='), ('a', 'b'), ('+++aaa%3D', '+%2B%2D')), 'frag+++%42')
    """
    scheme, authority, path, query, fragment = basic_urisplit(uri)
    if authority:
        authority = split_authority(authority)
    if query:
        query = split_query(query)
    return (scheme and scheme or None, authority and authority or None,
            path and path or None, query and query or None,
            fragment and fragment or None)

def uri_tree_normalize(uri_tree):
    """
    Transform an URI tree so that adjacent all-empty fields are coalesced
    into a single None at parent level.
    The return value can be used for validation.
    As a result, no distinction is made between empty and absent fields.
    It is believed that this limitation is harmless because this is the
    behavior of most implementations, and even useful in the context of
    this Python module because empty strings are already not distinguished
    from None when converting to boolean, so we are only generalizing this
    concept in order to keep code small and minimize special cases.

    If the distinction is ever really needed, for example to support empty
    anchor special HTTP script related URI in a clean way, one will
    probably need to completely rewrite (or at least review and modify)
    this module, and special care would be needed to distinguish between '',
    (), None, and others everywhere implicit boolean conversion is
    performed.  The behavior should then be checked in regards to its
    conformance with RFC 3986, especially (but this would probably not be
    sufficient) the classification switches of some URI parts according to
    the content of others.
    """
    scheme, authority, path, query, fragment = uri_tree
    if authority and (filter(bool, authority) == ()):
        authority = None
    if query:
        query = filter(lambda (x, y): bool(x) or bool(y), query)
    return (scheme or None, authority or None, path or None,
            query or None, fragment or None)

def uri_tree_validate(uri_tree):
    """
    Validate a tree splitted URI in format returned by
    uri_tree_normalize(), raising an exception in case something invalid
    is detected - that is RFC 3986 is not respected - and returning the
    unmodified uri_tree otherwise.

    This function must be called on something similar to the return value
    of uri_tree_normalize() - and not uri_tree_decode() or directly
    uri_split_tree() - to have a meaningful action.

    The following deviations from RFC 3986 - and also design choice - are
    allowed and no exception will be raised in the following cases.

    - IPv4address can contain decimal / octal / hexadecimal representation
      of individual bytes.
    - In a similar way h16 in IPv6address can be zero-prepended.
    - No percent encoding validation is performed, so that non pct-encoded
      sequences begining with an '%' will be leaved untouched later by the
      percent decoding algorithm.
    - This function will not attempt to classify path as path-absolute
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
        if not valid_scheme(scheme):
            raise InvalidSchemeError, "Invalid scheme %r" % (scheme,)
    if authority:
        user, passwd, host, port = authority
        if user and not __all_in(user, USER_CHAR):
            raise InvalidUserError, "Invalid user %r" % (user,)
        if passwd and not __all_in(passwd, PASSWD_CHAR):
            raise InvalidPasswdError, "Invalid passwd %r" % (passwd,)
        if host:
            type_host = host_type(host)
            if type_host == HOST_REG_NAME:
                if not __all_in(host, REG_NAME_CHAR):
                    raise InvalidRegNameError, "Invalid reg-name %r" % (host,)
            elif type_host == HOST_IP_LITERAL:
                if not __valid_IPLiteral(host):
                    raise InvalidIPLiteralError, "Invalid IP-literal %r" % (host,)
        if port and not __all_in(port, DIGIT):
            raise InvalidPortError, "Invalid port %r" % (port,)
    if path:
        if not __all_in(path, PCHAR):
            raise InvalidPathError, "Invalid path %r - invalid character detected" % (path,)
        if authority and path[0] != '/':
            raise InvalidPathError, "Invalid path %r - non-absolute path can't be used with an authority" % (path,)
        if (not authority) and (not scheme) and (':' in path.split('/', 1)[0]):
            raise InvalidPathError, "Invalid path %r - path-noscheme can't have a ':' if no '/' before" % (path,)
    if query and (not __valid_query(query)):
        raise InvalidQueryError, "Invalid splitted query tuple %r" % (query,)
    if fragment and (not __all_in(fragment, QUEFRAG_CHAR)):
        raise InvalidFragmentError, "Invalid fragment %r" % (fragment,)
    return uri_tree

def uri_tree_decode(uri_tree):
    """
    Decode a tree splitted URI in format returned by uri_split_tree() or
    uri_tree_normalize(), the returned value keeping the same layout.

    user, passwd, path, fragment are percent decoded, and so is host if of
    type reg-name.

    >>> uri_tree_decode(
    ...     uri_split_tree(
    ...             'http://%42%20+blabla:lol@%77ww.foobar.org/%7Exilun/' +
    ...             '?query=+++%2b&=&===&a=b&&&+++aaa%3D=+%2B%2D&&&&' +
    ...             '#frag+++%42'))
    ('http', ('B +blabla', 'lol', 'www.foobar.org', None), '/~xilun/', (('query', '   +'), ('', ''), ('', '=='), ('a', 'b'), ('   aaa=', ' +-')), 'frag+++B')
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
        query = tuple([(query_elt_decode(x), query_elt_decode(y)) for (x, y) in query])
    return (scheme, authority, path, query, fragment)

def uri_help_split(uri):
    """
    Return uri_tree_decode(
                    uri_tree_validate(
                            uri_tree_normalize(
                                    uri_split_tree(uri))))
    """
    return uri_tree_decode(
                    uri_tree_validate(
                            uri_tree_normalize(
                                    uri_split_tree(uri))))

def uri_tree_precode_check(uri_tree, type_host = HOST_REG_NAME):
    """
    Call this function to validate a raw URI tree before trying to
    encode it.
    """
    scheme, authority, path, query, fragment = uri_tree # pylint: disable-msg=W0612
    if scheme:
        if not valid_scheme(scheme):
            raise InvalidSchemeError, "Invalid scheme %r" % (scheme,)
    if authority:
        user, passwd, host, port = authority # pylint: disable-msg=W0612
        if port and not __all_in(port, DIGIT):
            raise InvalidPortError, "Invalid port %r" % (port,)
        if type_host == HOST_IP_LITERAL:
            if host and (not __valid_IPLiteral(host)):
                raise InvalidIPLiteralError, "Invalid IP-literal %r" % (host,)
        elif type_host == HOST_IPV4_ADDRESS:
            if host and (not __valid_IPv4address(host)):
                raise InvalidIPv4addressError, "Invalid IPv4address %r" % (host,)
    if path:
        if authority and path[0] != '/':
            raise InvalidPathError, "Invalid path %r - non-absolute path can't be used with an authority" % (path,)
    return uri_tree

def uri_tree_encode(uri_tree, type_host = HOST_REG_NAME):
    """
    Percent/Query encode a raw URI tree.
    """
    scheme, authority, path, query, fragment = uri_tree
    if authority:
        user, passwd, host, port = authority
        if user:
            user = pct_encode(user, USER_ENCDCT)
        if passwd:
            passwd = pct_encode(passwd, PASSWD_ENCDCT)
        if host and type_host == HOST_REG_NAME:
            host = pct_encode(host, REG_NAME_ENCDCT)
        authority = (user, passwd, host, port)
    if path:
        path = pct_encode(path, P_ENCDCT)
        if (not authority) and (not scheme):
            # check for path-noscheme special case
            sppath = path.split('/', 1)
            if ':' in sppath[0]:
                sppath[0] = sppath[0].replace(':', '%3A')
                path = '/'.join(sppath)
    if query:
        query = tuple([(query_elt_encode(x, QUERY_KEY_ENCDCT),
                        query_elt_encode(y, QUERY_VAL_ENCDCT)) for (x, y) in query])
    if fragment:
        fragment = pct_encode(fragment, FRAG_ENCDCT)
    return (scheme, authority, path, query, fragment)

def uri_unsplit_tree(uri_tree):
    """
    Unsplit a coded URI tree, which must also be coalesced by
    uri_tree_normalize().
    """
    scheme, authority, path, query, fragment = uri_tree
    if authority:
        user, passwd, host, port = authority
        if user and passwd:
            userinfo = user + ':' + passwd
        elif user:
            userinfo = user
        elif passwd:
            userinfo = ':' + passwd
        else:
            userinfo = None
        if host and port:
            hostport = host + ':' + port
        elif host:
            hostport = host
        elif port:
            hostport = ':' + port
        else:
            hostport = None
        if userinfo and hostport:
            authority = userinfo + '@' + hostport
        elif userinfo:
            authority = userinfo + '@'
        elif hostport:
            authority = hostport
        else:
            authority = None
    if query:
        query = unsplit_query(query)
    uri = ''
    if scheme:
        uri += scheme + ':'
    if authority:
        uri += '//' + authority
    if path:
        if (not authority) and path[0:2] == '//':
            uri += '//'
        uri += path
    if query:
        uri += '?' + query
    if fragment:
        uri += '#' + fragment
    return uri

def uri_help_unsplit(uri_tree):
    """
    Return uri_unsplit_tree(
                   uri_tree_encode(
                           uri_tree_precode_check(
                                   uri_tree_normalize(uri_tree))))
    """
    return uri_unsplit_tree(
                   uri_tree_encode(
                           uri_tree_precode_check(
                                   uri_tree_normalize(uri_tree))))

def _test():
    import doctest
    doctest.testmod()

if __name__ == "__main__":
    _test()

#
# TODO: write more tests
#
# ex: ('http', ('xilun:/?#[]@!$&\'"()*+,;=-._~%:@/?% ', 'kikoolol/?#[]@!$&\'"()*+,;=-._~%:@/?% ', 'www./?#[]@!$&\'"()*+,;=-._~%:@/? %xivo.fr', '8080'), '/kikoololerie//?#[]@!$&\'"()*+,;=-. _~%:@/?%', (('k1/?#[]@!$&\'"()*+,;=-._~%:@/?% ', 'v1/?#[]@!$&\'"()*+,;=-._~%:@/?% '), ('k2/?#[]@!$&\'"()*+,;=-._~%:@/?% ', 'v2/?#[]@!$&\'"()*+,;=-._~%:@/?% ')), 'foobar2000/?#[]@!$&\'"()*+,;=-._~%:@/?% ')
#     -> "http://xilun%3A%2F%3F%23%5B%5D%40!$&'%22()*+,;=-._~%25%3A%40%2F%3F%25%20:kikoolol%2F%3F%23%5B%5D%40!$&'%22()*+,;=-._~%25:%40%2F%3F%25%20@www.%2F%3F%23%5B%5D%40!$&'%22()*+,;=-._~%25%3A%40%2F%3F%20%25xivo.fr:8080/kikoololerie//%3F%23%5B%5D@!$&'%22()*+,;=-.%20_~%25:@/%3F%25?k1/?%23%5B%5D@!$%26'%22()*%2B,;%3D-._~%25:@/?%25+=v1/?%23%5B%5D@!$%26'%22()*%2B,;=-._~%25:@/?%25+&k2/?%23%5B%5D@!$%26'%22()*%2B,;%3D-._~%25:@/?%25+=v2/?%23%5B%5D@!$%26'%22()*%2B,;=-._~%25:@/?%25+#foobar2000/?%23%5B%5D@!$&'%22()*+,;=-._~%25:@/?%25%20"
#     (None, None, ':blabla', None, None)
#     (None, None, '//foobar', None, None)
#     ('http', None, '//foobar', None, None)
# ...
#

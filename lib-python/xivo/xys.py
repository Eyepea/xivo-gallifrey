"""XIVO YAML Schema - v0.01

Copyright (C) 2008-2010  Proformatique

The basic idea behind XYS is to write a schema as much as possible as
you would write documents that are valid by this schema.

If a mapping is needed at any level in documents, you just have to put
one at the same place in the schema.  The keys in the schema are the
ones that are allowed in documents.  If a key in the schema is a string
that ends with a '?' then it is optional and when it appears in a
document it does not end with this final '?'.  A value in the schema
contains a sub-schema that will be used to validate each corresponding
value in documents.

This type equivalence principle stands valid for sequences and scalars.
If a sequence is needed in documents, you write, in the schema, a
sequence of only one element.  This element is a sub-schema that will be
used to validate each element of each corresponding sequence in
documents.  The type of a scalar in the schema is used to validate the
type of each corresponding scalar in documents.

Schema example:
---
peoples:
   - name: ''
     age?: 20
     sex?: ''
numbers?: [ 1 ]
car?:
   brand: ''
   horsepower?: 130
...

Document 1 - Valid:
---
peoples:
   - name: Xilun
   - name: Steven
     age: 42
     sex: M
numbers:
   - 1
   - 3
   - 42
car:
   brand: Ferrari
...

Document 2 - Valid:
---
peoples: []
...

Document 3 - Invalid:
---
numbers: [ 1 ]
...

Document 4 - Invalid:
---
peoples:
   - name: 10
...

A typical feature of schema languages is the capability to describe
usual subsets for scalars, for example it can be useful to declare that
some strings in documents must start with "fortytwo_".  This is done in
XYS using personalized YAML tags.  Standard XYS qualifiers are provided;
their tags starts with '!~~'.  An application can also define its own
qualifiers with tags starting with '!~' (but not '!~~').  Some
qualifying tags will have parameters, for example !~between(42,128) is a
tag which means that corresponding integers in documents must be between
42 and 128.  Because of the YAML grammar, these tags must be integers in
decimal representation or symbols of the form /[A-Za-z_][A-Za-z0-9_]*/
and there must be no space character between the opening parenthesis and
the closing one.  Qualifiers which never take parameters must be written
with no parenthesis.

Example of schema with qualifiers:
---
resolvConf:
   search?: !~search_domain bla.tld
   nameservers?: !~~seqlen(1,3) [ !~ipv4_address 192.168.0.200 ]
ipConfs:
   !~~prefixedDec static_:
      address: !~ipv4_address 192.168.0.100
      netmask: !~ipv4_address 255.255.255.0
      broadcast?: !~ipv4_address 192.168.0.255
      gateway?: !~ipv4_address 192.168.0.254
      mtu?: !~~between(68,1500) 1500
...

Application-specific validation functions:

import re

def ipv4_address(nstr, schema):
    elts = nstr.split('.', 4)
    if len(elts) != 4:
        return False
    for e in elts:
        try:
            i = int(e)
        except ValueError:
            return False
        if i < 0 or i > 255:
            return False
    return True

def search_domain(nstr, schema):
    domain_label_ok = \\
        re.compile(r'[a-zA-Z]([-a-zA-Z0-9]*[a-zA-Z0-9])?$').match
    return nstr and len(nstr) <= 251 and \\
           all((((len(label) <= 63)
                 and domain_label_ok(label))
                for label in nstr.split('.')))


Registration of above functions:

xys.add_validator(search_domain, u'!!str')
xys.add_validator(ipv4_address, u'!!str')


In this schema, most scalars are used just as an example of what can
appear in a valid document.  An exception is for !~~prefixedDec static_,
where static_ is used to check that in documents, keys start with
static_ (with a decimal in their right part).  As you can see in the
registration, qualifying XYS types derive from base YAML types.  The
base types are used for two purposes: during construction of the
internal representation of the schema, scalars are converted according
to the base type specification; and when validating a document, the type
of scalars it contains is checked against the specified base type prior
to the call to the validation function.

"""

__version__ = "$Revision$ $Date$"
__license__ = """
    Copyright (C) 2008-2010  Proformatique

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


from xivo import UpCollections # pylint: disable-msg=W0611

from collections import namedtuple
import copy
import yaml
import logging


log = logging.getLogger("xivo.xys") # pylint: disable-msg=C0103


# NOTE: content must stay first
ValidatorNode = namedtuple('ValidatorNode', 'content validator')
Optional = namedtuple('Optional', 'content')
Mandatory = namedtuple('Mandatory', 'content')


def _optmand_content(x):
    """
    When a dictionary key is optional/mandatory, its corresponding
    _value_ is decorated in an Optional/Mandatory tuple.
    This function undo the decoration when necessary.
    """
    if isinstance(x, Optional) or isinstance(x, Mandatory):
        return x.content
    else:
        return x


def _construct_node(loader, node, base_tag):
    "Warning: depends on python-yaml internals"
    node = copy.copy(node) # bypass YAML anti recursion
    best_tag = base_tag
    best_fit = 0
    for key, val in loader.DEFAULT_TAGS.iteritems():
        lenk = len(key)
        if lenk <= best_fit:
            continue
        if base_tag.startswith(key):
            best_tag = val + base_tag[lenk:]
            best_fit = lenk
    node.tag = best_tag
    return loader.construct_object(node, deep=True)


def _maybe_int(s):
    "Coerces to int if starts with a digit else return s"
    if s and s[0] in "0123456789":
        return int(s)
    else:
        return s


def _split_params(tag_prefix, tag_suffix):
    "Split comma-separated tag_suffix[:-1] and map with _maybe_int"
    if tag_suffix[-1:] != ')':
        raise ValueError, "unbalanced parenthesis in type %s%s" % (tag_prefix, tag_suffix)
    return map(_maybe_int, tag_suffix[:-1].split(','))


def add_validator(validator, base_tag, tag=None):
    """
    Add a validator for the given tag, which defines a subset of base_tag.
    If tag is None, it is automatically constructed as
    u'!~' + validator.__name__
    Validator is a function that accepts a document node (in the form of a
    Python object), a schema node (also a Python object) and a tracing
    object, and returns True if the document node is valid according to the
    schema node.  Note that the validator function does not have to recurse
    in sub-nodes, because XYS already does it.
    """
    if not tag:
        tag = u'!~' + validator.__name__
    yaml.add_constructor(
        tag,
        lambda loader, node:
            ValidatorNode(
                _construct_node(loader, node, base_tag),
                validator))


def add_parameterized_validator(param_validator, base_tag, tag_prefix=None):
    """
    Add a parameterized validator for the given tag prefix.
    If tag_prefix is None, it is automatically constructed as
    u'!~%s(' % param_validator.__name__
    A parametrized validator is a function that accepts a document node
    (in the form of a Python object), a schema node (also a Python
    object), and other parameters (integer or string) that directly come
    from its complete YAML name in the schema.  It returns True if the
    document node is valid according to the schema node.  Note that the
    validator function does not have to recurse in sub-nodes, because
    XYS already does that.
    """
    # pylint: disable-msg=C0111,W0621
    if not tag_prefix:
        tag_prefix = u'!~%s(' % param_validator.__name__
    def multi_constructor(loader, tag_suffix, node):
        def temp_validator(node, schema):
            return param_validator(node, schema, *_split_params(tag_prefix, tag_suffix))
        temp_validator.__name__ = str(tag_prefix + tag_suffix)
        return ValidatorNode(_construct_node(loader, node, base_tag), temp_validator)
    yaml.add_multi_constructor(tag_prefix, multi_constructor)


def _add_validator_internal(validator, base_tag):
    "with builtin tag prefixing"
    add_validator(validator, base_tag, tag = u'!~~' + validator.__name__)


def _add_parameterized_validator_internal(param_validator, base_tag):
    "with builtin tag prefixing"
    add_parameterized_validator(param_validator, base_tag, tag_prefix=u'!~~%s(' % param_validator.__name__)


def enum(nstr, schema, *symbols): # pylint: disable-msg=W0613
    """
    !~~enum(symb1[,symb2[,...]])
        corresponding strings in documents must be in the set of
        given symbols.
    """
    return nstr in symbols


def seqlen(lst, schema, min_len, max_len): # pylint: disable-msg=W0613
    """
    !~~seqlen(min,max)
        corresponding sequences in documents must have a length between
        min and max, included.
    """
    return min_len <= len(lst) <= max_len


def between(val, schema, min_val, max_val): # pylint: disable-msg=W0613
    """
    !~~between(min,max)
        corresponding integers in documents must be between min and max,
        included.
    """
    return min_val <= val <= max_val


def fixed(nstr, schema):
    """
    !~~fixedStr
    !~~fixedInt
        nst == schema
    """
    return nstr == schema


def startswith(nstr, schema):
    """
    !~~startswith
        corresponding strings in documents must begin with the
        associated string in the schema.
    """
    return nstr.startswith(schema)


def prefixedDec(nstr, schema):
    """
    !~~prefixedDec
        corresponding strings in documents must begin with the
        associated string in the schema, and the right part of strings
        in documents must be decimal.
    """
    if not nstr.startswith(schema):
        return False
    postfix = nstr[len(schema):]
    try:
        int(postfix)
    except ValueError:
        return False
    return True


_add_parameterized_validator_internal(seqlen, u'!!seq')
_add_parameterized_validator_internal(between, u'!!int')
_add_parameterized_validator_internal(enum, u'!!str')
add_validator(fixed, u'!!str', '!~~fixedStr')
add_validator(fixed, u'!!int', '!~~fixedInt') # XXX: validation tag overloading?
_add_validator_internal(startswith, u'!!str')
_add_validator_internal(prefixedDec, u'!!str')


def _qualify_map(key, content):
    "see _optmand_content() pydoc"
    if isinstance(key, basestring):
        if key[-1:] == '?':
            return key[:-1], Optional(content)
        else:
            return key, Mandatory(content)
    else:
        return key, content


def _transschema(x):
    """
    Transform a schema, once loaded from its YAML representation, to its
    final internal representation
    """
    if isinstance(x, tuple):
        return x.__class__(_transschema(x[0]), *x[1:])
    elif isinstance(x, dict):
        return dict((_qualify_map(key, _transschema(val)) for key, val in x.iteritems()))
    elif isinstance(x, list):
        return map(_transschema, x)
    else:
        return x


def load(src):
    """
    Parse the first XYS schema in a stream and produce the corresponding
    internal representation.
    """
    return _transschema(yaml.load(src))


Nothing = object()


# TODO: display the document path to errors, and other error message enhancements
# TODO: allow error messages from validators

def validate(document, schema):
    """
    If the document is valid according to the schema, this function returns
    True.
    If the document is not valid according to the schema, errors are logged
    then False is returned.
    """
    if isinstance(schema, ValidatorNode):
        if not validate(document, schema.content):
            return False
        if not schema.validator(document, schema.content):
            log.error("%r failed to validate with qualifier %s", document, schema.validator.__name__)
            return False
        return True
    elif isinstance(schema, dict):
        if not isinstance(document, dict):
            log.error("wanted a dictionary, got a %s", document.__class__.__name__)
            return False
        generic = []
        optional = {}
        mandatory = []
        for key, schema_val in schema.iteritems():
            schema_val_content = _optmand_content(schema_val)
            if isinstance(key, ValidatorNode):
                generic.append((key, schema_val_content))
            elif isinstance(schema_val, Optional):
                optional[key] = schema_val_content
            else:
                mandatory.append((key, schema_val_content))
        doc_copy = document.copy()
        for key, schema_val in mandatory:
            doc_val = doc_copy.get(key, Nothing)
            if doc_val is Nothing:
                log.error("missing key %r in document", key)
                return False
            if not validate(doc_val, schema_val):
                return False
            del doc_copy[key]
        for key, doc_val in doc_copy.iteritems():
            schema_val = optional.get(key, Nothing)
            if schema_val is Nothing:
                for gen_key, schema_val in generic:
                    if validate(key, gen_key):
                        break
                else:
                    log.error("forbidden key %s in document", key)
                    return False
            if not validate(doc_val, schema_val):
                return False
        return True
    elif isinstance(schema, list):
        if not isinstance(document, list):
            log.error("wanted a list, got a %s", document.__class__.__name__)
        for elt in document:
            # XXX: give a meaning when there are multiple element in a sequence of a schema?
            if not validate(elt, schema[0]):
                return False
        return True
    else: # scalar
        if isinstance(schema, str):
            schema = unicode(schema)
        if isinstance(document, str):
            document = unicode(document)
        if schema.__class__ != document.__class__:
            log.error("wanted a %s, got a %s", schema.__class__.__name__, document.__class__.__name__)
            return False
        return True


__all__ = [
        'validate', 'load',
        'seqlen', 'between', 'startswith', 'prefixedDec',
        'add_validator', 'add_parameterized_validator',
        'ValidatorNode', 'Optional', 'Mandatory',
]


# IDEAS:
# 04:05 < obk> xilun: You use '?' for optional... do you use '*' and '?' for zero-or-more and one-or-more (in sequences)?
# 04:05 < obk> '*' and '+' I meant
# 04:12 < xilun> im not sure where i could put the tag
# 04:13 < xilun> perhaps an abbreviated form of seqlen
# 04:13 < xilun> which need to be extended so it supports lengths < and lengths > too, not just ranges
# 04:22 < obk> xilun: Hmm... good point
# 04:24 < obk> Of course you could put it in the tag (!*, !+, !?) - that would preclude specifying tags however...
# 04:24 < obk> Actually - you could postfix the original tag
# 04:24 < obk> E.g.:
# 04:24 < obk> ---
# 04:24 < obk> !?!!str foo:
# 04:25 < obk> - !*!!int 7
# 04:25 < obk> ...
# 04:25 < obk> Means 'foo' is optional and a string, contains zero-or-more integers
# 04:25 < obk> And of course you'd omit the '!!int', '!!str' etc. 99.9999% of the time
# 04:25 < obk> ---
# 04:25 < obk> !? foo:
# 04:25 < obk> - !* 7
#
# NOTE: i'll rather write it like (really? not sure about that)
# ---
# !? foo: !*
#   - 1
# ...
#
# !~~seqlen(3,5) should probably be written like: ![3,5]
# and it should be possible to do stuff like
# ![,5] <=> ![0,5]
# ![3,] <=> ![3,infinity]
# ![3] <=> exactly 3...
#
# NOTE: !{3,5} won't work because of YAML (grammar|parser)
#
# So here are the basic (and classic...) equivalences:
# !* <=> ![,]
# !+ <=> ![1,]
# !? <=> ![0,1]
#
#
# IDEA: If, in a schema, a sequence is not qualified, the
# corresponding part of the document must be a matching sequence
# in which the
#
# schema ex:
# ---
# foo:
#   - kikoo: 1
#   - lol: 2
# ...
#
# valid document:
# ---
# foo:
#   - kikoo: 42
#   - lol: 666
# ...
#
# invalid document:
# ---
# foo:
#   - kikoo: 42
# ...
#
# problem if adopted: how to represent optional key in an ordered map?
# we could try:
# ---
# - mandatorykey: bla
# - !? optionalkey: foo
# ...
#
# But it also seems to mean that the list must have two elements and the second
# can either be a singleton dictionary or an empty one? (or a null entry maybe?)
# A simple solution is to use an other qualifying tag. ex:
# ---
# - mandatorykey: bla
# - !$ optionalkey: foo
# ...
# possible tags
# !$    - bad because $ means end of something in regexp syntax)
# !&    - why not
# !&?   - could be derived in !&* and !&+ but this starts to be complicated)
# !#    - not visually attractive
# !?seq - a little long, also should be !?element but even longer and !?elt is
#         not derived from YAML basic type.
# !'    - good because visually small, so you can use the mnemotechnic help
#         "really small, can disappear" :p
#
# for now i prefer !&? (maybe along with derivatives) or !'
#
#
# TODO: support (simple) notion of uniqueness
#
# 16:10 < xilun> i think 'ill try to add automatic typing of documents according to the schema too

"""Simple Xml to Python Object Tree Adaptor

Copyright (C) 2007, Proformatique

This module let you do that (amongst other cool things):
XXX: this would be even greater with a schema :)

$ cat > test.xml
<?xml version="1.0" ?><root t="c">
        <scal>42</scal>
        <convilist t="l">
                <li>blabla</li>
                <li>foobar</li>
                <li></li>
                <li t="c">
                        <kikoo>42</kikoo>
                        <lol></lol>
                        <mdr t="c"/>
                </li>
        </convilist>
        <othercont t="c"/>
</root>
$ python
>>> from PinstXml import *
>>> test,root_name = pinst_xml_parse('test.xml')
>>> test.scal
u'42'
>>> test.convilist[3].kikoo
u'42'
>>> test.convilist[3].lol  
''
>>> test.convilist[0]    
u'blabla'
>>> for n,v in test._iteritems():
...     print n,v
... 
scal 42
convilist [u'blabla', u'foobar', '', AttrDict(<CtrlMap of {u'kikoo': u'42', u'lol': '', u'mdr': AttrDict(<CtrlMap of {}>)}>)]
othercont AttrDict(<CtrlMap of {}>)
>>> test.convilist[3].kikoo = "666 is the number of the beast"
>>> print xml_pinst(test,root_name)
<?xml version="1.0" ?><root t="c">
        <scal>42</scal>
        <convilist t="l">
                <li>blabla</li>
                <li>foobar</li>
                <li></li>
                <li t="c">
                        <kikoo>666 is the number of the beast</kikoo>
                        <lol></lol>
                        <mdr t="c"/>
                </li>
        </convilist>
        <othercont t="c"/>
</root>

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

from xml.dom.minidom import parse, getDOMImplementation
from itertools import *
from AttrDict import *
from CtrlMap import *
from OrdDict import *
from pyfunc import *

DOM_Imple = getDOMImplementation()

ELT_SCALAR = 0
ELT_CONTAINER = 1
ELT_LIST = 2

# XXX use a kind of Schema instead of in-band attributes to switch
# between scalars, lists and containers, and also introduce scalar types.

def pinst_dom(element):
	elt_type = ELT_SCALAR
	i = 0
	while i < element.attributes.length:
		attr_item = element.attributes.item(i)
		if attr_item.name == 't':
			if attr_item.value == 'c':
				elt_type = ELT_CONTAINER
			elif attr_item.value == 'l':
				elt_type = ELT_LIST
			elif attr_item.value == 's':
				elt_type = ELT_SCALAR
			else:
				raise ValueError, "When provided, attribute t of an XML element must have value 'c', 'l' or 's'"
			break
		i+=1
	if elt_type == ELT_SCALAR:
		if not element.childNodes:
			return '', element.nodeName
		text_item = find(lambda e: e.nodeType == e.TEXT_NODE, element.childNodes)
		if text_item is None:
			raise ValueError, "Scalar element provided but it has no text child"
		return text_item.nodeValue, element.nodeName
	elif elt_type == ELT_CONTAINER:
		x = AttrDict(CtrlMap(ordDict(), allow_insert = True,
		                     allow_modify = False, allow_delete = True,
				     fuse_underl = True, fuse_allow = False))
		for el in ifilter(lambda e: e.nodeType == e.ELEMENT_NODE, element.childNodes):
			x[el.nodeName] = pinst_dom(el)[0]
		x._allow_modify = True
		return x, element.nodeName
	elif elt_type == ELT_LIST:
		return ([pinst_dom(e)[0] for e in element.childNodes
		         if e.nodeType == e.ELEMENT_NODE and e.nodeName == 'li'],
			element.nodeName)

def set_attr_type(doc, nel, elt_type):
	if elt_type == ELT_LIST:
		nel.setAttributeNode(doc.createAttribute(u't'))
		nel.setAttribute(u't', u'l')
	elif elt_type == ELT_CONTAINER:
		nel.setAttributeNode(doc.createAttribute(u't'))
		nel.setAttribute(u't', u'c')

def fill_one(doc, elt, subs, stype, indent, level):
	set_attr_type(doc, elt, stype)
	if len(subs) < 1 or (len(subs) == 1 and subs[0].nodeType == doc.TEXT_NODE):
		for payl in subs:
			elt.appendChild(payl)
	else:
		for payl in subs:
			elt.appendChild(doc.createTextNode("\n"+indent*(level+1)))
			elt.appendChild(payl)
		elt.appendChild(doc.createTextNode("\n"+indent*level))

def elem_pinst_rec(doc, x, indent, level):
	def create_one(el_name, el_cont):
		subs,stype = elem_pinst_rec(doc, el_cont, indent, level+1)
		nel = doc.createElement(el_name)
		fill_one(doc, nel, subs, stype, indent, level)
		return nel
	if isinstance(x, AttrDict):
		return [create_one(el_name, el_cont)
		        for (el_name,el_cont) in x._iteritems()], ELT_CONTAINER
	elif isinstance(x, list):
		return [create_one(u'li', px) for px in x], ELT_LIST
	else:
		return [doc.createTextNode(str(x))], ELT_SCALAR

def xml_pinst(x, root_name, indent='\t'):
	doc = DOM_Imple.createDocument(None, root_name, None)
	subs,stype = elem_pinst_rec(doc, x, indent, 1)
	fill_one(doc, doc.documentElement, subs, stype, indent, 0)
	return doc.toxml()

def pinst_xml_parse(filename_or_file, parser=None, bufsize=None):
	dom = parse(filename_or_file, parser, bufsize)
	return pinst_dom(dom.documentElement)

__all__ = ['xml_pinst', 'pinst_xml_parse']

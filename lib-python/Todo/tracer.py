"""This module makes use of the inspect module and the sys.settrace() function,
and allow precise tracing of calls and returns of functions with no modification
of the instrumented source code.

Use of this feature is very easy; one just need to write from the point he wants
the program to be traced, for example:

	import tracer
	tracer.enable_tofp(open(os.path.expanduser("~/traces"), 'w'))

With the code above, a file named "traces" will be created in your home
directory (or its content wiped if it already existed) and the function call
traces of the current execution will be sent there.

enable_tofp() takes two arguments:
-fp: file pointer (like open(filename, 'w') or even sys.stderr if you want)
-returns: optional, default to 1 -
	  traces of function returns with their return value will be written if
	  and only if 'returns' evaluates to True.

Some functions of inspect.py have been copied and corrected in this module (they
will eventually be bug reported and fixed upstream if we have some time to
report the issues)

Precisely, formatargvalues() from inspect fails to format arguments of functions
which prototypes include one or multiple tuple pattern matching, for example of
the form:

	def function1((a,b)):
		...
or:

	def function2(a, (b,c), ((d,),)):
		...

It has been found, by trial and error, that when such parameter exists in a
function definition, its constituents are not directly referenced by their names
in the local dictionary as returned in the fourth value by
inspect.getargvalues(), but that instead the local dictionary contains an entry
'.X' for each top level tuple argument to be pattern matched, where X is a
decimal number and its value is 2 times the argument number, starting at 0 from
the left to the right. For example, in the function2() prototype given above,
the value of (b,c) is stored in inspect.getargvalues()[3][".2"] and the value of
((d,),) in ".4".
Both formatargvalues(), strseq() and the nested closure function convert()
defined inside formatargvalues() have consequently been adapted to process
function parameters in the way they are stored as described above. The interface
of strseq() has changed, therefore it has been renamed strseqval().

Finally safe_repr() is introduced so that we can get, while tracing a call to an
__init__() constructor, a representation of an instance which class implements a
personalized __repr__() directly or via __getattr__(), which needs some members
that are not yet available when the __init__() trace is to be emitted.

XXX: the TraceToFile class should be documented, cleaned up, and could be
enhanced.

"""

__version__ = "$Revision$ $Date$"
__license__ = """
    Copyright (c) 2001, 2002, 2003, 2004 Python Software Foundation;
    Copyright (C) 2007, Proformatique
                                        All Rights Reserved

    Under PSF LICENSE AGREEMENT FOR PYTHON
    See the following URI for the full license:
        http://www.python.org/download/releases/2.4.4/license/
"""

import sys, linecache, time
import inspect, types

def safe_repr(o, fname):
	"""This function lets a representation of an object be safely taken when
	tracing a code containing instantiations of classes where __getattr__()
	is defined but not __repr__() and the implementation of __getattr__()
	contains access to instance member set in the body of __init__().

	Because the trace function is called at the start of the __init__()
	method of the target, before anything of its body really got executed,
	trying to repr() such an instance will result in __getattr__() being
	called, which would eventually lead to an infinite loop if __getattr__()
	tries to use a member that is not already defined (because no instance
	member is defined before __init__ is executed).

	- o: the object you want a repr() of
	- fname: the function name so that __getattr__() infinite loop
	  workaround is tried only when tracing __init__(). """
	if fname != '__init__':
		return '=' + repr(o)
	try:
		cls = o.__class__
		clsdir = dir(cls)
	except AttributeError:
		return '=' + repr(o)
	if ('__repr__' not in clsdir) and ('__getattr__' not in clsdir):
		try:
			return '=' + repr(o)
		except:
			pass
	addr = object.__repr__(o).split()[-1][:-1]
	clsname = repr(cls).split()[1]
	return "=<%s instance at %s>" % (clsname, addr)

# from inspect.py
def joinseq(seq):
	if len(seq) == 1:
		return '(' + seq[0] + ',)'
	else:
		return '(' + ", ".join(seq) + ')'

# from inspect.py (strseq())
def strseqval(obj, val, convert, join=joinseq):
	"""Recursively walk a sequence, stringifying each element."""
	if type(obj) in [types.ListType, types.TupleType]:
		return join([strseqval(subobj, subval, convert, join)
		             for (subobj,subval) in zip(obj,val)])
	else:
		return convert(obj, val)

# formatargvalues() stolen from inspect.py and fixed:
def formatargvalues(args, varargs, varkw, lcals,
                    formatarg=str,
                    formatvarargs=lambda name: '*' + name,
                    formatvarkw=lambda name: '**' + name,
                    formatvalue=lambda value: '=' + repr(value),
                    join=joinseq):
	"""Format an argument spec from the 4 values returned by getargvalues.

	The first four arguments are (args, varargs, varkw, lcals).  The
	next four arguments are the corresponding optional formatting functions
	that are called to turn names and values into strings.  The ninth
	argument is an optional function to format the sequence of arguments."""
	def convert(name, val,
        	    formatarg=formatarg, formatvalue=formatvalue):
		return formatarg(name) + formatvalue(val)
	values = []
	for p,onearg in enumerate(args):
		if type(onearg) in [types.ListType, types.TupleType]:
			topval = lcals['.' + str(p*2)]
		else:
			topval = lcals[onearg]
		values.append(strseqval(onearg, topval, convert, join))
	if varargs:
		values.append(formatvarargs(varargs) + formatvalue(lcals[varargs]))
	if varkw:
		values.append(formatvarkw(varkw) + formatvalue(lcals[varkw]))
	return '(' + ", ".join(values) + ')'

#def foobar2000(a,(b,),(c,),(d,)):
#	pass
#
#foobar2000(1,(2,),(3,),(4,))
#
#def foobar((a,(b,),c,(d,(e,((f,),))),g),(h,i)):
#	pass
#
#foobar((0,(1,),2,(3,(4,((5,),))),6),(7,8))
#
#
#def foobar3000(a,(c,),(d,),(e,),(f,),(g,),b,(h,),(i,),(j,),(k,),(l,),((m,),)):
#	pass
#
#foobar3000(1,(3,),(4,),(5,),(6,),(7,),42,(8,),(9,),(10,),(11,),(12,),((13,),))
#
# Fixed implementation of formatargvalues() should be able to trace function
# calls above :)
#
# XXX TODO: Write real non regression tests

class TraceToFile:

	def __init__(self, fp, returns = 1):
		"""Parameters:

		- fp: file pointer traces will be written to
		- returns: dump return values in traces
		"""
		self.fp = fp
		self.returns = returns
		self.call_level = 0

	def traceit(self, frame, event, arg):

		modname = frame.f_globals["__name__"]
		lineno = frame.f_lineno

		# if name != '__main__':
		#	return self.traceit

		if event == 'call':
			args, varargs, varkw, lcals = inspect.getargvalues(frame)
			funcname = frame.f_code.co_name
			print >> self.fp, "%.03f %24s %05d : %s%s%s" % (
				time.time(),
				modname,
				lineno,
				'\t' * self.call_level,
				funcname,
				formatargvalues(
					args, varargs, varkw, lcals,
					formatvalue = lambda x: safe_repr(
					                          x, funcname)),
			)
			self.call_level += 1

		elif event == 'return':
			self.call_level -= 1
			if self.returns:
				print >> self.fp, "%.03f %24s %05d : %s... %s() returns %s" % (
					time.time(),
					modname,
					lineno,
					'\t' * self.call_level,
					frame.f_code.co_name,
					repr(arg)
				)

		elif 0 and event == 'line':
			lineno = frame.f_lineno
			filename = frame.f_globals["__file__"]
			if (filename.endswith(".pyc")
			    or filename.endswith(".pyo")):
				filename = filename[:-1]
			name = frame.f_globals["__name__"]
			# XXX 
			if name != '__main__':
				return self.traceit
			line = linecache.getline(filename, lineno)
			print >> self.fp, "%.03f %s:%s: %s" % (time.time(), name, lineno, line.rstrip())

		return self.traceit

def enable_tofp(fp, returns = 1):
	"""Enable function call traces and send them to the file pointer
	referenced by 'fp'. Can optionally trace function returns and the values
	returned.
	
	This function takes two arguments:
	-fp: file pointer (like open(filename, 'w') or sys.stderr)
	-returns: optional, default to 1 -
	          traces function returns with their return values if and only
	          if 'returns' evaluates to True.
	
	It is not possible to stop the tracing once it has been started
	(XXX: this feature could be easily added if needed)
	"""
	sys.settrace(TraceToFile(fp, returns).traceit)

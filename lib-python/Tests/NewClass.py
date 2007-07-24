#!/usr/bin/python

def trace(cls_name):
	def in_trace(f):
		def func(self, *params):
			print "enter %s" %cls_name
			f(self, *params)
			print "leaving %s" %cls_name
		return func
	return in_trace

class AIAOO(object):
	@trace("AIAOO")
	def __init__(self):
		super(AIAOO, self).__init__()

class BIBOO(AIAOO):
	@trace("BIBOO")
	def __init__(self):
		super(BIBOO, self).__init__()

class CICOO(AIAOO):
	@trace("CICOO")
	def __init__(self):
		super(CICOO, self).__init__()

class GIGOO(object):
	@trace("GIGOO")
	def __init__(self):
		super(GIGOO, self).__init__()

class EIEOO(GIGOO):
	@trace("EIEOO")
	def __init__(self):
		super(EIEOO, self).__init__()

class FIFOO(object):
	@trace("FIFOO")
	def __init__(self):
		super(FIFOO, self).__init__()

class DIDOO(EIEOO, BIBOO, CICOO, FIFOO):
	@trace("DIDOO")
	def __init__(self):
		super(DIDOO, self).__init__()

class filt(object):
	@trace("filt")
	def __init__(self):
		super(filt, self).__init__()

class deriv(filt, DIDOO):
	@trace("deriv")
	def __init__(self):
		super(deriv, self).__init__()



class to_decorate(dict):
	@trace("to_decorate")
	def __init__(self, x = []):
		super(to_decorate, self).__init__(x)

class decorating(object):
	@trace("decorating")
	def __init__(self, *E, **F):
		super(decorating, self).__init__(*E, **F)
	def __getitem__(self, k):
		print "KIKOO entering overrided __getitem__", k
		v = super(decorating, self).__getitem__(k)
		print "KIKOO entering overrided __getitem__", k , v
		return v
	def __setitem__(self, k, v):
		raise ValueError, "AHAHAHA, c'est disable"
	def __delitem__(self, k):
		raise ValueError, "AHAHAHA, t'as pas le droit de delete"

class deco(decorating, to_decorate):
	pass


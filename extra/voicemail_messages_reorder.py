#!/usr/bin/env python
# -*- coding: utf8 -*-
import sys, os, os.path, glob, tempfile

"""
	Asterisk voicemails tree structure:
		context/
			voicemailid/
				Inbox/
				Old/
				Plop/
					msg0000.txt
					msg0000.wav
					msg0000.alaw
					...
"""

def corrector(basedir):
	vmdir = os.path.join(basedir, 'var/spool/asterisk/voicemail')
	if not os.path.exists(vmdir):
		return False

	tmp = tempfile.mkdtemp()

	for context in os.listdir(vmdir):
		if not os.path.isdir(os.path.join(vmdir, context)):
			continue

		for vmid in os.listdir(os.path.join(vmdir, context)):
				if not os.path.isdir(os.path.join(vmdir, context, vmid)):
					continue
				
				print "-| %s (%s) |-" % (vmid, context)
				for root, dirs, files in os.walk(os.path.join(vmdir, context, vmid)):
					# exclude vm root dir as it can contains sound files
					if root == os.path.join(vmdir, context, vmid):
						continue

					fixit(root, files, tmp)

def fixit(basedir, files, tmp):
	cnt = 0

	# sort by creation date
	# old message nº → new message nº (based on .txt file)
	order = map(lambda f: os.path.splitext(f)[0], sorted(filter(lambda f: os.path.splitext(f)[1] == '.txt', files), key=lambda f: os.stat(os.path.join(basedir, f)).st_mtime))
	for f in files:
		(base, ext) = os.path.splitext(f)
		try:
			i = order.index(base)
			to = "msg%04d%s" % (i, ext)

			if to != f:
				os.rename(os.path.join(basedir, f), os.path.join(tmp, to)); cnt += 1

		except ValueError:
			print "ERROR: %s not found in reorder map" % f

	for root, dirs, files in os.walk(tmp):
		for f in files:
			os.rename(os.path.join(tmp, f), os.path.join(basedir, f))

	print "  . [%s] %d files renamed" % (os.path.split(basedir)[1], cnt)


if __name__ == '__main__':
	basedir = '/'
	if len(sys.argv) > 1:
		basedir = sys.argv[1]

	corrector(basedir)

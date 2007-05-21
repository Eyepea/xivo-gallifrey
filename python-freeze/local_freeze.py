#! /usr/bin/env python
# -*- coding: iso-8859-15 -*-

# TODO: handle multi-modules python programs

import os
import os.path
import string
import sys

def excludable(module_name):
	if module_name in excludables:
		return False
	if module_name[0:len('encodings')] == 'encodings':
		return False
	return True

TMP_DIR = "TMP_LOCAL_FREEZE_PYTHON"
excludables = ["__main__", 're', 'sre', 'sre_compile', 'os', 'encodings', 'encodings.aliases', 'codecs']
excludables.append("threading")

noupx = False

#if len(sys.argv) < 3 or len(sys.argv[2]) <= 3 or string.find(sys.argv[2], ".py") != len(sys.argv[2]) - 3:
if len(sys.argv) < 3 or len(sys.argv[2]) <= 3:
	print >> sys.stderr, "Syntax: <path>local_freeze.py /path/to/freeze.py /alt/path/to/script/to/build.py [noupx]\n"
	sys.exit(2)

if len(sys.argv) >= 4 and sys.argv[3].lower() == "noupx":
	noupx = True

freeze_script = os.path.abspath(sys.argv[1])

scripts_to_build = sys.argv[2].split(",")
nscripts = len(scripts_to_build)

script_pathname = os.path.abspath(scripts_to_build[nscripts-1])
script_path = os.path.dirname(script_pathname)
script_name = os.path.basename(script_pathname)

executable_pathname = os.path.abspath(scripts_to_build[nscripts-1][0:len(scripts_to_build[nscripts-1]) - 3])
executable_path = os.path.dirname(executable_pathname)
executable_name = os.path.basename(executable_pathname)


fullline = "cp -r " + script_pathname
for extramod in scripts_to_build[:nscripts-1]:
	fullline = fullline + " " + script_path + "/" + extramod
	if extramod.find(".py") > 0:
		excludables.append(extramod[0:len(extramod[nscripts-1]) - 3])
	else:
		excludables.append(extramod)
fullline = fullline + " " + TMP_DIR

os.mkdir(TMP_DIR)
os.system(fullline)


for extramod in scripts_to_build[:nscripts-1]:
	if extramod.find(".py") < 0:
		fullline = "cp " + script_path + "/" + extramod + "__init__.py" + " " + TMP_DIR + "/" + extramod + "/__init__.py"
		print "####", fullline
		os.system(fullline)
#		os.system("rm " + TMP_DIR + "/" + extramod + "/*.pyc")

# sys.exit(5)

os.chdir(TMP_DIR)


fullline = freeze_script + " " + script_name
for extramod in scripts_to_build[:nscripts-1]:
	fullline = fullline + " " + extramod
fullline = fullline + " | grep '^freezing' | cut -d ' ' -f 2-2"

f = os.popen(fullline, 'r')
exclude_list = [ module.strip() for module in f if excludable(module.strip()) ]
f.close()

os.system("rm *.c Makefile")

command = freeze_script + " "
for module in exclude_list:
	command = command + "-x " + module + " "
command = command + script_name

os.system(command)

os.system("sed -i -e 's:libpython$(VERSION)\.a:libpython$(VERSION).so:' Makefile")
os.system("make")
os.system("strip " + executable_name)
if noupx == False:
	os.system("upx --best " + executable_name)
os.system("rm *.c *.o Makefile")
os.chdir("..")

os.system("cp " + TMP_DIR + "/" + executable_name + " " + executable_path)
os.system("rm -rf " + TMP_DIR)

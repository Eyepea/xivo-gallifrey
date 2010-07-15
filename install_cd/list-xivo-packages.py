#!/usr/bin/python
from __future__ import with_statement

import sys, urllib2, httplib, os.path

DAKBASE = 'http://dak.proformatique.com/debian'


if __name__ == '__main__':
	stats     = {'size': 0, 'installed-size': 0}
	packages = []

	sources = [
		DAKBASE + '/dists/lenny-xivo-gallifrey-dev/main/binary-i386/Packages',
		DAKBASE + '/dists/lenny-xivo-gallifrey-dev/contrib/binary-i386/Packages',
		DAKBASE + '/dists/lenny-xivo-gallifrey-dev/non-free/binary-i386/Packages',
#		DAKBASE + '/dists/lenny-xivo-gallifrey/main/binary-i386/Packages',
#		DAKBASE + '/dists/lenny-xivo-gallifrey/contrib/binary-i386/Packages',
#		DAKBASE + '/dists/lenny-xivo-gallifrey/non-free/binary-i386/Packages',

		DAKBASE + '/dists/lenny/main/binary-i386/Packages',
		DAKBASE + '/dists/lenny/contrib/binary-i386/Packages',
		DAKBASE + '/dists/lenny/non-free/binary-i386/Packages',

		DAKBASE + '/dists/lenny-dev/main/binary-i386/Packages',
		DAKBASE + '/dists/lenny-dev/contrib/binary-i386/Packages',
		DAKBASE + '/dists/lenny-dev/non-free/binary-i386/Packages',
	]
	
	# whitelist
	include = [
		'pf-fai-xivo-1.1-gallifrey-dev',
		'pf-fai-dev',
	]
	
	exclude = [
		'dahdi-linux-source',
		'misdn-kernel-source',
		'wireshark',
		'tshark',
		'swig',
		'squid',
		'python-crack',
		'python-cap',
		'pfbotnet', 'libpfbotnet',
		'pf-sys-ssh',
#		'pf-server',
		'mpb',
		'meep', 'libmeep',
		'libruby',
		'libctl',
		'libcrack2',
#		'libcap',
		'cracklib',
#		'pf-fai', REQUIRED
		'libnet-ssh-ruby',
		'sangoma-wanpipe-source',
		'sangoma-dbg',
	]

	skip=False
	
	for src in sources:
		f = urllib2.urlopen(src)
		for line in f.readlines():
			if line.startswith('Package:'):
				skip = True
				pacnam = line.split(' ')[1][:-1]
#				print pacnam,
				if not pacnam in include and \
					(pacnam.endswith('-dev') or \
					len(filter(lambda x: pacnam.startswith(x), exclude)) > 0):
#					print 'EXCLUDED: ', line[:-1]
					continue
				
				skip = False
#				print

			if skip:
				continue

			if line.startswith('Installed-Size:'):
				stats['installed-size'] += int(line.split(' ')[1])
			elif line.startswith('Size:'):
				stats['size'] += int(line.split(' ')[1])
			elif line.startswith('Filename:'):
				if 'dalek' in line:
					continue
			
				packages.append(line.split(' ')[1][:-1])

		f.close()
	
	
	packages.sort()
	import pprint; pprint.pprint(packages)
	print stats

#	sys.exit(1)

	DWPATH = 'xivo_packages/'
	# 2. downloading packages
	conn = httplib.HTTPConnection('dak.proformatique.com')
	for package in packages:
		debfile = package.rsplit('/', 1)[-1]
		print " . downloading", debfile, ':',

		if os.path.exists(DWPATH + debfile):
			localsize = os.path.getsize(DWPATH + debfile)
			
			conn.request("HEAD", '/debian/' + package)
			resp = conn.getresponse()
			try:
				netsize   = int(dict(resp.getheaders()).get('content-length'))
			except:
				netsize   = -1
			conn.close()
			
			if netsize == localsize:
				print 'skipping...'
				continue
		
		print '...'
		conn.request("GET", '/debian/' + package)
		resp = conn.getresponse()
		
		with open('xivo_packages/' + debfile, 'wb') as f:
			while True:
				data = resp.read(8192)
				if len(data) == 0:
					break
					
				f.write(data)
			
		conn.close()

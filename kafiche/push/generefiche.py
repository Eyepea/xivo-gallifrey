# Thomas Bernard
# generateur de fiche XML pour la remontee de fiche

import socket

# 1) instancier
# 2) ajouter des infos
# 3) utiliser getxml() ou sendtouser()

class Fiche:
	"""Fiche class"""
	def __init__(self, sessionid=''):
		self.sessionid = sessionid
		self.infos = []
	def __str__(self):
		return "Fiche : sessionid=" + self.sessionid + " infos=" + str(self.infos)
	def addinfo(self, name, type, value):
		"""add a field in the profile"""
		self.infos.append( (name, type, value) )
	def getxml(self):
		"""get a string containing the xml"""
		s = "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
		s += '<profile sessionid="' + self.sessionid + '">\n'
		s += '<user>\n'
		for (name, type, value) in self.infos:
			s += '<info name="' + name + '" type="' + type + '">'
			s += value
			s += '</info>\n'
		s += '</user>\n'
		s += '</profile>\n'
		return s
	def sendtouser(self, address):
		"""send the profile to a user using TCP"""
		try:
			s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
			s.connect(address)
			fs = s.makefile('w')
			s.close()
			fs.write(self.getxml())
			fs.flush()
			fs.close()
			return True
		except:
			return False

def getuserlocation(shost, sport, user):
	"""return None or the address and session id"""
	try:
		s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
		s.connect((shost, sport))
		fs = s.makefile("r")
		s.send("QUERY " + user + "\r\n")
		s.close()
		list = fs.readline().strip().split(' ')
		fs.close()
		if len(list) < 8 or list[0] == 'ERROR':
			return None
		sessionid = list[3]
		ip = list[5]
		port = int(list[7])
		return {'address':(ip, port), 'sessionid':sessionid}
	except:
		return None



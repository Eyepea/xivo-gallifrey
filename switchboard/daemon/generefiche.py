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
            self.message = ''
    def __str__(self):
            return "Fiche : sessionid=" + self.sessionid + " infos=" + str(self.infos)
    def __repr__(self):
            return str(self)
    def addinfo(self, name, type, value):
            """add a field in the profile"""
            self.infos.append( (name, type, value) )
    def setmessage(self, msg):
            """set the message to be displayed in systray message"""
            self.message = msg
    def getxml(self):
            """get a string containing the xml"""
            s = ['<?xml version="1.0" encoding="utf-8"?>']
            s.append('<profile sessionid="%s">' % self.sessionid)
            s.append('<user>')
            if len(self.message) > 0:
                    s.append('<message>%s</message>' % self.message)
            for (name, type, value) in self.infos:
                    s.append('<info name="%s" type="%s"><![CDATA[%s]]></info>' %(name, type, value))
            s.append('</user>')
            s.append('</profile>')
            return ''.join(s)
    def sendtouser(self, address):
            """send the profile to a user using TCP"""
            try:
                    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                    #print 'timeout=', s.gettimeout()
                    #s.settimeout(3.0)
                    s.connect(address)
                    fs = s.makefile('w')
                    s.close()
                    fs.write(self.getxml())
                    fs.flush()
                    fs.close()
                    return True
            except Exception, e:
                    #print e
                    return False

def getuserlocation(shost, sport, user):
	"""return None or the address and session id"""
	try:
		s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
		#s.settimeout(0.1)
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
		state = list[9]
		return {'address':(ip, port), 'sessionid':sessionid, 'state':state}
	except:
		return None



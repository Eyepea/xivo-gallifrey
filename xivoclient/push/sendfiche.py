# Thomas Bernard

import ConfigParser
import threading, sys
import ldap

import generefiche

#
## \class Info
class Info:
    def __init__(self, ititle, itype, ivalue):
        self.title = ititle
        self.type = itype
        self.value = ivalue
    def __str__(self):
        return self.title + '(' + self.type + ')="' + self.value + '"'
    def __repr__(self):
        return str(self)
    def getTitle(self):
        return self.title
    def getType(self):
        return self.type
    def getValue(self):
        return self.value

## \class myLDAP
class myLDAP:
	def __init__(self, ihost, iport, iuser, ipass):
		try:
			self.l = ldap.initialize("ldap://%s:%s" %(ihost, iport))
			self.l.protocol_version = ldap.VERSION3
			self.l.simple_bind_s(iuser, ipass)
			
		except ldap.LDAPError, e:
			print e
			sys.exit()

	def getldap(self, ibase, filter, attrib):
		try:
			resultat = self.l.search_s(ibase,
						   ldap.SCOPE_SUBTREE,
						   filter,
						   attrib)
			return resultat
		except ldap.LDAPError, e:
			print e

## \brief Returns the kind of database
# \param cfg
def get_dbkind(cfg):
	try:
		dbkind = cfg.get('general', 'db')
	except:
		dbkind = ""
	return dbkind


## \brief Returns informations fetched from the LDAP database
# \param config
# \param cid
def get_ldap_infos(config, cid):
	[callid, mail, name, firstname, company] = ["", "", "", "", ""]
	ldapid = myLDAP(config.get('general', 'host'),
			config.get('general', 'port'),
			config.get('general', 'user'),
			config.get('general', 'pass'))
	result = ldapid.getldap(config.get('general', 'dbname'),
				"(|(telephonenumber=%s)(mobile=%s)(pager=%s))" %(cid,cid,cid),
				['cn','mail','sn','givenName','o'])
	try:
		who = result[0][1]
		if 'cn' in who.keys():        callid    = who['cn'][0]
		sys.stdout.write("SET CALLERID \"%s\"\n" %callid)
		if 'mail' in who.keys():      mail      = who['mail'][0]
		if 'givenName' in who.keys(): firstname = who['givenName'][0]
		if 'sn' in who.keys():        name      = who['sn'][0]
		if 'o' in who.keys():         company   = who['o'][0]
	except Exception, e:
		print "No callerid from OX", e, result
	return [callid, mail, name, firstname, company]


## \brief Returns informations fetched from the MySQL database
# \param config configuration informations
# \param cid callerid requested
def get_mysql_infos(config, cid):
	results = []
	try:
		conn = MySQLdb.connect(host = config.get('general', 'host'),
				      port = int(config.get('general', 'port')),
				      user = config.get('general', 'user'),
				      passwd = config.get('general', 'pass'),
				      db = config.get('general', 'dbname'))
		cursor = conn.cursor()

		searchname = config.get('general', 'dbmatch')
		table = config.get('general', 'dbtable')
		sql = "SELECT * FROM %s WHERE %s REGEXP '%s' LIMIT 1;" %(table,searchname,cid)
	        cursor.execute(sql)
		results = [cursor.fetchone()] # vs. fetchall() if needed
		conn.close()

	except Exception, e:
		print "Connection to MySQL failed", config.get('general', 'host')

	return results


## \brief Returns informations fetched from the sqlite database
# \param config configuration informations
# \param cid callerid requested
def get_sqlite_infos(config, cid):
	results = []
	try:
		conn = sqlite.connect(db = config.get('general', 'dbname'))
		cursor = conn.cursor()

		searchname = config.get('general', 'dbmatch')
		table = config.get('general', 'dbtable')
		sql = "SELECT * FROM %s WHERE %s = '%s' LIMIT 1;" %(table,searchname,cid)
	        cursor.execute(sql)
		results = [cursor.fetchone()] # vs. fetchall() if needed
		conn.close()

	except Exception, e:
		print_verbose("Connection to sqlite failed")

	return results


## \brief fills the variable fields according to the recetived data
# \param callerid
# \param x
def set_fields_from_resultline(callerid, x):
	field = {}
	try:
		field["fullnumber"] = x[0]
		field["agency"]     = x[1]
		field["refnumber"]  = x[2]
		field["name"]       = x[3]
		field["zipcode"]    = x[4]
		field["town"]       = x[5]
		field["medal"]      = x[6]
	except Exception, e:
		print_verbose("Customer not found in mysql db")
		if len(callerid) == 9:
			field["fullnumber"] = callerid
		else:
			field["fullnumber"] = ""

	if len(field["fullnumber"]) == 9:
		tmpnum = "0" + field["fullnumber"][0] + "." + \
			 field["fullnumber"][1] + field["fullnumber"][2] + "." + \
			 field["fullnumber"][3] + field["fullnumber"][4] + "." + \
			 field["fullnumber"][5] + field["fullnumber"][6] + "." + \
			 field["fullnumber"][7] + field["fullnumber"][8]
		field["fullnumber"] = tmpnum

	return field


## \brief Builds the contents of the Sheet according to titles and default values
# \param items
# \param formats
# \param flds
def make_fields(items, formats, flds):
	liste = []
	for field_to_display in items:
		# a "/" character splits the order of the field in the displayed popup
		# and its kind
		if field_to_display[0].find("|") >= 0:
			kindoffield  = field_to_display[0].split("|")[1]

			fieldtype    = field_to_display[1].split("|")[0]
			argum        = field_to_display[1].split("|")[1]
			defaultvalue = field_to_display[1].split("|")[2]

			prestr = ""
			poststr = ""
			if kindoffield in formats.keys():
				prestr = formats[kindoffield].split("|")[0]
				poststr = formats[kindoffield].split("|")[1]
			value = prestr + defaultvalue + poststr
			if kindoffield in flds:
				if flds[kindoffield] != "":
					value = prestr + flds[kindoffield] + poststr
			liste.append(Info(argum, fieldtype, value))
	return liste


# opens the xivo_push.conf config file
config = ConfigParser.ConfigParser()
try:
	#config.readfp(open("/etc/asterisk/xivo_push.conf"))
	config.readfp(open("/home/nanard/xivo_push.conf"))
except:
	try:
		config.readfp(open("xivo_push.conf"))
	except:
		print "no xivo_push.conf file found"
		sys.exit(2)

# reads the kind of database and the sheet's format
databasekind = get_dbkind(config)
fitems = config.items("fiche")
fitems.sort()
fformats = {}
if "formats" in config.sections() :
	for x in config.items("formats"):
		fformats[x[0]] = x[1]

class FicheSender:
    def __call__(self, sessionid, address, state, callerid, msg):
        global databasekind, fitems, fformats
        #print 'FicheSend.__class__(%s, %s, %s)' % (sessionid, address, state)
        if state == 'available':
            liste = []
            fields = {}

            if databasekind == "ldap":
                fields["req_cid"] = callerid
                fields["callidname"], fields["mail"], fields["name"], fields["firstname"], fields["company"] = get_ldap_infos(config, callerid)
                if callerid in config.options('photo'):
                    fields["picture"] = config.get('photo', callerid)
                print "=====", fields
                liste = make_fields(fitems, fformats, fields)
                print "=====", liste
            elif databasekind == "mysql" or databasekind == "sqlite":
                listes = []
                if databasekind == "mysql":
                    results = get_mysql_infos(config, callerid)
                if databasekind == "sqlite":
                    results = get_sqlite_infos(config, callerid)
            #print_verbose("%d result(s) found for %s" %(len(results),callerid))
                if len(results) > 0:
                    for z in xrange(len(results)):
                        fields = set_fields_from_resultline(callerid, results[z])
                        fields["req_cid"] = callerid
                        liste = make_fields(fitems, fformats, fields)
                        listes.append(liste)
                else:
                    fields["req_cid"] = callerid
                    liste = make_fields(fitems, fformats, fields)

            print "*=*", address
            fiche = generefiche.Fiche(sessionid)
            fiche.setmessage(msg)
            for x in liste:
                fiche.addinfo(x.getTitle(), x.getType(), x.getValue())
            fiche.sendtouser(address)
            print '*** fiche.sendtouser() finished!!! ***'


def sendficheasync(userinfo, callerid, msg):
    sender = FicheSender()
    params = {'sessionid':userinfo['sessionid'],
              'address':(userinfo['ip'], int(userinfo['port'])),
              'state':userinfo['state'],
              'callerid':callerid,
              'msg':msg}
    t = threading.Thread(None, sender, None, (), params)
    t.start()




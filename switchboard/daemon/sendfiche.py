# Thomas Bernard

import ConfigParser
import getopt
import ldap
import sys
import syslog
import threading

import generefiche

# XIVO lib-python modules initialization
from xivo import ConfigPath
from xivo.ConfigPath import *
xivoconffile            = "/etc/asterisk/xivo_push.conf"
GETOPT_SHORTOPTS        = 'dc:'
GETOPT_LONGOPTS         = ["daemon", "config="]
CONFIG_LIB_PATH         = 'py_lib_path'
def config_path():
        global xivoconffile
        for opt, arg in getopt.getopt(sys.argv[1:], "dc:", ["daemon", "config="])[0]:
                if opt == "-c":
                        xivoconffile = arg
        ConfiguredPathHelper(xivoconffile, CONFIG_LIB_PATH)
config_path()
debug_mode = (sys.argv.count('-d') > 0)

# XIVO lib-python modules imports
import anysql

import xivo_ldap



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

def varlog(string):
        syslog.syslog(syslog.LOG_NOTICE, "sendfiche : " + string)
        return 0

def log_debug(string):
#        if debug_mode:
        print "#debug# (sendfiche) " + string
        return varlog(string)

## \brief Returns informations fetched from the LDAP database
# \param config
# \param cid
def get_ldap_infos(config, cid):
	[callid, mail, name, firstname, company] = ["", "", "", "", ""]
	ldapid = xivo_ldap.xivo_ldap(config.get('general', 'dir_db_uri'))
	result = ldapid.getldap("(|(telephonenumber=%s)(mobile=%s)(pager=%s))" %(cid,cid,cid),
				['cn','mail','sn','givenName','o'])
	try:
		who = result[0][1]
		if 'cn' in who.keys():        callid    = who['cn'][0]
		sys.stdout.write("SET CALLERID \"%s\"\n" %callid)
		if 'mail' in who.keys():      mail      = who['mail'][0]
		if 'givenName' in who.keys(): firstname = who['givenName'][0]
		if 'sn' in who.keys():        name      = who['sn'][0]
		if 'o' in who.keys():         company   = who['o'][0]
	except Exception, exc:
                log_debug('No callerid from OX : %s : %s' %(str(exc), str(result)))
	return [callid, mail, name, firstname, company]


## \brief Returns informations fetched from the SQL database
# \param config configuration informations
# \param cid callerid requested
def get_sql_infos(config, cid):
	results = []
	try:
		conn = anysql.connect(config.get('general', 'dir_db_uri'))
		cursor = conn.cursor()

		searchname = config.get('general', 'dbmatch')
		table = config.get('general', 'dbtable')
		sql = "SELECT * FROM %s WHERE %s REGEXP '%s' LIMIT 1;" %(table,searchname,cid)
	        cursor.execute(sql)
		results = [cursor.fetchone()] # vs. fetchall() if needed
		conn.close()

	except Exception, exc:
                log_debug('Connection to SQL <%s> failed : %s'
                          % (config.get('general', 'db_uri'),
                             str(exc)))

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
	except Exception, exc:
                log_debug('Customer not found in SQL db : %s' % str(exc))
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
	config.readfp(open(xivoconffile))
except:
	try:
		config.readfp(open("xivo_push.conf"))
	except:
                log_debug('no xivo_push.conf file found')
		sys.exit(2)

# reads the kind of database and the sheet's format
fitems = config.items("fiche")
fitems.sort()
fformats = {}
if "formats" in config.sections() :
	for x in config.items("formats"):
		fformats[x[0]] = x[1]

class FicheSender:
    def __call__(self, sessionid, address, state, callerid, msg, tcpmode, socket):
        global fitems, fformats
        #print 'FicheSend.__class__(%s, %s, %s)' % (sessionid, address, state)
        if state == 'available':
                liste = []
                fields = {}

                databasekind = config.get('general', 'dir_db_uri').split(':')[0]
                log_debug('databasekind = %s' % databasekind)
                if databasekind == "ldap":
                        fields["req_cid"] = callerid
                        fields["callidname"], fields["mail"], fields["name"], fields["firstname"], fields["company"] = get_ldap_infos(config, callerid)
                        if callerid in config.options('photo'):
                                fields["picture"] = config.get('photo', callerid)
                        log_debug('fields = %s' % str(fields))
                        liste = make_fields(fitems, fformats, fields)
                        log_debug('liste = %s' % str(liste))
                elif databasekind == "mysql" or databasekind == "sqlite":
                        listes = []
                        results = get_sql_infos(config, callerid)
                        # log_debug("%d result(s) found for %s" %(len(results),callerid))
                        if len(results) > 0:
                                for z in xrange(len(results)):
                                        fields = set_fields_from_resultline(callerid, results[z])
                                        fields["req_cid"] = callerid
                                        liste = make_fields(fitems, fformats, fields)
                                        listes.append(liste)
                        else:
                                fields["req_cid"] = callerid
                                liste = make_fields(fitems, fformats, fields)

                log_debug('address = %s' % str(address))
                fiche = generefiche.Fiche(sessionid)
                fiche.setmessage(msg)
                for x in liste:
                        fiche.addinfo(x.getTitle(), x.getType(), x.getValue())
                if tcpmode:
                        #fs = socket.makefile('w')
                        #fs.write(fiche.getxml())
                        #fs.flush()
                        #fs.close()
                        socket.write(fiche.getxml())
                        socket.flush()
                else:
                        fiche.sendtouser(address)
                log_debug('fiche.sendtouser() finished')


def sendficheasync(userinfo, callerid, msg):
        sender = FicheSender()
        log_debug('sendficheasync : %s callerid=<%s> msg=<%s>' % (userinfo, callerid, msg))
        params = {'sessionid':userinfo['sessionid'],
                  'address':(userinfo['ip'], int(userinfo['port'])),
                  'state':userinfo['state'],
                  'callerid':callerid,
                  'msg':msg,
                  'tcpmode':userinfo['tcpmode']}
        if userinfo['tcpmode']:
                params['socket'] = userinfo['socket']
        else:
                params['socket'] = None
        t = threading.Thread(None, sender, None, (), params)
        t.start()


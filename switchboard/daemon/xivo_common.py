
import anysql
import random
import string

__alphanums__ = string.uppercase + string.lowercase + string.digits

fullstat_heavies = {}
# user list initialized empty
configs = {}
ulist = {}
plist = {}
contexts_cl = {}

ITEMS_PER_PACKET = 500
HISTSEPAR = ';'

DUMMY_DIR = ''
DUMMY_RCHAN = ''
DUMMY_EXTEN = ''
DUMMY_MYNUM = ''
DUMMY_CLID = ''
DUMMY_STATE = ''

DIR_TO_STRING = '>'
DIR_FROM_STRING = '<'

## \brief Tells whether a channel is a "normal" one, i.e. SIP, IAX2, mISDN, Zap
# or not (like Local, Agent, ... anything else).
# \param chan the channel-like string (that should be like "proto/phone-id")
# \return True or False according to the above description
def is_normal_channel(chan):
        if chan.find("SIP/") == 0 or chan.find("IAX2/") == 0 or \
           chan.find("mISDN/") == 0 or chan.find("Zap/") == 0: return True
        else: return False

## \brief Builds the full list of callerIDNames/hints in order to send them to the requesting client.
# This should be done after a command called "callerid".
# \return a string containing the full callerIDs/hints list
# \sa manage_tcp_connection
def build_callerids_hints(icommand):
        kind = icommand.name
        if len(icommand.args) == 0:
                reqid = kind + '-' + ''.join(random.sample(__alphanums__, 10)) + "-" + hex(int(time.time()))
                log_debug(SYSLOG_INFO, 'transaction ID for %s is %s' % (kind, reqid))
                fullstat_heavies[reqid] = []
                if kind == 'phones-list':
                        for astid in configs:
                                plist_n = plist[astid]
                                plist_normal_keys = filter(lambda j: plist_n.normal[j].towatch, plist_n.normal.iterkeys())
                                plist_normal_keys.sort()
                                for phonenum in plist_normal_keys:
                                        phoneinfo = ("ful",
                                                     plist_n.astid,
                                                     plist_n.normal[phonenum].build_basestatus(),
                                                     plist_n.normal[phonenum].build_cidstatus(),
                                                     plist_n.normal[phonenum].build_fullstatlist() + ";")
                                        #    + "groupinfos/technique"
                                        fullstat_heavies[reqid].append(':'.join(phoneinfo))
                elif kind == 'phones-add':
                        for astid in configs:
                                fullstat_heavies[reqid].extend(plist[astid].lsttoadd)
                elif kind == 'phones-del':
                        for astid in configs:
                                fullstat_heavies[reqid].extend(plist[astid].lsttodel)
        else:
                reqid = icommand.args[0]

        if reqid in fullstat_heavies:
                fullstat = []
                nstat = len(fullstat_heavies[reqid])/ITEMS_PER_PACKET
                for j in xrange(ITEMS_PER_PACKET):
                        if len(fullstat_heavies[reqid]) > 0:
                                fullstat.append(fullstat_heavies[reqid].pop())
                if nstat > 0:
                        rtab = '%s=%s;%s' %(kind, reqid, ''.join(fullstat))
                else:
                        del fullstat_heavies[reqid]
                        rtab = '%s=0;%s'  %(kind, ''.join(fullstat))
                        log_debug(SYSLOG_INFO, 'building last packet reply for <%s ...>' %(rtab[0:40]))
                return rtab
        else:
                log_debug(SYSLOG_INFO, 'reqid <%s> not defined for %s reply' %(reqid, kind))
                return ''

## \brief Builds the features reply.
def build_features_get(reqlist):
        astid = reqlist[0]
        context = reqlist[1]
        srcnum = reqlist[2]
        repstr = ''

        # XXXXX : make a monitor command to define who is monitored
        ulist[astid].setmonitor(astid, icommand.args)

        conn = anysql.connect_by_uri(configs[astid].userfeatures_db_uri)
        cursor = conn.cursor()
        params = [srcnum, context]
        query = 'SELECT ${columns} FROM userfeatures WHERE number = %s AND context = %s'

        for key in ['enablevoicemail', 'callrecord', 'callfilter', 'enablednd']:
                try:
                        columns = (key,)
                        cursor.query(query, columns, params)
                        results = cursor.fetchall()
                        repstr += "%s;%s:;" %(key, str(results[0][0]))
                except Exception, exc:
                        log_debug(SYSLOG_ERR, '--- exception --- features_get(bool) id=%s key=%s : %s'
                                  %(str(reqlist), key, str(exc)))
                        return ('get', 'KO')

        for key in ['unc', 'busy', 'rna']:
                try:
                        columns = ('enable' + key,)
                        cursor.query(query, columns, params)
                        resenable = cursor.fetchall()

                        columns = ('dest' + key,)
                        cursor.query(query, columns, params)
                        resdest = cursor.fetchall()

                        repstr += '%s;%s:%s;' % (key, str(resenable[0][0]), str(resdest[0][0]))

                except Exception, exc:
                        log_debug(SYSLOG_ERR, '--- exception --- features_get(str) id=%s key=%s : %s'
                                  %(str(reqlist), key, str(exc)))
                        return ('get', 'KO')

        conn.close()

        if len(repstr) == 0:
                repstr = 'KO'
        return ('get', repstr)

## \brief Builds the features reply.
def build_features_put(reqlist):
        context = reqlist[1]
        srcnum = reqlist[2]
        try:
                len_reqlist = len(reqlist)
                if len_reqlist >= 4:
                        key = reqlist[3]
                        value = ''
                        if len_reqlist >= 5:
                                value = reqlist[4]
                        query = 'UPDATE userfeatures SET ' + key + ' = %s WHERE number = %s AND context = %s'
                        params = [value, srcnum, context]
                        conn = anysql.connect_by_uri(configs[reqlist[0]].userfeatures_db_uri)
                        cursor = conn.cursor()
                        cursor.query(query, parameters = params)
                        conn.commit()
                        conn.close()
                        repstr = 'OK'
                        response = ('put', '%s;%s;%s;' %(repstr, key, value))
                else:
                        response = ('put', 'KO')
        except Exception, exc:
                log_debug(SYSLOG_ERR, '--- exception --- features_put id=%s : %s'
                          %(str(reqlist), str(exc)))
                response = ('put', 'KO')
        return response



def update_availstate(me, state):
        astid    = me[0]
        username = me[1]
        do_state_update = False
        ulist[astid].acquire()
        try:
                userinfo = ulist[astid].finduser(username)
                if userinfo != None:
                        if 'sessiontimestamp' in userinfo:
                                userinfo['sessiontimestamp'] = time.time()
                        if state in allowed_states:
                                userinfo['state'] = state
                        else:
                                log_debug(SYSLOG_WARNING, '%s : (user %s) : state <%s> is not an allowed one => undefinedstate-updated'
                                          % (astid, username, state))
                                userinfo['state'] = 'undefinedstate-updated'
                        do_state_update = True
        finally:
                ulist[astid].release()

        if do_state_update:
                plist[astid].send_availstate_update(username, state)
        return ""

## \brief Function that fetches the call history from a database
# \param cfg the asterisk's config
# \param techno technology (SIP/IAX/ZAP/etc...)
# \param phoneid phone id
# \param phonenum the phone number
# \param nlines the number of lines to fetch for the given phone
# \param kind kind of list (ingoing, outgoing, missed calls)
def update_history_call(cfg, techno, phoneid, phonenum, nlines, kind):
        results = []
        if cfg.cdr_db_uri == '':
                log_debug(SYSLOG_WARNING, '%s : no CDR uri defined for this asterisk - see cdr_db_uri parameter' % cfg.astid)
        else:
                try:
                        cursor = cfg.cdr_db_conn.cursor()
                        columns = ('calldate', 'clid', 'src', 'dst', 'dcontext', 'channel', 'dstchannel',
                                   'lastapp', 'lastdata', 'duration', 'billsec', 'disposition', 'amaflags',
                                   'accountcode', 'uniqueid', 'userfield')
                        likestring = '%s/%s-%%' %(techno, phoneid)
                        orderbycalldate = "ORDER BY calldate DESC LIMIT %s" % nlines
                        
                        if kind == "0": # outgoing calls (all)
                                cursor.query("SELECT ${columns} FROM cdr WHERE channel LIKE %s " + orderbycalldate,
                                             columns,
                                             (likestring,))
                        elif kind == "1": # incoming calls (answered)
                                cursor.query("SELECT ${columns} FROM cdr WHERE disposition='ANSWERED' AND dstchannel LIKE %s " + orderbycalldate,
                                             columns,
                                             (likestring,))
                        else: # missed calls (received but not answered)
                                cursor.query("SELECT ${columns} FROM cdr WHERE disposition!='ANSWERED' AND dstchannel LIKE %s " + orderbycalldate,
                                             columns,
                                             (likestring,))
                        results = cursor.fetchall()
                except Exception, exc:
                        log_debug(SYSLOG_ERR, '--- exception --- %s : Connection to DataBase %s failed in History request : %s'
                                  %(cfg.astid, cfg.cdr_db_uri, str(exc)))
        return results


def build_history_string(requester_id, nlines, kind, commandclass):
        [dummyp, astid_src, dummyx, techno, phoneid, phonenum] = requester_id.split('/')
        if astid_src in configs:
                try:
                        reply = []
                        hist = update_history_call(configs[astid_src], techno, phoneid, phonenum, nlines, kind)
                        for x in hist:
                                try:
                                        ry1 = x[0].isoformat() + HISTSEPAR + x[1].replace('"', '') \
                                              + HISTSEPAR + str(x[10]) + HISTSEPAR + x[11]
                                except:
                                        ry1 = x[0] + HISTSEPAR + x[1].replace('"', '') \
                                              + HISTSEPAR + str(x[10]) + HISTSEPAR + x[11]

                                if kind == '0':
                                        num = x[3].replace('"', '')
                                        sipcid = "SIP/%s" % num
                                        cidname = num
                                        if sipcid in plist[astid_src].normal:
                                                cidname = '%s %s <%s>' %(plist[astid_src].normal[sipcid].calleridfirst,
                                                                         plist[astid_src].normal[sipcid].calleridlast,
                                                                         num)
                                        ry2 = HISTSEPAR + cidname + HISTSEPAR + 'OUT'
                                else:   # display callerid for incoming calls
                                        ry2 = HISTSEPAR + x[1].replace('"', '') + HISTSEPAR + 'IN'

                                reply.append(ry1)
                                reply.append(ry2)
                                reply.append(';')
                        return commandclass.history_srv2clt(reply)
                except Exception, exc:
                        log_debug(SYSLOG_ERR, '--- exception --- (%s) error : history : (client %s) : %s'
                                  %(astid_src, requester_id, str(exc)))
        else:
                return commandclass.dmessage_srv2clt('history KO : no such asterisk id <%s>' % astid_src)

## \brief Builds the full list of customers in order to send them to the requesting client.
# This should be done after a command called "customers".
# \return a string containing the full customers list
# \sa manage_tcp_connection
def build_customers(ctx, searchpatterns, commandclass):
        searchpattern = ' '.join(searchpatterns)
        if ctx in contexts_cl:
                z = contexts_cl[ctx]
        else:
                log_debug(SYSLOG_WARNING, 'there has been no section defined for context %s : can not proceed directory search' % ctx)
                z = Context()

        fullstatlist = []

        if searchpattern == "":
                return commandclass.directory_srv2clt(z, [])

        dbkind = z.uri.split(":")[0]
        if dbkind == 'ldap':
                selectline = []
                for fname in z.search_matching_fields:
                        if searchpattern == "*":
                                selectline.append("(%s=*)" % fname)
                        else:
                                selectline.append("(%s=*%s*)" %(fname, searchpattern))

                try:
                        ldapid = xivo_ldap.xivo_ldap(z.uri)
                        results = ldapid.getldap("(|%s)" % ''.join(selectline),
                                        z.search_matching_fields)
                        for result in results:
                                result_v = {}
                                for f in z.search_matching_fields:
                                        if f in result[1]:
                                                result_v[f] = result[1][f][0]
                                fullstatlist.append(';'.join(z.result_by_valid_field(result_v)))
                except Exception, exc:
                        log_debug(SYSLOG_ERR, '--- exception --- ldaprequest : %s' % str(exc))

        elif dbkind == 'file' or dbkind == 'http':
                log_debug(SYSLOG_WARNING, 'the URI <%s> is not supported yet for directory search queries' %(dbkind))

        elif dbkind != '':
                if searchpattern == '*':
                        whereline = ''
                else:
                        wl = []
                        for fname in z.search_matching_fields:
                                wl.append("%s REGEXP '%s'" %(fname, searchpattern))
                        whereline = 'WHERE ' + ' OR '.join(wl)

                try:
                        conn = anysql.connect_by_uri(z.uri)
                        cursor = conn.cursor()
                        cursor.query("SELECT ${columns} FROM " + z.sqltable + " " + whereline,
                                     tuple(z.search_matching_fields),
                                     None)
                        results = cursor.fetchall()
                        conn.close()
                        for result in results:
                                result_v = {}
                                n = 0
                                for f in z.search_matching_fields:
                                        result_v[f] = result[n]
                                        n += 1
                                fullstatlist.append(';'.join(z.result_by_valid_field(result_v)))
                except Exception, exc:
                        log_debug(SYSLOG_ERR, '--- exception --- sqlrequest : %s' % str(exc))
        else:
                log_debug(SYSLOG_WARNING, "no database method defined - please fill the dir_db_uri field of the <%s> context" % ctx)

        uniq = {}
        fullstatlist.sort()
        fullstat_body = []
        for fsl in [uniq.setdefault(e,e) for e in fullstatlist if e not in uniq]:
                fullstat_body.append(fsl)
        return commandclass.directory_srv2clt(z, fullstat_body)


def send_msg_to_cti_clients(strupdate):
        if strupdate is not None:
                for astid in ulist:
                        ulist[astid].sendmessage(strupdate)


## \brief Splits a channel name, allowing for instance local-extensions-3fb2,1 to be correctly split.
# \param channel the full channel name
# \return the phone id
def channel_splitter(channel):
        sp = channel.split("-")
        if len(sp) > 1:
                sp.pop()
        return "-".join(sp)


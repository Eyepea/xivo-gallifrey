# $Revision$
# $Date$

__version__ = "$Revision$ $Date$"

import time

class OutgoingCall:
        def __init__(self, commid, astid,
                     cursor_operat, socname,
                     uinfo, agentnum, agentname, dest, nsoc, ncli, ncol):
                self.dir    = 'o'
                self.commid = commid
                self.nsoc   = nsoc
                self.ncli   = ncli
                self.ncol   = ncol
                self.taxes  = None
                self.ctime  = time.localtime()
                self.parking = None
                self.dest = dest
                self.agentnum  = agentnum
                self.agentname = agentname
                self.astid = astid
                self.uinfo = uinfo
                
                self.parking = None
                self.parkexten = None
                self.peerchannel = None
                self.aboute = None
                self.appelaboute = None
                self.tocall = False
                self.toretrieve = None
                self.forceacd = None
                
                self.stimes = {time.time() : 'init'}
                self.ttimes = {time.time() : 'init'}

                self.cursor_operat = cursor_operat
                columns = ('N', 'NLIST')
                self.cursor_operat.query('USE %s_clients' % socname)
                self.cursor_operat.query('SELECT ${columns} FROM clients WHERE N = %s',
                                         columns,
                                         ncli)
                results = self.cursor_operat.fetchall()
                if len(results) > 0:
                        self.cliname = results[0][1]

                columns = ('N', 'NLIST')
                self.cursor_operat.query('SELECT ${columns} FROM collaborateurs WHERE N = %s',
                                         columns,
                                         ncol)
                results = self.cursor_operat.fetchall()
                if len(results) > 0:
                        self.colname = results[0][1]

                return

        def set_timestamp_tax(self, status):
                try:
                        self.ttimes[time.time()] = status
                except Exception, exc:
                        print '--- exception --- set_timestamp_tax (%s) : %s' % (status, str(exc))
                return
        
        def set_timestamp_stat(self, status):
                try:
                        self.stimes[time.time()] = status
                except Exception, exc:
                        print '--- exception --- set_timestamp_stat (%s) : %s' % (status, str(exc))
                return
        
        def settaxes(self, triplet):
                self.taxes = triplet


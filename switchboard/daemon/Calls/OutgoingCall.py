# -*- coding: latin-1 -*-

"""
The Outgoing Calls' properties are managed here.
Not so much actions as in IncomingCall are performed here : apart from the basic database requests, this is more used in a 'structure' way than a true 'class' way.
"""

__version__   = '$Revision$'
__date__      = '$Date$'
__copyright__ = 'Copyright (C) 2007, 2008, Proformatique'
__author__    = 'Corentin Le Gall'

import time

class OutgoingCall:
        """
        Class for outgoing calls management.
        """
        
        dir = 'o'
        ctime = time.localtime()
        parking = False
        parkexten = None
        peerchannel = None
        aboute = None
        appelaboute = None
        tocall = False
        toretrieve = None
        forceacd = None
        
        stimes = {time.time() : 'init'}
        ttimes = {time.time() : 'init'}

        def __init__(self, commid, astid,
                     cursor_operat, socname,
                     uinfo, agentnum, agentname, dest, nsoc, ncli, ncol):
                """
                Sets the call properties.
                """
                self.commid = commid
                self.nsoc   = nsoc
                self.ncli   = ncli
                self.ncol   = ncol
                self.mohclass = '/'.join([nsoc, ncli, ncol])
                self.taxes  = None
                self.dest = dest
                self.agentnum  = agentnum
                self.agentname = agentname
                self.astid = astid
                self.uinfo = uinfo
                
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
                """
                Sets the timestamps related to a status' change.
                It allows primarily to compute the ringing time.
                """
                try:
                        self.ttimes[time.time()] = status
                except Exception, exc:
                        print '--- exception --- set_timestamp_tax (%s) : %s' % (status, str(exc))
                return


        def set_timestamp_stat(self, status):
                """
                Sets the status' change at a given time, in order to build the call history later on.
                """
                try:
                        self.stimes[time.time()] = status
                except Exception, exc:
                        print '--- exception --- set_timestamp_stat (%s) : %s' % (status, str(exc))
                return


        def settaxes(self, triplet):
                """
                Sets the call's tax informations.
                """
                self.taxes = triplet
                return

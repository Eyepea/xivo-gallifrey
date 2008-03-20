# -*- coding: latin-1 -*-

"""
The Incoming Calls' properties are managed here.
The constructor does the first database requests according to the SDA, and the instance thus created is managed by the CallBooster class.
As time goes by, further updates or requests are made to one given instance, through get_sda_profiles() first, through findaction() or check_operator_status() then.
- findaction() loops over the Dialplan iterations
- check_operator_status() might be called during the Dialplan process, or during Status updates from any Agent
"""

__version__   = '$Revision$'
__date__      = '$Date$'
__copyright__ = 'Copyright (C) 2007, 2008, Proformatique'
__author__    = 'Corentin Le Gall'

# This is an extension to XIVO Daemon, authorized by Pro-formatique SARL
# for sub-licensing under a separated contract.
#
# Licensing of this code is NOT bounded by the terms of the
# GNU General Public License.
#
# See the LICENSE file at top of the source tree or delivered in the
# installable package in which XIVO Daemon is distributed for more details.

import socket
import time

WEEKDAY = ['LUN', 'MAR', 'MER', 'JEU', 'VEN', 'SAM', 'DIM']
DATEFMT = '%Y-%m-%d'
DATETIMEFMT = DATEFMT + ' %H:%M:%S'

class IncomingCall:
        """
        Class for incoming calls management.
        """
        dir = 'i'

        def __init__(self, cursor_operat, cidnum, sdanum, queuenum, opejd, opend):
                """
                Sets the queue properties and does the first requests in order to know whether
                the SDA can be handled.
                This constructor is called only once (of course) for each incoming call.
                """

                self.ctime = time.localtime()

                self.cidnum    = cidnum
                self.sdanum    = sdanum
                self.queuename = 'qcb_%05d' % queuenum
                self.commid    = str(100000 + queuenum)
                self.cursor_operat = cursor_operat
                self.opejd = opejd.split(':')
                self.opend = opend.split(':')

                self.nsoc = 0 # None
                self.ncli = 0 # None
                self.ncol = 0 # None
                self.whereph = 'INTRO'
                self.wherephS = None
                self.wherephF = None
                self.cliname = ''
                self.colname = ''
                self.socname = ''
                self.statacd2_state = 'NC'
                self.statacd2_tt = 'TT_RAF'
        
                self.waiting = True
                self.parking = False
                self.parkexten = None
                self.peerchannel = None
                self.aboute = None
                self.appelaboute = None
                self.tocall = False
                self.toretrieve = None
                self.svirt = None
                self.forceacd = None
                self.agentlist = []
                self.annuleraccroche = False

                self.elect_prio = None

                self.stimes = {time.time() : 'init'}
                self.ttimes = {time.time() : 'init'}
                self.uinfo = None
                self.statdone = False
                self.secours_allowed = False

                self.dialplan = {'welcome' : 0,
                                 'callerid' : 1,
                                 'record' : None,
                                 'rescue' : 1,
                                 'rescue_details' : None,
                                 'hassun' : False,
                                 'sun' : False,
                                 'sounds' : []}

                columns = ('NSDA', 'NSOC', 'NCLI', 'NCOL', 'NOM', 'DateD', 'DateF', 'Valide')
                self.cursor_operat.query('USE system')
                self.cursor_operat.query('SELECT ${columns} FROM sda WHERE NSDA = %s',
                                         columns,
                                         self.sdanum)
                results = self.cursor_operat.fetchall()
                if len(results) > 0:
                        self.system_sda = results[0]
                        self.nsoc = str(self.system_sda[1])
                        self.ncli = str(self.system_sda[2])
                        self.ncol = str(self.system_sda[3])

                        columns = ('N', 'NOM', 'ID', 'Dossier')
                        self.cursor_operat.query('USE system')
                        self.cursor_operat.query('SELECT ${columns} FROM societes WHERE N = %s',
                                                 columns,
                                                 self.nsoc)
                        system_societes = self.cursor_operat.fetchall()
                        if len(system_societes) > 0:
                                self.nsoc_global = system_societes[0][2]
                                self.socname = system_societes[0][3]
                        else:
                                self.socname = 'adh_inconnu'

                        self.dialplan['soundpath'] = '%s/%s/%s' % (self.nsoc, self.ncli, self.ncol)
                        isvalid = self.system_sda[7]
                        if isvalid == 1:
                                nowdate = time.strftime(DATEFMT, self.ctime)
                                ok_days = False
                                if self.system_sda[5] == None and self.system_sda[6] == None:
                                        ok_days = True
                                else:
                                        date1 = time.strptime(str(self.system_sda[5]), DATETIMEFMT)
                                        date2 = time.strptime(str(self.system_sda[6]), DATETIMEFMT)
                                        dateN = time.strptime(nowdate + ' ' + '00:00:00', DATETIMEFMT)
                                        if dateN >= date1 and dateN <= date2:
                                                ok_days = True
                                
                                if ok_days:
                                        self.statacd2_state = 'V'
                                else:
                                        self.statacd2_state = 'HDV'
                        else:
                                self.statacd2_state = 'NV'


        def settaxes(self, triplet):
                """
                Sets the call's tax informations.
                """
                self.taxes = triplet
                return


        def setclicolnames(self):
                """
                Sets the name of CLIent and COLlaborator
                """
                columns = ('N', 'NLIST')
                self.cursor_operat.query('USE %s_clients' % self.socname)
                self.cursor_operat.query('SELECT ${columns} FROM clients WHERE N = %s',
                                         columns,
                                         self.ncli)
                results = self.cursor_operat.fetchall()
                if len(results) > 0:
                        self.cliname = results[0][1]

                columns = ('N', 'NLIST')
                self.cursor_operat.query('SELECT ${columns} FROM collaborateurs WHERE N = %s',
                                         columns,
                                         self.ncol)
                results = self.cursor_operat.fetchall()
                if len(results) > 0:
                        self.colname = results[0][1]


        def get_sda_profiles(self, sda_busyness):
                """
                Reads the available scenarios according to the SDA, the date and time, and
                the current occupation of the SDA.
                """
                self.elect_competence = None
                if sda_busyness is None: sda_busyness = 0 # happens at first call for this SDA
                
                columns = ('N', 'NCLI', 'NCOL', 'NSDA', 'Script', 'Flag', 'Priorite', 'CompLocale', 'VoiesSim')
                self.cursor_operat.query('USE %s_clients' % self.socname)
                self.cursor_operat.query('SELECT ${columns} FROM sda WHERE NSDA = %s',
                                         columns,
                                         self.sdanum)
                results = self.cursor_operat.fetchall()
                if len(results) > 0:
                        clients_sda = results[0]
                        flags = clients_sda[5][:4]
                        self.dialplan['welcome']  = int(flags[0])
                        self.dialplan['callerid'] = 1 - int(flags[1])
                        self.dialplan['rescue']   = 1 - int(flags[2])
                        if flags[3] == '1':
                                self.dialplan['record'] = '-'.join([self.sdanum,
                                                                    time.strftime('%Y%m%d-%H%M', self.ctime),
                                                                    self.commid])
                        self.elect_prio       = int(clients_sda[6])
                        self.elect_competence = int(clients_sda[7])
                        simult = int(clients_sda[8])
                        if simult > -1 and sda_busyness >= simult:
                                self.statacd2_tt = 'TT_ASD'
                                return False
                else:
                        self.statacd2_tt = 'TT_SND'
                        return False

                columns = ('NSOC', 'DATEJ', 'TCOMME')
                self.cursor_operat.query('USE system')
                self.cursor_operat.query('SELECT ${columns} FROM jferies WHERE NSOC = %s',
                                         columns,
                                         self.nsoc)
                system_jferies = self.cursor_operat.fetchall()

                self.period = ['JOUR', '']
                weekday_today = WEEKDAY[self.ctime[6]]
                if weekday_today == 'DIM' or weekday_today == 'SAM' and self.ctime[3] >= 12:
                        self.period[1] = 'WE'
                today_ferie = time.strftime('%d-%m', self.ctime)
                for z in system_jferies:
                        if z[1] == today_ferie:
                                self.period[1] = 'FERIE'
                                weekday_today = z[2]
                                break

                self.competences = []
                self.languages_sv = []
                self.languages = {}

                # fichiers sons
                columns = ('NCLI', 'NCOL', 'Nom')
                self.cursor_operat.query('USE %s_clients' % self.socname)
                self.cursor_operat.query('SELECT ${columns} FROM son WHERE NCLI = %s AND NCOL = %s',
                                     columns,
                                     (self.ncli, self.ncol))
                clients_son = self.cursor_operat.fetchall()
                for cson in clients_son:
                        self.dialplan['sounds'].append(cson[2])

                # competences
                columns = ('NCLI', 'NCOL', 'NComp')
                self.cursor_operat.query('SELECT ${columns} FROM clicomp WHERE NCLI = %s AND NCOL = %s',
                                         columns,
                                         (self.ncli, self.ncol))
                clients_clicomp = self.cursor_operat.fetchall()
                for t in clients_clicomp:
                        self.competences.append(str(t[2]))

                # langues
                columns = ('NCLI', 'NCOL', 'NLang', 'Niveau')
                self.cursor_operat.query('SELECT ${columns} FROM clilang WHERE NCLI = %s AND NCOL = %s',
                                         columns,
                                         (self.ncli, self.ncol))
                clients_clilang = self.cursor_operat.fetchall()
                for t in clients_clilang:
                        self.languages[str(t[2])] = t[3]
                        self.languages_sv.append('%d-%s' %(t[2], t[3]))

                print 'INCOMING CALL ... company =', self.socname, '/ call time =', self.ctime, '/ competences =', self.competences, '/ lang =', self.languages

                # Description des profils
                # Mode Secours ? no_rescue
                # Mode non secours
                columns = ('N', 'NCLI', 'NCOL', 'TypeP', 'JOUR', 'PlgD', 'PlgF', 'TypeT', 'Detail', 'Ordre', 'DelaiGRP', 'DATED', 'DATEF')

                self.cursor_operat.query("SELECT ${columns} FROM profil WHERE TypeP = 'SEC' AND NCLI = 0 AND NCOL = 0",
                                         columns)
                results = self.cursor_operat.fetchall()
                if results == ():
                        results = []

                self.cursor_operat.query('SELECT ${columns} FROM profil WHERE NCLI = %s AND NCOL = %s',
                                         columns,
                                         [self.ncli, self.ncol])
                results.extend(self.cursor_operat.fetchall())
                if results == ():
                        results = []

                if len(results) > 0:
                        # according to when / what / which groups ...
                        self.true_clients_profil = {}
                        prio_by_phase = {}
                        nowdate = time.strftime(DATEFMT, self.ctime)
                        for clients_profil in results:
                                prio = 0 # in order to distinguish between date definitions
                                ok_weekdays = False
                                ok_days = False

                                if weekday_today == clients_profil[4]:
                                        prio = 1
                                        ok_weekdays = True
                                elif clients_profil[4] == 'TOU':
                                        ok_weekdays = True

                                if clients_profil[11] == None and clients_profil[12] == None:
                                        ok_days = True
                                else:
                                        date1 = time.strptime(str(clients_profil[11]), DATETIMEFMT)
                                        date2 = time.strptime(str(clients_profil[12]), DATETIMEFMT)
                                        dateN = time.strptime(nowdate + ' ' + '00:00:00', DATETIMEFMT)
                                        if dateN >= date1 and dateN <= date2:
                                                prio = 2
                                                ok_days = True

                                if ok_weekdays and ok_days:
                                        pld = time.strptime(nowdate + ' ' + clients_profil[5], DATETIMEFMT)
                                        plf = time.strptime(nowdate + ' ' + clients_profil[6], DATETIMEFMT)
                                        typep = clients_profil[3]
                                        if typep == 'SUN':
                                                self.dialplan['hassun'] = True
                                        if self.ctime > pld and self.ctime < plf:
                                                if typep == 'SUN':
                                                        self.dialplan['sun'] = True
                                                elif typep == 'SEC' and self.dialplan['rescue'] == 1:
                                                        if self.dialplan['rescue'] == 1:
                                                                self.dialplan['rescue_details'] = clients_profil[9]
                                                                self.true_clients_profil[typep] = clients_profil
                                                else:
                                                        if typep in prio_by_phase:
                                                                if prio > prio_by_phase[typep]:
                                                                        prio_by_phase[typep] = prio
                                                                        self.true_clients_profil[typep] = clients_profil
                                                        else:
                                                                prio_by_phase[typep] = prio
                                                                self.true_clients_profil[typep] = clients_profil
                else:
                        self.statacd2_tt = 'TT_SND'

                if len(self.true_clients_profil) == 0:
                        self.statacd2_tt = 'TT_SND'

                for typep in self.true_clients_profil:
                        print 'INCOMING CALL : phases', typep, self.true_clients_profil[typep]

                # are we the day or the night ???
                are_we_day = True
                time1 = int(self.opejd[0]) * 60 + int(self.opejd[1])
                time2 = int(self.opend[0]) * 60 + int(self.opend[1])
                timex = self.ctime[3] * 60 + self.ctime[4]

                if self.dialplan['hassun']:
                        if not self.dialplan['sun']:
                                are_we_day = False
                else:
                        if timex > time2 or timex < time1:
                                are_we_day = False

                if are_we_day:
                        self.period[0] = 'JOUR'
                        if 'M001' in self.dialplan['sounds']:
                                self.dialplan['M001003'] = 'M001'
                        else:
                                self.dialplan['M001003'] = None
                else:
                        self.period[0] = 'NUIT'
                        if 'M003' in self.dialplan['sounds']:
                                self.dialplan['M001003'] = 'M003'
                        elif 'M001' in self.dialplan['sounds']:
                                self.dialplan['M001003'] = 'M001'
                        else:
                                self.dialplan['M001003'] = None

                print 'day/night status', self.dialplan['hassun'], self.dialplan['sun'], time1, time2, timex, self.period, self.dialplan['M001003']

                # print out a summary of dialplan statuses
                print 'INCOMING CALL : dialplan settings =', self.dialplan

                return True


        def __local_group_composition__(self):
                """
                Returns the local group composition (code number 0).
                """
                columns = ('CODE', 'NOM', 'GL')
                self.cursor_operat.query('USE agents')
                self.cursor_operat.query('SELECT ${columns} FROM agents WHERE GL = 1',
                                         columns)
                agents_agents = self.cursor_operat.fetchall()
                lstlocal = []
                if len(agents_agents) > 0:
                        for ag in agents_agents:
                                lstlocal.append(ag[1])
                return lstlocal


        def __groups_composition__(self, nprof, ngroup):
                """
                Returns the groups' composition according to 'nprof' and 'ngroup'.
                """
                columns = ('NPERM', 'NGROUP', 'NOM', 'NPROF')
                self.cursor_operat.query('USE %s_clients' % self.socname)
                self.cursor_operat.query('SELECT ${columns} FROM groupes WHERE NGROUP = %s AND NPROF = %s',
                                         columns,
                                         (ngroup, nprof))
                clients_groupes_liste = self.cursor_operat.fetchall()
                list_operators = []
                list_permanences = []

                print '  COMING CALL : __groups_composition__, ngroup = %s / nprof = %s' % (ngroup, nprof), clients_groupes_liste
                if int(ngroup) < 0:
                        for clients_groupes in clients_groupes_liste:
                                nom = clients_groupes[2]
                                if nom[0] != '%':
                                        list_operators.append(nom)
                                elif nom[0] == '%':
                                        nom_groupe = nom[1:]
                                        columns = ('Nom', 'NOPE', 'NomOPE')
                                        self.cursor_operat.query('SELECT ${columns} FROM groupes_ope WHERE Nom = %s AND NOPE != 0',
                                                                 columns,
                                                                 nom_groupe)
                                        clients_groupes_ope = self.cursor_operat.fetchall()
                                        for res in clients_groupes_ope:
                                                list_operators.append(res[2])
                elif int(ngroup) > 0:
                        list_permanences = []
                        for clients_groupes in clients_groupes_liste:
                                list_permanences.append(str(clients_groupes[0]))

                return [list_operators, ','.join(list_permanences)]


        def check_operator_status(self, operatorname):
                """
                Finds the operator availability, according to one's properties
                and the current call's.
                """
                columns = ('CODE', 'NOM')
                # AGPI : records outgoing calls
                self.cursor_operat.query('USE agents')
                self.cursor_operat.query('SELECT ${columns} FROM agents WHERE NOM = %s',
                                         columns,
                                         operatorname)
                agents_agents = self.cursor_operat.fetchall()
                if len(agents_agents) > 0:
                        operator_code = agents_agents[0][0]

                        agent_comp = []
                        columns = ('Nagt', 'NComp')
                        self.cursor_operat.query('USE agents')
                        self.cursor_operat.query('SELECT ${columns} FROM agtcomp WHERE Nagt = %s',
                                                 columns,
                                                 operator_code)
                        agents_agtcomp = self.cursor_operat.fetchall()
                        for t in agents_agtcomp:
                                agent_comp.append(t[1])

                        agent_lang = {}
                        columns = ('Nagt', 'NLang', 'Niveau')
                        self.cursor_operat.query('USE agents')
                        self.cursor_operat.query('SELECT ${columns} FROM agtlang WHERE Nagt = %s',
                                                 columns,
                                                 operator_code)
                        agents_agtlang = self.cursor_operat.fetchall()
                        for t in agents_agtlang:
                                agent_lang[str(t[1])] = t[2]

                        language_allowed = True
                        for lid, lreq in self.languages.iteritems():
                                if lid in agent_lang:
                                        if agent_lang[lid] < lreq:
                                                language_allowed = False
                                else:
                                        language_allowed = False
                        comp_allowed = True
                        for cid in self.competences:
                                if cid not in agent_comp: comp_allowed = False

                        if not (language_allowed and comp_allowed):
                                print operatorname.encode('latin1'), ':', language_allowed, comp_allowed
                                return None

                        columns = ('NOPE', 'AdrNet', 'CompLocale', 'Etat', 'Voie',
                                   'NbT8', 'NbT4', 'NbT2', 'NbT1',
                                   'Niveau', 'Derange', 'Priorite')
                        self.cursor_operat.query("SELECT ${columns} FROM acd WHERE NOPE = %s AND Derange != 0 AND Etat != 'Sortie' AND CompLocale >= %s",
                                                 columns,
                                                 (operator_code, self.elect_competence))
                        agents_acd = self.cursor_operat.fetchall()
                        if len(agents_acd) > 0:
                                dispo = agents_acd[0]
                                bness = dispo[5] * 8 + dispo[6] * 4 + dispo[7] * 2 + dispo[8]
                                print 'busy-ness : %d/%d/%d/%d weight = %d' % (dispo[5], dispo[6], dispo[7], dispo[8], bness)
                                if dispo[3] == 'Prêt'.decode('latin1'):
                                        status = 'Pret%d' % dispo[4]
                                else:
                                        status = dispo[3]
                                return [status, dispo[1], dispo[9], dispo[11], bness]
                        else:
                                return None
                else:
                        print '  COMING CALL : check_operator_status() : no match for %s in agents.agents' % operatorname.encode('latin1')
                        return None


        def __list_operators__(self, nprof, detail_tab, upto):
                """
                Returns the list of agents, as well as their status, that are available for the groups defined from
                index 0 to upto.
                """
                listopers = []
                listperms = {}
                for num in xrange(upto):
                        id = detail_tab[num]
                        if id == '0': # local group
                                for lgc in self.__local_group_composition__():
                                        if lgc not in listopers:
                                                listopers.append(lgc)
                        else: # localN and globalN groups
                                [lopers, lperms] = self.__groups_composition__(nprof, id)
                                for lgc in lopers:
                                        if lgc not in listopers:
                                                listopers.append(lgc)
                                if len(lperms) > 0:
                                        listperms[id] = lperms

                # should we return the ones which are in 'sortie' status there ?
                return [listopers, listperms]


        def __typet_secretariat__(self, nprof, detail, delaigrp):
                """
                Handles the 'Secretariat' actions of 'PH' scenarios.
                """
                whattodo = None
                # if no detail is provided, consider the 'local' group is there
                if len(detail) == 0:
                        detail = '0'

                detail_tab = detail.split(',')
                delaigrp_tab = delaigrp.split(',')
                ngroupes = len(detail_tab)
                if self.wherephS is None:
                        upto = 1
                else:
                        upto = self.wherephS + 1

                print '  COMING CALL : __typet_secretariat__, upto = ', upto
                list_svirt_tmp = {}
                while True: # loop over the detailed items
                        self.wherephS = upto
                        if upto > ngroupes:
                                whattodo = None
                                break

                        print '  COMING CALL : __typet_secretariat__, prevnum = %d / grouplist = %s' % (self.wherephS, ';'.join(detail_tab[0:upto+1]))
                        [self.list_operators, list_svirt_tmp] = self.__list_operators__(nprof, detail_tab, upto)
                        print '  COMING CALL : __typet_secretariat__, operators and rooms :', self.list_operators, list_svirt_tmp

                        if upto == ngroupes or len(self.list_operators) == 0 and len(list_svirt_tmp) == 0:
                                delay = 0
                        else:
                                if upto > 0:
                                        delay = int(delaigrp_tab[upto] - delaigrp_tab[upto - 1])
                                else:
                                        # should not occur
                                        delay = int(delaigrp_tab[0])

                        print '  COMING CALL : __typet_secretariat__, upto = %d, delay = %d s' % (upto, delay)

                        if len(self.list_operators) == 0 and len(list_svirt_tmp) == 0:
                                upto += 1
                        else:
                                self.statacd2_tt = 'TT_SFA'
                                whattodo = {'action'   : 'secretariat',
                                            'delay'    : delay,
                                            'argument' : None}
                                break

                self.list_svirt = {}
                for ngroup, perms in list_svirt_tmp.iteritems():
                        request = [str(self.nsoc_global),
                                   self.ncli,
                                   self.ncol,
                                   self.sdanum,
                                   self.commid,
                                   ngroup,
                                   self.cidnum,
                                   perms,
                                   ','.join(self.competences),
                                   ','.join(self.languages_sv),
                                   self.commid]
                        self.list_svirt[ngroup + '.' + perms] = {'status' : 'init',
                                                                 'request' : request}

                print '  COMING CALL : __typet_secretariat__ : ', upto, whattodo
                return whattodo


        def __get_fichier__(self, detail):
                """
                Reads the 'fichier' entries for 'F' and 'T' actions.
                """
                columns = ('NCLI', 'NCOL', 'NOM', 'NTEL', 'TypeN', 'ModeA', 'NbC', 'NbP', 'DSonn', 'SDA', 'Ordre')
                self.cursor_operat.query('USE %s_clients' % self.socname)
                if self.whereph == 'SEC':
                        self.cursor_operat.query('SELECT ${columns} FROM fichier WHERE NCLI = 0 AND NCOL = 0 AND NOM = %s ORDER BY Ordre',
                                                 columns,
                                                 (detail))
                else:
                        self.cursor_operat.query('SELECT ${columns} FROM fichier WHERE NCLI = %s AND NCOL = %s AND NOM = %s ORDER BY Ordre',
                                                 columns,
                                                 (self.ncli, self.ncol, detail))
                clients_fichier = self.cursor_operat.fetchall()
                return clients_fichier


        def __typet_fichier__(self, detail):
                """
                Handles the 'Fichier' actions of 'PH' scenarios.
                """
                results = self.__get_fichier__(detail)
                nresults = len(results)
                if nresults > 0:
                        if self.wherephF is None:
                                self.wherephF = 0
                        else:
                                self.wherephF += 1
                        if self.wherephF < nresults:
                                self.statacd2_tt = 'TT_FIC'
                                b = results[self.wherephF]
                                delay = b[8]
                                print 'Fichier %s : %s (%s) %s %s %s' % (str(b), b[3], b[5], b[6], b[7], delay)
                                if b[5] == 'STAND':
                                        delay = ''
                                whattodo = {'action'   : 'fic',
                                            'delay'    : 0,
                                            'argument' : '%s-%s-%s-%s' %(b[3], delay, b[6], b[7])}
                        else:
                                whattodo = None
                else:
                        whattodo = None
                return whattodo


        def __typet_telephone__(self, detail):
                """
                Handles the 'Telephone' actions of 'PH' scenarios.
                """
                results = self.__get_fichier__(detail)
                nresults = len(results)
                if nresults > 0:
                        if self.wherephF is None:
                                self.statacd2_tt = 'TT_TEL'
                                self.wherephF = 0
                                b = results[0]
                                delay = b[8]
                                print 'Telephone %s : %s (%s) %s %s %s' % (str(b), b[3], b[5], b[6], b[7], delay)
                                if b[5] == 'STAND':
                                        delay = ''
                                whattodo = {'action'   : 'tel',
                                            'delay'    : 0,
                                            'argument' : '%s-%s-%s-%s' %(b[3], delay, b[6], b[7])}
                        else:
                                whattodo = None
                else:
                        whattodo = None
                return whattodo


        def __typet_base__(self, detail):
                """
                Handles the 'Base' actions of 'PH' scenarios.
                """
                whattodo = None
                columns = ('NCLI', 'NCOL', 'Nom', 'DateJ', 'Plages', 'Plage1', 'Plage2', 'Plage3', 'Plage4')
                self.cursor_operat.query('USE %s_clients' % self.socname)
                self.cursor_operat.query('SELECT ${columns} FROM gardes WHERE NCLI = %s AND NCOL = 0 AND Nom = %s',
                                         columns,
                                         (self.ncli, detail)) # (self.ncli, self.ncol, detail))
                clients_gardes = self.cursor_operat.fetchall()
                if len(clients_gardes) > 0 and self.wherephF is None:
                        nows = time.mktime(self.ctime)
                        str_today = time.strftime(DATEFMT, self.ctime)
                        str_yesterday = time.strftime(DATEFMT, time.localtime(nows - 86400))
                        realidx = None
                        for b in clients_gardes:
                                if str(b[3])[:10] == str_today:
                                        plbase = []
                                        dateplage_prev = time.strptime(str(b[3])[:11] + '00:00', DATEFMT + ' %H:%M')
                                        for pp in b[4].split('.'):
                                                dateplage = time.strptime(str(b[3])[:11] + pp, DATEFMT + ' %H:%M')
                                                dplage = time.mktime(dateplage)
                                                # manage the time wrap ...
                                                if dateplage < dateplage_prev:
                                                        plbase.append([dplage, 86400])
                                                else:
                                                        plbase.append([dplage, 0])
                                                dateplage_prev = dateplage

                                        idx = None
                                        for plages in plbase:
                                                t = plages[0] + plages[1]
                                                if nows < t:
                                                        idx = plbase.index(plages)
                                                        break
                                        # idx = 0 : cf. "yesterday", idx = None : last
                                        if idx is None:
                                                realidx = 3
                                                realbase = b
                                                break
                                        elif idx == 0:
                                                realidx = None
                                        else:
                                                realidx = idx - 1
                                                realbase = b
                                                break

                        if realidx is None:
                                # trying a match with yesterday's last span
                                for b in clients_gardes:
                                        if str(b[3])[:10] == str_yesterday:
                                                realidx = 3
                                                realbase = b
                                                break

                        if realidx is None:
                                print 'WARNING - did not found BASE entry for this date and time'
                        else:
                                fichenum = realbase[5 + realidx].split('.')[0]
                                print 'Base', plbase, realidx, realbase
                                if fichenum != '0':
                                        self.statacd2_tt = 'TT_BAS'
                                        self.wherephF = 0
                                        columns = ('N', 'NCLI', 'Nom', 'Tel1', 'Tel2', 'Tel3', 'Tel4', 'Tel5', 'Tel6', 'TelSel')
                                        self.cursor_operat.query('SELECT ${columns} FROM fichesgarde WHERE N = %s AND NCLI = %s',
                                                                 columns,
                                                                 (fichenum, self.ncli))
                                        res2 = self.cursor_operat.fetchall()
                                        if len(res2) > 0:
                                                res3 = res2[0]
                                                which = res3[9]
                                                fg = res3[2 + which]
                                                [num, intext, normstd, ncherche, npatiente, withsda, wtime] = fg.split('.')
                                                print 'intext = %s, normstd = %s, num = %s, ncherche = %s, npatiente = %s, time = %s, withsda = %s, which = %d' \
                                                      %(intext, normstd, num, ncherche, npatiente, wtime, withsda, which)
                                                if which == 6:
                                                        # number is a Repondeur file
                                                        whattodo = {'action'   : 'rep',
                                                                    'delay'    : 0,
                                                                    'argument' : self.__wavstrip__(num)}
                                                else:
                                                        if normstd == '1':
                                                                wtime = ''
                                                        whattodo = {'action'   : 'bas',
                                                                    'delay'    : 0,
                                                                    'argument' : '-'.join([num, wtime, ncherche, npatiente])}

                return whattodo


        def __wavstrip__(self, filename):
                """
                Utility to remove '.wav' and '.WAV' extensions.
                """
                if filename.find('.wav') == len(filename) - 4 or filename.find('.WAV') == len(filename) - 4:
                        return filename[:-4]
                else:
                        return filename


        def __spanprofiles_secretariat__(self):
                """
                To be called by the main loop, for each incoming call, in order to decide later on who will be chosen.
                """


        def __typep_phases__(self, typep, clients_profil):
                """
                Handles the 'PH1', 'PH2' (and casually 'SEC') scenarios.
                """
                nprof = clients_profil[0]
                typet = clients_profil[7]
                detail = clients_profil[8]
                delaigrp = clients_profil[10]
                if typep in ['PH1', 'PH2']:
                        if typet == 'S':
                                self.secours_allowed = True
                        else:
                                self.secours_allowed = False

                if typep == 'SEC':
                        if self.secours_allowed:
                                self.stimes[time.time()] = 'secours'
                        else:
                                return None

                if typet == 'S':
                        print '  COMING CALL : __typep_phases__, typep/typet is %s/Secretariat' % typep
                        whattodo = self.__typet_secretariat__(nprof, detail, delaigrp)
                elif typet == 'F':
                        print '  COMING CALL : __typep_phases__, typep/typet is %s/Fichier' % typep
                        whattodo = self.__typet_fichier__(detail)
                elif typet == 'T':
                        print '  COMING CALL : __typep_phases__, typep/typet is %s/Telephone' % typep
                        whattodo = self.__typet_telephone__(detail)
                elif typet == 'B':
                        print '  COMING CALL : __typep_phases__, typep/typet is %s/Base' % typep
                        whattodo = self.__typet_base__(detail)
                elif typet == '0':
                        print '  COMING CALL : __typep_phases__, typep/typet is %s/Aucun' % typep
                        whattodo = None
                else:
                        print '  COMING CALL : __typep_phases__, typep/typet is %s/%s' % (typep, typet)
                        whattodo = None
                return whattodo


        def __typep_rep__(self, clients_profil):
                """
                Handles a 'REP' scenario.
                """
                filename = self.__wavstrip__(clients_profil[8])
                print '  COMING CALL : __typep_rep__', filename
                self.statacd2_tt = 'TT_REP'
                whattodo = {'action'   : 'rep',
                            'delay'    : 0,
                            'argument' : filename}
                return whattodo


        def __typep_mes__(self, clients_profil):
                """
                Handles a 'MES' scenario.
                """
                columns = ('N', 'NCLI', 'NCOL', 'NPROF', 'NomMess', 'Duree', 'Envoie', 'Mail', 'Conserve')
                self.cursor_operat.query('USE %s_clients' % self.socname)
                self.cursor_operat.query('SELECT ${columns} FROM messagerie WHERE NCLI = %s AND NCOL = %s AND NPROF = %s',
                                         columns,
                                         (self.ncli, self.ncol, clients_profil[0]))
                clients_messagerie = self.cursor_operat.fetchall()
                if len(clients_messagerie) > 0:
                        clients_messagerie_item = clients_messagerie[0]

                        print '  COMING CALL : __typep_mes__, typep is MES : %s' % str(clients_messagerie_item)
                        self.statacd2_tt = 'TT_MES'
                        filename = self.__wavstrip__(clients_messagerie_item[4])
                        maxlength = str(clients_messagerie_item[5])
                        msgindice = str(clients_messagerie_item[0])
                        whattodo = {'action'   : 'mes',
                                    'delay'    : 0,
                                    'argument' : ';'.join([filename, maxlength, msgindice])}
                else:
                        whattodo = None
                return whattodo


        def __spanprofiles__(self):
                """
                Handles the steps of an incoming call treatment PH1 / PH2 / REP / MES ...
                """
                print '  COMING CALL : __spanprofiles__ : where s = %s/%s/%s' % (self.whereph, self.wherephS, self.wherephF)

                whattodo = None
                if self.whereph == 'INTRO':
                        if self.dialplan['welcome'] == 1:
                                whattodo = {'action'   : 'intro',
                                            'delay'    : 0,
                                            'argument' : None}
                        self.whereph = 'PH1'

                for [thisstep, nextstep] in ['PH1', 'PH2'], ['PH2', 'SEC'], ['SEC', 'REP']:
                        if whattodo is None:
                                if self.whereph == thisstep:
                                        if thisstep in self.true_clients_profil:
                                                whattodo = self.__typep_phases__(thisstep, self.true_clients_profil.get(thisstep))
                                                if whattodo is None:
                                                        self.whereph = nextstep
                                                        self.wherephS = None
                                                        self.wherephF = None
                                        else:
                                                self.whereph = nextstep

                if whattodo is None:
                        if self.whereph == 'REP':
                                if 'REP' in self.true_clients_profil:
                                        whattodo = self.__typep_rep__(self.true_clients_profil.get('REP'))
                                        if whattodo is None:
                                                self.whereph = 'MES'
                                else:
                                        self.whereph = 'MES'

                if whattodo is None:
                        if self.whereph == 'MES':
                                if 'MES' in self.true_clients_profil:
                                        whattodo = self.__typep_mes__(self.true_clients_profil.get('MES'))
                                        if whattodo is None:
                                                self.whereph = 'END'
                                else:
                                        self.whereph = 'END'

                print '  COMING CALL : __spanprofiles__, before return :', whattodo, self.whereph, self.wherephS, self.wherephF
                return [whattodo, self.whereph]


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


        def findaction(self, upto):
                """
                Looks up the action to perform, after an AGI request has been issued.
                This is a public function entry point (from CallBooster class for instance) for __spanprofiles__().
                """
                whattodo = {'action'   : 'exit',
                            'delay'    : 0,
                            'argument' : None}
                newupto = None
                try:
                        [whattodo, newupto] = self.__spanprofiles__()
                        if whattodo is None:
                                whattodo = {'action'   : 'exit',
                                            'delay'    : 0,
                                            'argument' : None}
                        self.set_timestamp_stat(whattodo['action'])
                except Exception, exc:
                        print 'exception when calling __spanprofiles__() : %s' % str(exc)

                return [whattodo['action'], whattodo['delay'], whattodo['argument'], newupto]

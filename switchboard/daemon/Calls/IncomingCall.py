import socket
import time

PDISPO = u'Pr\xeat'
WEEKDAY = ['LUN', 'MAR', 'MER', 'JEU', 'VEN', 'SAM', 'DIM']
DATEFMT = '%Y-%m-%d'
DATETIMEFMT = DATEFMT + ' %H:%M:%S'

## \brief Logs a message into the Asterisk CLI
# \param txt message to send to the CLI
def print_verbose(txt):
        print txt
        return

class Action:
        def __init__(self, action, delay, arg):
                self.action = action
                self.delay = delay
                self.argument = arg
        def status(self):
                return '[Action : %s/%s/%s]' % (self.action, self.delay, self.argument)


class IncomingCall:
        def __init__(self, conn_agents, conn_system, cidnum, sdanum, queuenum, soperat_socket, soperat_port):
                self.cidnum    = cidnum
                self.sdanum    = sdanum
                self.num       = 0
                self.queuename = 'qcb_%05d' % queuenum
                self.commid    = str(100000 + queuenum)
                self.ctime     = time.localtime()
                #                self.chan = None
                self.conn_system = conn_system
                self.conn_agents = conn_agents
                self.whereph = 'INTRO'
                self.wherephS = None
                self.wherephF = None
                self.nsoc = 0 # None
                self.ncli = 0 # None
                self.ncol = 0 # None
                self.cliname = ''
                self.colname = ''
                self.socname = ''
                self.waiting = True
                self.statacd2_state = 'NC'
                self.statacd2_tt = 'TT_RAF'
                self.parking = None
                self.parkexten = None
                self.peerchannel = None
                self.appelaboute = None
                self.tocall = False
                self.toretrieve = None
                self.stimes = {time.time() : 'init'}
                self.ttimes = {time.time() : 'init'}
                self.uinfo = None
                self.statdone = False

                self.dialplan = {'welcome' : 0,
                                 'callerid' : 1,
                                 'record' : None,
                                 'rescue' : 1,
                                 'rescue_details' : None,
                                 'sun' : 0,
                                 'sounds' : []}
                self.soperat_socket = soperat_socket
                self.soperat_port   = soperat_port


                columns = ('NSDA', 'NSOC', 'NCLI', 'NCOL', 'NOM', 'DateD', 'DateF', 'Valide')
                cursor_system = self.conn_system.cursor()
                cursor_system.query('SELECT ${columns} FROM sda WHERE NSDA = %s',
                                    columns,
                                    self.sdanum)
                results = cursor_system.fetchall()
                if len(results) > 0:
                        self.system_sda = results[0]
                        self.nsoc = str(self.system_sda[1])
                        self.ncli = str(self.system_sda[2])
                        self.ncol = str(self.system_sda[3])

                        columns = ('N', 'NOM', 'ID', 'Dossier')
                        cursor_system = self.conn_system.cursor()
                        cursor_system.query('SELECT ${columns} FROM societes WHERE N = %s',
                                            columns,
                                            self.nsoc)
                        system_societes = cursor_system.fetchall()
                        if len(system_societes) > 0:
                                self.nsoc_global = system_societes[0][2]
                                self.socname = system_societes[0][3]
                        else:
                                self.socname = 'adh_inconnu'

                        self.dialplan['soundpath'] = '%s/%s/%s/Sons' % (self.socname, self.ncli, self.ncol)
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
                self.taxes = triplet

        def setclicolnames(self, conn_clients):
                self.conn_clients = conn_clients
                cursor_clients = self.conn_clients.cursor()

                columns = ('N', 'NLIST')
                cursor_clients.query('SELECT ${columns} FROM clients WHERE N = %s',
                                     columns,
                                     self.ncli)
                results = cursor_clients.fetchall()
                if len(results) > 0:
                        self.cliname = results[0][1]

                columns = ('N', 'NLIST')
                cursor_clients.query('SELECT ${columns} FROM collaborateurs WHERE N = %s',
                                     columns,
                                     self.ncol)
                results = cursor_clients.fetchall()
                if len(results) > 0:
                        self.colname = results[0][1]

        def get_sda_profiles(self, conn_clients, nsda):
                self.elect_competence = None
                self.conn_clients = conn_clients


                columns = ('N', 'NCLI', 'NCOL', 'NSDA', 'Script', 'Flag', 'Priorite', 'CompLocale', 'VoiesSim')
                cursor_clients = self.conn_clients.cursor()
                cursor_clients.query('SELECT ${columns} FROM sda WHERE NSDA = %s',
                                     columns,
                                     self.sdanum)
                results = cursor_clients.fetchall()
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
                        if simult > -1 and nsda >= simult:
                                self.statacd2_tt = 'TT_ASD'
                                return False
                else:
                        self.statacd2_tt = 'TT_SND'
                        return False

                columns = ('NSOC', 'DATEJ', 'TCOMME')
                cursor_system = self.conn_system.cursor()
                cursor_system.query('SELECT ${columns} FROM jferies WHERE NSOC = %s',
                                    columns,
                                    self.nsoc)
                results = cursor_system.fetchall()

                self.period = ['JOUR', '']
                weekday_today = WEEKDAY[self.ctime[6]]
                if weekday_today == 'SAM' or weekday_today == 'DIM':
                        self.period[1] = 'WE'
                today_ferie = time.strftime('%d-%m', self.ctime)
                for z in results:
                        if z[1] == today_ferie:
                                self.period[1] = 'FERIE'
                                weekday_today = z[2]
                                break
                print 'jferies ?', today_ferie, weekday_today, self.period

                self.competences = []
                self.languages_sv = []
                self.languages = {}

                # fichiers sons
                columns = ('NCLI', 'NCOL', 'Nom')
                cursor_clients = self.conn_clients.cursor()
                cursor_clients.query('SELECT ${columns} FROM son WHERE NCLI = %s AND NCOL = %s',
                                     columns,
                                     (self.ncli, self.ncol))
                results = cursor_clients.fetchall()
                for t in results:
                        self.dialplan['sounds'].append(t[2])

                # competences
                columns = ('NCLI', 'NCOL', 'NComp')
                cursor_clients = self.conn_clients.cursor()
                cursor_clients.query('SELECT ${columns} FROM clicomp WHERE NCLI = %s AND NCOL = %s',
                                     columns,
                                     (self.ncli, self.ncol))
                results = cursor_clients.fetchall()
                for t in results:
                        self.competences.append(str(t[2]))

                # langues
                columns = ('NCLI', 'NCOL', 'NLang', 'Niveau')
                cursor_clients = self.conn_clients.cursor()
                cursor_clients.query('SELECT ${columns} FROM clilang WHERE NCLI = %s AND NCOL = %s',
                                     columns,
                                     (self.ncli, self.ncol))
                results = cursor_clients.fetchall()
                for t in results:
                        self.languages[str(t[2])] = t[3]
                        self.languages_sv.append('%d-%s' %(t[2], t[3]))

                print 'INCOMING CALL ... comp =', self.competences, 'lang =', self.languages

                # Description des profils
                # Mode Secours ? no_rescue
                # Mode non secours
                columns = ('N', 'NCLI', 'NCOL', 'TypeP', 'JOUR', 'PlgD', 'PlgF', 'TypeT', 'Detail', 'Ordre', 'DelaiGRP', 'DATED', 'DATEF')

                cursor_clients = self.conn_clients.cursor()
                cursor_clients.query("SELECT ${columns} FROM profil WHERE TypeP = 'SEC' AND NCLI = 0 AND NCOL = 0",
                                     columns)
                results = cursor_clients.fetchall()
                cursor_clients.query('SELECT ${columns} FROM profil WHERE NCLI = %s AND NCOL = %s',
                                     columns,
                                     [self.ncli, self.ncol])
                results.extend(cursor_clients.fetchall())

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
                                        if self.ctime > pld and self.ctime < plf:
                                                typep = clients_profil[3]
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
                        for typep in self.true_clients_profil:
                                print 'INCOMING CALL : phases', typep, self.true_clients_profil[typep]


                        if self.dialplan['sun']:
                                if 'M001' in self.dialplan['sounds']:
                                        self.dialplan['welcomefile'] = '%s/%s' % (self.dialplan['soundpath'], 'M001')
                                else:
                                        self.dialplan['welcomefile'] = None
                        else:
                                self.period[0] = 'NUIT'
                                if 'M003' in self.dialplan['sounds']:
                                        self.dialplan['welcomefile'] = '%s/%s' % (self.dialplan['soundpath'], 'M003')
                                elif 'M001' in self.dialplan['sounds']:
                                        self.dialplan['welcomefile'] = '%s/%s' % (self.dialplan['soundpath'], 'M001')
                                else:
                                        self.dialplan['welcomefile'] = None
                        print 'welcomefile', self.dialplan['welcomefile']

                        if len(self.true_clients_profil) == 0:
                                self.statacd2_tt = 'TT_SND'
                else:
                        self.statacd2_tt = 'TT_SND'

                # print out a summary of dialplan statuses
                print 'INCOMING CALL : dialplan settings =', self.dialplan

                return True


        def __local_group_composition(self):
                columns = ('CODE', 'NOM', 'GL')
                cursor_agents = self.conn_agents.cursor()
                cursor_agents.query('SELECT ${columns} FROM agents WHERE GL = 1',
                                    columns)
                agents_agents = cursor_agents.fetchall()
                lstlocal = []
                if len(agents_agents) > 0:
                        for ag in agents_agents:
                                lstlocal.append(ag[1])
                return lstlocal


        def __localN_group_composition(self, nprof, id):
                columns = ('NPERM', 'NGROUP', 'NOM', 'NPROF')
                cursor_clients = self.conn_clients.cursor()
                cursor_clients.query('SELECT ${columns} FROM groupes WHERE NGROUP = %s',
                                     columns,
                                     id)
                results = cursor_clients.fetchall()
                list_operators = []
                list_requests  = []

                print '__localN_group_composition', id, results
                for clients_groupes in results:
                        nperm  = clients_groupes[0]
                        ngroup = clients_groupes[1]
                        nom    = clients_groupes[2]
                        if ngroup < 0 and nom[0] != '%':
                                list_operators.append(nom)
                        elif ngroup < 0 and nom[0] == '%':
                                nom_groupe = nom[1:]
                                columns = ('Nom', 'NOPE', 'NomOPE')
                                cursor_clients = self.conn_clients.cursor()
                                cursor_clients.query('SELECT ${columns} FROM groupes_ope WHERE Nom = %s AND NOPE != 0',
                                                     columns,
                                                     nom_groupe)
                                results = cursor_clients.fetchall()
                                for res in results:
                                        list_operators.append(res[2])
                        elif ngroup > 0:
                                list_requests.append([str(self.nsoc_global),
                                                      self.ncli,
                                                      self.ncol,
                                                      self.sdanum,
                                                      self.commid,
                                                      str(ngroup),
                                                      self.cidnum,
                                                      str(nperm),
                                                      ','.join(self.competences),
                                                      ','.join(self.languages_sv),
                                                      self.commid])

                return [list_operators, list_requests]


        def check_operator_status(self, operatorname):
                columns = ('CODE', 'NOM', 'AGPI')
                # AGPI : records outgoing calls
                cursor_agents = self.conn_agents.cursor()
                cursor_agents.query('SELECT ${columns} FROM agents WHERE NOM = %s',
                                    columns,
                                    operatorname)
                agents_agents = cursor_agents.fetchall()
                if len(agents_agents) > 0:
                        operator_code = agents_agents[0][0]

                        agent_comp = []
                        columns = ('Nagt', 'NComp')
                        cursor_agents = self.conn_agents.cursor()
                        cursor_agents.query('SELECT ${columns} FROM agtcomp WHERE Nagt = %s',
                                            columns,
                                            operator_code)
                        agents_agtcomp = cursor_agents.fetchall()
                        for t in agents_agtcomp:
                                agent_comp.append(t[1])

                        agent_lang = {}
                        columns = ('Nagt', 'NLang', 'Niveau')
                        cursor_agents = self.conn_agents.cursor()
                        cursor_agents.query('SELECT ${columns} FROM agtlang WHERE Nagt = %s',
                                            columns,
                                            operator_code)
                        agents_agtlang = cursor_agents.fetchall()
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
                                print operatorname, ':', language_allowed, comp_allowed
                                return None

                        columns = ('NOPE', 'AdrNet', 'CompLocale', 'Etat', 'Voie', 'NbT8', 'NbT4', 'NbT2', 'NbT1', 'Niveau', 'Derange', 'Priorite', 'NoDecroche')
                        cursor_agents = self.conn_agents.cursor()
                        cursor_agents.query("SELECT ${columns} FROM acd WHERE NOPE = %s AND Derange != 0 AND Etat != 'Sortie' AND CompLocale >= %s",
                                            columns,
                                            (operator_code, self.elect_competence))
                        agents_acd = cursor_agents.fetchall()
                        if len(agents_acd) > 0:
                                dispo = agents_acd[0]
                                if dispo[3] == PDISPO:
                                        truedispo = 'Pret'
                                else:
                                        truedispo = dispo[3]

                                if truedispo == 'Pret':
                                        status = 'Pret%d' % dispo[4]
                                else:
                                        status = truedispo
                                return [status, dispo[1], dispo[9], dispo[11]]
                        else:
                                return None
                else:
                        print_verbose('no match for %s in agents.agents' % operatorname)
                        return None


        def __list_operators(self, nprof, detail_tab, upto):
                """
                Returns the list of agents, as well as thir status, that are available for the groups defined from
                index 0 to upto.
                """
                listopers = []
                listsv    = []
                print '__list_operators', upto
                for num in xrange(upto):
                        id = detail_tab[num]
                        if id == '0': # local group
                                for lgc in self.__local_group_composition():
                                        if lgc not in listopers:
                                                listopers.append(lgc)
                        else: # localN and globalN groups
                                [lopers, lsv] = self.__localN_group_composition(nprof, id)
                                for lgc in lopers:
                                        if lgc not in listopers:
                                                listopers.append(lgc)
                                listsv.extend(lsv)

                # should we return the ones which are in 'sortie' status there ?
                return [listopers, listsv]


        def __typet_secretariat(self, nprof, detail, delaigrp):
                whattodo = None
                if len(detail) == 0:
                        detail = '0'

                detail_tab = detail.split(',')
                delaigrp_tab = delaigrp.split(',')
                ngroupes = len(detail_tab)
                if self.wherephS is None:
                        upto = 1
                else:
                        upto = self.wherephS + 1

                print '__typet_secretariat, upto = ', upto
                while True: # loop over the detailed items
                                self.wherephS = upto
                                if upto > ngroupes:
                                        whattodo = None
                                        break

                                print 'prevnum = %d / %s' % (self.wherephS, detail_tab[0:upto+1])
                                [self.list_operators, self.list_svirt] = self.__list_operators(nprof, detail_tab, upto)
                                print 'operators and rooms :', self.list_operators, self.list_svirt

                                if upto == ngroupes or len(self.list_operators) == 0 and len(self.list_svirt) == 0:
                                        delay = 0
                                else:
                                        delay = int(delaigrp_tab[upto])

                                print '__typet_secretariat, upto = ', upto, delay

                                if len(self.list_operators) == 0 and len(self.list_svirt) == 0:
                                        upto += 1
                                else:
                                        whattodo = Action('sec', delay, self.dialplan['welcomefile'])
                                        for areq in self.list_svirt:
                                                req = 'ACDAddRequest' + chr(2) + chr(2).join(areq[:6]) + chr(2) + chr(2).join(areq[6:]) + chr(2) + str(self.soperat_port) + chr(3)
                                                print 'cg', self.nsoc, self.ncli, delay, self.ncol, areq, '<%s>' % req
                                                self.soperat_socket.send(req)
                                        break
                                
                if whattodo is not None:
                        print '__typet_secretariat : ', upto, whattodo.status()
                else:
                        print '__typet_secretariat : ', upto, whattodo
                return whattodo


        def __get_fichier(self, detail):
                columns = ('NCLI', 'NCOL', 'NOM', 'NTEL', 'TypeN', 'ModeA', 'NbC', 'NbP', 'DSonn', 'SDA', 'Ordre')
                cursor_clients = self.conn_clients.cursor()
                if self.whereph == 'SEC':
                        cursor_clients.query('SELECT ${columns} FROM fichier WHERE NCLI = 0 AND NCOL = 0 AND NOM = %s',
                                             columns,
                                             (detail))
                else:
                        cursor_clients.query('SELECT ${columns} FROM fichier WHERE NCLI = %s AND NCOL = %s AND NOM = %s',
                                             columns,
                                             (self.ncli, self.ncol, detail))
                results = cursor_clients.fetchall()
                return results


        def __typet_fichier(self, detail):
                results = self.__get_fichier(detail)
                nresults = len(results)
                if nresults > 0:
                        if self.wherephF is None:
                                self.wherephF = 0
                        else:
                                self.wherephF += 1
                        if self.wherephF < nresults:
                                self.statacd2_tt = 'TT_FIC'
                                b = results[self.wherephF]
                                print_verbose('Fichier %s : %s %s %s %s' % (str(b), b[3], b[6], b[7], b[8]))
                                whattodo = Action('fic', 0, '%s-%s-%s-%s' %(b[3], b[8], b[6], b[7]))
                        else:
                                whattodo = None
                else:
                        whattodo = None
                return whattodo


        def __typet_telephone(self, detail):
                results = self.__get_fichier(detail)
                nresults = len(results)
                if nresults > 0:
                        if self.wherephF is None:
                                self.statacd2_tt = 'TT_TEL'
                                self.wherephF = 0
                                b = results[0]
                                print_verbose('Telephone %s : %s %s %s %s' % (str(b), b[3], b[6], b[7], b[8]))
                                whattodo = Action('tel', 0, '%s-%s-%s-%s' %(b[3], b[8], b[6], b[7]))
                        else:
                                whattodo = None
                else:
                        whattodo = None
                return whattodo


        def __typet_base(self, detail):
                whattodo = None
                columns = ('NCLI', 'NCOL', 'Nom', 'DateJ', 'Plages', 'Plage1', 'Plage2', 'Plage3', 'Plage4')
                cursor_clients = self.conn_clients.cursor()
                cursor_clients.query('SELECT ${columns} FROM gardes WHERE NCLI = %s AND NCOL = 0 AND Nom = %s',
                                     columns,
                                     (self.ncli, detail)) # (self.ncli, self.ncol, detail))
                results = cursor_clients.fetchall()
                if len(results) > 0 and self.wherephF is None:
                        nows = time.mktime(self.ctime)
                        str_today = time.strftime(DATEFMT, self.ctime)
                        realidx = None
                        for b in results:
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

                        if realidx is not None:
                                fichenum = realbase[5 + realidx].split('.')[0]
                                print 'Base', now, plbase, realidx, realbase
                                if fichenum != '0':
                                        self.statacd2_tt = 'TT_BAS'
                                        self.wherephF = 0
                                        columns = ('N', 'NCLI', 'Nom', 'Tel1', 'Tel2', 'Tel3', 'Tel4', 'Tel5', 'Tel6', 'TelSel')
                                        cursor_clients = self.conn_clients.cursor()
                                        cursor_clients.query('SELECT ${columns} FROM fichesgarde WHERE N = %s AND NCLI = %s',
                                                             columns,
                                                             (fichenum, self.ncli))
                                        res2 = cursor_clients.fetchall()
                                        if len(res2) > 0:
                                                res3 = res2[0]
                                                which = res3[9]
                                                fg = res3[2 + which]
                                                [num, intext, normstd, ncherche, npatiente, withsda, wtime] = fg.split('.')
                                                print 'intext = %s, normstd = %s, num = %s, ncherche = %s, npatiente = %s, time = %s, withsda = %s' \
                                                      %(intext, normstd, num, ncherche, npatiente, wtime, withsda)
                                                whattodo = Action('bas', 0, '-'.join([num, wtime, ncherche, npatiente]))

                return whattodo


        def __spanprofiles_secretariat(self):
                """
                To be called by the main loop, for each incoming call, in order to decide later on who will be chosen.
                """


        def __typep_phases(self, typep, clients_profil):
                nprof = clients_profil[0]
                typet = clients_profil[7]
                detail = clients_profil[8]
                delaigrp = clients_profil[10]
                if typet == 'S':
                        print_verbose('typep/typet is %s/Secretariat' % typep)
                        whattodo = self.__typet_secretariat(nprof, detail, delaigrp)
                elif typet == 'F':
                        print_verbose('typep/typet is %s/Fichier' % typep)
                        whattodo = self.__typet_fichier(detail)
                elif typet == 'T':
                        print_verbose('typep/typet is %s/Telephone' % typep)
                        whattodo = self.__typet_telephone(detail)
                elif typet == 'B':
                        print_verbose('typep/typet is %s/Base' % typep)
                        whattodo = self.__typet_base(detail)
                elif typet == '0':
                        print_verbose('typep/typet is %s/Aucun' % typep)
                        whattodo = None
                else:
                        print_verbose('typep/typet is %s/%s' % (typep, typet))
                        whattodo = None
                return whattodo


        def __typep_rep(self, clients_profil):
                filename = '%s/%s/%s/Repondeurs/%s' % (self.socname, self.ncli, self.ncol, clients_profil[8])
                print '__typep_rep', filename
                # removing the .wav => [:-4]
                whattodo = Action('rep', 0, filename[:-4])
                return whattodo


        def __typep_mes(self, clients_profil):
                columns = ('NCLI', 'NCOL', 'NPROF', 'Duree', 'Envoie', 'Mail', 'Conserve')
                cursor_clients = self.conn_clients.cursor()
                cursor_clients.query('SELECT ${columns} FROM messagerie WHERE NCLI = %s AND NCOL = %s AND NPROF = %s',
                                     columns,
                                     (self.ncli, self.ncol, clients_profil[0]))
                results = cursor_clients.fetchall()
                if len(results) > 0:
                        print_verbose('typep is MES : %s' % str(results[0]))
                        self.statacd2_tt = 'TT_MES'
                        whattodo = Action('mes', 0, None)
                else:
                        whattodo = None
                return whattodo


        def __spanprofiles(self):
                print 'INCOMING CALL : __spanprofiles : where\'s = %s/%s/%s' % (self.whereph, self.wherephS, self.wherephF)

                whattodo = None
                if self.whereph == 'INTRO':
                        if self.dialplan['welcome'] == 1 and self.dialplan['welcomefile'] is not None:
                                whattodo = Action('intro', 0, self.dialplan['welcomefile'])
                        self.whereph = 'PH1'

                for [thisstep, nextstep] in ['PH1', 'PH2'], ['PH2', 'SEC'], ['SEC', 'REP']:
                        if whattodo is None:
                                if self.whereph == thisstep:
                                        if thisstep in self.true_clients_profil:
                                                whattodo = self.__typep_phases(thisstep, self.true_clients_profil.get(thisstep))
                                                if whattodo is None:
                                                        self.whereph = nextstep
                                                        self.wherephS = None
                                                        self.wherephF = None
                                        else:
                                                self.whereph = nextstep

                if whattodo is None:
                        if self.whereph == 'REP':
                                if 'REP' in self.true_clients_profil:
                                        whattodo = self.__typep_rep(self.true_clients_profil.get('REP'))
                                        if whattodo is None:
                                                self.whereph = 'MES'
                                else:
                                        self.whereph = 'MES'

                if whattodo is None:
                        if self.whereph == 'MES':
                                if 'MES' in self.true_clients_profil:
                                        whattodo = self.__typep_mes(self.true_clients_profil.get('MES'))
                                        if whattodo is None:
                                                self.whereph = 'END'
                                else:
                                        self.whereph = 'END'

                if whattodo is None:
                        # if we get there nothing has been found
                        print 'INCOMING CALL : (before return) :', whattodo, self.whereph, self.wherephS, self.wherephF
                else:
                        print 'INCOMING CALL : (before return) :', whattodo.status(), self.whereph, self.wherephS, self.wherephF
                return [whattodo, self.whereph]


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


        # called from elect() (called from push AGI)
        def findaction(self, upto):
                whattodo = Action('exit', 0, None)
                newupto = None
                try:
                        [whattodo, newupto] = self.__spanprofiles()
                        self.set_timestamp_stat(whattodo.action)
                except Exception, exc:
                        print 'exception when calling __spanprofiles() : %s' % str(exc)

                return [whattodo.action, whattodo.delay, whattodo.argument, newupto]

        def showstatus(self):
                print 'showstatus', self.cidnum, self.__spanprofiles()

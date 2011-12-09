# vim: set fileencoding=utf-8 :
# XiVO CTI Server

__version__   = '$Revision$'
__date__      = '$Date$'
__copyright__ = 'Copyright (C) 2007-2011 Proformatique'
__author__    = 'Corentin Le Gall'

# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.
#
# Alternatively, XiVO CTI Server is available under other licenses directly
# contracted with Pro-formatique SARL. See the LICENSE file at top of the
# source tree or delivered in the installable package in which XiVO CTI Server
# is distributed for more details.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

import cjson
import logging
import os
import time
import urllib
from xivo_ctiservers import xivo_records_db

LENGTH_AGENT = 6
ASTERISK_RECORDS_PATH = '/var/spool/asterisk/monitor'

log = logging.getLogger('records_base')

class XivoRecords():

    HIERARCHY_MIN = 0
    HIERARCHY_AGENT = 1
    HIERARCHY_SUPER = 2
    HIERARCHY_ADMIN = 3
    HIERARCHY_MAX = 4

    # rec states : rec_started -> rec_notag -> rec_topurge -> rec_tokeep
    #                                          rec_tokeep  -> rec_topurge
    #                                                             -> auto_purged
    #                                       -> manual_purged


    def __init__(self, cset, dbpath):
        self.cset = cset
        self.recordcampaignconfig = {}
        self.recorded_channels = {}
        self.records_db_path = dbpath
        self.records_db_table = 'callcenter_campaigns_records'
        self.cron_filename = '/etc/cron.d/pf-xivo-cti-records-purge'
        self.records_db = xivo_records_db.Records(self.records_db_path,
                                                  self.records_db_table)
        return


    def __level_rules__(self, hlevel_r, hlevel_i, action):
        allowed = False
        if action == 'info':
            if (hlevel_r > hlevel_i) or (hlevel_r == hlevel_i and hlevel_r > self.HIERARCHY_AGENT):
                allowed = True
        elif action in ['read', 'tag', 'comment']:
            if hlevel_r > hlevel_i:
                allowed = True
        elif action == 'remove':
            if hlevel_r == self.HIERARCHY_ADMIN and hlevel_r > hlevel_i:
                allowed = True
        return allowed


    def __hierarchy_value__(self, userinfo, defaultvalue):
        """
        Returns the hierarchy level according to capabilities
        definitions.
        """
        profileid = userinfo.get('capaid')
        capa = self.cset.capas.get(profileid)
        ucapa = capa.all()
        hv = defaultvalue
        # capa.capafuncs gives the actual list of capabilities
        if capa.match_funcs(ucapa, 'agents'):
            hv = self.HIERARCHY_AGENT
        if capa.match_funcs(ucapa, 'supervisor'):
            hv = self.HIERARCHY_SUPER
        if capa.match_funcs(ucapa, 'administrator'):
            hv = self.HIERARCHY_ADMIN
        return hv


    def __check_rights__(self, astid, userinfo, hlevel_r, agentnumbers, action):
        allowed = False
        for agentchannel in agentnumbers.split(','):
            if agentchannel.startswith('Agent/'):
                agentnumber = agentchannel[LENGTH_AGENT:]
                agentid = self.cset.__find_agentid_by_agentnum__(astid, agentnumber)
                if agentid:
                    zs = self.cset.__find_userinfos_by_agentid__(astid, agentid)
                    if userinfo in zs:
                        if action != 'remove':
                            allowed = True
                            break
                    for z in zs:
                        # if the requested does not match a hierarchy indicator,
                        # give him the highest hierarchy
                        hlevel_i = self.__hierarchy_value__(z, self.HIERARCHY_MAX)
                        log.info('request %s : levels are %s and %s' % (action, hlevel_r, hlevel_i))
                        if self.__level_rules__(hlevel_r, hlevel_i, action):
                            allowed = True
                else:
                    log.warning('no agentid for agent channel %s' % agentchannel)
            else:
                log.warning('agent channel %s is not an agent' % agentchannel)

        return allowed


    def records_campaign(self, userinfo, command):
        repstr = ''
        astid = userinfo.get('astid')
        function = command.get('function')

        # if the requester does not match a hierarchy indicator,
        # give him the lowest hierarchy
        hlevel_r = self.__hierarchy_value__(userinfo, self.HIERARCHY_MIN)
        log.info('requester %s : hierarchy level is %s' % (userinfo.get('user'), hlevel_r))

        if function in ['search']:
            searchitems = command.get('searchitems')
            requested = ('id', 'uniqueid', 'channel',
                         'filename',
                         'callstart', 'callstop', 'callduration',
                         'calleridnum', 'queuenames',
                         'agentnumbers', 'agentnames',
                         'direction', 'skillrules',
                         'recordstatus',
                         'svientries', 'svivariables', 'svichoices',
                         'callrecordtag', 'callrecordcomment')
            results = self.records_db.get(requested)
            finalresults = list()
            for resultitem in results:
                callrecordcomment = resultitem.get('callrecordcomment')
                if callrecordcomment and isinstance(callrecordcomment, str):
                    resultitem['callrecordcomment'] = callrecordcomment.decode('utf8')
                appendme_or = False
                appendme_and = True
                searchoperator = command.get('searchoperator')
                for searchitem in searchitems:
                    searchkind = searchitem.get('searchkind')
                    searchfield = searchitem.get('searchfield')
                    if searchkind == 0:
                        if resultitem.get('agentnames').lower().find(searchfield.lower()) >= 0:
                            appendme_or = True
                            continue
                        else:
                            appendme_and = False
                    elif searchkind == 1:
                        if resultitem.get('queuenames').lower().find(searchfield.lower()) >= 0:
                            appendme_or = True
                            continue
                        else:
                            appendme_and = False
                    elif searchkind == 2:
                        if resultitem.get('skillrules').lower().find(searchfield.lower()) >= 0:
                            appendme_or = True
                            continue
                        else:
                            appendme_and = False
                    elif searchkind == 3:
                        if resultitem.get('direction').lower() == searchfield.lower():
                            appendme_or = True
                            continue
                        else:
                            appendme_and = False

                if appendme_or and searchoperator == 'or' or appendme_and and searchoperator == 'and':
                    allowed = self.__check_rights__(astid, userinfo, hlevel_r,
                                                    resultitem.get('agentnumbers'), 'info')
                    if allowed:
                        resultitem['id'] = '%06d' % resultitem.get('id')
                        finalresults.append(resultitem)

            tosend = { 'class' : 'records-campaign',
                       'function' : function,
                       'payload' : finalresults
                       }
            repstr = self.cset.__cjson_encode__(tosend)

        elif function in ['getprops']:
            tosend = { 'class' : 'records-campaign',
                       'function' : function,
                       'tags' : self.recordcampaignconfig.get('tags')
                       }
            repstr = self.cset.__cjson_encode__(tosend)

        elif function in ['tag']:
            idv = command.get('id')
            tag = command.get('tag')
            retvalue = 'ko-unknown'

            calldata = { 'id' : idv }
            requested = ('id', 'agentnumbers', 'recordstatus', 'callrecordtag', 'filename')
            resultitem = self.records_db.get_one_record(calldata, requested)

            agentnumbers = resultitem.get('agentnumbers')
            recordstatus = resultitem.get('recordstatus')
            callrecordtag = resultitem.get('callrecordtag')

            allowed = self.__check_rights__(astid, userinfo, hlevel_r, agentnumbers, 'tag')
            if allowed:
                tagdefs = self.recordcampaignconfig.get('tags')
                if tag in tagdefs:
                    action = tagdefs.get(tag).get('action')
                    log.info('tag request from %s on id %s : tag=%s => action=%s'
                             % (userinfo.get('user'), idv, tag, action))

                    if action == 'keep':
                        if recordstatus in ['rec_notag', 'rec_topurge']:
                            calldata = {'callrecordtag' : tag}
                            calldata.update({'recordstatus' : 'rec_tokeep'})
                            self.records_db.update_call(idv, calldata)
                            retvalue = 'ok'
                            log.info('RECCAMP:manual:%s:%s:%s:%s'
                                     % (function, action, userinfo.get('user'), idv))
                        else:
                            retvalue = 'ko:badstatus:%s' % recordstatus
                            log.warning('bad current recordstatus %s for action %s'
                                        % (recordstatus, action))

                    elif action == 'purge':
                        if recordstatus in ['rec_notag', 'rec_tokeep']:
                            calldata = {'callrecordtag' : tag}
                            calldata.update({'recordstatus' : 'rec_topurge'})
                            self.records_db.update_call(idv, calldata)
                            retvalue = 'ok'
                            log.info('RECCAMP:manual:%s:%s:%s:%s'
                                     % (function, action, userinfo.get('user'), idv))
                        else:
                            retvalue = 'ko:badstatus:%s' % recordstatus
                            log.warning('bad current recordstatus %s for action %s'
                                        % (recordstatus, action))

                    elif action == 'removenow':
                        if recordstatus in ['rec_notag', 'rec_topurge', 'rec_tokeep']:
                            calldata = {'callrecordtag' : tag}
                            calldata.update({'recordstatus' : 'manual_purged'})
                            # XXX if the file can not be deleted, update the status anyway ?
                            if self.__remove_files__(resultitem):
                                self.records_db.update_call(idv, calldata)
                                log.info('RECCAMP:manual:%s:%s:%s:%s'
                                         % (function, action, userinfo.get('user'), idv))
                                retvalue = 'ok'
                            else:
                                retvalue = 'ko:nofile'
                        else:
                            retvalue = 'ko:badstatus:%s' % recordstatus
                            log.warning('bad current recordstatus %s for action %s'
                                        % (recordstatus, action))
                    else:
                        retvalue = 'ko:badaction:%s' % action
                        log.warning('unknown action request %s from %s on id %s'
                                    % (action, userinfo.get('user'), idv))
                else:
                    retvalue = 'ko:badtag:%s' % tag
                    log.warning('unknown tag request %s from %s on id %s'
                                % (tag, userinfo.get('user'), idv))
            else:
                retvalue = 'ko:unallowed'
                log.warning('unallowed tag request from %s on id %s : tag=%s'
                            % (userinfo.get('user'), idv, tag))

            tosend = { 'class' : 'records-campaign',
                       'function' : function,
                       'id' : idv,
                       'returncode' : retvalue
                       }
            repstr = self.cset.__cjson_encode__(tosend)

        elif function in ['comment']:
            idv = command.get('id')
            comment = command.get('comment')
            retvalue = 'ko:unknown'

            calldata = { 'id' : idv }
            requested = ('agentnumbers',)
            agentnumbers = self.records_db.get_one_record(calldata, requested).get('agentnumbers')
            allowed = self.__check_rights__(astid, userinfo, hlevel_r, agentnumbers, 'comment')
            if allowed:
                calldata = {'callrecordcomment' : comment}
                self.records_db.update_call(idv, calldata)
                retvalue = 'ok'
                log.info('RECCAMP:manual:%s:-:%s:%s' % (function, userinfo.get('user'), idv))
            else:
                retvalue = 'ko:unallowed'
                log.warning('unallowed comment request from %s on id %s : comment=%s'
                            % (userinfo.get('user'), idv, comment))

            tosend = { 'class' : 'records-campaign',
                       'function' : function,
                       'id' : idv,
                       'returncode' : retvalue
                       }
            repstr = self.cset.__cjson_encode__(tosend)

        elif function in ['read']:
            idv = command.get('id')
            retvalue = 'ko:unknown'

            calldata = { 'id' : idv }
            requested = ('agentnumbers',)
            agentnumbers = self.records_db.get_one_record(calldata, requested).get('agentnumbers')
            allowed = self.__check_rights__(astid, userinfo, hlevel_r, agentnumbers, 'read')
            if allowed:
                subfunction = command.get('subfunction')
                if subfunction in ['fetch', 'play', 'forward', 'rewind']:
                    pass
            else:
                retvalue = 'ko:unallowed'

            tosend = { 'class' : 'records-campaign',
                       'function' : function,
                       'id' : idv,
                       'returncode' : retvalue
                       }
            repstr = self.cset.__cjson_encode__(tosend)

        return repstr


    def __make_cron__(self):
        tmpfilename = '/tmp/cron_xivo_records_purge.%s' % os.getpid()
        try:
            cfgpurges = self.recordcampaignconfig.get('purges')
            cronpurge = {}
            cronpurge['syst untagged'] = cfgpurges.get('syst').get('untagged')
            cronpurge['syst tagged'] = cfgpurges.get('syst').get('tagged')
            cronpurge['punct'] = cfgpurges.get('punct')

            outfile = open(tmpfilename, 'w')
            print >>outfile, '# this file has been generated by the CTI server on %s' % time.asctime()
            print >>outfile, 'MAILTO=""'
            for k, v in cronpurge.iteritems():
                try:
                    (cron_hour, cron_min) = v.get('when').split(':')
                    action = 'echo cron-actions %s | nc -q 1 localhost 5005' % k
                    when = '%s %s * * *' % (cron_min, cron_hour)
                    print >>outfile, '%s root %s' % (when, action)
                except:
                    log.exception('problem when reading item %s %s' % (k, v))
            outfile.close()

            try:
                os.rename(tmpfilename, self.cron_filename)
            except:
                log.exception('moving %s to %s' % (tmpfilename, self.cron_filename))
        except:
            log.exception('trying to update %s' % cronfilename)


    def fetch_config(self, astid):
        try:
            self.configurl = self.cset.weblist.get('callcenter_campaigns').get(astid).url
            log.info('fetching records configuration ... from %s' % self.configurl)
            urlobj = urllib.urlopen(self.configurl)
            rl = urlobj.readlines()
            urlobj.close()
            flattext = ''.join(rl)
            self.recordcampaignconfig = cjson.decode(flattext)
            self.__make_cron__()
        except:
            log.exception('fetch_config')
            self.recordcampaignconfig = {}
        return


    def __lsdir__(self, any_path):
        # looking up the path
        lsdir = None
        try:
            if os.path.isdir(any_path):
                lsdir = os.listdir(any_path)
                if not lsdir:
                    log.warning('__lsdir__ : %s directory is empty' % any_path)
            else:
                log.warning('__lsdir__ : %s does not exist as a directory' % any_path)
        except:
            log.exception('__lsdir__ : %s' % any_path)

        return lsdir


    def __remove_files__(self, resultitem):
        ret = False
        fullfilename = resultitem.get('filename')
        record_path = os.path.dirname(fullfilename)
        record_base = os.path.basename(fullfilename)
        lsdir = self.__lsdir__(record_path)
        if lsdir:
            idv = resultitem.get('id')
            prefix = record_base[:-4]
            infile = '%s-in.wav' % prefix
            outfile = '%s-out.wav' % prefix

            if infile in lsdir:
                log.info('%s file is in %s, removing it now' % (infile, record_path))
                os.unlink('%s/%s-in.wav' % (record_path, prefix))
                ret = True

                calldata = { 'recordstatus' : 'auto_purged' }
                self.records_db.update_call(idv, calldata)
            else:
                log.warning('did not find %s' % infile)

            if outfile in lsdir:
                log.info('%s file is in %s, removing it now' % (outfile, record_path))
                os.unlink('%s/%s-out.wav' % (record_path, prefix))
                ret = ret & True
            else:
                log.warning('did not find %s' % infile)
        return ret

    def purge_records(self, arguments):
        # campaign

        sp = None
        tu = None
        if len(arguments) > 0:
            sp = arguments[0]
        if len(arguments) > 1:
            tu = arguments[1]

        if sp not in ['syst', 'punct']:
            return
        if sp == 'syst' and tu not in ['tagged', 'untagged']:
            return

        cfgpurge = self.recordcampaignconfig.get('purges')

        if tu:
            delay = cfgpurge.get(sp).get(tu).get('delay')
        else:
            delay = cfgpurge.get(sp).get('delay')
        dbrequest = sp[0].upper()

        # database fetch for matching results
        ddate = time.time() - delay
        ascdate = time.asctime(time.localtime(ddate))
        log.info('looking for calls beginning before now - N=%d seconds (%s) + "%s"'
                 % (delay, ascdate, dbrequest))
        requested = ('id', 'uniqueid', 'channel',
                     'callstop', 'callstart',
                     'filename', 'callstatus', 'recordstatus')
        res = self.records_db.get_before_date(requested, { 'campaignkind' : dbrequest }, ddate)
        log.info('found %d results matching the request (%s, "%s")' % (len(res), ascdate, dbrequest))

        for resultitem in res:
            dopurge = False
            recordstatus = resultitem.get('recordstatus')
            idv = resultitem.get('id')
            if recordstatus == 'rec_topurge':
                dopurge = True
            elif recordstatus == 'rec_notag':
                action = self.recordcampaignconfig.get('tags').get('notag').get('action')
                if action == 'purge':
                    dopurge = True
                else:
                    log.warning('record status is %s and action %s : do not purge'
                                % (recordstatus, action))
            else:
                log.warning('record status is %s : do not purge' % recordstatus)

            if dopurge:
                if self.__remove_files__(resultitem):
                    log.info('RECCAMP:auto:remove:-:-:%s' % (action, idv))

        return


    def __match_filters__(self, filters,
                          direction, queueid, agentid, skillrule):
        log.info('__match_filter__ %s : %s %s %s %s'
                 % (filters, direction, queueid, agentid, skillrule))

        # the result is an and between filters, which are true when not defined

        filters_directions = filters.get('directions')
        dmd = True
        if filters_directions and direction not in filters_directions:
            dmd = False

        filters_queues = filters.get('queues')
        dmq = True
        if filters_queues and int(queueid) not in filters_queues:
            dmq = False

        filters_agents = filters.get('agents')
        dma = True
        if filters_agents and int(agentid) not in filters_agents:
            dma = False

        filters_skillrules = filters.get('competences')
        dmc = False
        if not filters_skillrules:
            dmc = True
        else:
            # it is enough to have one of the defined skillrules to match
            for fc in self.__skillvars__(skillrule):
                if fc in filters_skillrules:
                    dmc = True
                    break

        log.info('result is %s %s %s %s'
                 % (dmd, dmq, dma, dmc))
        domatches = (dmd and dmq and dma and dmc)
        return domatches


    def __skillrule_to_dict__(self, skillrule):
        bracketstart = skillrule.find('(')
        skillrule_dict = { 'rule' : skillrule[:bracketstart],
                           'vars' : {} }
        varlist = skillrule[bracketstart + 1 : -1].split(',')
        for v in varlist:
            (var, val) = v.split('=', 1)
            skillrule_dict['vars'][var] = val
        return skillrule_dict


    def __skillvars__(self, skillrule):
        return self.__skillrule_to_dict__(skillrule).get('vars').keys()


    def record_if_required(self, astid, direction,
                           uniqueid,
                           channel,
                           queuename,
                           agent_channel,
                           skillrule):
        # XXX handle self.recordcampaignconfig.get('records_announce')
        electedcampaign = None
        for defineddate, v in self.recordcampaignconfig.get('campaigns').iteritems():
            # date checks
            o_selective = False
            o_stopdate = v.get('end')
            t_now = time.time()
            fmt = '%Y-%m-%d %H:%M:%S'

            if o_stopdate:
                t_stopdate = time.mktime(time.strptime(o_stopdate, fmt))
                o_selective = True
                if t_now > t_stopdate:
                    continue
            o_startdate = v.get('start')
            if not o_startdate:
                continue
            t_startdate = time.mktime(time.strptime(o_startdate, fmt))
            if t_now < t_startdate:
                continue

            # filter checks
            queueid = self.cset.weblist['queues'][astid].reverse_index.get(queuename)
            agentid = self.cset.weblist['agents'][astid].reverse_index.get(agent_channel[LENGTH_AGENT:])
            if self.__match_filters__(v.get('filters'),
                                      direction, queueid, agentid, skillrule):
                if electedcampaign:
                    defineddate_already = electedcampaign.get('defineddate')
                    t_defineddate_already = time.mktime(time.strptime(defineddate_already, fmt))
                    t_defineddate = time.mktime(time.strptime(defineddate, fmt))
                    if t_defineddate > t_defineddate_already:
                        log.info('replacing %s by %s'
                                 % (defineddate_already, defineddate))
                        electedcampaign = { 'defineddate' : defineddate,
                                            'details' : v
                                            }
                    else:
                        log.info('keeping %s, rejecting %s' %
                                 (defineddate_already, defineddate))
                else:
                    log.info('selecting %s' % defineddate)
                    electedcampaign = { 'defineddate' : defineddate,
                                        'details' : v
                                        }

        if electedcampaign:
            log.info('channel %s uniqueid %s' % (channel, uniqueid))

            #  check against "already started"
            #  action (monitor, filename, mix false)
            #  who is this agent and what are his rights ?
            # dbentry create : svi data, caller

            filenametmp = 'rec-%s-%s' % (uniqueid, channel)
            filenamecmd = filenametmp.lower().replace('/', '-').replace('.', '-')
            records_path = self.recordcampaignconfig.get('records_path').replace('\/', '/')
            filename = '%s/%s.wav' % (records_path, filenamecmd)
            campaignkind = 'P' if o_selective else 'S'

            extradata = self.cset.uniqueids[astid][uniqueid]
            calleridnum = extradata.get('calleridnum')
            varsets = extradata.get('dialplan_data')

            lut = self.recordcampaignconfig.get('dialplan_variables')
            se = list()
            sv = list()
            sc = list()
            if lut:
                for k, v in lut.get('svientries', {}).iteritems():
                    if v in varsets:
                        se.append('%s=%s' % (k, varsets.get(v)))
                for k, v in lut.get('svivariables', {}).iteritems():
                    if v in varsets:
                        sv.append('%s=%s' % (k, varsets.get(v)))
                for k, v in lut.get('svichoices', {}).iteritems():
                    if v in varsets:
                        sc.append('%s=%s' % (k, varsets.get(v)))

            aid = self.cset.__ami_execute__(astid, 'monitor', channel, filenamecmd, 'false')
            self.recorded_channels[channel] = True

            rights = ''
            firstname = ''
            lastname = ''
            if agentid:
                for uinfo in self.cset.__find_userinfos_by_agentid__(astid, agentid):
                    rights = uinfo.get('capaids')[0]
                    firstname = self.cset.weblist['agents'][astid].keeplist[agentid].get('firstname')
                    lastname = self.cset.weblist['agents'][astid].keeplist[agentid].get('lastname')

            if skillrule is None:
                skillrule = ''

            calldata = {
                'uniqueid' : uniqueid,
                'channel' : channel,
                'filename' : filename,
                'campaignkind' : campaignkind,
                'direction' : direction,
                'calleridnum' : calleridnum,
                'callstart' : time.time(),
                'callstatus' : 'callstarted',
                'recordstatus' : 'rec_started',
                'skillrules' : skillrule,
                'queuenames' : queuename,
                'agentnames' : '%s %s' % (firstname, lastname),
                'agentnumbers' : agent_channel,
                'agentrights' : rights,
                'svientries' : ','.join(se),
                'svivariables' : ','.join(sv),
                'svichoices' : ','.join(sc)
                }
            self.records_db.new_call(calldata)
        return


    def hangup_if_started(self, channel, uniqueid):
        # end of record = end of call => to change to Monitor event when exists ...
        if channel in self.recorded_channels:
            calldata = {
                'uniqueid' : uniqueid,
                'channel' : channel
                }
            requested = ('id', 'uniqueid', 'channel', 'filename',
                         'callstatus', 'callstart', 'recordstatus')
            callmatch = self.records_db.get_one_record(calldata, requested)
            idv = callmatch.get('id')
            callstart = float(callmatch.get('callstart'))
            callstop = time.time()
            calldata = {
                'callstop' : callstop,
                'callduration' : (callstop - callstart),
                'callstatus' : 'finished',
                'recordstatus' : 'rec_notag'
                }
            self.records_db.update_call(idv, calldata)

            source_records_path = ASTERISK_RECORDS_PATH
            target_records_path = self.recordcampaignconfig.get('records_path').replace('\/', '/')

            lsdir = self.__lsdir__(source_records_path)
            if lsdir:
                if not os.path.isdir(target_records_path):
                    log.warning('destination path %s did not exist : creating it' % target_records_path)
                    try:
                        os.makedirs(target_records_path)
                    except:
                        log.exception('mkdir %s' % target_records_path)

                fullfilename = callmatch.get('filename')
                record_path = os.path.dirname(fullfilename)
                record_base = os.path.basename(fullfilename)
                prefix = record_base[:-4]
                infile = '%s-in.wav' % prefix
                outfile = '%s-out.wav' % prefix
                if infile in lsdir:
                    log.info('ok for %s : will move it' % infile)
                    src = '%s/%s' % (source_records_path, infile)
                    dst = '%s/%s' % (target_records_path, infile)
                    try:
                        os.rename(src, dst)
                    except:
                        log.exception('moving %s to %s' % (src, dst))
                if outfile in lsdir:
                    log.info('ok for %s : will move it' % outfile)
                    src = '%s/%s' % (source_records_path, outfile)
                    dst = '%s/%s' % (target_records_path, outfile)
                    try:
                        os.rename(src, dst)
                    except:
                        log.exception('moving %s to %s' % (src, dst))

            # sox to mix ?
    # XXX log misc actions to file

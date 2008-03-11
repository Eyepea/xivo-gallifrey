# XIVO Daemon

__version__   = '$Revision$'
__date__      = '$Date$'
__copyright__ = 'Copyright (C) 2007, 2008, Proformatique'
__author__    = 'Corentin Le Gall'

# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# Alternatively, XIVO Daemon is available under other licenses directly
# contracted with Pro-formatique SARL. See the LICENSE file at top of the
# source tree or delivered in the installable package in which XIVO Daemon
# is distributed for more details.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License along
# with this program; if not, you will find one at
# <http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>.

"""
Phones lists.
"""

import time
from easyslog import *

DIR_TO_STRING = '>'
DIR_FROM_STRING = '<'

DUMMY_DIR = ''
DUMMY_RCHAN = ''
DUMMY_EXTEN = ''
DUMMY_MYNUM = ''
DUMMY_CLID = ''
DUMMY_STATE = ''


def channel_splitter(channel):
        sp = channel.split("-")
        if len(sp) > 1:
                sp.pop()
        return "-".join(sp)


def varlog(syslogprio, string):
        if syslogprio <= SYSLOG_NOTICE:
                syslogf(syslogprio, 'xivo_fb : ' + string)
        return 0


def log_debug(syslogprio, string):
        if syslogprio <= SYSLOG_INFO:
                print '#debug# %s' % string
        return varlog(syslogprio, string)


class Phones:
        lstadd = {}
        lstdel = {}
        list = {}
        configs = {}
        rough_phonelist = {}
        
        def __init__(self):
                return

        def __getitem__(self, astid):
                return self.list[astid]

        def setcommandclass(self, commandclass):
                self.commandclass = commandclass
                return

        def update(self, astid, cfg):
                # self.list = self.commandclass.get_phonelist()
                self.list[astid] = PhoneList(astid, self.commandclass)
                self.configs[astid] = cfg
                try:
                        self.__update_phonelist__(astid)
                except Exception, exc:
                        print '--- exception update ---', exc
                return


        def roughlist(self, astid):
                return self.rough_phonelist[astid]


## \brief Updates the list of sip numbers according to the SSO then sends old and new peers to the UIs.
# The reconnection to the AMI is also done here when it has been broken.
# If the AMI sockets are dead, a reconnection is also attempted here.
# \param astid the asterisk Id
# \return none
# \sa get_phonelist_fromurl
        def __update_phonelist__(self, astid):
         try:
                dt1 = time.time()
                self.rough_phonelist[astid] = self.configs[astid].get_phonelist_fromurl()
                if self.rough_phonelist[astid] is None:
                        return
                dt2 = time.time()
                for x in self.configs[astid].extrachannels.split(','):
                        if x != '': self.rough_phonelist[astid][x] = [x, '', '', x.split('/')[1], '', False]
                sipnumlistnew = dict.fromkeys(self.rough_phonelist[astid].keys())
         except Exception, exc:
                 log_debug(SYSLOG_ERR, '--- exception --- %s : get_phonelist_fromurl failed : %s' %(astid, str(exc)))
                 self.rough_phonelist[astid] = {}

         sipnumlistold = dict.fromkeys(filter(lambda j: self.list[astid].normal[j].towatch, self.list[astid].normal))
         dt3 = time.time()
         self.lstadd[astid] = []
         self.lstdel[astid] = []
         for snl in sipnumlistold:
                pln = self.list[astid].normal[snl]
                if snl not in sipnumlistnew:
                        self.lstdel[astid].append(":".join(["del",
                                                       astid,
                                                       pln.build_basestatus() + ';']))
                        deluser(astid, pln.tech.lower() + pln.phoneid)
                        del self.list[astid].normal[snl] # or = "Absent"/0 ?
                else:
                        pln.updateIfNeeded(self.rough_phonelist[astid][snl])
         dt4 = time.time()
         for snl in sipnumlistnew:
                if snl not in sipnumlistold:
##                        if astid in AMI_array_events_fd:
##                                AMI_array_events_fd[astid].write('Action: ExtensionState\r\n'
##                                                                 'Exten: %s\r\n'
##                                                                 'Context: %s\r\n'
##                                                                 '\r\n'
##                                                                 %(sipnuml[snl][3], sipnuml[snl][4]))
                        if snl.find("SIP") == 0:
                                self.list[astid].normal[snl] = LineProp("SIP",
                                                                    snl.split("/")[1],
                                                                    self.rough_phonelist[astid][snl][3],
                                                                    self.rough_phonelist[astid][snl][4],
                                                                    "Timeout", True)
                                # replaced previous 'BefSubs' initial status here : avoids flooding of Timeout's
                                # when many phones are added at once
                        elif snl.find("IAX2") == 0:
                                self.list[astid].normal[snl] = LineProp("IAX2",
                                                                    snl.split("/")[1],
                                                                    self.rough_phonelist[astid][snl][3],
                                                                    self.rough_phonelist[astid][snl][4],
                                                                    "Ready", True)
                        elif snl.find("mISDN") == 0:
                                self.list[astid].normal[snl] = LineProp("mISDN",
                                                                    snl.split("/")[1],
                                                                    self.rough_phonelist[astid][snl][3],
                                                                    self.rough_phonelist[astid][snl][4],
                                                                    "Ready", True)
                        elif snl.find("Zap") == 0:
                                self.list[astid].normal[snl] = LineProp("Zap",
                                                                    snl.split("/")[1],
                                                                    self.rough_phonelist[astid][snl][3],
                                                                    self.rough_phonelist[astid][snl][4],
                                                                    "Ready", True)
                        else:
                                log_debug(SYSLOG_WARNING, 'format <%s> not supported' % snl)
                                
                        if snl in self.list[astid].normal:
                                self.list[astid].normal[snl].set_callerid(self.rough_phonelist[astid][snl])

                        self.lstadd[astid].append(":".join(["add",
                                                            astid,
                                                            self.list[astid].normal[snl].build_basestatus(),
                                                            self.list[astid].normal[snl].build_cidstatus(),
                                                            self.list[astid].normal[snl].build_fullstatlist() + ';']))
         dt5 = time.time()
         log_debug(SYSLOG_INFO, '%s : sent ExtensionState request for new phones (%f seconds)' %(astid, (dt5-dt4)))
         if len(self.lstdel[astid]) > 0 or len(self.lstadd[astid]) > 0:
                 self.commandclass.phones_update('signal-deloradd',
                                                 [astid,
                                                  len(self.lstdel[astid]),
                                                  len(self.lstadd[astid]),
                                                  len(self.list[astid].normal)])
         dt6 = time.time()
         # print 'update_phonelist', dt2-dt1, dt3-dt2, dt4-dt3, dt5-dt4, dt6-dt5



## \class PhoneList
# \brief Properties of the lines of a given Asterisk
class PhoneList:
        ## \var astid
        # \brief Asterisk id, the same as the one given in the configs

        ## \var normal
        # \brief "Normal" phone lines, like SIP, IAX, Zap, ...

        ## \var queues
        # \brief To store queues' statuses

        ## \var oldqueues
        # \brief To store closed queues channels

        ##  \brief Class initialization.
        def __init__(self, iastid, cclass):
                self.astid = iastid
                self.normal = {}
                self.queues = {}
                self.oldqueues = {}
                self.star10 = []
                self.commandclass = cclass


        def update_gui_clients(self, phonenum, fromwhom):
                phoneinfo = (fromwhom,
                             self.astid,
                             self.normal[phonenum].build_basestatus(),
                             self.normal[phonenum].build_cidstatus(),
                             self.normal[phonenum].build_fullstatlist())
                if self.normal[phonenum].towatch:
                        self.commandclass.phones_update('update', phoneinfo)
                else:
                        self.commandclass.phones_update('noupdate', phoneinfo)


        def send_availstate_update(self, username, state):
                try:
                        if username.find("sip") == 0:
                                phoneid = "SIP/" + username.split("sip")[1]
                        elif username.find("iax") == 0:
                                phoneid = "IAX/" + username.split("iax")[1]
                        else:
                                phoneid = None

                        if phoneid in self.normal:
                                if state == "unknown" or self.normal[phoneid].imstat != state:
                                        self.normal[phoneid].set_imstat(state)
                                        self.normal[phoneid].update_time()
                                        self.update_gui_clients(phoneid, "kfc-sau")
                        elif phoneid is not None:
                                log_debug(SYSLOG_WARNING, "<%s> is not in my phone list" % phoneid)
                except Exception, exc:
                        log_debug(SYSLOG_ERR, '--- exception --- send_availstate_update : %s' % str(exc))


        def normal_channel_fills(self, chan_src, num_src,
                                 action, timeup, direction,
                                 chan_dst, num_dst, comment):
                phoneid_src = channel_splitter(chan_src)
                phoneid_dst = channel_splitter(chan_dst)

                if phoneid_src not in self.normal:
                        self.normal[phoneid_src] = LineProp(phoneid_src.split("/")[0],
                                                            phoneid_src.split("/")[1],
                                                            phoneid_src.split("/")[1],
                                                            "which-context?", "hintstatus?", False)
                do_update = self.normal[phoneid_src].set_chan(chan_src, action, timeup, direction, chan_dst, num_dst, num_src)
                if do_update:
                        self.update_gui_clients(phoneid_src, comment + "F")


        def normal_channel_hangup(self, chan_src, comment):
                phoneid_src = channel_splitter(chan_src)
                if phoneid_src not in self.normal:
                        self.normal[phoneid_src] = LineProp(phoneid_src.split("/")[0],
                                                            phoneid_src.split("/")[1],
                                                            phoneid_src.split("/")[1],
                                                            "which-context?", "hintstatus?", False)
                self.normal[phoneid_src].set_chan_hangup(chan_src)
                self.update_gui_clients(phoneid_src, comment + "H")
                self.normal[phoneid_src].del_chan(chan_src)
                self.update_gui_clients(phoneid_src, comment + "D")
                if len(self.normal[phoneid_src].chann) == 0 and self.normal[phoneid_src].towatch == False:
                        del self.normal[phoneid_src]

        ## \brief Updates some channels according to the Dial events occuring in the AMI.
        # \param src the source channel
        # \param dst the dest channel
        # \param clid the callerid
        # \param clidn the calleridname
        # \return none
        def handle_ami_event_dial(self, src, dst, clid, clidn):
                self.normal_channel_fills(src, DUMMY_MYNUM,
                                          "Calling", 0, DIR_TO_STRING,
                                          dst, DUMMY_EXTEN,
                                          "ami-ed1")
                self.normal_channel_fills(dst, DUMMY_MYNUM,
                                          "Ringing", 0, DIR_FROM_STRING,
                                          src, clid,
                                          "ami-ed2")


        ## \brief Updates some channels according to the Link events occuring in the AMI.
        # \param src the source channel
        # \param dst the dest channel
        # \param clid1 the src callerid
        # \param clid2 the dest callerid
        # \return none
        def handle_ami_event_link(self, src, dst, clid1, clid2):
                if src not in self.star10:
                        self.normal_channel_fills(src, DUMMY_MYNUM,
                                                  "On the phone", 0, DIR_TO_STRING,
                                                  dst, clid2,
                                                  "ami-el1")
                if dst not in self.star10:
                        self.normal_channel_fills(dst, DUMMY_MYNUM,
                                                  "On the phone", 0, DIR_FROM_STRING,
                                                  src, clid1,
                                                  "ami-el2")


        # \brief Fills the star10 field on unlink events.
        # \param src the source channel
        # \param dst the dest channel
        # \param clid1 the src callerid
        # \param clid2 the dest callerid
        # \return none
        def handle_ami_event_unlink(self, src, dst, clid1, clid2):
                if src not in self.star10:
                        self.star10.append(src)
                if dst not in self.star10:
                        self.star10.append(dst)


        # \brief Updates some channels according to the Hangup events occuring in the AMI.
        # \param chan the channel
        # \param cause the reason why there has been a hangup (not used)
        # \return
        def handle_ami_event_hangup(self, chan, cause):
                if chan in self.star10:
                        self.star10.remove(chan)
                self.normal_channel_hangup(chan, "ami-eh0")




## \class LineProp
# \brief Properties of a phone line. It might contain many channels.
class LineProp:
        ## \var tech
        # \brief Protocol of the phone (SIP, IAX2, ...)
        
        ## \var phoneid
        # \brief Phone identifier
        
        ## \var phonenum
        # \brief Phone number
        
        ## \var context
        # \brief Context
        
        ## \var lasttime
        # \brief Last time the phone has received a reply from a SUBSCRIBE
        
        ## \var chann
        # \brief List of Channels, with their properties as ChannelStatus
        
        ## \var hintstatus
        # \brief Status given through SIP presence detection
        
        ## \var imstat
        # \brief Instant Messaging status, as given by Xivo Clients
        
        ## \var voicemail
        # \brief Voicemail status
        
        ## \var queueavail
        # \brief Queue availability
        
        ## \var callerid
        # \brief Caller ID
        
        ## \var towatch
        # \brief Boolean value that tells whether this phone is watched by the switchboards
        
        ##  \brief Class initialization.
        def __init__(self, itech, iphoneid, iphonenum, icontext, ihintstatus, itowatch):
                self.tech = itech
                self.phoneid  = iphoneid
                self.phonenum = iphonenum
                self.context = icontext
                self.lasttime = 0
                self.chann = {}
                self.hintstatus = ihintstatus # Asterisk "hints" status
                self.imstat = "unknown"  # XMPP / Instant Messaging status
                self.voicemail = ""  # Voicemail status
                self.queueavail = "" # Availability as a queue member
                self.calleridfull = "nobody"
                self.calleridfirst = "nobody"
                self.calleridlast = "nobody"
                self.groups = ""
                self.towatch = itowatch
        def set_tech(self, itech):
                self.tech = itech
        def set_phoneid(self, iphoneid):
                self.phoneid = iphoneid
        def set_phonenum(self, iphonenum):
                self.phonenum = iphonenum
        def set_hintstatus(self, ihintstatus):
                self.hintstatus = ihintstatus
        def set_imstat(self, istatus):
                self.imstat = istatus
        def set_lasttime(self, ilasttime):
                self.lasttime = ilasttime

        def build_basestatus(self):
                basestatus = (self.tech.lower(),
                              self.phoneid,
                              self.phonenum,
                              self.context,
                              self.imstat,
                              self.hintstatus,
                              self.voicemail,
                              self.queueavail)
                return ":".join(basestatus)
        def build_cidstatus(self):
                cidstatus = (self.calleridfull,
                             self.calleridfirst,
                             self.calleridlast)
                return ":".join(cidstatus)
        ## \brief Builds the channel-by-channel part for the hints/update replies.
        # \param phoneid the "pointer" to the Asterisk phone statuses
        # \return the string containing the statuses for each channel of the given phone
        def build_fullstatlist(self):
                nchans = len(self.chann)
                fstat = [str(nchans)]
                for chan, phone_chan in self.chann.iteritems():
                        fstat.extend((":", chan, ":",
                                      phone_chan.getStatus(), ":",
                                      str(phone_chan.getDeltaTime()), ":",
                                      phone_chan.getDirection(), ":",
                                      phone_chan.getChannelPeer(), ":",
                                      phone_chan.getChannelNum()))
                return ''.join(fstat)

        def set_callerid(self, icallerid):
                self.calleridfull  = icallerid[0]
                self.calleridfirst = icallerid[1]
                self.calleridlast  = icallerid[2]
        def updateIfNeeded(self, icallerid):
                if icallerid[0:3] != (self.calleridfull, self.calleridfirst, self.calleridlast):
                        log_debug(SYSLOG_INFO, 'updated parameters for user <%s/%s> : %s => %s'
                                  % (self.tech, self.phoneid,
                                     (self.calleridfull, self.calleridfirst, self.calleridlast),
                                     icallerid[0:3]))
                        self.calleridfull  = icallerid[0]
                        self.calleridfirst = icallerid[1]
                        self.calleridlast  = icallerid[2]
        ##  \brief Updates the time elapsed on a channel according to current time.
        def update_time(self):
                nowtime = time.time()
                for ic in self.chann:
                        dtime = int(nowtime - self.chann[ic].getTime())
                        self.chann[ic].updateDeltaTime(dtime)
        ##  \brief Removes all channels.
        def clear_channels(self):
                self.chann = {}
        ##  \brief Adds a channel or changes its properties.
        # If the values of status, itime, peerch and/or peernum are empty,
        # they are not updated : the previous value is kept.
        # \param ichan the Channel to hangup.
        # \param status the status to set
        # \param itime the elapsed time to set
        def set_chan(self, ichan, status, itime, idir, peerch, peernum, mynum):
                #print "<%s> <%s> <%s> <%s> <%s> <%s> <%s>" %(ichan, status, itime, idir, peerch, peernum, mynum)
                do_update = True
                if mynum == "<unknown>" and is_normal_channel(ichan):
                        mynum = channel_splitter(ichan)
                        #               if peernum == "<unknown>" and is_normal_channel(peerch):
                        #                       peernum = channel_splitter(peerch)
                # does not update peerch and peernum if the new values are empty
                newstatus = status
                newdir = idir
                newpeerch = peerch
                newpeernum = peernum
                newmynum = mynum
                if ichan in self.chann:
                        thischannel = self.chann[ichan]
                        oldpeernum = thischannel.getChannelNum()

                        if status  == "": newstatus = thischannel.getStatus()
                        if idir    == "": newdir = thischannel.getDirection()
                        if peerch  == "": newpeerch = thischannel.getChannelPeer()
                        if oldpeernum != "": newpeernum = oldpeernum
                        if mynum   == "": newmynum = thischannel.getChannelMyNum()

                        # mynum != thischannel.getChannelMyNum()
                        # when dialing "*10", there are many successive newextens occuring, that reset
                        # the time counter to 0
                        if status == thischannel.getStatus() and \
                           idir == thischannel.getDirection() and \
                           peerch == thischannel.getChannelPeer() and \
                           peernum == thischannel.getChannelNum():
                                do_update = False

                if do_update:
                        firsttime = time.time()
                        self.chann[ichan] = ChannelStatus(newstatus, itime, newdir,
                                                          newpeerch, newpeernum, firsttime - itime,
                                                          newmynum)
                        for ic in self.chann:
                                self.chann[ic].updateDeltaTime(int(firsttime - self.chann[ic].getTime()))
                return do_update


        ##  \brief Hangs up a Channel.
        # \param ichan the Channel to hangup.
        def set_chan_hangup(self, ichan):
                nichan = ichan
                if ichan.find("<ZOMBIE>") >= 0:
                        log_debug(SYSLOG_INFO, "sch channel contains a <ZOMBIE> part (%s) : sending hup to %s anyway" %(ichan,nichan))
                        nichan = ichan.split("<ZOMBIE>")[0]
                firsttime = time.time()
                self.chann[nichan] = ChannelStatus("Hangup", 0, "", "", "", firsttime, "")
                for ic in self.chann:
                        self.chann[ic].updateDeltaTime(int(firsttime - self.chann[ic].getTime()))

        ##  \brief Removes a Channel.
        # \param ichan the Channel to remove.
        def del_chan(self, ichan):
                nichan = ichan
                if ichan.find("<ZOMBIE>") >= 0:
                        log_debug(SYSLOG_INFO, "dch channel contains a <ZOMBIE> part (%s) : deleting %s anyway" %(ichan,nichan))
                        nichan = ichan.split("<ZOMBIE>")[0]
                if nichan in self.chann: del self.chann[nichan]


## \class ChannelStatus
# \brief Properties of a Channel, as given by the AMI.
class ChannelStatus:
        ## \var status
        # \brief Channel status

        ## \var deltatime
        # \brief Elapsed time spent by the channel with the current status

        ## \var time
        # \brief Absolute time

        ## \var direction
        # \brief "To" or "From"

        ## \var channel_peer
        # \brief Channel name of the peer with whom it is in relation

        ## \var channel_num
        # \brief Phone number of the peer with whom it is in relation

        ##  \brief Class initialization.
        def __init__(self, istatus, dtime, idir, ipeerch, ipeernum, itime, imynum):
                self.status = istatus
                self.special = "" # voicemail, meetme, ...
                self.deltatime = dtime
                self.time = itime
                self.direction = idir
                self.channel_peer = ipeerch
                self.channel_num = ipeernum
                self.channel_mynum = imynum
        def updateDeltaTime(self, dtime):
                self.deltatime = dtime
        def setChannelPeer(self, ipeerch):
                self.channel_peer = ipeerch
        def setChannelNum(self, ipeernum):
                self.channel_num = ipeernum

        def getChannelPeer(self):
                return self.channel_peer
        def getChannelNum(self):
                return self.channel_num
        def getChannelMyNum(self):
                return self.channel_mynum
        def getDirection(self):
                return self.direction
        def getTime(self):
                return self.time
        def getDeltaTime(self):
                return self.deltatime
        def getStatus(self):
                return self.status



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

"""
Phones lists.
"""

import logging
import time
import cti_urllist

log = logging.getLogger('phones')

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

## \class PhoneList
# \brief Properties of the lines of a given Asterisk
class PhoneList:
    ## \var astid
    # \brief Asterisk id, the same as the one given in the configs

    ## \var normal
    # \brief "Normal" phone lines, like SIP, IAX, Zap, ...

    ##  \brief Class initialization.
    def __init__(self, url):
        self.normal = {}
        self.star10 = []
        return

    def setcommandclass(self, commandclass):
        self.commandclass = commandclass
        return

    def update_gui_clients(self, phonenum, fromwhom):
        if self.normal[phonenum].towatch:
            self.commandclass.phones_update('update',
                self.normal[phonenum].build_basestatus(),
                self.normal[phonenum].build_fullstatlist())
        else:
            self.commandclass.phones_update('noupdate',
                self.normal[phonenum].build_basestatus(),
                self.normal[phonenum].build_fullstatlist())

    def normal_channel_fills(self, chan_src, num_src, action, timeup, direction, chan_dst, num_dst, comment):
        phoneid_src = channel_splitter(chan_src)
        phoneid_dst = channel_splitter(chan_dst)

        # when remote atxfered arrives ...
        # chan_src, num_src, action, timeup, direction, chan_dst, num_dst, comment
        #  Local/101@default-66c2,1  On the phone 0 < SIP/103-081bd818 103 ami-el2

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
            dst, DUMMY_EXTEN)
        self.normal_channel_fills(dst, DUMMY_MYNUM,
            "Ringing", 0, DIR_FROM_STRING,
            src, clid)


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

    def update(self):
        self.normal[snl] = LineProp(self.rough_phonelist[snl]['tech'],
            self.rough_phonelist[snl]['phoneid'],
            self.rough_phonelist[snl]['number'],
            self.rough_phonelist[snl]['context'],
            'Timeout',
            True)
        # replaced previous 'BefSubs' initial status here : avoids flooding of Timeout's
        # when many phones are added at once
        return

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
        self.groups = ""
        self.towatch = itowatch

    def set_hintstatus(self, ihintstatus):
        self.hintstatus = ihintstatus
    def set_lasttime(self, ilasttime):
        self.lasttime = ilasttime

    def build_basestatus(self):
        return [self.context,
            self.tech,
            self.phoneid,
            self.hintstatus]

    ## \brief Builds the channel-by-channel part for the hints/update replies.
    # \param phoneid the "pointer" to the Asterisk phone statuses
    # \return the string containing the statuses for each channel of the given phone
    def build_fullstatlist(self):
        fstat = []
        for chan, phone_chan in self.chann.iteritems():
            fstat.append([chan,
                phone_chan.getStatus(),
                str(phone_chan.getDeltaTime()),
                phone_chan.getDirection(),
                phone_chan.channel_peer,
                phone_chan.channel_num])
        return fstat

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
            oldpeernum = thischannel.channel_num

            if status  == "": newstatus = thischannel.getStatus()
            if idir    == "": newdir = thischannel.getDirection()
            if peerch  == "": newpeerch = thischannel.channel_peer
            if oldpeernum != "": newpeernum = oldpeernum
            if mynum   == "": newmynum = thischannel.channel_mynum

            # mynum != thischannel.channel_mynum
            # when dialing "*10", there are many successive newextens occuring, that reset
            # the time counter to 0
            if status == thischannel.getStatus() and \
                idir == thischannel.getDirection() and \
                peerch == thischannel.channel_peer and \
                peernum == thischannel.channel_num:
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
            # log.info("sch channel contains a <ZOMBIE> part (%s) : sending hup to %s anyway" %(ichan,nichan))
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
            # log.info("dch channel contains a <ZOMBIE> part (%s) : deleting %s anyway" %(ichan,nichan))
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
        self.deltatime = dtime
        self.time = itime
        self.direction = idir
        self.channel_peer = ipeerch
        if ipeernum in ['', '<Unknown>', '<unknown>', 'anonymous', '(null)']:
            self.channel_num = '(?)'
        else:
            self.channel_num = ipeernum
        self.channel_mynum = imynum
    def updateDeltaTime(self, dtime):
        self.deltatime = dtime

    def getDirection(self):
        return self.direction
    def getTime(self):
        return self.time
    def getDeltaTime(self):
        return self.deltatime
    def getStatus(self):
        return self.status

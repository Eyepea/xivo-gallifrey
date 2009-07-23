<?php

#
# XiVO Web-Interface
# Copyright (C) 2006-2009  Proformatique <technique@proformatique.com>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#

$array = array();

$array['import'] = array();
$array['import']['file'] = array();
$array['import']['file']['name'] = '/^[a-z0-9_-]{1,50}(?:\.[a-z]{1,10})?$/i';
$array['import']['file']['mime'] = array('text/plain','text/csv');

$array['import']['field'] = array();
$array['import']['field']['firstname'] = array('ufeatures','firstname');
$array['import']['field']['lastname'] = array('ufeatures','lastname');
$array['import']['field']['username'] = array('protocol','name');
$array['import']['field']['interface'] = array('protocol','interface');
$array['import']['field']['phonenumber'] = array('ufeatures','number');
$array['import']['field']['password'] = array('protocol','secret');
$array['import']['field']['protocol'] = array('protocol','protocol');
$array['import']['field']['context'] = array('protocol','context');
$array['import']['field']['callerid'] = array('protocol','callerid');
$array['import']['field']['subscribemwi'] = array('protocol','subscribemwi');
$array['import']['field']['voicemailname'] = array('voicemail','fullname');
$array['import']['field']['voicemailmailbox'] = array('voicemail','mailbox');
$array['import']['field']['voicemailpassword'] = array('voicemail','password');
$array['import']['field']['voicemailemail'] = array('voicemail','email');
$array['import']['field']['voicemailskippass'] = array('vmfeatures','skipcheckpass');
$array['import']['field']['voicemailattach'] = array('voicemail','attach');
$array['import']['field']['voicemaildelete'] = array('voicemail','deletevoicemail');
$array['import']['field']['outcallerid'] = array('ufeatures','outcallerid');
$array['import']['field']['enableclient'] = array('ufeatures','enableclient');
$array['import']['field']['loginclient'] = array('ufeatures','loginclient');
$array['import']['field']['passwdclient'] = array('ufeatures','passwdclient');
$array['import']['field']['profileclient'] = array('ufeatures','profileclient');
$array['import']['field']['enablehint'] = array('ufeatures','enablehint');
$array['import']['field']['agentnumber'] = array('ufeatures','agentnumber');
$array['import']['field']['mobilephonenumber'] = array('ufeatures','mobilephonenumber');
$array['import']['field']['bosssecretary'] = array('ufeatures','bsfilter');
$array['import']['field']['incallexten'] = array('incall','exten');
$array['import']['field']['incallcontext'] = array('incall','context');
$array['import']['field']['incallringseconds'] = array('incall','ringseconds');

?>

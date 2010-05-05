<?php

#
# XiVO Web-Interface
# Copyright (C) 2006-2010  Proformatique <technique@proformatique.com>
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

$array['accessfeatures'] = array();
$array['accessfeatures']['table'] = 'accessfeatures';
$array['accessfeatures']['primary'] = array('id' => '');

$array['agentfeatures'] = array();
$array['agentfeatures']['table'] = 'agentfeatures';
$array['agentfeatures']['primary'] = array('id' => '');

$array['agentgroup'] = array();
$array['agentgroup']['table'] = 'agentgroup';
$array['agentgroup']['primary'] = array('id' => '');

$array['autoprov'] = array();
$array['autoprov']['table'] = 'phone';
$array['autoprov']['primary'] = array('macaddr' => '');

$array['callerid'] = array();
$array['callerid']['table'] = 'callerid';
$array['callerid']['primary'] = array('type' => '','typeval' => '');

$array['callfilter'] = array();
$array['callfilter']['table'] = 'callfilter';
$array['callfilter']['primary'] = array('id' => '');

$array['callfiltermember'] = array();
$array['callfiltermember']['table'] = 'callfiltermember';
$array['callfiltermember']['primary'] = array('id' => '');

$array['cdr'] = array();
$array['cdr']['table'] = 'cdr';
$array['cdr']['primary'] = array('id' => '');

$array['context'] = array();
$array['context']['table'] = 'context';
$array['context']['primary'] = array('name' => '');

$array['contextinclude'] = array();
$array['contextinclude']['table'] = 'contextinclude';
$array['contextinclude']['primary'] = array('context' => '','include' => '');

$array['contextmember'] = array();
$array['contextmember']['table'] = 'contextmember';
$array['contextmember']['primary'] = array('context' => '','type' => '','typeval' => '','varname' => '');

$array['contextnumbers'] = array();
$array['contextnumbers']['table'] = 'contextnumbers';
$array['contextnumbers']['primary'] = array('context' => '','type' => '','numberbeg' => '','numberend' => '');

$array['contextnummember'] = array();
$array['contextnummember']['table'] = 'contextnummember';
$array['contextnummember']['primary'] = array('context' => '','type' => '','typeval' => '');

$array['cticontexts'] = array();
$array['cticontexts']['table'] = 'cticontexts';
$array['cticontexts']['primary'] = array('id' => '');

$array['ctidirectories'] = array();
$array['ctidirectories']['table'] = 'ctidirectories';
$array['ctidirectories']['primary'] = array('id' => '');

$array['ctidisplays'] = array();
$array['ctidisplays']['table'] = 'ctidisplays';
$array['ctidisplays']['primary'] = array('id' => '');

$array['ctimain'] = array();
$array['ctimain']['table'] = 'ctimain';
$array['ctimain']['primary'] = array('id' => '');

$array['ctiphonehints'] = array();
$array['ctiphonehints']['table'] = 'ctiphonehints';
$array['ctiphonehints']['primary'] = array('id' => '');

$array['ctipresences'] = array();
$array['ctipresences']['table'] = 'ctipresences';
$array['ctipresences']['primary'] = array('id' => '');

$array['ctiprofiles'] = array();
$array['ctiprofiles']['table'] = 'ctiprofiles';
$array['ctiprofiles']['primary'] = array('id' => '');

$array['ctireversedirectories'] = array();
$array['ctireversedirectories']['table'] = 'ctireversedirectories';
$array['ctireversedirectories']['primary'] = array('id' => '');

$array['ctisheetevents'] = array();
$array['ctisheetevents']['table'] = 'ctisheetevents';
$array['ctisheetevents']['primary'] = array('id' => '');

$array['ctisheetactions'] = array();
$array['ctisheetactions']['table'] = 'ctisheetactions';
$array['ctisheetactions']['primary'] = array('id' => '');

$array['ctistatus'] = array();
$array['ctistatus']['table'] = 'ctistatus';
$array['ctistatus']['primary'] = array('id' => '');

$array['dialaction'] = array();
$array['dialaction']['table'] = 'dialaction';
$array['dialaction']['primary'] = array('event' => '','category' => '','categoryval' => '');

$array['extensions'] = array();
$array['extensions']['table'] = 'extensions';
$array['extensions']['primary'] = array('id' => '');

$array['extenumbers'] = array();
$array['extenumbers']['table'] = 'extenumbers';
$array['extenumbers']['primary'] = array('id' => '');

$array['features'] = array();
$array['features']['table'] = 'features';
$array['features']['primary'] = array('id' => '');

$array['groupfeatures'] = array();
$array['groupfeatures']['table'] = 'groupfeatures';
$array['groupfeatures']['primary'] = array('id' => '');

$array['handynumbers'] = array();
$array['handynumbers']['table'] = 'handynumbers';
$array['handynumbers']['primary'] = array('id' => '');

$array['incall'] = array();
$array['incall']['table'] = 'incall';
$array['incall']['primary'] = array('id' => '');

$array['ldapfilter'] = array();
$array['ldapfilter']['table'] = 'ldapfilter';
$array['ldapfilter']['primary'] = array('id' => '');

$array['meetmefeatures'] = array();
$array['meetmefeatures']['table'] = 'meetmefeatures';
$array['meetmefeatures']['primary'] = array('id' => '');

$array['meetmeguest'] = array();
$array['meetmeguest']['table'] = 'meetmeguest';
$array['meetmeguest']['primary'] = array('id' => '');

$array['musiconhold'] = array();
$array['musiconhold']['table'] = 'musiconhold';
$array['musiconhold']['primary'] = array('id' => '');

$array['outcall'] = array();
$array['outcall']['table'] = 'outcall';
$array['outcall']['primary'] = array('id' => '');

$array['outcalltrunk'] = array();
$array['outcalltrunk']['table'] = 'outcalltrunk';
$array['outcalltrunk']['primary'] = array('outcallid' => '','trunkfeaturesid' => '');

$array['phonebook'] = array();
$array['phonebook']['table'] = 'phonebook';
$array['phonebook']['primary'] = array('id' => '');

$array['phonebookaddress'] = array();
$array['phonebookaddress']['table'] = 'phonebookaddress';
$array['phonebookaddress']['primary'] = array('id' => '');

$array['phonebooknumber'] = array();
$array['phonebooknumber']['table'] = 'phonebooknumber';
$array['phonebooknumber']['primary'] = array('id' => '');

$array['phonefunckey'] = array();
$array['phonefunckey']['table'] = 'phonefunckey';
$array['phonefunckey']['primary'] = array('iduserfeatures' => '','fknum' => '');

$array['queue'] = array();
$array['queue']['table'] = 'queue';
$array['queue']['primary'] = array('name' => '');

$array['queuefeatures'] = array();
$array['queuefeatures']['table'] = 'queuefeatures';
$array['queuefeatures']['primary'] = array('id' => '');

$array['queuemember'] = array();
$array['queuemember']['table'] = 'queuemember';
$array['queuemember']['primary'] = array('queue_name' => '','interface' => '');

$array['queueskill'] = array();
$array['queueskill']['table'] = 'queueskill';
$array['queueskill']['primary'] = array('id' => '');

$array['queueskillcat'] = array();
$array['queueskillcat']['table'] = 'queueskillcat';
$array['queueskillcat']['primary'] = array('id' => '');

$array['queueskillrule'] = array();
$array['queueskillrule']['table'] = 'queueskillrule';
$array['queueskillrule']['primary'] = array('id' => '');

$array['rightcall'] = array();
$array['rightcall']['table'] = 'rightcall';
$array['rightcall']['primary'] = array('id' => '');

$array['rightcallexten'] = array();
$array['rightcallexten']['table'] = 'rightcallexten';
$array['rightcallexten']['primary'] = array('id' => '');

$array['rightcallmember'] = array();
$array['rightcallmember']['table'] = 'rightcallmember';
$array['rightcallmember']['primary'] = array('id' => '');

$array['schedule'] = array();
$array['schedule']['table'] = 'schedule';
$array['schedule']['primary'] = array('id' => '');

$array['serverfeatures'] = array();
$array['serverfeatures']['table'] = 'serverfeatures';
$array['serverfeatures']['primary'] = array('id' => '');

$array['staticagent'] = array();
$array['staticagent']['table'] = 'staticagent';
$array['staticagent']['primary'] = array('id' => '');

$array['staticiax'] = array();
$array['staticiax']['table'] = 'staticiax';
$array['staticiax']['primary'] = array('id' => '');

$array['staticmeetme'] = array();
$array['staticmeetme']['table'] = 'staticmeetme';
$array['staticmeetme']['primary'] = array('id' => '');

$array['staticqueue'] = array();
$array['staticqueue']['table'] = 'staticqueue';
$array['staticqueue']['primary'] = array('id' => '');

$array['staticsip'] = array();
$array['staticsip']['table'] = 'staticsip';
$array['staticsip']['primary'] = array('id' => '');

$array['staticvoicemail'] = array();
$array['staticvoicemail']['table'] = 'staticvoicemail';
$array['staticvoicemail']['primary'] = array('id' => '');

$array['trunkcustom'] = array();
$array['trunkcustom']['table'] = 'usercustom';
$array['trunkcustom']['primary'] = array('id' => '');

$array['trunkfeatures'] = array();
$array['trunkfeatures']['table'] = 'trunkfeatures';
$array['trunkfeatures']['primary'] = array('id' => '');

$array['trunkiax'] = array();
$array['trunkiax']['table'] = 'useriax';
$array['trunkiax']['primary'] = array('id' => '');

$array['trunksip'] = array();
$array['trunksip']['table'] = 'usersip';
$array['trunksip']['primary'] = array('id' => '');

$array['usercustom'] = array();
$array['usercustom']['table'] = 'usercustom';
$array['usercustom']['primary'] = array('id' => '');

$array['userfeatures'] = array();
$array['userfeatures']['table'] = 'userfeatures';
$array['userfeatures']['primary'] = array('id' => '');

$array['useriax'] = array();
$array['useriax']['table'] = 'useriax';
$array['useriax']['primary'] = array('id' => '');

$array['userqueueskill'] = array();
$array['userqueueskill']['table'] = 'userqueueskill';
$array['userqueueskill']['primary'] = array('userid' => '', 'skillid' => '');

$array['usersip'] = array();
$array['usersip']['table'] = 'usersip';
$array['usersip']['primary'] = array('id' => '');

$array['voicemail'] = array();
$array['voicemail']['table'] = 'voicemail';
$array['voicemail']['primary'] = array('uniqueid' => '');

$array['voicemailfeatures'] = array();
$array['voicemailfeatures']['table'] = 'voicemailfeatures';
$array['voicemailfeatures']['primary'] = array('id' => '');

$array['voicemenu'] = array();
$array['voicemenu']['table'] = 'voicemenu';
$array['voicemenu']['primary'] = array('id' => '');

$array['usersccp'] = array();
$array['usersccp']['table'] = 'usersccp';
$array['usersccp']['primary'] = array('id' => '');

$array['sccpline'] = array();
$array['sccpline']['table'] = 'sccpline';
$array['sccpline']['primary'] = array('id' => '');

$array['sccpbuttonconfig'] = array();
$array['sccpbuttonconfig']['table'] = 'sccpbuttonconfig';
$array['sccpbuttonconfig']['primary'] = array('id' => '');

?>

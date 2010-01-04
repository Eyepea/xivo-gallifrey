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

$array['exten'] = array();
$array['exten']['agentdynamiclogin'] = array();
$array['exten']['agentdynamiclogin']['app'] = 'Macro';
$array['exten']['agentdynamiclogin']['appdata'] = array();
$array['exten']['agentdynamiclogin']['appdata'][0] = 'agentdynamiclogin';
$array['exten']['agentdynamiclogin']['appdata'][1] = '${EXTEN:$<EXTNUMLEN>}';

$array['exten']['agentstaticlogin'] = array();
$array['exten']['agentstaticlogin']['app'] = 'Macro';
$array['exten']['agentstaticlogin']['appdata'] = array();
$array['exten']['agentstaticlogin']['appdata'][0] = 'agentstaticlogin';
$array['exten']['agentstaticlogin']['appdata'][1] = '${EXTEN:$<EXTNUMLEN>}';

$array['exten']['agentstaticlogoff'] = array();
$array['exten']['agentstaticlogoff']['app'] = 'Macro';
$array['exten']['agentstaticlogoff']['appdata'] = array();
$array['exten']['agentstaticlogoff']['appdata'][0] = 'agentstaticlogoff';
$array['exten']['agentstaticlogoff']['appdata'][1] = '${EXTEN:$<EXTNUMLEN>}';

$array['exten']['agentstaticlogtoggle'] = array();
$array['exten']['agentstaticlogtoggle']['app'] = 'Macro';
$array['exten']['agentstaticlogtoggle']['appdata'] = array();
$array['exten']['agentstaticlogtoggle']['appdata'][0] = 'agentstaticlogtoggle';
$array['exten']['agentstaticlogtoggle']['appdata'][1] = '${EXTEN:$<EXTNUMLEN>}';

$array['exten']['bsfilter'] = array();
$array['exten']['bsfilter']['app'] = 'Macro';
$array['exten']['bsfilter']['appdata'] = array();
$array['exten']['bsfilter']['appdata'][0] = 'bsfilter';
$array['exten']['bsfilter']['appdata'][1] = '${EXTEN:$<EXTNUMLEN>}';

$array['exten']['callgroup'] = array();
$array['exten']['callgroup']['app'] = 'Macro';
$array['exten']['callgroup']['appdata'] = array();
$array['exten']['callgroup']['appdata'][0] = 'group';
$array['exten']['callgroup']['appdata'][1] = '${EXTEN:$<EXTNUMLEN>}';
$array['exten']['callgroup']['appdata'][2] = '';

$array['exten']['calllistening'] = array();
$array['exten']['calllistening']['app'] = 'Macro';
$array['exten']['calllistening']['appdata'] = 'calllistening';

$array['exten']['callmeetme'] = array();
$array['exten']['callmeetme']['app'] = 'Macro';
$array['exten']['callmeetme']['appdata'] = array();
$array['exten']['callmeetme']['appdata'][0] = 'meetme';
$array['exten']['callmeetme']['appdata'][1] = '${EXTEN:$<EXTNUMLEN>}';
$array['exten']['callmeetme']['appdata'][2] = '';

$array['exten']['callqueue'] = array();
$array['exten']['callqueue']['app'] = 'Macro';
$array['exten']['callqueue']['appdata'] = array();
$array['exten']['callqueue']['appdata'][0] = 'queue';
$array['exten']['callqueue']['appdata'][1] = '${EXTEN:$<EXTNUMLEN>}';
$array['exten']['callqueue']['appdata'][2] = '';

$array['exten']['callrecord'] = array();
$array['exten']['callrecord']['app'] = 'Macro';
$array['exten']['callrecord']['appdata'] = 'callrecord';

$array['exten']['calluser'] = array();
$array['exten']['calluser']['app'] = 'Macro';
$array['exten']['calluser']['appdata'] = array();
$array['exten']['calluser']['appdata'][0] = 'user';
$array['exten']['calluser']['appdata'][1] = '${EXTEN:$<EXTNUMLEN>}';
$array['exten']['calluser']['appdata'][2] = '';

$array['exten']['directoryaccess'] = array();
$array['exten']['directoryaccess']['app'] = 'Directory';
$array['exten']['directoryaccess']['appdata'] = '${CONTEXT}';

$array['exten']['enablednd'] = array();
$array['exten']['enablednd']['app'] = 'Macro';
$array['exten']['enablednd']['appdata'] = 'enablednd';

$array['exten']['enablevm'] = array();
$array['exten']['enablevm']['app'] = 'Macro';
$array['exten']['enablevm']['appdata'] = 'enablevm';

$array['exten']['enablevmbox'] = array();
$array['exten']['enablevmbox']['app'] = 'Macro';
$array['exten']['enablevmbox']['appdata'] = 'enablevmbox';

$array['exten']['enablevmboxslt'] = array();
$array['exten']['enablevmboxslt']['app'] = 'Macro';
$array['exten']['enablevmboxslt']['appdata'] = array();
$array['exten']['enablevmboxslt']['appdata'][0] = 'enablevmbox';
$array['exten']['enablevmboxslt']['appdata'][1] = '${EXTEN:$<EXTNUMLEN>}';

$array['exten']['enablevmslt'] = array();
$array['exten']['enablevmslt']['app'] = 'Macro';
$array['exten']['enablevmslt']['appdata'] = array();
$array['exten']['enablevmslt']['appdata'][0] = 'enablevm';
$array['exten']['enablevmslt']['appdata'][1] = '${EXTEN:$<EXTNUMLEN>}';

$array['exten']['fwdbusy'] = array();
$array['exten']['fwdbusy']['app'] = 'Macro';
$array['exten']['fwdbusy']['appdata'] = array();
$array['exten']['fwdbusy']['appdata'][0] = 'feature_forward';
$array['exten']['fwdbusy']['appdata'][1] = 'busy';
$array['exten']['fwdbusy']['appdata'][2] = '${EXTEN:$<EXTNUMLEN>}';

$array['exten']['fwdrna'] = array();
$array['exten']['fwdrna']['app'] = 'Macro';
$array['exten']['fwdrna']['appdata'][0] = 'feature_forward';
$array['exten']['fwdrna']['appdata'][1] = 'rna';
$array['exten']['fwdrna']['appdata'][2] = '${EXTEN:$<EXTNUMLEN>}';

$array['exten']['fwdunc'] = array();
$array['exten']['fwdunc']['app'] = 'Macro';
$array['exten']['fwdunc']['appdata'][0] = 'feature_forward';
$array['exten']['fwdunc']['appdata'][1] = 'unc';
$array['exten']['fwdunc']['appdata'][2] = '${EXTEN:$<EXTNUMLEN>}';

$array['exten']['fwdundoall'] = array();
$array['exten']['fwdundoall']['app'] = 'Macro';
$array['exten']['fwdundoall']['appdata'] = 'fwdundoall';

$array['exten']['groupaddmember'] = array();
$array['exten']['groupaddmember']['app'] = 'Macro';
$array['exten']['groupaddmember']['appdata'] = array();
$array['exten']['groupaddmember']['appdata'][0] = 'groupmember';
$array['exten']['groupaddmember']['appdata'][1] = 'group';
$array['exten']['groupaddmember']['appdata'][2] = 'add';
$array['exten']['groupaddmember']['appdata'][3] = '${EXTEN:$<EXTNUMLEN>}';

$array['exten']['groupremovemember'] = array();
$array['exten']['groupremovemember']['app'] = 'Macro';
$array['exten']['groupremovemember']['appdata'] = array();
$array['exten']['groupremovemember']['appdata'][0] = 'groupmember';
$array['exten']['groupremovemember']['appdata'][1] = 'group';
$array['exten']['groupremovemember']['appdata'][2] = 'remove';
$array['exten']['groupremovemember']['appdata'][3] = '${EXTEN:$<EXTNUMLEN>}';

$array['exten']['grouptogglemember'] = array();
$array['exten']['grouptogglemember']['app'] = 'Macro';
$array['exten']['grouptogglemember']['appdata'] = array();
$array['exten']['grouptogglemember']['appdata'][0] = 'groupmember';
$array['exten']['grouptogglemember']['appdata'][1] = 'group';
$array['exten']['grouptogglemember']['appdata'][2] = 'toggle';
$array['exten']['grouptogglemember']['appdata'][3] = '${EXTEN:$<EXTNUMLEN>}';

$array['exten']['guestprov'] = array();
$array['exten']['guestprov']['app'] = 'Macro';
$array['exten']['guestprov']['appdata'] = 'guestprov';

$array['exten']['incallfilter'] = array();
$array['exten']['incallfilter']['app'] = 'Macro';
$array['exten']['incallfilter']['appdata'] = 'incallfilter';

$array['exten']['phoneprogfunckey'] = array();
$array['exten']['phoneprogfunckey']['app'] = 'Macro';
$array['exten']['phoneprogfunckey']['appdata'] = array();
$array['exten']['phoneprogfunckey']['appdata'][0] = 'phoneprogfunckey';
$array['exten']['phoneprogfunckey']['appdata'][1] = '${EXTEN:0:$<EXTNUMLEN>}';
$array['exten']['phoneprogfunckey']['appdata'][2] = '${EXTEN:$<EXTNUMLEN>}';

$array['exten']['phonestatus'] = array();
$array['exten']['phonestatus']['app'] = 'Macro';
$array['exten']['phonestatus']['appdata'] = 'phonestatus';

$array['exten']['pickup'] = array();
$array['exten']['pickup']['app'] = 'Pickup';
$array['exten']['pickup']['appdata'] = '${EXTEN:$<EXTNUMLEN>}%${CONTEXT}@PICKUPMARK';

$array['exten']['queueaddmember'] = array();
$array['exten']['queueaddmember']['app'] = 'Macro';
$array['exten']['queueaddmember']['appdata'] = array();
$array['exten']['queueaddmember']['appdata'][0] = 'groupmember';
$array['exten']['queueaddmember']['appdata'][1] = 'queue';
$array['exten']['queueaddmember']['appdata'][2] = 'add';
$array['exten']['queueaddmember']['appdata'][3] = '${EXTEN:$<EXTNUMLEN>}';

$array['exten']['queueremovemember'] = array();
$array['exten']['queueremovemember']['app'] = 'Macro';
$array['exten']['queueremovemember']['appdata'] = array();
$array['exten']['queueremovemember']['appdata'][0] = 'groupmember';
$array['exten']['queueremovemember']['appdata'][1] = 'queue';
$array['exten']['queueremovemember']['appdata'][2] = 'remove';
$array['exten']['queueremovemember']['appdata'][3] = '${EXTEN:$<EXTNUMLEN>}';

$array['exten']['queuetogglemember'] = array();
$array['exten']['queuetogglemember']['app'] = 'Macro';
$array['exten']['queuetogglemember']['appdata'] = array();
$array['exten']['queuetogglemember']['appdata'][0] = 'groupmember';
$array['exten']['queuetogglemember']['appdata'][1] = 'queue';
$array['exten']['queuetogglemember']['appdata'][2] = 'toggle';
$array['exten']['queuetogglemember']['appdata'][3] = '${EXTEN:$<EXTNUMLEN>}';

$array['exten']['recsnd'] = array();
$array['exten']['recsnd']['app'] = 'Macro';
$array['exten']['recsnd']['appdata'] = array();
$array['exten']['recsnd']['appdata'][0] = 'recsnd';
$array['exten']['recsnd']['appdata'][1] = 'wav';

$array['exten']['vmboxmsgslt'] = array();
$array['exten']['vmboxmsgslt']['app'] = 'Macro';
$array['exten']['vmboxmsgslt']['appdata'] = array();
$array['exten']['vmboxmsgslt']['appdata'][0] = 'vmboxmsg';
$array['exten']['vmboxmsgslt']['appdata'][1] = '${EXTEN:$<EXTNUMLEN>}';

$array['exten']['vmboxpurgeslt'] = array();
$array['exten']['vmboxpurgeslt']['app'] = 'Macro';
$array['exten']['vmboxpurgeslt']['appdata'] = array();
$array['exten']['vmboxpurgeslt']['appdata'][0] = 'vmboxpurge';
$array['exten']['vmboxpurgeslt']['appdata'][1] = '${EXTEN:$<EXTNUMLEN>}';

$array['exten']['vmboxslt'] = array();
$array['exten']['vmboxslt']['app'] = 'Macro';
$array['exten']['vmboxslt']['appdata'] = array();
$array['exten']['vmboxslt']['appdata'][0] = 'vmbox';
$array['exten']['vmboxslt']['appdata'][1] = '${EXTEN:$<EXTNUMLEN>}';

$array['exten']['vmusermsg'] = array();
$array['exten']['vmusermsg']['app'] = 'Macro';
$array['exten']['vmusermsg']['appdata'] = 'vmusermsg';

$array['exten']['vmuserpurge'] = array();
$array['exten']['vmuserpurge']['app'] = 'Macro';
$array['exten']['vmuserpurge']['appdata'] = 'vmuserpurge';

$array['exten']['vmuserpurgeslt'] = array();
$array['exten']['vmuserpurgeslt']['app'] = 'Macro';
$array['exten']['vmuserpurgeslt']['appdata'] = array();
$array['exten']['vmuserpurgeslt']['appdata'][0] = 'vmuserpurge';
$array['exten']['vmuserpurgeslt']['appdata'][1] = '${EXTEN:$<EXTNUMLEN>}';

$array['exten']['vmuserslt'] = array();
$array['exten']['vmuserslt']['app'] = 'Macro';
$array['exten']['vmuserslt']['appdata'] = array();
$array['exten']['vmuserslt']['appdata'][0] = 'vmuser';
$array['exten']['vmuserslt']['appdata'][1] = '${EXTEN:$<EXTNUMLEN>}';

$array['element'] = array();

$array['element']['agentdynamiclogin'] = array();
$array['element']['agentdynamiclogin']['default'] = '*33.';

$array['element']['agentstaticlogin'] = array();
$array['element']['agentstaticlogin']['default'] = '*31.';

$array['element']['agentstaticlogoff'] = array();
$array['element']['agentstaticlogoff']['default'] = '*32.';

$array['element']['agentstaticlogtoggle'] = array();
$array['element']['agentstaticlogtoggle']['default'] = '*30.';

$array['element']['bsfilter'] = array();
$array['element']['bsfilter']['default'] = '*37.';

$array['element']['callgroup'] = array();
$array['element']['callgroup']['default'] = '*664.';

$array['element']['calllistening'] = array();
$array['element']['calllistening']['default'] = '*34';

$array['element']['callmeetme'] = array();
$array['element']['callmeetme']['default'] = '*667.';

$array['element']['callqueue'] = array();
$array['element']['callqueue']['default'] = '*665.';

$array['element']['callrecord'] = array();
$array['element']['callrecord']['default'] = '*26';

$array['element']['calluser'] = array();
$array['element']['calluser']['default'] = '*666.';

$array['element']['directoryaccess'] = array();
$array['element']['directoryaccess']['default'] = '*36';

$array['element']['enablednd'] = array();
$array['element']['enablednd']['default'] = '*25';

$array['element']['enablevm'] = array();
$array['element']['enablevm']['default'] = '*90';

$array['element']['enablevmbox'] = array();
$array['element']['enablevmbox']['default'] = '*91';

$array['element']['enablevmboxslt'] = array();
$array['element']['enablevmboxslt']['default'] = '*91.';

$array['element']['enablevmslt'] = array();
$array['element']['enablevmslt']['default'] = '*90.';

$array['element']['fwdbusy'] = array();
$array['element']['fwdbusy']['default'] = '*23.';

$array['element']['fwdrna'] = array();
$array['element']['fwdrna']['default'] = '*22.';

$array['element']['fwdunc'] = array();
$array['element']['fwdunc']['default'] = '*21.';

$array['element']['fwdundoall'] = array();
$array['element']['fwdundoall']['default'] = '*20';

$array['element']['groupaddmember'] = array();
$array['element']['groupaddmember']['default'] = '*51.';

$array['element']['groupremovemember'] = array();
$array['element']['groupremovemember']['default'] = '*52.';

$array['element']['grouptogglemember'] = array();
$array['element']['grouptogglemember']['default'] = '*50.';

$array['element']['guestprov'] = array();
$array['element']['guestprov']['default'] = '*48378';

$array['element']['incallfilter'] = array();
$array['element']['incallfilter']['default'] = '*27';

$array['element']['phonestatus'] = array();
$array['element']['phonestatus']['default'] = '*10';

$array['element']['pickup'] = array();
$array['element']['pickup']['default'] = '*8.';

$array['element']['phoneprogfunckey'] = array();
$array['element']['phoneprogfunckey']['default'] = '*735.';

$array['element']['queueaddmember'] = array();
$array['element']['queueaddmember']['default'] = '*56.';

$array['element']['queueremovemember'] = array();
$array['element']['queueremovemember']['default'] = '*57.';

$array['element']['queuetogglemember'] = array();
$array['element']['queuetogglemember']['default'] = '*55.';

$array['element']['recsnd'] = array();
$array['element']['recsnd']['default'] = '*9';

$array['element']['vmboxmsgslt'] = array();
$array['element']['vmboxmsgslt']['default'] = '*99.';

$array['element']['vmboxpurgeslt'] = array();
$array['element']['vmboxpurgeslt']['default'] = '*93.';

$array['element']['vmboxslt'] = array();
$array['element']['vmboxslt']['default'] = '*97.';

$array['element']['vmusermsg'] = array();
$array['element']['vmusermsg']['default'] = '*98';

$array['element']['vmuserpurge'] = array();
$array['element']['vmuserpurge']['default'] = '*92';

$array['element']['vmuserpurgeslt'] = array();
$array['element']['vmuserpurgeslt']['default'] = '*92.';

$array['element']['vmuserslt'] = array();
$array['element']['vmuserslt']['default'] = '*96.';

$array['filter'] = array();
$array['filter']['agentdynamiclogin'] = array('minlen' => 1,'maxlen' => 40);
$array['filter']['agentstaticlogin'] = array('minlen' => 1,'maxlen' => 40);
$array['filter']['agentstaticlogoff'] = array('minlen' => 1,'maxlen' => 40);
$array['filter']['agentstaticlogtoggle'] = array('minlen' => 1,'maxlen' => 40);
$array['filter']['bsfilter'] = array('minlen' => 1,'maxlen' => 40);
$array['filter']['callgroup'] = array('minlen' => 1,'maxlen' => 40);
$array['filter']['calllistening'] = array('minlen' => 1,'maxlen' => 40,'regexp' => '/^[0-9\*#]+$/');
$array['filter']['callmeetme'] = array('minlen' => 1,'maxlen' => 40);
$array['filter']['callqueue'] = array('minlen' => 1,'maxlen' => 40);
$array['filter']['callrecord'] = array('minlen' => 1,'maxlen' => 40,'regexp' => '/^[0-9\*#]+$/');
$array['filter']['calluser'] = array('minlen' => 1,'maxlen' => 40);
$array['filter']['directoryaccess'] = array('minlen' => 1,'maxlen' => 40,'regexp' => '/^[0-9\*#]+$/');
$array['filter']['enablednd'] = array('minlen' => 1,'maxlen' => 40,'regexp' => '/^[0-9\*#]+$/');
$array['filter']['enablevm'] = array('minlen' => 1,'maxlen' => 40,'regexp' => '/^[0-9\*#]+$/');
$array['filter']['enablevmbox'] = array('minlen' => 1,'maxlen' => 40,'regexp' => '/^[0-9\*#]+$/');
$array['filter']['enablevmboxslt'] = array('minlen' => 1,'maxlen' => 40);
$array['filter']['enablevmslt'] = array('minlen' => 1,'maxlen' => 40);
$array['filter']['fwdbusy'] = array('minlen' => 1,'maxlen' => 40);
$array['filter']['fwdrna'] = array('minlen' => 1,'maxlen' => 40);
$array['filter']['fwdunc'] = array('minlen' => 1,'maxlen' => 40);
$array['filter']['fwdundoall'] = array('minlen' => 1,'maxlen' => 40,'regexp' => '/^[0-9\*#]+$/');
$array['filter']['groupaddmember'] = array('minlen' => 1,'maxlen' => 40);
$array['filter']['groupremovemember'] = array('minlen' => 1,'maxlen' => 40);
$array['filter']['grouptogglemember'] = array('minlen' => 1,'maxlen' => 40);
$array['filter']['guestprov'] = array('minlen' => 1,'maxlen' => 40,'regexp' => '/^[0-9\*#]+$/');
$array['filter']['incallfilter'] = array('minlen' => 1,'maxlen' => 40,'regexp' => '/^[0-9\*#]+$/');
$array['filter']['phoneprogfunckey'] = array('minlen' => 1,'maxlen' => 40);
$array['filter']['phonestatus'] = array('minlen' => 1,'maxlen' => 40,'regexp' => '/^[0-9\*#]+$/');
$array['filter']['pickup'] = array('minlen' => 1,'maxlen' => 40);
$array['filter']['queueaddmember'] = array('minlen' => 1,'maxlen' => 40);
$array['filter']['queueremovemember'] = array('minlen' => 1,'maxlen' => 40);
$array['filter']['queuetogglemember'] = array('minlen' => 1,'maxlen' => 40);
$array['filter']['recsnd'] = array('minlen' => 1,'maxlen' => 40,'regexp' => '/^[0-9\*#]+$/');
$array['filter']['vmboxmsgslt'] = array('minlen' => 1,'maxlen' => 40);
$array['filter']['vmboxpurgeslt'] = array('minlen' => 1,'maxlen' => 40);
$array['filter']['vmboxslt'] = array('minlen' => 1,'maxlen' => 40);
$array['filter']['vmusermsg'] = array('minlen' => 1,'maxlen' => 40,'regexp' => '/^[0-9\*]+$/');
$array['filter']['vmuserpurge'] = array('minlen' => 1,'maxlen' => 40,'regexp' => '/^[0-9\*#]+$/');
$array['filter']['vmuserpurgeslt'] = array('minlen' => 1,'maxlen' => 40);
$array['filter']['vmuserslt'] = array('minlen' => 1,'maxlen' => 40);

?>

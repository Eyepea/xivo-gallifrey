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
$array['exten']['voicemsg'] = array();
$array['exten']['voicemsg']['app'] = 'Macro';
$array['exten']['voicemsg']['appdata'] = 'voicemsg';

$array['exten']['fwdundoall'] = array();
$array['exten']['fwdundoall']['app'] = 'Macro';
$array['exten']['fwdundoall']['appdata'] = 'fwdundoall';

$array['exten']['fwdundorna'] = array();
$array['exten']['fwdundorna']['app'] = 'Macro';
$array['exten']['fwdundorna']['appdata'] = 'fwdundorna';

$array['exten']['fwdundobusy'] = array();
$array['exten']['fwdundobusy']['app'] = 'Macro';
$array['exten']['fwdundobusy']['appdata'] = 'fwdundobusy';

$array['exten']['fwdundounc'] = array();
$array['exten']['fwdundounc']['app'] = 'Macro';
$array['exten']['fwdundounc']['appdata'] = 'fwdundounc';

$array['exten']['fwdrna'] = array();
$array['exten']['fwdrna']['app'] = 'Macro';
$array['exten']['fwdrna']['appdata'] = array();
$array['exten']['fwdrna']['appdata'][0] = 'fwdrna';
$array['exten']['fwdrna']['appdata'][1] = '${EXTEN:$<EXTNUMLEN>}';

$array['exten']['fwdbusy'] = array();
$array['exten']['fwdbusy']['app'] = 'Macro';
$array['exten']['fwdbusy']['appdata'] = array();
$array['exten']['fwdbusy']['appdata'][0] = 'fwdbusy';
$array['exten']['fwdbusy']['appdata'][1] = '${EXTEN:$<EXTNUMLEN>}';

$array['exten']['fwdunc'] = array();
$array['exten']['fwdunc']['app'] = 'Macro';
$array['exten']['fwdunc']['appdata'] = array();
$array['exten']['fwdunc']['appdata'][0] = 'fwdunc';
$array['exten']['fwdunc']['appdata'][1] = '${EXTEN:$<EXTNUMLEN>}';

$array['exten']['recsnd'] = array();
$array['exten']['recsnd']['app'] = 'Macro';
$array['exten']['recsnd']['appdata'] = array();
$array['exten']['recsnd']['appdata'][0] = 'recsnd';
$array['exten']['recsnd']['appdata'][1] = 'wav';

$array['exten']['phonestatus'] = array();
$array['exten']['phonestatus']['app'] = 'Macro';
$array['exten']['phonestatus']['appdata'] = 'phonestatus';

$array['exten']['enablevm'] = array();
$array['exten']['enablevm']['app'] = 'Macro';
$array['exten']['enablevm']['appdata'] = 'enablevm';

$array['exten']['enablednd'] = array();
$array['exten']['enablednd']['app'] = 'Macro';
$array['exten']['enablednd']['appdata'] = 'enablednd';

$array['exten']['incallrec'] = array();
$array['exten']['incallrec']['app'] = 'Macro';
$array['exten']['incallrec']['appdata'] = 'incallrec';

$array['exten']['incallfilter'] = array();
$array['exten']['incallfilter']['app'] = 'Macro';
$array['exten']['incallfilter']['appdata'] = 'incallfilter';

$array['exten']['pickup'] = array();
$array['exten']['pickup']['app'] = 'Pickup';
$array['exten']['pickup']['appdata'] = '${EXTEN:$<EXTNUMLEN>}';

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

$array['exten']['agentdynamiclogin'] = array();
$array['exten']['agentdynamiclogin']['app'] = 'AgentLogin';
$array['exten']['agentdynamiclogin']['appdata'] = '';

$array['exten']['calllistening'] = array();
$array['exten']['calllistening']['app'] = 'Macro';
$array['exten']['calllistening']['appdata'] = 'calllistening';

$array['exten']['vmdelete'] = array();
$array['exten']['vmdelete']['app'] = 'Macro';
$array['exten']['vmdelete']['appdata'] = 'vmdelete';

$array['exten']['directoryaccess'] = array();
$array['exten']['directoryaccess']['app'] = 'Directory';
$array['exten']['directoryaccess']['appdata'] = '${CONTEXT}';

$array['exten']['bsfilter'] = array();
$array['exten']['bsfilter']['app'] = 'Macro';
$array['exten']['bsfilter']['appdata'] = array();
$array['exten']['bsfilter']['appdata'][0] = 'bsfilter';
$array['exten']['bsfilter']['appdata'][1] = '${EXTEN:$<EXTNUMLEN>}';

$array['element'] = array();
$array['element']['voicemsg'] = array();
$array['element']['voicemsg']['default'] = '*98';

$array['element']['fwdundoall'] = array();
$array['element']['fwdundoall']['default'] = '*20';

$array['element']['fwdundorna'] = array();
$array['element']['fwdundorna']['default'] = '*22';

$array['element']['fwdundobusy'] = array();
$array['element']['fwdundobusy']['default'] = '*23';

$array['element']['fwdundounc'] = array();
$array['element']['fwdundounc']['default'] = '*21';

$array['element']['fwdrna'] = array();
$array['element']['fwdrna']['default'] = '*22.';

$array['element']['fwdbusy'] = array();
$array['element']['fwdbusy']['default'] = '*23.';

$array['element']['fwdunc'] = array();
$array['element']['fwdunc']['default'] = '*21.';

$array['element']['recsnd'] = array();
$array['element']['recsnd']['default'] = '*9';

$array['element']['phonestatus'] = array();
$array['element']['phonestatus']['default'] = '*10';

$array['element']['enablevm'] = array();
$array['element']['enablevm']['default'] = '*24';

$array['element']['enablednd'] = array();
$array['element']['enablednd']['default'] = '*25';

$array['element']['incallrec'] = array();
$array['element']['incallrec']['default'] = '*26';

$array['element']['incallfilter'] = array();
$array['element']['incallfilter']['default'] = '*27';

$array['element']['pickup'] = array();
$array['element']['pickup']['default'] = '*8.';

$array['element']['agentstaticlogin'] = array();
$array['element']['agentstaticlogin']['default'] = '*31.';

$array['element']['agentstaticlogoff'] = array();
$array['element']['agentstaticlogoff']['default'] = '*32.';

$array['element']['agentdynamiclogin'] = array();
$array['element']['agentdynamiclogin']['default'] = '*33';

$array['element']['calllistening'] = array();
$array['element']['calllistening']['default'] = '*34';

$array['element']['vmdelete'] = array();
$array['element']['vmdelete']['default'] = '*35';

$array['element']['directoryaccess'] = array();
$array['element']['directoryaccess']['default'] = '*36';

$array['element']['bsfilter'] = array();
$array['element']['bsfilter']['default'] = '*37.';

$array['filter'] = array();
$array['filter']['voicemsg'] = array('minlen' => 1,'maxlen' => 40,'regexp' => '/^[0-9\*#]+$/');
$array['filter']['fwdundoall'] = array('minlen' => 1,'maxlen' => 40,'regexp' => '/^[0-9\*#]+$/');
$array['filter']['fwdundorna'] = array('minlen' => 1,'maxlen' => 40,'regexp' => '/^[0-9\*#]+$/');
$array['filter']['fwdundobusy'] = array('minlen' => 1,'maxlen' => 40,'regexp' => '/^[0-9\*#]+$/');
$array['filter']['fwdundounc'] = array('minlen' => 1,'maxlen' => 40,'regexp' => '/^[0-9\*#]+$/');
$array['filter']['fwdrna'] = array('minlen' => 1,'maxlen' => 40);
$array['filter']['fwdbusy'] = array('minlen' => 1,'maxlen' => 40);
$array['filter']['fwdunc'] = array('minlen' => 1,'maxlen' => 40);
$array['filter']['recsnd'] = array('minlen' => 1,'maxlen' => 40,'regexp' => '/^[0-9\*#]+$/');
$array['filter']['phonestatus'] = array('minlen' => 1,'maxlen' => 40,'regexp' => '/^[0-9\*#]+$/');
$array['filter']['enablevm'] = array('minlen' => 1,'maxlen' => 40,'regexp' => '/^[0-9\*#]+$/');
$array['filter']['enablednd'] = array('minlen' => 1,'maxlen' => 40,'regexp' => '/^[0-9\*#]+$/');
$array['filter']['incallrec'] = array('minlen' => 1,'maxlen' => 40,'regexp' => '/^[0-9\*#]+$/');
$array['filter']['incallfilter'] = array('minlen' => 1,'maxlen' => 40,'regexp' => '/^[0-9\*#]+$/');
$array['filter']['pickup'] = array('minlen' => 1,'maxlen' => 40);
$array['filter']['agentstaticlogin'] = array('minlen' => 1,'maxlen' => 40);
$array['filter']['agentstaticlogoff'] = array('minlen' => 1,'maxlen' => 40);
$array['filter']['agentdynamiclogin'] = array('minlen' => 1,'maxlen' => 40,'regexp' => '/^[0-9\*#]+$/');
$array['filter']['calllistening'] = array('minlen' => 1,'maxlen' => 40,'regexp' => '/^[0-9\*#]+$/');
$array['filter']['vmdelete'] = array('minlen' => 1,'maxlen' => 40,'regexp' => '/^[0-9\*#]+$/');
$array['filter']['directoryaccess'] = array('minlen' => 1,'maxlen' => 40,'regexp' => '/^[0-9\*#]+$/');
$array['filter']['bsfilter'] = array('minlen' => 1,'maxlen' => 40);

?>

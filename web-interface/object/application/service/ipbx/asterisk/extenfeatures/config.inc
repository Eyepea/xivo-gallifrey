<?php

$array = array();

$array['exten'] = array();
$array['exten']['voicemsg'] = array();
$array['exten']['voicemsg']['_line'][] = array('priority' => 1,'app' => 'VoiceMailMain','appdata' => '${CALLERID(num)}@${CONTEXT}|s');
$array['exten']['voicemsg']['_line'][] = array('priority' => 2,'app' => 'Hangup','appdata' => '');

$array['exten']['fwdundoall'] = array();
$array['exten']['fwdundoall']['app'] = 'Macro';
$array['exten']['fwdundoall']['_line'][] = array('priority' => 1,'appdata' => 'features|FWD/Unc/Status|||1');
$array['exten']['fwdundoall']['_line'][] = array('priority' => 2,'appdata' => 'features|FWD/RNA/Status|||1');
$array['exten']['fwdundoall']['_line'][] = array('priority' => 3,'appdata' => 'features|FWD/Busy/Status|all-forward-off|all-forward-off|1');

$array['exten']['fwdundorna'] = array();
$array['exten']['fwdundorna']['priority'] = 1;
$array['exten']['fwdundorna']['app'] = 'Macro';
$array['exten']['fwdundorna']['appdata'] = 'features|FWD/RNA/Status|forward-on|forward-off|1';

$array['exten']['fwdundobusy'] = array();
$array['exten']['fwdundobusy']['priority'] = 1;
$array['exten']['fwdundobusy']['app'] = 'Macro';
$array['exten']['fwdundobusy']['appdata'] = 'features|FWD/Busy/Status|forward-on|forward-off|1';

$array['exten']['fwdundounc'] = array();
$array['exten']['fwdundounc']['priority'] = 1;
$array['exten']['fwdundounc']['app'] = 'Macro';
$array['exten']['fwdundounc']['appdata'] = 'features|FWD/Unc/Status|forward-on|forward-off|1';

$array['exten']['fwdrna'] = array();
$array['exten']['fwdrna']['app'] = 'Set';
$array['exten']['fwdrna']['appdata'] = '';
$array['exten']['fwdrna']['_line'][] = array('priority' => 1,'app' => 'Answer');
$array['exten']['fwdrna']['_line'][] = array('priority' => 2,'app' => 'Wait','appdata' => '0.5');
$array['exten']['fwdrna']['_line'][] = array('priority' => 3,'appdata' => 'USER=${CALLERID(num)}');
$array['exten']['fwdrna']['_line'][] = array('priority' => 4,'app' => 'Macro','appdata' => 'antiloops');
$array['exten']['fwdrna']['_line'][] = array('priority' => 5,'appdata' => 'DB(${CONTEXT}/users/${USER}/FWD/RNA/Number)=${EXTEN:$<EXTNUMLEN>}');
$array['exten']['fwdrna']['_line'][] = array('priority' => 6,'appdata' => 'DB(${CONTEXT}/users/${USER}/FWD/RNA/Status)=1');
$array['exten']['fwdrna']['_line'][] = array('priority' => 7,'app' => 'Playback','appdata' => 'forward-on');
$array['exten']['fwdrna']['_line'][] = array('priority' => 8,'app' => 'Hangup');

$array['exten']['fwdbusy'] = array();
$array['exten']['fwdbusy']['app'] = 'Set';
$array['exten']['fwdbusy']['appdata'] = '';
$array['exten']['fwdbusy']['_line'][] = array('priority' => 1,'app' => 'Answer');
$array['exten']['fwdbusy']['_line'][] = array('priority' => 2,'app' => 'Wait','appdata' => '0.5');
$array['exten']['fwdbusy']['_line'][] = array('priority' => 3,'appdata' => 'USER=${CALLERID(num)}');
$array['exten']['fwdbusy']['_line'][] = array('priority' => 4,'app' => 'Macro','appdata' => 'antiloops');
$array['exten']['fwdbusy']['_line'][] = array('priority' => 5,'appdata' => 'DB(${CONTEXT}/users/${USER}/FWD/Busy/Number)=${EXTEN:$<EXTNUMLEN>}');
$array['exten']['fwdbusy']['_line'][] = array('priority' => 6,'appdata' => 'DB(${CONTEXT}/users/${USER}/FWD/Busy/Status)=1');
$array['exten']['fwdbusy']['_line'][] = array('priority' => 7,'app' => 'Playback','appdata' => 'forward-on');
$array['exten']['fwdbusy']['_line'][] = array('priority' => 8,'app' => 'Hangup');

$array['exten']['fwdunc'] = array();
$array['exten']['fwdunc']['app'] = 'Set';
$array['exten']['fwdunc']['appdata'] = '';
$array['exten']['fwdunc']['_line'][] = array('priority' => 1,'app' => 'Answer');
$array['exten']['fwdunc']['_line'][] = array('priority' => 2,'app' => 'Wait','appdata' => '0.5');
$array['exten']['fwdunc']['_line'][] = array('priority' => 3,'appdata' => 'USER=${CALLERID(num)}');
$array['exten']['fwdunc']['_line'][] = array('priority' => 4,'app' => 'Macro','appdata' => 'antiloops');
$array['exten']['fwdunc']['_line'][] = array('priority' => 5,'appdata' => 'DB(${CONTEXT}/users/${USER}/FWD/Unc/Number)=${EXTEN:$<EXTNUMLEN>}');
$array['exten']['fwdunc']['_line'][] = array('priority' => 6,'appdata' => 'DB(${CONTEXT}/users/${USER}/FWD/Unc/Status)=1');
$array['exten']['fwdunc']['_line'][] = array('priority' => 7,'app' => 'Playback','appdata' => 'forward-on');
$array['exten']['fwdunc']['_line'][] = array('priority' => 8,'app' => 'Hangup');

$array['exten']['recsnd'] = array();
$array['exten']['recsnd']['app'] = 'Set';
$array['exten']['recsnd']['appdata'] = '';
$array['exten']['recsnd']['_line'][] = array('priority' => 1,'app' => 'Playback','appdata' => 'record-message-after-beep');
$array['exten']['recsnd']['_line'][] = array('priority' => 2,'appdata' => 'ROOT=/usr/share/asterisk/sounds/web-interface/recordings');
$array['exten']['recsnd']['_line'][] = array('priority' => 3,'appdata' => 'RECORDFILE=${ROOT}/${CALLERID(num)}-${EPOCH}');
$array['exten']['recsnd']['_line'][] = array('priority' => 4,'appdata' => 'EXTRECORDFILE=wav');
$array['exten']['recsnd']['_line'][] = array('priority' => 5,'appdata' => 'RECORDFILENAME=${RECORDFILE}.${EXTRECORDFILE}');
$array['exten']['recsnd']['_line'][] = array('priority' => 6,'app' => 'Record','appdata' => '${RECORDFILE}:${EXTRECORDFILE}');
$array['exten']['recsnd']['_line'][] = array('priority' => 7,'app' => 'Wait','appdata' => 1);
$array['exten']['recsnd']['_line'][] = array('priority' => 8,'app' => 'Playback','appdata' => '${RECORDFILE}');
$array['exten']['recsnd']['_line'][] = array('priority' => 9,'app' => 'Hangup');

$array['exten']['phonestatus'] = array();
$array['exten']['phonestatus']['app'] = 'Playback';
$array['exten']['phonestatus']['appdata'] = '';
$array['exten']['phonestatus']['_line'][] = array('priority' => 1,'app' => 'Answer');
$array['exten']['phonestatus']['_line'][] = array('priority' => 2,'appdata' => 'status-phone');
$array['exten']['phonestatus']['_line'][] = array('priority' => 3,'appdata' => 'forward-inc');
$array['exten']['phonestatus']['_line'][] = array('priority' => 4,'app' => 'Macro','appdata' => 'checkdbfeatures|FWD/Unc/Status|1|FWD/Unc/Number');
$array['exten']['phonestatus']['_line'][] = array('priority' => 5,'appdata' => 'forward-busy');
$array['exten']['phonestatus']['_line'][] = array('priority' => 6,'app' => 'Macro','appdata' => 'checkdbfeatures|FWD/Busy/Status|1|FWD/Busy/Number');
$array['exten']['phonestatus']['_line'][] = array('priority' => 7,'appdata' => 'forward-rna');
$array['exten']['phonestatus']['_line'][] = array('priority' => 8,'app' => 'Macro','appdata' => 'checkdbfeatures|FWD/RNA/Status|1|FWD/RNA/Number');
$array['exten']['phonestatus']['_line'][] = array('priority' => 9,'appdata' => 'vm-status');
$array['exten']['phonestatus']['_line'][] = array('priority' => 10,'app' => 'Macro','appdata' => 'checkdbfeatures|VM');
$array['exten']['phonestatus']['_line'][] = array('priority' => 11,'appdata' => 'screening-status');
$array['exten']['phonestatus']['_line'][] = array('priority' => 12,'app' => 'Macro','appdata' => 'checkdbfeatures|Screen');
$array['exten']['phonestatus']['_line'][] = array('priority' => 13,'appdata' => 'record-status');
$array['exten']['phonestatus']['_line'][] = array('priority' => 14,'app' => 'Macro','appdata' => 'checkdbfeatures|Record');
$array['exten']['phonestatus']['_line'][] = array('priority' => 15,'appdata' => 'dnd-status');
$array['exten']['phonestatus']['_line'][] = array('priority' => 16,'app' => 'Macro','appdata' => 'checkdbfeatures|DND');
$array['exten']['phonestatus']['_line'][] = array('priority' => 17,'appdata' => 'bye');
$array['exten']['phonestatus']['_line'][] = array('priority' => 18,'app' => 'Hangup');

$array['exten']['enablevm'] = array();
$array['exten']['enablevm']['priority'] = 1;
$array['exten']['enablevm']['app'] = 'Macro';
$array['exten']['enablevm']['appdata'] = 'features|VM|vm-on|vm-off';

$array['exten']['enablednd'] = array();
$array['exten']['enablednd']['priority'] = 1;
$array['exten']['enablednd']['app'] = 'Macro';
$array['exten']['enablednd']['appdata'] = 'features|DND|dnd-on|dnd-off';

$array['exten']['enablednd'] = array();
$array['exten']['enablednd']['priority'] = 1;
$array['exten']['enablednd']['app'] = 'Macro';
$array['exten']['enablednd']['appdata'] = 'features|DND|dnd-on|dnd-off';

$array['exten']['incallrec'] = array();
$array['exten']['incallrec']['priority'] = 1;
$array['exten']['incallrec']['app'] = 'Macro';
$array['exten']['incallrec']['appdata'] = 'features|Record|record-call-on|record-call-off';

$array['exten']['incallfilter'] = array();
$array['exten']['incallfilter']['priority'] = 1;
$array['exten']['incallfilter']['app'] = 'Macro';
$array['exten']['incallfilter']['appdata'] = 'features|Screen|screening-on|screening-off';

$array['exten']['pickup'] = array();
$array['exten']['pickup']['priority'] = 1;
$array['exten']['pickup']['app'] = 'Pickup';
$array['exten']['pickup']['appdata'] = '${EXTEN:$<EXTNUMLEN>}';

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

?>

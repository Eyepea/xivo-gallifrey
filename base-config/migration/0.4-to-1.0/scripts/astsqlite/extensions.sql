INSERT INTO extensions VALUES (NULL,0,'xivo-features','*91',1,'Macro','enablevmbox','enablevmbox');
INSERT INTO extensions VALUES (NULL,0,'xivo-features','_*91.',1,'Macro','enablevmbox|${EXTEN:3}','enablevmboxslt');
INSERT INTO extensions VALUES (NULL,0,'xivo-features','_*90.',1,'Macro','enablevm|${EXTEN:3}','enablevmslt');
INSERT INTO extensions VALUES (NULL,0,'xivo-features','*48378',1,'Macro','guestprov','guestprov');
INSERT INTO extensions VALUES (NULL,0,'xivo-features','_*99.',1,'Macro','vmboxmsg|${EXTEN:3}','vmboxmsgslt');
INSERT INTO extensions VALUES (NULL,1,'xivo-features','_*93.',1,'Macro','vmboxpurge|${EXTEN:3}','vmboxpurgeslt');
INSERT INTO extensions VALUES (NULL,0,'xivo-features','_*97.',1,'Macro','vmbox|${EXTEN:3}','vmboxslt');
INSERT INTO extensions VALUES (NULL,1,'xivo-features','_*92.',1,'Macro','vmuserpurge|${EXTEN:3}','vmuserpurgeslt');
INSERT INTO extensions VALUES (NULL,0,'xivo-features','_*96.',1,'Macro','vmuser|${EXTEN:3}','vmuserslt');

UPDATE extensions
SET app = 'Macro',
    appdata = 'agentdynamiclogin|${EXTEN:'||LENGTH(exten)||'}',
    exten = '_'||exten||'.'
WHERE name = 'agentdynamiclogin' AND app = 'AgentLogin';

UPDATE extensions
SET appdata = 'agentstaticlogoff|${EXTEN:'||LENGTH(exten)||'}',
    exten = '_'||exten||'.'
WHERE name = 'agentstaticlogoff'
AND SUBSTR(exten,0,1) != '_'
AND appdata NOT LIKE 'agentstaticlogoff|${EXTEN:%}';

UPDATE extensions
SET exten = '*90'
WHERE name = 'enablevm' AND exten = '*24';

UPDATE extensions
SET name = 'vmuserpurge', appdata = 'vmuserpurge'
WHERE name = 'vmdelete';

UPDATE extensions
SET exten = '*92'
WHERE name = 'vmuserpurge' AND exten = '*35';

UPDATE extensions
SET name = 'vmusermsg', appdata = 'vmusermsg'
WHERE name = 'voicemsg';

UPDATE extensions
SET appdata = appdata||'%${CONTEXT}@PICKUPMARK'
WHERE name = 'pickup' AND appdata NOT LIKE '%${CONTEXT}@PICKUPMARK';

UPDATE extensions
SET appdata = appdata||'|'
WHERE app = 'Macro' AND appdata LIKE 'voicemail|%' AND SUBSTR(appdata,LENGTH(appdata),1) != '|';

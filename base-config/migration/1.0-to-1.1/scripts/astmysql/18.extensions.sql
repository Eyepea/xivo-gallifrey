
-- delete
INSERT INTO `extensions` VALUES (NULL,1,'xivo-features','_*30.',1,'Macro','agentstaticlogtoggle|${EXTEN:3}'     ,'agentstaticlogtoggle');
INSERT INTO `extensions` VALUES (NULL,0,'xivo-features','_*51.',1,'Macro','groupmember|group|add|${EXTEN:3}'    ,'groupaddmember');
INSERT INTO `extensions` VALUES (NULL,0,'xivo-features','_*52.',1,'Macro','groupmember|group|remove|${EXTEN:3}' ,'groupremovemember');
INSERT INTO `extensions` VALUES (NULL,0,'xivo-features','_*50.',1,'Macro','groupmember|group|toggle|${EXTEN:3}' ,'grouptogglemember');
INSERT INTO `extensions` VALUES (NULL,0,'xivo-features','_*56.',1,'Macro','groupmember|queue|add|${EXTEN:3}'    ,'queueaddmember');
INSERT INTO `extensions` VALUES (NULL,0,'xivo-features','_*57.',1,'Macro','groupmember|queue|remove|${EXTEN:3}' ,'queueremovemember');
INSERT INTO `extensions` VALUES (NULL,0,'xivo-features','_*55.',1,'Macro','groupmember|queue|toggle|${EXTEN:3}' ,'queuetogglemember');
INSERT INTO `extensions` VALUES (NULL,0,'xivo-features','_*664.',1,'Macro','group|${EXTEN:4}|'  ,'callgroup');
INSERT INTO `extensions` VALUES (NULL,0,'xivo-features','_*667.',1,'Macro','meetme|${EXTEN:4}|' ,'callmeetme');
INSERT INTO `extensions` VALUES (NULL,0,'xivo-features','_*665.',1,'Macro','queue|${EXTEN:4}|'  ,'callqueue');
INSERT INTO `extensions` VALUES (NULL,0,'xivo-features','_*666.',1,'Macro','user|${EXTEN:4}|'   ,'calluser');
INSERT INTO `extensions` VALUES (NULL,0,'xivo-features','_*735.',1,'Macro','phoneprogfunckey|${EXTEN:0:4}|${EXTEN:4}','phoneprogfunckey');

-- delete
DELETE FROM `extensions` WHERE context = 'xivo-features' AND exten = '*23';
DELETE FROM `extensions` WHERE context = 'xivo-features' AND exten = '*22';
DELETE FROM `extensions` WHERE context = 'xivo-features' AND exten = '*21';

-- alter
UPDATE `extensions` SET
		appdata = 'feature_forward|busy|${EXTEN:3}'
	WHERE
		context = 'xivo-features' AND
		exten   = '_*23.';

UPDATE `extensions` SET
		appdata = 'feature_forward|rna|${EXTEN:3}'
	WHERE
		context = 'xivo-features' AND
		exten   = '_*22.';

UPDATE `extensions` SET
		appdata = 'feature_forward|unc|${EXTEN:3}'
	WHERE
		context = 'xivo-features' AND
		exten   = '_*21.';

UPDATE `extensions` SET
		appdata = 'callrecord',
		name    = 'callrecord'
	WHERE
		context = 'xivo-features' AND
		exten   = '*26';


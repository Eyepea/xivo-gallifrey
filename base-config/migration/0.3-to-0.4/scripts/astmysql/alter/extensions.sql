ALTER TABLE `extensions` MODIFY COLUMN `exten` varchar(40) binary NOT NULL DEFAULT '';
ALTER TABLE `extensions` MODIFY COLUMN `priority` tinyint unsigned NOT NULL DEFAULT 0;

UPDATE `extensions`
SET `context` = 'xivo-features'
WHERE `context` = 'features';

UPDATE `extensions`
SET `context` = 'xivo-handynumbers'
WHERE `context` = 'handynumbers';

UPDATE `extensions`
SET `appdata` = 'agentstaticlogoff|${EXTEN:3}'
WHERE `name` = 'agentstaticlogoff';

UPDATE `extensions`
SET `appdata` = 'recsnd|wav'
WHERE `name` = 'recsnd';

UPDATE `extensions`
SET `appdata` = 'vmdelete'
WHERE `name` = 'vmdelete';

UPDATE `extensions`
SET `appdata` = (
	SELECT CONCAT('user|',`userfeatures`.`id`,'|')
	FROM `userfeatures`
	WHERE `extensions`.`exten` = `userfeatures`.`number`
	AND `extensions`.`context` = `userfeatures`.`context`)
WHERE `app` = 'Macro' AND `appdata` = 'incoming_user';

UPDATE `extensions`
SET `appdata` = (
	SELECT CONCAT('group|',`groupfeatures`.`id`,'|')
	FROM `groupfeatures`
	WHERE `extensions`.`exten` = `groupfeatures`.`number`
	AND `extensions`.`context` = `groupfeatures`.`context`
	AND NOT `groupfeatures`.`deleted`)
WHERE `app` = 'Macro' AND `appdata` = 'incoming_group';

UPDATE `extensions`
SET `appdata` = (
	SELECT CONCAT('queue|',`queuefeatures`.`id`,'|')
	FROM `queuefeatures`
	WHERE `extensions`.`exten` = `queuefeatures`.`number`
	AND `extensions`.`context` = `queuefeatures`.`context`)
WHERE `app` = 'Macro' AND `appdata` = 'incoming_queue';

UPDATE `extensions`
SET `appdata` = (
	SELECT CONCAT('meetme|',`meetmefeatures`.`id`,'|')
	FROM `meetmefeatures`
	WHERE `extensions`.`exten` = `meetmefeatures`.`number`
	AND `extensions`.`context` = `meetmefeatures`.`context`)
WHERE `app` = 'Macro' AND `appdata` = 'incoming_meetme';

UPDATE `extensions`
SET
	`context` = 'from-extern',
	`appdata` = CONCAT('did|',SUBSTRING_INDEX(`extensions`.`appdata`,'|',-1))
WHERE `app` = 'Macro' AND `appdata` LIKE 'incoming_did|%';

UPDATE `extensions`
SET `appdata` = (
	SELECT CONCAT('outcall|',`outcall`.`id`)
	FROM `outcall`
	WHERE `extensions`.`exten` = `outcall`.`exten`
	AND `extensions`.`context` = `outcall`.`context`)
WHERE `app` = 'Macro' AND `appdata` LIKE 'outgoing_user|%';

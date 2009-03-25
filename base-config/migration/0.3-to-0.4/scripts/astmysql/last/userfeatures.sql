UPDATE `userfeatures`
SET `name` = REPLACE(`name`,'zap/','dahdi/')
WHERE `name` LIKE 'zap/%' AND `protocol` = 'custom';

UPDATE `userfeatures`
SET `voicemailid` = (
	SELECT `voicemail`.`uniqueid`
	FROM `usersip`, `voicemail`
	WHERE `usersip`.`mailbox` = CONCAT(`voicemail`.`mailbox`,'@',`voicemail`.`context`)
	AND `userfeatures`.`protocol` = 'sip'
	AND `userfeatures`.`protocolid` = `usersip`.`id`)
WHERE NOT `internal`;

UPDATE `userfeatures`
SET `voicemailid` = (
	SELECT `voicemail`.`uniqueid`
	FROM `useriax`, `voicemail`
	WHERE `useriax`.`mailbox` = CONCAT(`voicemail`.`mailbox`,'@',`voicemail`.`context`)
	AND `userfeatures`.`protocol` = 'iax'
	AND `userfeatures`.`protocolid` = `useriax`.`id`)
WHERE NOT `internal` AND `voicemailid` IS NULL;

UPDATE `userfeatures`
SET `loginclient` = IFNULL((
	SELECT SUBSTRING_INDEX(
			IFNULL(
				NULLIF(`voicemail`.`email`,''),
				IFNULL(NULLIF(`userfeatures`.`number`,''),`userfeatures`.`name`)),
			'@',1)
	FROM `voicemail`
	WHERE `userfeatures`.`voicemailid` = `voicemail`.`uniqueid`),'')
WHERE `enableclient` AND NOT `internal`;

UPDATE `userfeatures`
SET `passwdclient` = IFNULL((
	SELECT IF(LENGTH(IFNULL(`voicemail`.`password`,'')) < 4,'0000',`voicemail`.`password`)
	FROM `voicemail`
	WHERE `userfeatures`.`voicemailid` = `voicemail`.`uniqueid`),'')
WHERE `enableclient` AND NOT `internal`;

UPDATE `userfeatures`
SET `profileclient` = 'client'
WHERE `enableclient` AND NOT `internal`;

UPDATE `userfeatures`
	INNER JOIN `userfeatures` AS `ufeatures`
	ON `userfeatures`.`loginclient` = `ufeatures`.`loginclient`
	AND `userfeatures`.`id` != `ufeatures`.`id`
SET
	`userfeatures`.`enableclient` = 0,
	`userfeatures`.`loginclient` = '',
	`userfeatures`.`passwdclient` = '',
	`userfeatures`.`profileclient` = ''
WHERE NOT `userfeatures`.`internal` AND `userfeatures`.`loginclient` != '';

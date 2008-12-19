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
WHERE NOT `internal`;

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
SET `loginclient` = IFNULL((
	SELECT IF(LENGTH(IFNULL(`voicemail`.`password`,'')) < 4,'0000',`voicemail`.`password`)
	FROM `voicemail`
	WHERE `userfeatures`.`voicemailid` = `voicemail`.`uniqueid`),'')
WHERE `enableclient` AND NOT `internal`;

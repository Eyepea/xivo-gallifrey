UPDATE `usersip`
SET `setvar` = IFNULL((
	SELECT CONCAT('XIVO_USERID=',`userfeatures`.`id`)
	FROM `userfeatures`
	WHERE `usersip`.`id` = `userfeatures`.`protocolid`
	AND `userfeatures`.`protocol` = 'sip'),'')
WHERE `usersip`.`category` = 'user';

UPDATE `usersip`
SET `call-limit` = IFNULL((
	SELECT (`userfeatures`.`simultcalls` * 2)
	FROM `userfeatures`
	WHERE `usersip`.`id` = `userfeatures`.`protocolid`
	AND `userfeatures`.`protocol` = 'sip'),0)
WHERE `usersip`.`category` = 'user';

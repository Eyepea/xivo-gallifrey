UPDATE `useriax`
SET `setvar` = IFNULL((
	SELECT CONCAT('XIVO_USERID=',`userfeatures`.`id`)
	FROM `userfeatures`
	WHERE `useriax`.`id` = `userfeatures`.`protocolid`
	AND `userfeatures`.`protocol` = 'iax'),'')
WHERE `useriax`.`category` = 'user';

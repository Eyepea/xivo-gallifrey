INSERT INTO `voicemailfeatures` (
	`id`,
	`voicemailid`,
	`skipcheckpass`)
SELECT
	NULL,
	`uservoicemail`.`uniqueid`,
	`userfeatures`.`skipvoicemailpass`
FROM
	`userfeatures`,
	`uservoicemail`,
	`usersip`
WHERE
	`usersip`.`mailbox` = CONCAT(`uservoicemail`.`mailbox`,'@',`uservoicemail`.`context`)
	AND `userfeatures`.`protocol` = 'sip'
	AND `userfeatures`.`protocolid` = `usersip`.`id`;

INSERT INTO `voicemailfeatures` (
	`id`,
	`voicemailid`,
	`skipcheckpass`)
SELECT
	NULL,
	`uservoicemail`.`uniqueid`,
	`userfeatures`.`skipvoicemailpass`
FROM
	`userfeatures`,
	`uservoicemail`,
	`useriax`
WHERE
	`useriax`.`mailbox` = CONCAT(`uservoicemail`.`mailbox`,'@',`uservoicemail`.`context`)
	AND `userfeatures`.`protocol` = 'iax'
	AND `userfeatures`.`protocolid` = `useriax`.`id`;

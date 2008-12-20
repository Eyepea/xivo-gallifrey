UPDATE `voicemail`
SET `attach` = NULL
WHERE `attach` = (
	SELECT IF(`staticvoicemail`.`var_val` = 'no',0,1)
	FROM `staticvoicemail`
	WHERE `staticvoicemail`.`filename` = 'voicemail.conf'
	AND `staticvoicemail`.`category` = 'general'
	AND `staticvoicemail`.`var_name` = 'attach');

UPDATE `voicemail`
SET `saycid` = NULL
WHERE `saycid` = (
	SELECT IF(`staticvoicemail`.`var_val` = 'no',0,1)
	FROM `staticvoicemail`
	WHERE `staticvoicemail`.`filename` = 'voicemail.conf'
	AND `staticvoicemail`.`category` = 'general'
	AND `staticvoicemail`.`var_name` = 'saycid');

UPDATE `voicemail`
SET `review` = NULL
WHERE `review` = (
	SELECT IF(`staticvoicemail`.`var_val` = 'no',0,1)
	FROM `staticvoicemail`
	WHERE `staticvoicemail`.`filename` = 'voicemail.conf'
	AND `staticvoicemail`.`category` = 'general'
	AND `staticvoicemail`.`var_name` = 'review');

UPDATE `voicemail`
SET `operator` = NULL
WHERE `operator` = (
	SELECT IF(`staticvoicemail`.`var_val` = 'no',0,1)
	FROM `staticvoicemail`
	WHERE `staticvoicemail`.`filename` = 'voicemail.conf'
	AND `staticvoicemail`.`category` = 'general'
	AND `staticvoicemail`.`var_name` = 'operator');

UPDATE `voicemail`
SET `envelope` = NULL
WHERE `envelope` = (
	SELECT IF(`staticvoicemail`.`var_val` = 'no',0,1)
	FROM `staticvoicemail`
	WHERE `staticvoicemail`.`filename` = 'voicemail.conf'
	AND `staticvoicemail`.`category` = 'general'
	AND `staticvoicemail`.`var_name` = 'envelope');

UPDATE `voicemail`
SET `sayduration` = NULL
WHERE `sayduration` = (
	SELECT IF(`staticvoicemail`.`var_val` = 'no',0,1)
	FROM `staticvoicemail`
	WHERE `staticvoicemail`.`filename` = 'voicemail.conf'
	AND `staticvoicemail`.`category` = 'general'
	AND `staticvoicemail`.`var_name` = 'sayduration');

UPDATE `voicemail`
SET `saydurationm` = NULL
WHERE `saydurationm` = (
	SELECT `staticvoicemail`.`var_val`
	FROM `staticvoicemail`
	WHERE `staticvoicemail`.`filename` = 'voicemail.conf'
	AND `staticvoicemail`.`category` = 'general'
	AND `staticvoicemail`.`var_name` = 'saydurationm');

UPDATE `voicemail`
SET `forcename` = NULL
WHERE `forcename` = (
	SELECT IF(`staticvoicemail`.`var_val` = 'no',0,1)
	FROM `staticvoicemail`
	WHERE `staticvoicemail`.`filename` = 'voicemail.conf'
	AND `staticvoicemail`.`category` = 'general'
	AND `staticvoicemail`.`var_name` = 'forcename');

UPDATE `voicemail`
SET `forcegreetings` = NULL
WHERE `forcegreetings` = (
	SELECT IF(`staticvoicemail`.`var_val` = 'no',0,1)
	FROM `staticvoicemail`
	WHERE `staticvoicemail`.`filename` = 'voicemail.conf'
	AND `staticvoicemail`.`category` = 'general'
	AND `staticvoicemail`.`var_name` = 'forcegreetings');

UPDATE `voicemail`
SET `maxmsg` = NULL
WHERE `maxmsg` = (
	SELECT `staticvoicemail`.`var_val`
	FROM `staticvoicemail`
	WHERE `staticvoicemail`.`filename` = 'voicemail.conf'
	AND `staticvoicemail`.`category` = 'general'
	AND `staticvoicemail`.`var_name` = 'maxmsg');

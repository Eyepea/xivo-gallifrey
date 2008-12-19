INSERT INTO `contextmember` (
	`context`,
	`type`,
	`typeval`,
	`varname`)
SELECT
	`agentfeatures`.`context`,
	'agent',
	`agentfeatures`.`id`,
	'context'
FROM `agentfeatures`;

INSERT INTO `contextmember` (
	`context`,
	`type`,
	`typeval`,
	`varname`)
SELECT
	`callfilter`.`context`,
	'callfilter',
	`callfilter`.`id`,
	'context'
FROM `callfilter`;

INSERT INTO `contextmember` (
	`context`,
	`type`,
	`typeval`,
	`varname`)
SELECT
	`staticiax`.`var_val`,
	'generaliax',
	'',
	`staticiax`.`var_name`
FROM `staticiax`
WHERE `filename` = 'iax.conf'
AND `category` = 'general'
AND `var_name` = 'regcontext'
AND NULLIF(`var_val`,'') IS NOT NULL
AND NOT `commented`;

INSERT INTO `contextmember` (
	`context`,
	`type`,
	`typeval`,
	`varname`)
SELECT
	`staticsip`.`var_val`,
	'generalsip',
	'',
	`staticsip`.`var_name`
FROM `staticsip`
WHERE `filename` = 'sip.conf'
AND `category` = 'general'
AND `var_name` IN('context','regcontext')
AND NULLIF(`var_val`,'') IS NOT NULL
AND NOT `commented`;

INSERT INTO `contextmember` (
	`context`,
	`type`,
	`typeval`,
	`varname`)
SELECT
	`staticvoicemail`.`var_val`,
	'generalvoicemail',
	'',
	`staticvoicemail`.`var_name`
FROM `staticvoicemail`
WHERE `filename` = 'voicemail.conf'
AND `category` = 'general'
AND `var_name` IN('cidinternalcontexts',
		  'dialout',
		  'callback',
		  'exitcontext')
AND NULLIF(`var_val`,'') IS NOT NULL
AND NOT `commented`;

INSERT INTO `contextmember` (
	`context`,
	`type`,
	`typeval`,
	`varname`)
SELECT
	`meetmefeatures`.`exitcontext`,
	'meetme',
	`meetmefeatures`.`id`,
	'exitcontext'
FROM `meetmefeatures`
WHERE `exitcontext` != '';

INSERT INTO `contextmember` (
	`context`,
	`type`,
	`typeval`,
	`varname`)
SELECT
	`queue`.`context`,
	'queue',
	`queue`.`name`,
	'context'
FROM `queue`
WHERE `context` IS NOT NULL;

INSERT INTO `contextmember` (
	`context`,
	`type`,
	`typeval`,
	`varname`)
SELECT
	`rightcall`.`context`,
	'rightcall',
	`rightcall`.`id`,
	'context'
FROM `rightcall`;

INSERT INTO `contextmember` (
	`context`,
	`type`,
	`typeval`,
	`varname`)
SELECT
	`schedule`.`context`,
	'schedule',
	`schedule`.`id`,
	'context'
FROM `schedule`;

INSERT INTO `contextmember` (
	`context`,
	`type`,
	`typeval`,
	`varname`)
SELECT
	`usersip`.`context`,
	'trunksip',
	`usersip`.`id`,
	'context'
FROM `usersip`
WHERE `category` = 'trunk'
AND `context` IS NOT NULL;

INSERT INTO `contextmember` (
	`context`,
	`type`,
	`typeval`,
	`varname`)
SELECT
	`useriax`.`context`,
	'trunkiax',
	`useriax`.`id`,
	'context'
FROM `useriax`
WHERE `category` = 'trunk'
AND `context` IS NOT NULL;

INSERT INTO `contextmember` (
	`context`,
	`type`,
	`typeval`,
	`varname`)
SELECT
	`usercustom`.`context`,
	'trunkcustom',
	`usercustom`.`id`,
	'context'
FROM `usercustom`
WHERE `category` = 'trunk'
AND `context` IS NOT NULL;

INSERT INTO `contextmember` (
	`context`,
	`type`,
	`typeval`,
	`varname`)
SELECT
	`outcall`.`context`,
	'outcall',
	`outcall`.`id`,
	'context'
FROM `outcall`;

INSERT INTO `contextmember` (
	`context`,
	`type`,
	`typeval`,
	`varname`)
SELECT
	`voicemail`.`context`,
	'voicemail',
	`voicemail`.`uniqueid`,
	'voicemail'
FROM `voicemail`;

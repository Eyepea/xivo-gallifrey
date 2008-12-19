INSERT INTO `dialaction` (
	`event`,
	`category`,
	`categoryval`,
	`action`,
	`actionarg1`,
	`actionarg2`,
	`linked`)
SELECT
	`dialstatus`.`status`,
	`dialstatus`.`category`,
	`dialstatus`.`categoryval`,
	'none',
	'',
	'',
	1
FROM `dialstatus`
WHERE `type` = 'endcall' AND `typeval` = 'none';

INSERT INTO `dialaction` (
	`event`,
	`category`,
	`categoryval`,
	`action`,
	`actionarg1`,
	`actionarg2`,
	`linked`)
SELECT
	`dialstatus`.`status`,
	`dialstatus`.`category`,
	`dialstatus`.`categoryval`,
	CONCAT('endcall:',`dialstatus`.`typeval`),
	'',
	'',
	1
FROM `dialstatus`
WHERE `type` = 'endcall' AND `typeval` IN('busy','congestion','hangup');

INSERT INTO `dialaction` (
	`event`,
	`category`,
	`categoryval`,
	`action`,
	`actionarg1`,
	`actionarg2`,
	`linked`)
SELECT
	`dialstatus`.`status`,
	`dialstatus`.`category`,
	`dialstatus`.`categoryval`,
	'application:callbackdisa',
	`dialstatus`.`applicationval`,
	'',
	1
FROM `dialstatus`
WHERE `type` = 'application' AND `typeval` = 'callback' AND `applicationval` NOT LIKE '%|%';

INSERT INTO `dialaction` (
	`event`,
	`category`,
	`categoryval`,
	`action`,
	`actionarg1`,
	`actionarg2`,
	`linked`)
SELECT
	`dialstatus`.`status`,
	`dialstatus`.`category`,
	`dialstatus`.`categoryval`,
	'application:callbackdisa',
	SUBSTRING_INDEX(`dialstatus`.`applicationval`,'|',1),
	SUBSTRING_INDEX(`dialstatus`.`applicationval`,'|',-1),
	1
FROM `dialstatus`
WHERE `type` = 'application' AND `typeval` = 'callback' AND `applicationval` LIKE '%|%';

INSERT INTO `dialaction` (
	`event`,
	`category`,
	`categoryval`,
	`action`,
	`actionarg1`,
	`actionarg2`,
	`linked`)
SELECT
	`dialstatus`.`status`,
	`dialstatus`.`category`,
	`dialstatus`.`categoryval`,
	'application:disa',
	`dialstatus`.`applicationval`,
	'',
	1
FROM `dialstatus`
WHERE `type` = 'application' AND `typeval` = 'disa' AND `applicationval` NOT LIKE '%|%';

INSERT INTO `dialaction` (
	`event`,
	`category`,
	`categoryval`,
	`action`,
	`actionarg1`,
	`actionarg2`,
	`linked`)
SELECT
	`dialstatus`.`status`,
	`dialstatus`.`category`,
	`dialstatus`.`categoryval`,
	'application:disa',
	SUBSTRING_INDEX(`dialstatus`.`applicationval`,'|',1),
	SUBSTRING_INDEX(`dialstatus`.`applicationval`,'|',-1),
	1
FROM `dialstatus`
WHERE `type` = 'application' AND `typeval` = 'disa' AND `applicationval` LIKE '%|%';

INSERT INTO `dialaction` (
	`event`,
	`category`,
	`categoryval`,
	`action`,
	`actionarg1`,
	`actionarg2`,
	`linked`)
SELECT
	`dialstatus`.`status`,
	`dialstatus`.`category`,
	`dialstatus`.`categoryval`,
	CONCAT('application:',`dialstatus`.`typeval`),
	`dialstatus`.`applicationval`,
	'',
	1
FROM `dialstatus`
WHERE `type` = 'application' AND `typeval` IN('directory','faxtomail');

INSERT INTO `dialaction` (
	`event`,
	`category`,
	`categoryval`,
	`action`,
	`actionarg1`,
	`actionarg2`,
	`linked`)
SELECT
	`dialstatus`.`status`,
	`dialstatus`.`category`,
	`dialstatus`.`categoryval`,
	'application:voicemailmain',
	`dialstatus`.`applicationval`,
	'',
	1
FROM `dialstatus`
WHERE `type` = 'application' AND `typeval` = 'voicemail';

INSERT INTO `dialaction` (
	`event`,
	`category`,
	`categoryval`,
	`action`,
	`actionarg1`,
	`actionarg2`,
	`linked`)
SELECT
	`dialstatus`.`status`,
	`dialstatus`.`category`,
	`dialstatus`.`categoryval`,
	'sound',
	CONCAT('/var/lib/pf-xivo/sounds/playback/',SUBSTRING(`typeval`,51)),
	'',
	1
FROM `dialstatus`
WHERE `type` = 'sound';

INSERT INTO `dialaction` (
	`event`,
	`category`,
	`categoryval`,
	`action`,
	`actionarg1`,
	`actionarg2`,
	`linked`)
SELECT
	`dialstatus`.`status`,
	`dialstatus`.`category`,
	`dialstatus`.`categoryval`,
	`dialstatus`.`type`,
	`dialstatus`.`typeval`,
	'',
	`dialstatus`.`linked`
FROM `dialstatus`
WHERE `type` NOT IN('endcall','application','sound');

INSERT INTO `dialaction` (
	`event`,
	`category`,
	`categoryval`,
	`action`,
	`actionarg1`,
	`actionarg2`,
	`linked`)
SELECT
	'answer',
	'incall',
	`incall`.`id`,
	'none',
	'',
	'',
	1
FROM `incall`
WHERE `type` = 'endcall' AND `typeval` = 'none';

INSERT INTO `dialaction` (
	`event`,
	`category`,
	`categoryval`,
	`action`,
	`actionarg1`,
	`actionarg2`,
	`linked`)
SELECT
	'answer',
	'incall',
	`incall`.`id`,
	CONCAT('endcall:',`incall`.`typeval`),
	'',
	'',
	1
FROM `incall`
WHERE `type` = 'endcall' AND `typeval` IN('busy','congestion','hangup');

INSERT INTO `dialaction` (
	`event`,
	`category`,
	`categoryval`,
	`action`,
	`actionarg1`,
	`actionarg2`,
	`linked`)
SELECT
	'answer',
	'incall',
	`incall`.`id`,
	'application:callbackdisa',
	`incall`.`applicationval`,
	'',
	1
FROM `incall`
WHERE `type` = 'application' AND `typeval` = 'callback' AND `applicationval` NOT LIKE '%|%';

INSERT INTO `dialaction` (
	`event`,
	`category`,
	`categoryval`,
	`action`,
	`actionarg1`,
	`actionarg2`,
	`linked`)
SELECT
	'answer',
	'incall',
	`incall`.`id`,
	'application:callbackdisa',
	SUBSTRING_INDEX(`incall`.`applicationval`,'|',1),
	SUBSTRING_INDEX(`incall`.`applicationval`,'|',-1),
	1
FROM `incall`
WHERE `type` = 'application' AND `typeval` = 'callback' AND `applicationval` LIKE '%|%';

INSERT INTO `dialaction` (
	`event`,
	`category`,
	`categoryval`,
	`action`,
	`actionarg1`,
	`actionarg2`,
	`linked`)
SELECT
	'answer',
	'incall',
	`incall`.`id`,
	'application:disa',
	`incall`.`applicationval`,
	'',
	1
FROM `incall`
WHERE `type` = 'application' AND `typeval` = 'disa' AND `applicationval` NOT LIKE '%|%';

INSERT INTO `dialaction` (
	`event`,
	`category`,
	`categoryval`,
	`action`,
	`actionarg1`,
	`actionarg2`,
	`linked`)
SELECT
	'answer',
	'incall',
	`incall`.`id`,
	'application:disa',
	SUBSTRING_INDEX(`incall`.`applicationval`,'|',1),
	SUBSTRING_INDEX(`incall`.`applicationval`,'|',-1),
	1
FROM `incall`
WHERE `type` = 'application' AND `typeval` = 'disa' AND `applicationval` LIKE '%|%';

INSERT INTO `dialaction` (
	`event`,
	`category`,
	`categoryval`,
	`action`,
	`actionarg1`,
	`actionarg2`,
	`linked`)
SELECT
	'answer',
	'incall',
	`incall`.`id`,
	CONCAT('application:',`incall`.`typeval`),
	`incall`.`applicationval`,
	'',
	1
FROM `incall`
WHERE `type` = 'application' AND `typeval` IN('directory','faxtomail');

INSERT INTO `dialaction` (
	`event`,
	`category`,
	`categoryval`,
	`action`,
	`actionarg1`,
	`actionarg2`,
	`linked`)
SELECT
	'answer',
	'incall',
	`incall`.`id`,
	'application:voicemailmain',
	`incall`.`applicationval`,
	'',
	1
FROM `incall`
WHERE `type` = 'application' AND `typeval` = 'voicemail';

INSERT INTO `dialaction` (
	`event`,
	`category`,
	`categoryval`,
	`action`,
	`actionarg1`,
	`actionarg2`,
	`linked`)
SELECT
	'answer',
	'incall',
	`incall`.`id`,
	'sound',
	CONCAT('/var/lib/pf-xivo/sounds/playback/',SUBSTRING(`typeval`,51)),
	'',
	1
FROM `incall`
WHERE `type` = 'sound';

INSERT INTO `dialaction` (
	`event`,
	`category`,
	`categoryval`,
	`action`,
	`actionarg1`,
	`actionarg2`,
	`linked`)
SELECT
	'answer',
	'incall',
	`incall`.`id`,
	`incall`.`type`,
	`incall`.`typeval`,
	'',
	`incall`.`linked`
FROM `incall`
WHERE `type` NOT IN('endcall','application','sound');

INSERT INTO `dialaction` (
	`event`,
	`category`,
	`categoryval`,
	`action`,
	`actionarg1`,
	`actionarg2`,
	`linked`)
SELECT
	'inschedule',
	'schedule',
	`schedule`.`id`,
	'none',
	'',
	'',
	1
FROM `schedule`
WHERE `typetrue` = 'endcall' AND `typevaltrue` = 'none';

INSERT INTO `dialaction` (
	`event`,
	`category`,
	`categoryval`,
	`action`,
	`actionarg1`,
	`actionarg2`,
	`linked`)
SELECT
	'outschedule',
	'schedule',
	`schedule`.`id`,
	'none',
	'',
	'',
	1
FROM `schedule`
WHERE `typefalse` = 'endcall' AND `typevalfalse` = 'none';

INSERT INTO `dialaction` (
	`event`,
	`category`,
	`categoryval`,
	`action`,
	`actionarg1`,
	`actionarg2`,
	`linked`)
SELECT
	'inschedule',
	'schedule',
	`schedule`.`id`,
	CONCAT('endcall:',`schedule`.`typevaltrue`),
	'',
	'',
	1
FROM `schedule`
WHERE `typetrue` = 'endcall' AND `typevaltrue` IN('busy','congestion','hangup');

INSERT INTO `dialaction` (
	`event`,
	`category`,
	`categoryval`,
	`action`,
	`actionarg1`,
	`actionarg2`,
	`linked`)
SELECT
	'outschedule',
	'schedule',
	`schedule`.`id`,
	CONCAT('endcall:',`schedule`.`typevalfalse`),
	'',
	'',
	1
FROM `schedule`
WHERE `typefalse` = 'endcall' AND `typevalfalse` IN('busy','congestion','hangup');

INSERT INTO `dialaction` (
	`event`,
	`category`,
	`categoryval`,
	`action`,
	`actionarg1`,
	`actionarg2`,
	`linked`)
SELECT
	'inschedule',
	'schedule',
	`schedule`.`id`,
	'application:callbackdisa',
	`schedule`.`applicationvaltrue`,
	'',
	1
FROM `schedule`
WHERE `typetrue` = 'application' AND `typevaltrue` = 'callback' AND `applicationvaltrue` NOT LIKE '%|%';

INSERT INTO `dialaction` (
	`event`,
	`category`,
	`categoryval`,
	`action`,
	`actionarg1`,
	`actionarg2`,
	`linked`)
SELECT
	'outschedule',
	'schedule',
	`schedule`.`id`,
	'application:callbackdisa',
	`schedule`.`applicationvalfalse`,
	'',
	1
FROM `schedule`
WHERE `typefalse` = 'application' AND `typevalfalse` = 'callback' AND `applicationvalfalse` NOT LIKE '%|%';

INSERT INTO `dialaction` (
	`event`,
	`category`,
	`categoryval`,
	`action`,
	`actionarg1`,
	`actionarg2`,
	`linked`)
SELECT
	'inschedule',
	'schedule',
	`schedule`.`id`,
	'application:callbackdisa',
	SUBSTRING_INDEX(`schedule`.`applicationvaltrue`,'|',1),
	SUBSTRING_INDEX(`schedule`.`applicationvaltrue`,'|',-1),
	1
FROM `schedule`
WHERE `typetrue` = 'application' AND `typevaltrue` = 'callback' AND `applicationvaltrue` LIKE '%|%';

INSERT INTO `dialaction` (
	`event`,
	`category`,
	`categoryval`,
	`action`,
	`actionarg1`,
	`actionarg2`,
	`linked`)
SELECT
	'outschedule',
	'schedule',
	`schedule`.`id`,
	'application:callbackdisa',
	SUBSTRING_INDEX(`schedule`.`applicationvalfalse`,'|',1),
	SUBSTRING_INDEX(`schedule`.`applicationvalfalse`,'|',-1),
	1
FROM `schedule`
WHERE `typefalse` = 'application' AND `typevalfalse` = 'callback' AND `applicationvalfalse` LIKE '%|%';

INSERT INTO `dialaction` (
	`event`,
	`category`,
	`categoryval`,
	`action`,
	`actionarg1`,
	`actionarg2`,
	`linked`)
SELECT
	'inschedule',
	'schedule',
	`schedule`.`id`,
	'application:disa',
	`schedule`.`applicationvaltrue`,
	'',
	1
FROM `schedule`
WHERE `typetrue` = 'application' AND `typevaltrue` = 'disa' AND `applicationvaltrue` NOT LIKE '%|%';

INSERT INTO `dialaction` (
	`event`,
	`category`,
	`categoryval`,
	`action`,
	`actionarg1`,
	`actionarg2`,
	`linked`)
SELECT
	'outschedule',
	'schedule',
	`schedule`.`id`,
	'application:disa',
	`schedule`.`applicationvalfalse`,
	'',
	1
FROM `schedule`
WHERE `typefalse` = 'application' AND `typevalfalse` = 'disa' AND `applicationvalfalse` NOT LIKE '%|%';

INSERT INTO `dialaction` (
	`event`,
	`category`,
	`categoryval`,
	`action`,
	`actionarg1`,
	`actionarg2`,
	`linked`)
SELECT
	'inschedule',
	'schedule',
	`schedule`.`id`,
	'application:disa',
	SUBSTRING_INDEX(`schedule`.`applicationvaltrue`,'|',1),
	SUBSTRING_INDEX(`schedule`.`applicationvaltrue`,'|',-1),
	1
FROM `schedule`
WHERE `typetrue` = 'application' AND `typevaltrue` = 'disa' AND `applicationvaltrue` LIKE '%|%';

INSERT INTO `dialaction` (
	`event`,
	`category`,
	`categoryval`,
	`action`,
	`actionarg1`,
	`actionarg2`,
	`linked`)
SELECT
	'outschedule',
	'schedule',
	`schedule`.`id`,
	'application:disa',
	SUBSTRING_INDEX(`schedule`.`applicationvalfalse`,'|',1),
	SUBSTRING_INDEX(`schedule`.`applicationvalfalse`,'|',-1),
	1
FROM `schedule`
WHERE `typefalse` = 'application' AND `typevalfalse` = 'disa' AND `applicationvalfalse` LIKE '%|%';

INSERT INTO `dialaction` (
	`event`,
	`category`,
	`categoryval`,
	`action`,
	`actionarg1`,
	`actionarg2`,
	`linked`)
SELECT
	'inschedule',
	'schedule',
	`schedule`.`id`,
	CONCAT('application:',`schedule`.`typevaltrue`),
	`schedule`.`applicationvaltrue`,
	'',
	1
FROM `schedule`
WHERE `typetrue` = 'application' AND `typevaltrue` IN('directory','faxtomail');

INSERT INTO `dialaction` (
	`event`,
	`category`,
	`categoryval`,
	`action`,
	`actionarg1`,
	`actionarg2`,
	`linked`)
SELECT
	'outschedule',
	'schedule',
	`schedule`.`id`,
	CONCAT('application:',`schedule`.`typevalfalse`),
	`schedule`.`applicationvalfalse`,
	'',
	1
FROM `schedule`
WHERE `typefalse` = 'application' AND `typevalfalse` IN('directory','faxtomail');

INSERT INTO `dialaction` (
	`event`,
	`category`,
	`categoryval`,
	`action`,
	`actionarg1`,
	`actionarg2`,
	`linked`)
SELECT
	'inschedule',
	'schedule',
	`schedule`.`id`,
	'application:voicemailmain',
	`schedule`.`applicationvaltrue`,
	'',
	1
FROM `schedule`
WHERE `typetrue` = 'application' AND `typevaltrue` = 'voicemail';

INSERT INTO `dialaction` (
	`event`,
	`category`,
	`categoryval`,
	`action`,
	`actionarg1`,
	`actionarg2`,
	`linked`)
SELECT
	'outschedule',
	'schedule',
	`schedule`.`id`,
	'application:voicemailmain',
	`schedule`.`applicationvalfalse`,
	'',
	1
FROM `schedule`
WHERE `typefalse` = 'application' AND `typevalfalse` = 'voicemail';

INSERT INTO `dialaction` (
	`event`,
	`category`,
	`categoryval`,
	`action`,
	`actionarg1`,
	`actionarg2`,
	`linked`)
SELECT
	'inschedule',
	'schedule',
	`schedule`.`id`,
	'sound',
	CONCAT('/var/lib/pf-xivo/sounds/playback/',SUBSTRING(`typevaltrue`,51)),
	'',
	1
FROM `schedule`
WHERE `typetrue` = 'sound';

INSERT INTO `dialaction` (
	`event`,
	`category`,
	`categoryval`,
	`action`,
	`actionarg1`,
	`actionarg2`,
	`linked`)
SELECT
	'outschedule',
	'schedule',
	`schedule`.`id`,
	'sound',
	CONCAT('/var/lib/pf-xivo/sounds/playback/',SUBSTRING(`typevalfalse`,51)),
	'',
	1
FROM `schedule`
WHERE `typefalse` = 'sound';

INSERT INTO `dialaction` (
	`event`,
	`category`,
	`categoryval`,
	`action`,
	`actionarg1`,
	`actionarg2`,
	`linked`)
SELECT
	'inschedule',
	'schedule',
	`schedule`.`id`,
	`schedule`.`typetrue`,
	`schedule`.`typevaltrue`,
	'',
	`schedule`.`linked`
FROM `schedule`
WHERE `typetrue` NOT IN('endcall','application','sound');

INSERT INTO `dialaction` (
	`event`,
	`category`,
	`categoryval`,
	`action`,
	`actionarg1`,
	`actionarg2`,
	`linked`)
SELECT
	'outschedule',
	'schedule',
	`schedule`.`id`,
	`schedule`.`typefalse`,
	`schedule`.`typevalfalse`,
	'',
	`schedule`.`linked`
FROM `schedule`
WHERE `typefalse` NOT IN('endcall','application','sound');

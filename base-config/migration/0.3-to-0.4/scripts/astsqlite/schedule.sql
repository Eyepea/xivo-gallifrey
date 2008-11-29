INSERT INTO tmp_dialaction (
	event,
	category,
	categoryval,
	action,
	actionarg1,
	actionarg2,
	linked)
SELECT
	'inschedule',
	'schedule',
	schedule.id,
	'none',
	'',
	'',
	1
FROM schedule
WHERE typetrue = 'endcall' AND typevaltrue = 'none';

INSERT INTO tmp_dialaction (
	event,
	category,
	categoryval,
	action,
	actionarg1,
	actionarg2,
	linked)
SELECT
	'outschedule',
	'schedule',
	schedule.id,
	'none',
	'',
	'',
	1
FROM schedule
WHERE typefalse = 'endcall' AND typevalfalse = 'none';

INSERT INTO tmp_dialaction (
	event,
	category,
	categoryval,
	action,
	actionarg1,
	actionarg2,
	linked)
SELECT
	'inschedule',
	'schedule',
	schedule.id,
	'endcall:'||schedule.typevaltrue,
	'',
	'',
	1
FROM schedule
WHERE typetrue = 'endcall' AND typevaltrue IN('busy','congestion','hangup');

INSERT INTO tmp_dialaction (
	event,
	category,
	categoryval,
	action,
	actionarg1,
	actionarg2,
	linked)
SELECT
	'outschedule',
	'schedule',
	schedule.id,
	'endcall:'||schedule.typevalfalse,
	'',
	'',
	1
FROM schedule
WHERE typefalse = 'endcall' AND typevalfalse IN('busy','congestion','hangup');

INSERT INTO tmp_dialaction (
	event,
	category,
	categoryval,
	action,
	actionarg1,
	actionarg2,
	linked)
SELECT
	'inschedule',
	'schedule',
	schedule.id,
	'application:callbackdisa',
	schedule.applicationvaltrue,
	'',
	1
FROM schedule
WHERE typetrue = 'application' AND typevaltrue = 'callback';

INSERT INTO tmp_dialaction (
	event,
	category,
	categoryval,
	action,
	actionarg1,
	actionarg2,
	linked)
SELECT
	'outschedule',
	'schedule',
	schedule.id,
	'application:callbackdisa',
	schedule.applicationvalfalse,
	'',
	1
FROM schedule
WHERE typefalse = 'application' AND typevalfalse = 'callback';

INSERT INTO tmp_dialaction (
	event,
	category,
	categoryval,
	action,
	actionarg1,
	actionarg2,
	linked)
SELECT
	'inschedule',
	'schedule',
	schedule.id,
	'application:disa',
	schedule.applicationvaltrue,
	'',
	1
FROM schedule
WHERE typetrue = 'application' AND typevaltrue = 'disa';

INSERT INTO tmp_dialaction (
	event,
	category,
	categoryval,
	action,
	actionarg1,
	actionarg2,
	linked)
SELECT
	'outschedule',
	'schedule',
	schedule.id,
	'application:disa',
	schedule.applicationvalfalse,
	'',
	1
FROM schedule
WHERE typefalse = 'application' AND typevalfalse = 'disa';

INSERT INTO tmp_dialaction (
	event,
	category,
	categoryval,
	action,
	actionarg1,
	actionarg2,
	linked)
SELECT
	'inschedule',
	'schedule',
	schedule.id,
	'application:'||schedule.typevaltrue,
	schedule.applicationvaltrue,
	'',
	1
FROM schedule
WHERE typetrue = 'application' AND typevaltrue IN('directory','faxtomail');

INSERT INTO tmp_dialaction (
	event,
	category,
	categoryval,
	action,
	actionarg1,
	actionarg2,
	linked)
SELECT
	'outschedule',
	'schedule',
	schedule.id,
	'application:'||schedule.typevalfalse,
	schedule.applicationvalfalse,
	'',
	1
FROM schedule
WHERE typefalse = 'application' AND typevalfalse IN('directory','faxtomail');

INSERT INTO tmp_dialaction (
	event,
	category,
	categoryval,
	action,
	actionarg1,
	actionarg2,
	linked)
SELECT
	'inschedule',
	'schedule',
	schedule.id,
	'application:voicemailmain',
	schedule.applicationvaltrue,
	'',
	1
FROM schedule
WHERE typetrue = 'application' AND typevaltrue = 'voicemail';

INSERT INTO tmp_dialaction (
	event,
	category,
	categoryval,
	action,
	actionarg1,
	actionarg2,
	linked)
SELECT
	'outschedule',
	'schedule',
	schedule.id,
	'application:voicemailmain',
	schedule.applicationvalfalse,
	'',
	1
FROM schedule
WHERE typefalse = 'application' AND typevalfalse = 'voicemail';

INSERT INTO tmp_dialaction (
	event,
	category,
	categoryval,
	action,
	actionarg1,
	actionarg2,
	linked)
SELECT
	'inschedule',
	'schedule',
	schedule.id,
	'sound',
	'/var/lib/pf-xivo/sounds/playback/'||SUBSTR(typevaltrue,51,LENGTH(typevaltrue)),
	'',
	1
FROM schedule
WHERE typetrue = 'sound';

INSERT INTO tmp_dialaction (
	event,
	category,
	categoryval,
	action,
	actionarg1,
	actionarg2,
	linked)
SELECT
	'outschedule',
	'schedule',
	schedule.id,
	'sound',
	'/var/lib/pf-xivo/sounds/playback/'||SUBSTR(typevalfalse,51,LENGTH(typevalfalse)),
	'',
	1
FROM schedule
WHERE typefalse = 'sound';

INSERT INTO tmp_dialaction (
	event,
	category,
	categoryval,
	action,
	actionarg1,
	actionarg2,
	linked)
SELECT
	'inschedule',
	'schedule',
	schedule.id,
	schedule.typetrue,
	schedule.typevaltrue,
	'',
	schedule.linked
FROM schedule
WHERE typetrue NOT IN('endcall','application','sound');

INSERT INTO tmp_dialaction (
	event,
	category,
	categoryval,
	action,
	actionarg1,
	actionarg2,
	linked)
SELECT
	'outschedule',
	'schedule',
	schedule.id,
	schedule.typefalse,
	schedule.typevalfalse,
	'',
	schedule.linked
FROM schedule
WHERE typefalse NOT IN('endcall','application','sound');

INSERT INTO tmp_schedule (
	id,
	name,
	context,
	timebeg,
	timeend,
	daynamebeg,
	daynameend,
	daynumbeg,
	daynumend,
	monthbeg,
	monthend,
	publicholiday,
	commented)
SELECT
	schedule.id,
	schedule.name,
	'default',
	schedule.timebeg,
	schedule.timeend,
	schedule.daynamebeg,
	schedule.daynameend,
	schedule.daynumbeg,
	schedule.daynumend,
	schedule.monthbeg,
	schedule.monthend,
	schedule.publicholiday,
	schedule.commented
FROM schedule;

INSERT INTO tmp_contextmember (
	context,
	type,
	typeval,
	varname)
SELECT
	tmp_schedule.context,
	'schedule',
	tmp_schedule.id,
	'context'
FROM tmp_schedule;

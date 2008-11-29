INSERT INTO tmp_dialaction (
	event,
	category,
	categoryval,
	action,
	actionarg1,
	actionarg2,
	linked)
SELECT
	dialstatus.status,
	dialstatus.category,
	dialstatus.categoryval,
	'none',
	'',
	'',
	1
FROM dialstatus
WHERE type = 'endcall' AND typeval = 'none';

INSERT INTO tmp_dialaction (
	event,
	category,
	categoryval,
	action,
	actionarg1,
	actionarg2,
	linked)
SELECT
	dialstatus.status,
	dialstatus.category,
	dialstatus.categoryval,
	'endcall:'||dialstatus.typeval,
	'',
	'',
	1
FROM dialstatus
WHERE type = 'endcall' AND typeval IN('busy','congestion','hangup');

INSERT INTO tmp_dialaction (
	event,
	category,
	categoryval,
	action,
	actionarg1,
	actionarg2,
	linked)
SELECT
	dialstatus.status,
	dialstatus.category,
	dialstatus.categoryval,
	'application:callbackdisa',
	dialstatus.applicationval,
	'',
	1
FROM dialstatus
WHERE type = 'application' AND typeval = 'callback';

INSERT INTO tmp_dialaction (
	event,
	category,
	categoryval,
	action,
	actionarg1,
	actionarg2,
	linked)
SELECT
	dialstatus.status,
	dialstatus.category,
	dialstatus.categoryval,
	'application:disa',
	dialstatus.applicationval,
	'',
	1
FROM dialstatus
WHERE type = 'application' AND typeval = 'disa';

INSERT INTO tmp_dialaction (
	event,
	category,
	categoryval,
	action,
	actionarg1,
	actionarg2,
	linked)
SELECT
	dialstatus.status,
	dialstatus.category,
	dialstatus.categoryval,
	'application:'||dialstatus.typeval,
	dialstatus.applicationval,
	'',
	1
FROM dialstatus
WHERE type = 'application' AND typeval IN('directory','faxtomail');

INSERT INTO tmp_dialaction (
	event,
	category,
	categoryval,
	action,
	actionarg1,
	actionarg2,
	linked)
SELECT
	dialstatus.status,
	dialstatus.category,
	dialstatus.categoryval,
	'application:voicemailmain',
	dialstatus.applicationval,
	'',
	1
FROM dialstatus
WHERE type = 'application' AND typeval = 'voicemail';

INSERT INTO tmp_dialaction (
	event,
	category,
	categoryval,
	action,
	actionarg1,
	actionarg2,
	linked)
SELECT
	dialstatus.status,
	dialstatus.category,
	dialstatus.categoryval,
	'sound',
	'/var/lib/pf-xivo/sounds/playback/'||SUBSTR(typeval,51,LENGTH(typeval)),
	'',
	1
FROM dialstatus
WHERE type = 'sound';

INSERT INTO tmp_dialaction (
	event,
	category,
	categoryval,
	action,
	actionarg1,
	actionarg2,
	linked)
SELECT
	dialstatus.status,
	dialstatus.category,
	dialstatus.categoryval,
	dialstatus.type,
	dialstatus.typeval,
	'',
	dialstatus.linked
FROM dialstatus
WHERE type NOT IN('endcall','application','sound');

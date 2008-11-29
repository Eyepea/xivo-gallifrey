INSERT INTO tmp_dialaction (
	event,
	category,
	categoryval,
	action,
	actionarg1,
	actionarg2,
	linked)
SELECT
	'answer',
	'incall',
	incall.id,
	'none',
	'',
	'',
	1
FROM incall
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
	'answer',
	'incall',
	incall.id,
	'endcall:'||incall.typeval,
	'',
	'',
	1
FROM incall
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
	'answer',
	'incall',
	incall.id,
	'application:callbackdisa',
	incall.applicationval,
	'',
	1
FROM incall
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
	'answer',
	'incall',
	incall.id,
	'application:disa',
	incall.applicationval,
	'',
	1
FROM incall
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
	'answer',
	'incall',
	incall.id,
	'application:'||incall.typeval,
	incall.applicationval,
	'',
	1
FROM incall
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
	'answer',
	'incall',
	incall.id,
	'application:voicemailmain',
	incall.applicationval,
	'',
	1
FROM incall
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
	'answer',
	'incall',
	incall.id,
	'sound',
	'/var/lib/pf-xivo/sounds/playback/'||SUBSTR(typeval,51,LENGTH(typeval)),
	'',
	1
FROM incall
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
	'answer',
	'incall',
	incall.id,
	incall.type,
	incall.typeval,
	'',
	incall.linked
FROM incall
WHERE type NOT IN('endcall','application','sound');

INSERT INTO tmp_incall (
	id,
	exten,
	context,
	preprocess_subroutine,
	faxdetectenable,
	faxdetecttimeout,
	faxdetectemail,
	commented)
SELECT
	incall.id,
	incall.exten,
	'from-extern',
	NULL,
	0,
	4,
	'',
	incall.commented
FROM incall;

INSERT INTO tmp_contextnummember (
	context,
	type,
	typeval,
	number)
SELECT
	tmp_incall.context,
	'incall',
	tmp_incall.id,
	tmp_incall.exten
FROM tmp_incall;

BEGIN TRANSACTION;

INSERT INTO tmp_incall (
	exten,
	context,
	type,
	typeval,
	applicationval,
	linked,
	commented)
SELECT
	didfeatures.number,
	'incall-extensions',
	didfeatures.type,
	CASE WHEN didfeatures.type = 'custom' THEN didfeatures.custom ELSE didfeatures.typeid END,
	'',
	1,
	didfeatures.commented
FROM didfeatures;

INSERT INTO tmp_extensions (
	commented,
	context,
	exten,
	priority,
	app,
	appdata,
	name)
SELECT
	0,
	tmp_incall.context,
	tmp_incall.exten,
	1,
	'Macro',
	'incoming_did|'||tmp_incall.exten,
	''
FROM tmp_incall;

INSERT INTO tmp_extenumbers (
	exten,
	extenhash,
	context,
	type,
	typeval)
SELECT
	tmp_incall.exten,
	'tohash;'||tmp_incall.exten,
	tmp_incall.context,
	'incall',
	tmp_incall.id
FROM tmp_incall;

COMMIT;

BEGIN TRANSACTION;

INSERT INTO tmp_meetme (
	cat_metric,
	var_metric,
	commented,
	filename,
	category,
	var_name,
	var_val)
SELECT
	1,
	0,
	meetme.commented,
	meetme.filename,
	meetme.category,
	meetme.var_name,
	meetme.var_val
FROM meetme
WHERE filename = 'meetme.conf'
	AND category = 'rooms'
	AND var_name = 'conf';

INSERT INTO tmp_meetmefeatures (
	name,
	number,
	meetmeid,
	mode,
	musiconhold,
	context,
	poundexit,
	quiet,
	record,
	adminmode,
	announceusercount,
	announcejoinleave,
	alwayspromptpin,
	starmenu,
	enableexitcontext,
	exitcontext)
SELECT
	lower(meetmefeatures.name),
	meetmefeatures.number,
	tmp_meetme.id,
	meetmefeatures.mode,
	lower(meetmefeatures.musiconhold),
	'default',
	meetmefeatures.exit,
	meetmefeatures.quiet,
	meetmefeatures.record,
	0,
	0,
	0,
	0,
	0,
	0,
	''
FROM tmp_meetme
INNER JOIN meetme
INNER JOIN meetmefeatures
ON tmp_meetme.filename = meetme.filename
	AND tmp_meetme.category = meetme.category
	AND tmp_meetme.var_name = meetme.var_name
	AND tmp_meetme.var_val = meetme.var_val
	AND meetmefeatures.meetmeid = meetme.id;

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
	tmp_meetmefeatures.context,
	tmp_meetmefeatures.number,
	1,
	'Macro',
	'incoming_meetme',
	''
FROM tmp_meetmefeatures;

INSERT INTO tmp_extenumbers (
	exten,
	extenhash,
	context,
	type,
	typeval)
SELECT
	tmp_meetmefeatures.number,
	'tohash;'||tmp_meetmefeatures.number,
	tmp_meetmefeatures.context,
	'meetme',
	tmp_meetmefeatures.id
FROM tmp_meetmefeatures;

COMMIT;

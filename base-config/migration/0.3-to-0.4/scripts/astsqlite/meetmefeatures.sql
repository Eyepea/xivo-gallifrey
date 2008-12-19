INSERT INTO tmp_meetmefeatures (
	id,
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
	exitcontext,
	preprocess_subroutine)
SELECT
	meetmefeatures.id,
	meetmefeatures.name,
	meetmefeatures.number,
	meetmefeatures.meetmeid,
	meetmefeatures.mode,
	meetmefeatures.musiconhold,
	meetmefeatures.context,
	meetmefeatures.poundexit,
	meetmefeatures.quiet,
	meetmefeatures.record,
	meetmefeatures.adminmode,
	meetmefeatures.announceusercount,
	meetmefeatures.announcejoinleave,
	meetmefeatures.alwayspromptpin,
	meetmefeatures.starmenu,
	meetmefeatures.enableexitcontext,
	meetmefeatures.exitcontext,
	NULL
FROM meetmefeatures;

INSERT INTO tmp_contextnummember (
	context,
	type,
	typeval,
	number)
SELECT
	tmp_meetmefeatures.context,
	'meetme',
	tmp_meetmefeatures.id,
	tmp_meetmefeatures.number,
FROM tmp_meetmefeatures;

INSERT INTO tmp_contextmember (
	context,
	type,
	typeval,
	varname)
SELECT
	tmp_meetmefeatures.exitcontext,
	'meetme',
	tmp_meetmefeatures.id,
	'exitcontext'
FROM tmp_meetmefeatures
WHERE exitcontext != '';

INSERT INTO tmp_agentfeatures (
	id,
	agentid,
	numgroup,
	firstname,
	lastname,
	number,
	passwd,
	context,
	language,
	silent,
	commented,
	description)
SELECT
	agentfeatures.id,
	agentfeatures.agentid,
	agentfeatures.numgroup,
	agentfeatures.firstname,
	agentfeatures.lastname,
	agentfeatures.number,
	agentfeatures.passwd,
	'default',
	'fr',
	agentfeatures.silent,
	agentfeatures.commented,
	''
FROM agentfeatures;

INSERT INTO tmp_contextmember (
	context,
	type,
	typeval,
	varname)
SELECT
	tmp_agentfeatures.context,
	'agent',
	tmp_agentfeatures.id,
	'context'
FROM tmp_agentfeatures;

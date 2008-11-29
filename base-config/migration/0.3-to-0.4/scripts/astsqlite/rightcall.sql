INSERT INTO tmp_rightcall (
	id,
	name,
	context,
	passwd,
	authorization,
	commented,
	description)
SELECT
	rightcall.id,
	rightcall.name,
	'default',
	rightcall.passwd,
	rightcall.authorization,
	rightcall.commented,
	rightcall.description
FROM rightcall;

INSERT INTO tmp_contextmember (
	context,
	type,
	typeval,
	varname)
SELECT
	tmp_rightcall.context,
	'rightcall',
	tmp_rightcall.id,
	'context'
FROM tmp_rightcall;

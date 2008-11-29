INSERT INTO tmp_context (
	name,
	displayname,
	entity,
	commented,
	description)
SELECT
	tmp_contextmember.context,
	tmp_contextmember.context,
	NULL,
	0,
	''
FROM tmp_contextmember
GROUP BY context;

INSERT INTO tmp_context (
	name,
	displayname,
	entity,
	commented,
	description)
SELECT
	tmp_contextnummember.context,
	tmp_contextnummember.context,
	NULL,
	0,
	''
FROM tmp_contextnummember
LEFT JOIN tmp_context
ON tmp_contextnummember.context = tmp_context.name
WHERE tmp_context.name IS NULL
GROUP BY tmp_contextnummember.context;

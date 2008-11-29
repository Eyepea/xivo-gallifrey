INSERT INTO tmp_contextmember (
	context,
	type,
	typeval,
	varname)
SELECT
	outcall.context,
	'outcall',
	outcall.id,
	'context'
FROM outcall;

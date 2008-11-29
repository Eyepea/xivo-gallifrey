SELECT
	'UPDATE tmp_incall SET typeval = '||quote(tmp_userfeatures.id)||'
	WHERE type = ''user''
		AND typeval = '||quote(userfeatures.id)||';'
FROM userfeatures
INNER JOIN tmp_userfeatures
ON userfeatures.protocol = tmp_userfeatures.protocol
	AND lower(userfeatures.name) = lower(tmp_userfeatures.name);

SELECT
	'UPDATE tmp_incall SET typeval = '||quote(tmp_meetmefeatures.id)||'
	WHERE type = ''meetme''
		AND typeval = '||quote(meetmefeatures.id)||';'
FROM meetmefeatures
INNER JOIN tmp_meetmefeatures
ON meetmefeatures.number = tmp_meetmefeatures.number;

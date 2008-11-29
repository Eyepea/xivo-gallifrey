BEGIN TRANSACTION;

INSERT INTO tmp_musiconhold (
	cat_metric,
	var_metric,
	commented,
	filename,
	category,
	var_name,
	var_val)
SELECT
	0,
	0,
	musiconhold.commented,
	musiconhold.filename,
	lower(musiconhold.category),
	musiconhold.var_name,
	CASE WHEN musiconhold.category = 'directory' THEN lower(musiconhold.var_val) ELSE musiconhold.var_val END
FROM musiconhold
WHERE musiconhold.filename = 'musiconhold.conf'
	AND musiconhold.category != 'default';

COMMIT;

BEGIN TRANSACTION;

INSERT INTO tmp_generalsip (
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
	commented,
	filename,
	category,
	var_name,
	var_val
FROM generalsip
WHERE filename = 'sip.conf'
	AND category = 'general'
	AND var_name = 'register';

COMMIT;

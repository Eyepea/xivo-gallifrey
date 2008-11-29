UPDATE meetme
SET var_name = 'audiobuffers'
WHERE filename = 'meetme.conf'
AND category = 'general'
AND var_name = 'audiobuffer';

DELETE FROM tmp_staticmeetme;

INSERT INTO tmp_staticmeetme (
	id,
	cat_metric,
	var_metric,
	commented,
	filename,
	category,
	var_name,
	var_val)
SELECT
	meetme.id,
	meetme.cat_metric,
	meetme.var_metric,
	meetme.commented,
	meetme.filename,
	meetme.category,
	meetme.var_name,
	meetme.var_val
FROM meetme;

INSERT INTO tmp_accessfeatures (
	id,
	host,
	feature,
	commented)
SELECT
	accessfeatures.id,
	accessfeatures.host,
	accessfeatures.type,
	accessfeatures.commented
FROM accessfeatures;

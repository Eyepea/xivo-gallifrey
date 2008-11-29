INSERT INTO tmp_serverfeatures (
	id,
	serverid,
	feature,
	type,
	commented)
SELECT
	serverfeatures.id,
	serverfeatures.serverid,
	'phonebook',
	serverfeatures.type,
	serverfeatures.commented
FROM serverfeatures;

INSERT INTO tmp_trunkfeatures (
	id,
	protocol,
	protocolid,
	registerid,
	registercommented,
	description)
SELECT
	trunkfeatures.id,
	trunkfeatures.trunk,
	trunkfeatures.trunkid,
	trunkfeatures.registerid,
	trunkfeatures.registercommented,
	''
FROM trunkfeatures;

INSERT INTO tmp_usercustom (
	id,
	name,
	context,
	interface,
	intfsuffix,
	commented,
	protocol,
	category)
SELECT
	usercustom.id,
	usercustom.name,
	NULLIF(usercustom.context,''),
	usercustom.interface,
	'',
	usercustom.commented,
	usercustom.protocol,
	usercustom.category
FROM usercustom;

UPDATE tmp_usercustom
SET interface = 'dahdi/'||SUBSTR(interface,5,LENGTH(interface))
WHERE interface LIKE 'zap/%';

INSERT INTO tmp_contextmember (
	context,
	type,
	typeval,
	varname)
SELECT
	tmp_usercustom.context,
	'trunkcustom',
	tmp_usercustom.id,
	'context'
FROM tmp_usercustom
WHERE category = 'trunk'
AND context IS NOT NULL;

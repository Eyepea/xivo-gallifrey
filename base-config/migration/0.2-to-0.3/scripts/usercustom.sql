BEGIN TRANSACTION;

INSERT INTO tmp_usercustom (
	id,
	name,
	context,
	interface,
	commented,
	protocol,
	category)
SELECT
	usercustom.id,
	usercustom.name,
	usercustom.context,
	usercustom.interface,
	usercustom.commented,
	'custom',
	usercustom.category
FROM usercustom;

COMMIT;

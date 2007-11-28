BEGIN TRANSACTION;

INSERT INTO tmp_useriax (
	name,
	commented,
	username,
	type,
	secret,
	md5secret,
	dbsecret,
	notransfer,
	trunk,
	inkeys,
	outkey,
	auth,
	accountcode,
	amaflags,
	callerid,
	callgroup,
	context,
	defaultip,
	host,
	language,
	mailbox,
	deny,
	permit,
	qualify,
	disallow,
	allow,
	ipaddr,
	port,
	regseconds,
	'call-limit',
	category)
SELECT
	lower(useriax.name),
	useriax.commented,
	useriax.username,
	useriax.type,
	useriax.secret,
	useriax.md5secret,
	useriax.dbsecret,
	useriax.notransfer,
	useriax.trunk,
	useriax.inkeys,
	useriax.outkey,
	useriax.auth,
	useriax.accountcode,
	useriax.amaflags,
	useriax.callerid,
	useriax.callgroup,
	CASE WHEN useriax.context = 'local-extensions' THEN 'default' ELSE useriax.context END,
	useriax.defaultip,
	useriax.host,
	useriax.language,
	useriax.mailbox,
	useriax.deny,
	useriax.permit,
	useriax.qualify,
	useriax.disallow,
	useriax.allow,
	useriax.ipaddr,
	nullif(useriax.port,''),
	useriax.regseconds,
	useriax.'call-limit',
	useriax.category
FROM useriax
INNER JOIN trunkfeatures
ON trunkfeatures.trunk = 'iax'
	AND trunkfeatures.trunkid = useriax.id
WHERE useriax.category = 'trunk';

INSERT INTO tmp_trunkfeatures (
	trunk,
	trunkid,
	registerid,
	registercommented)
SELECT
	trunkfeatures.trunk,
	tmp_useriax.id,
	ifnull(tmp_generaliax.id,0),
	ifnull(tmp_generaliax.commented,0)
FROM trunkfeatures
INNER JOIN useriax
INNER JOIN tmp_useriax
LEFT JOIN generaliax
LEFT JOIN tmp_generaliax
ON trunkfeatures.trunk = 'iax'
	AND trunkfeatures.trunkid = useriax.id
	AND lower(useriax.name) = lower(tmp_useriax.name)
	AND tmp_useriax.category = 'trunk'
	AND trunkfeatures.registerid = generaliax.id
	AND generaliax.filename = 'iax.conf'
	AND generaliax.category = 'general'
	AND generaliax.var_name = 'register'
	AND tmp_generaliax.filename = 'iax.conf'
	AND tmp_generaliax.category = 'general'
	AND tmp_generaliax.var_name = 'register'
	AND generaliax.var_val = tmp_generaliax.var_val;

COMMIT;

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
	CASE WHEN length(ifnull(userfeatures.number,'')) > 0
	THEN userfeatures.number||'@'||
		CASE WHEN useriax.context = 'local-extensions'
		THEN 'default'
		ELSE useriax.context END
	ELSE '' END,
	useriax.deny,
	useriax.permit,
	useriax.qualify,
	useriax.disallow,
	useriax.allow,
	useriax.ipaddr,
	NULL,
	useriax.regseconds,
	useriax.'call-limit',
	useriax.category
FROM useriax
INNER JOIN userfeatures
ON userfeatures.protocol = 'iax'
	AND userfeatures.protocolid = useriax.id
WHERE useriax.category = 'user';

INSERT INTO tmp_userfeatures (
	protocol,
	protocolid,
	firstname,
	lastname,
	name,
	number,
	context,
	provisioningid,
	ringseconds,
	simultcalls,
	enableclient,
	enablehint,
	enablevoicemail,
	skipvoicemailpass,
	enablexfer,
	enableautomon,
	callrecord,
	callfilter,
	enablednd,
	enableunc,
	destunc,
	enablerna,
	destrna,
	enablebusy,
	destbusy,
	musiconhold,
	outcallerid,
	internal,
	commented,
	description)
SELECT
	userfeatures.protocol,
	tmp_useriax.id,
	userfeatures.firstname,
	userfeatures.lastname,
	lower(tmp_useriax.name),
	userfeatures.number,
	tmp_useriax.context,
	userfeatures.provisioningid,
	userfeatures.ringseconds,
	userfeatures.simultcalls,
	userfeatures.popupwidget,
	1,
	1,
	1,
	0,
	0,
	0,
	0,
	0,
	0,
	'',
	0,
	'',
	0,
	'',
	lower(userfeatures.musiconhold),
	CASE WHEN length(userfeatures.outnumber) > 0 THEN userfeatures.outnumber ELSE 'default' END,
	0,
	tmp_useriax.commented,
	userfeatures.comment
FROM userfeatures
INNER JOIN tmp_useriax
	ON userfeatures.protocol = 'iax'
	AND lower(userfeatures.name) = lower(tmp_useriax.name)
	AND tmp_useriax.category = 'user';

INSERT INTO tmp_extensions (
	commented,
	context,
	exten,
	priority,
	app,
	appdata,
	name)
SELECT
	0,
	tmp_userfeatures.context,
	tmp_userfeatures.number,
	1,
	'Macro',
	'incoming_user',
	''
FROM tmp_userfeatures
WHERE length(tmp_userfeatures.number) > 0
	AND tmp_userfeatures.number IS NOT NULL;

INSERT INTO tmp_extensions (
	commented,
	context,
	exten,
	priority,
	app,
	appdata,
	name)
SELECT	0,
	'hints',
	tmp_userfeatures.number,
	1,
	'Macro',
	'IAX2/'||tmp_userfeatures.name,
	''
FROM tmp_userfeatures
WHERE length(tmp_userfeatures.number) > 0
        AND tmp_userfeatures.number IS NOT NULL;

INSERT INTO tmp_extenumbers (
	exten,
	extenhash,
	context,
	type,
	typeval)
SELECT	
	tmp_userfeatures.number,
	'tohash;'||tmp_userfeatures.number,
	tmp_userfeatures.context,
	'user',
	tmp_userfeatures.id
FROM tmp_userfeatures
WHERE length(tmp_userfeatures.number) > 0
        AND tmp_userfeatures.number IS NOT NULL;

COMMIT;

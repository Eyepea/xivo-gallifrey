SELECT
	'UPDATE tmp_userfeatures
	SET voicemailid = '||QUOTE(uservoicemail.uniqueid)||'
	WHERE id = '||QUOTE(userfeatures.id)||';'
FROM
	usersip,
	uservoicemail,
	userfeatures
WHERE
	usersip.mailbox = uservoicemail.mailbox||'@'||uservoicemail.context
	AND userfeatures.protocol = 'sip'
	AND userfeatures.protocolid = usersip.id;

SELECT
	'UPDATE tmp_userfeatures
	SET voicemailid = '||QUOTE(uservoicemail.uniqueid)||'
	WHERE id = '||QUOTE(userfeatures.id)||';'
FROM
	useriax,
	uservoicemail,
	userfeatures
WHERE
	useriax.mailbox = uservoicemail.mailbox||'@'||uservoicemail.context
	AND userfeatures.protocol = 'iax'
	AND userfeatures.protocolid = useriax.id;

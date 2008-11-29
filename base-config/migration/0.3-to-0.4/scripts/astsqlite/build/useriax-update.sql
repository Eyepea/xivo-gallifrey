SELECT
	'UPDATE tmp_useriax
	SET setvar = '||QUOTE('XIVO_USERID='||userfeatures.id)||'
	WHERE id = '||QUOTE(useriax.id)||';'
FROM
	userfeatures,
	useriax
WHERE
	useriax.id = userfeatures.protocolid
	AND userfeatures.protocol = 'iax';

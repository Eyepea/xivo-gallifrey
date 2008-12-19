SELECT
	'UPDATE extensions
	SET appdata = '||QUOTE('user|'||userfeatures.id||'|')||'
	WHERE id = '||QUOTE(extensions.id)||';'
FROM
	extensions,
	userfeatures
WHERE
	extensions.exten = userfeatures.number
	AND extensions.context = userfeatures.context;

SELECT
	'UPDATE extensions
	SET appdata = '||QUOTE('group|'||groupfeatures.id||'|')||'
	WHERE id = '||QUOTE(extensions.id)||';'
FROM
	extensions,
	groupfeatures
WHERE
	extensions.exten = groupfeatures.number
	AND extensions.context = groupfeatures.context
	AND NOT groupfeatures.deleted;

SELECT
	'UPDATE extensions
	SET appdata = '||QUOTE('queue|'||queuefeatures.id||'|')||'
	WHERE id = '||QUOTE(extensions.id)||';'
FROM
	extensions,
	queuefeatures
WHERE
	extensions.exten = queuefeatures.number
	AND extensions.context = queuefeatures.context;

SELECT
	'UPDATE extensions
	SET appdata = '||QUOTE('meetme|'||meetmefeatures.id||'|')||'
	WHERE id = '||QUOTE(extensions.id)||';'
FROM
	extensions,
	meetmefeatures
WHERE
	extensions.exten = meetmefeatures.number
	AND extensions.context = meetmefeatures.context;

SELECT
	'UPDATE extensions
	SET context = ''from-extern'', appdata = '||QUOTE('did|'||SUBSTR(appdata,14,LENGTH(appdata)))||'
	WHERE id = '||QUOTE(extensions.id)||';'
FROM
	extensions
WHERE
	appdata LIKE 'incoming_did|%';

SELECT
	'UPDATE extensions
	SET appdata = '||QUOTE('outcall|'||outcall.id)||'
	WHERE id = '||QUOTE(extensions.id)||';'
FROM
	extensions,
	outcall
WHERE
	extensions.exten = outcall.exten
	AND extensions.context = outcall.context
	AND appdata LIKE 'outgoing_user|%';

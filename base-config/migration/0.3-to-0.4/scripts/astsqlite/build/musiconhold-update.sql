SELECT
	'UPDATE musiconhold
	SET var_val = '||QUOTE('/var/lib/pf-xivo/moh/'||SUBSTR(var_val,25,LENGTH(var_val)))||'
	WHERE id = '||QUOTE(id)||';'
FROM musiconhold
WHERE var_val LIKE '/usr/share/asterisk/moh/%';

SELECT
	'UPDATE features
	SET var_val = '||QUOTE('/var/lib/pf-xivo/sounds/features/'||SUBSTR(var_val,51,LENGTH(var_val)))||'
	WHERE id = '||QUOTE(id)||';'
FROM features
WHERE var_val LIKE '/usr/share/asterisk/sounds/web-interface/features/%';

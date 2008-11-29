SELECT
	'UPDATE tmp_staticqueue
	SET var_val = '||QUOTE(var_val)||'
	WHERE filename = ''queues.conf'' AND category = ''general'' AND var_name = ''persistentmembers'';'
FROM
	generalqueue
WHERE
	filename = 'queues.conf'
	AND category = 'general'
	AND var_name = 'persistentmembers';

SELECT
	'UPDATE userfeatures
	SET enableclient = 0, loginclient = '''', passwdclient = '''', profileclient = ''''
	WHERE loginclient = '||QUOTE(loginclient)||';--', COUNT(*) AS cnt,'--;'
FROM userfeatures
WHERE internal = 0 AND loginclient != ''
GROUP BY loginclient
HAVING cnt > 1;

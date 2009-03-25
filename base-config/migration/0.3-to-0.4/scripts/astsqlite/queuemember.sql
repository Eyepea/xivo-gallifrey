UPDATE queuemember
SET interface = 'dahdi/'||SUBSTR(interface,5,LENGTH(interface))
WHERE interface LIKE 'zap/%';

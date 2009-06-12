SELECT
	id,
	exten
FROM extenumbers
WHERE extenhash = 'UPDATE_SHA1_HASH'
AND SUBSTR(exten,1,1) != '_';

SELECT
	id,
	SUBSTR(exten,2,LENGTH(exten))
FROM extenumbers
WHERE extenhash = 'UPDATE_SHA1_HASH'
AND SUBSTR(exten,1,1) = '_';

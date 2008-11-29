INSERT INTO contextnumbers (
	context,
	type,
	numberbeg,
	numberend,
	didlength)
SELECT
	context,
	type,
	MIN(number),
	MAX(number),
	0
FROM contextnummember
WHERE type = 'user'
AND number != ''
GROUP BY LENGTH(number);

INSERT INTO contextnumbers (
	context,
	type,
	numberbeg,
	numberend,
	didlength)
SELECT
	context,
	type,
	MIN(number),
	MAX(number),
	0
FROM contextnummember
WHERE type = 'group'
AND number != ''
GROUP BY LENGTH(number);

INSERT INTO contextnumbers (
	context,
	type,
	numberbeg,
	numberend,
	didlength)
SELECT
	context,
	type,
	MIN(number),
	MAX(number),
	0
FROM contextnummember
WHERE type = 'queue'
AND number != ''
GROUP BY LENGTH(number);

INSERT INTO contextnumbers (
	context,
	type,
	numberbeg,
	numberend,
	didlength)
SELECT
	context,
	type,
	MIN(number),
	MAX(number),
	0
FROM contextnummember
WHERE type = 'meetme'
AND number != ''
GROUP BY LENGTH(number);

INSERT INTO contextnumbers (
	context,
	type,
	numberbeg,
	numberend,
	didlength)
SELECT
	context,
	type,
	MIN(number),
	MAX(number),
	LENGTH(number)
FROM contextnummember
WHERE type = 'incall'
AND number != ''
GROUP BY LENGTH(number);

BEGIN TRANSACTION;

INSERT INTO tmp_dialstatus (
	id,
	status,
	category,
	categoryval,
	type,
	typeval,
	applicationval,
	linked)
SELECT
	dialstatus.id,
	dialstatus.status,
	dialstatus.category,
	dialstatus.categoryval,
	dialstatus.type,
	dialstatus.typeval,
	dialstatus.applicationval,
	dialstatus.linked
FROM dialstatus;

COMMIT;

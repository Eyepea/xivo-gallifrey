UPDATE generaliax
SET var_val = NULL, commented = 1
WHERE filename = 'iax.conf'
AND category = 'general'
AND var_name = 'tos';

UPDATE generaliax
SET var_name = 'transfer', var_val = 'yes'
WHERE filename = 'iax.conf'
AND category = 'general'
AND var_name = 'notransfer'
AND var_val != 'yes';

UPDATE generaliax
SET var_name = 'transfer', var_val = 'no'
WHERE filename = 'iax.conf'
AND category = 'general'
AND var_name = 'notransfer'
AND var_val = 'yes';

UPDATE generaliax
SET var_val = 0
WHERE filename = 'iax.conf'
AND category = 'general'
AND var_name = 'maxauthreq'
AND var_val = 3;

DELETE FROM generaliax
WHERE filename = 'iax.conf'
AND category = 'general'
AND var_name IN('dropcount',
		'minexcessbuffer',
		'maxexcessbuffer',
		'jittershrinkrate',
		'mailboxdetail');

INSERT INTO generaliax (
	id,
	cat_metric,
	var_metric,
	commented,
	filename,
	category,
	var_name,
	var_val)
SELECT
	NULL,
	tmp_staticiax.cat_metric,
	tmp_staticiax.var_metric,
	tmp_staticiax.commented,
	tmp_staticiax.filename,
	tmp_staticiax.category,
	tmp_staticiax.var_name,
	tmp_staticiax.var_val
FROM tmp_staticiax
WHERE filename = 'iax.conf'
AND category = 'general'
AND var_name IN('iaxthreadcount',
		'iaxmaxthreadcount',
		'adsi',
		'mohinterpret',
		'mohsuggest');

DELETE FROM tmp_staticiax;

INSERT INTO tmp_staticiax (
	id,
	cat_metric,
	var_metric,
	commented,
	filename,
	category,
	var_name,
	var_val)
SELECT
	generaliax.id,
	generaliax.cat_metric,
	generaliax.var_metric,
	generaliax.commented,
	generaliax.filename,
	generaliax.category,
	generaliax.var_name,
	generaliax.var_val
FROM generaliax;

INSERT INTO tmp_contextmember (
	context,
	type,
	typeval,
	varname)
SELECT
	tmp_staticiax.var_val,
	'generaliax',
	'',
	tmp_staticiax.var_name
FROM tmp_staticiax
WHERE filename = 'iax.conf'
AND category = 'general'
AND var_name = 'regcontext'
AND NULLIF(var_val,'') IS NOT NULL
AND commented = 0;

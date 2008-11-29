UPDATE generalvoicemail
SET var_val = NULL, commented = 1
WHERE filename = 'voicemail.conf'
AND category = 'general'
AND var_name = 'cidinternalcontexts'
AND var_val = 'default';

INSERT INTO generalvoicemail (
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
	tmp_staticvoicemail.cat_metric,
	tmp_staticvoicemail.var_metric,
	tmp_staticvoicemail.commented,
	tmp_staticvoicemail.filename,
	tmp_staticvoicemail.category,
	tmp_staticvoicemail.var_name,
	tmp_staticvoicemail.var_val
FROM tmp_staticvoicemail
WHERE filename = 'voicemail.conf'
AND category = 'general'
AND var_name IN('tempgreetwarn',
		'volgain',
		'smdiport');

DELETE FROM tmp_staticvoicemail;

INSERT INTO tmp_staticvoicemail (
	id,
	cat_metric,
	var_metric,
	commented,
	filename,
	category,
	var_name,
	var_val)
SELECT
	generalvoicemail.id,
	generalvoicemail.cat_metric,
	generalvoicemail.var_metric,
	generalvoicemail.commented,
	generalvoicemail.filename,
	generalvoicemail.category,
	generalvoicemail.var_name,
	generalvoicemail.var_val
FROM generalvoicemail;

INSERT INTO tmp_contextmember (
	context,
	type,
	typeval,
	varname)
SELECT
	tmp_staticvoicemail.var_val,
	'generalvoicemail',
	'',
	tmp_staticvoicemail.var_name
FROM tmp_staticvoicemail
WHERE filename = 'voicemail.conf'
AND category = 'general'
AND var_name IN('cidinternalcontexts',
		'dialout',
		'callback',
		'exitcontext')
AND NULLIF(var_val,'') IS NOT NULL
AND commented = 0;

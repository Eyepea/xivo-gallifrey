SELECT
    'UPDATE tmp_voicemail
    SET attach = NULL
    WHERE attach = '||CASE WHEN var_val = 'no' THEN 0 ELSE 1 END||';'
FROM tmp_staticvoicemail
WHERE filename = 'voicemail.conf'
AND category = 'general'
AND var_name = 'attach';

SELECT
    'UPDATE tmp_voicemail
    SET saycid = NULL
    WHERE saycid = '||CASE WHEN var_val = 'no' THEN 0 ELSE 1 END||';'
FROM tmp_staticvoicemail
WHERE filename = 'voicemail.conf'
AND category = 'general'
AND var_name = 'saycid';

SELECT
    'UPDATE tmp_voicemail
    SET review = NULL
    WHERE review = '||CASE WHEN var_val = 'no' THEN 0 ELSE 1 END||';'
FROM tmp_staticvoicemail
WHERE filename = 'voicemail.conf'
AND category = 'general'
AND var_name = 'review';

SELECT
    'UPDATE tmp_voicemail
    SET operator = NULL
    WHERE operator = '||CASE WHEN var_val = 'no' THEN 0 ELSE 1 END||';'
FROM tmp_staticvoicemail
WHERE filename = 'voicemail.conf'
AND category = 'general'
AND var_name = 'operator';

SELECT
    'UPDATE tmp_voicemail
    SET envelope = NULL
    WHERE envelope = '||CASE WHEN var_val = 'no' THEN 0 ELSE 1 END||';'
FROM tmp_staticvoicemail
WHERE filename = 'voicemail.conf'
AND category = 'general'
AND var_name = 'envelope';

SELECT
    'UPDATE tmp_voicemail
    SET sayduration = NULL
    WHERE sayduration = '||CASE WHEN var_val = 'no' THEN 0 ELSE 1 END||';'
FROM tmp_staticvoicemail
WHERE filename = 'voicemail.conf'
AND category = 'general'
AND var_name = 'sayduration';

SELECT
    'UPDATE tmp_voicemail
    SET saydurationm = NULL
    WHERE saydurationm = '||QUOTE(var_val)||';'
FROM tmp_staticvoicemail
WHERE filename = 'voicemail.conf'
AND category = 'general'
AND var_name = 'saydurationm';

SELECT
    'UPDATE tmp_voicemail
    SET forcename = NULL
    WHERE forcename = '||CASE WHEN var_val = 'no' THEN 0 ELSE 1 END||';'
FROM tmp_staticvoicemail
WHERE filename = 'voicemail.conf'
AND category = 'general'
AND var_name = 'forcename';

SELECT
    'UPDATE tmp_voicemail
    SET forcegreetings = NULL
    WHERE forcegreetings = '||CASE WHEN var_val = 'no' THEN 0 ELSE 1 END||';'
FROM tmp_staticvoicemail
WHERE filename = 'voicemail.conf'
AND category = 'general'
AND var_name = 'forcegreetings';

SELECT
    'UPDATE tmp_voicemail
    SET maxmsg = NULL
    WHERE maxmsg = '||QUOTE(var_val)||';'
FROM tmp_staticvoicemail
WHERE filename = 'voicemail.conf'
AND category = 'general'
AND var_name = 'maxmsg';

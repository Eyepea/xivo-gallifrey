INSERT INTO staticvoicemail VALUES (NULL,0,0,0,'voicemail.conf','general','emaildatelocale','fr_FR.UTF-8');

UPDATE staticvoicemail
SET var_val = '%A %d %B %Y à %H:%M:%S %Z'
WHERE filename = 'voicemail.conf'
AND category = 'general'
AND var_name = 'emaildateformat';

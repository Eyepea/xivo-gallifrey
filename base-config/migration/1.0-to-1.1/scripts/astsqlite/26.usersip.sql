
UPDATE OR IGNORE usersip SET
  language   = 'fr_FR'
WHERE
  language != 'en' AND
  language NOT NULL;

UPDATE OR IGNORE usersip SET
  language  = 'en_US'
WHERE
  language  = 'en';


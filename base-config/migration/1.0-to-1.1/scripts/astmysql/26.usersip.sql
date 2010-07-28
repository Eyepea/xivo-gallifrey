
UPDATE IGNORE `usersip` SET
  var_val = 'fr_FR'
WHERE
  var_name = 'language' AND
  var_val != 'en';

UPDATE IGNORE `usersip` SET
  var_val = 'en_US'
WHERE
  var_name = 'language' AND
  var_val = 'en';


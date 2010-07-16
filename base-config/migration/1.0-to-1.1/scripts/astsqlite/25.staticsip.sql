

UPDATE OR IGNORE staticsip SET
  var_val   = 'fr_FR'
WHERE
  var_name != 'en';

UPDATE OR IGNORE staticsip SET
  var_val  = 'en_US'
WHERE
  var_name = 'en';


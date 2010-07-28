
INSERT INTO staticiax VALUES (NULL,0,0,0,'iax.conf','general','requirecalltoken','no');
INSERT INTO staticiax VALUES (NULL,0,0,0,'iax.conf','general','calltokenoptional','0.0.0.0');

UPDATE OR IGNORE staticiax SET
  var_val   = 'fr_FR'
WHERE
  var_name = 'language' AND
  var_val != 'en';

UPDATE OR IGNORE staticiax SET
  var_val  = 'en_US'
WHERE
  var_name = 'language' AND
  var_val  = 'en';


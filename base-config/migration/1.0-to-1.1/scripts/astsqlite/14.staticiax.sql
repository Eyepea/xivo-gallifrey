
INSERT INTO staticiax VALUES (NULL,0,0,0,'iax.conf','general','requirecalltoken','no');
INSERT INTO staticiax VALUES (NULL,0,0,0,'iax.conf','general','calltokenoptional','0.0.0.0');

UPDATE OR IGNORE staticiax SET
  language   = 'fr_FR'
WHERE
  language != 'en' AND
  language NOT NULL;

UPDATE OR IGNORE staticiax SET
  language  = 'en_US'
WHERE
  language  = 'en';


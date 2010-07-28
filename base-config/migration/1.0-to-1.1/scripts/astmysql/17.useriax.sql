
ALTER TABLE `useriax` ADD COLUMN `requirecalltoken` varchar(4) NOT NULL DEFAULT '' AFTER `commented`;

UPDATE IGNORE `useriax` SET
  var_val = 'fr_FR'
WHERE
  var_name = 'language' AND
  var_val != 'en';

UPDATE IGNORE `useriax` SET
  var_val = 'en_US'
WHERE
  var_name = 'language' AND
  var_val = 'en';


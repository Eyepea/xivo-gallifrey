
ALTER TABLE `useriax` ADD COLUMN `requirecalltoken` varchar(4) NOT NULL DEFAULT '' AFTER `commented`;

UPDATE IGNORE `useriax` SET
  language = 'fr_FR'
WHERE
  language != 'en' AND
  language IS NOT NULL;

UPDATE IGNORE `useriax` SET
  language = 'en_US'
WHERE
  language = 'en';


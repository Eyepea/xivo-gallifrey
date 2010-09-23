
ALTER TABLE `usersip` ADD `outboundproxy` varchar(1024) AFTER `category`;

UPDATE IGNORE `usersip` SET
  language = 'fr_FR'
WHERE
  language != 'en' AND
  language IS NOT NULL;

UPDATE IGNORE `usersip` SET
  language = 'en_US'
WHERE
  language = 'en';


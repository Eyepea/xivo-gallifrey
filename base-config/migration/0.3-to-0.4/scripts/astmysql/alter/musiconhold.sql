UPDATE `musiconhold`
SET `var_val` = CONCAT('/var/lib/pf-xivo/moh/',SUBSTRING(`musiconhold`.`var_val`,25))
WHERE `var_val` LIKE '/usr/share/asterisk/moh/%';

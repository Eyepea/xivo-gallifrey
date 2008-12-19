UPDATE `features`
SET `var_val` = CONCAT('/var/lib/pf-xivo/sounds/features/',SUBSTRING(`features`.`var_val`,51))
WHERE `var_val` LIKE '/usr/share/asterisk/sounds/web-interface/features/%';

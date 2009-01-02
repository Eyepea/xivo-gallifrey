UPDATE `usercustom`
SET `interface` = REPLACE(`interface`,'zap/','dahdi/')
WHERE `interface` LIKE 'zap/%';

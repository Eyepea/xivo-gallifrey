UPDATE `dialaction`
SET `actionarg1` = REPLACE(`actionarg1`,',','|')
WHERE `action` = 'custom' AND `actionarg1` LIKE '%,%';

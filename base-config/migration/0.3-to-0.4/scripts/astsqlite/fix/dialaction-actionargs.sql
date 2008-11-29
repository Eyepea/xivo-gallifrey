SELECT actionarg1
FROM tmp_dialaction
WHERE action IN('application:callbackdisa','application:disa')
AND actionarg1 LIKE '%|%';

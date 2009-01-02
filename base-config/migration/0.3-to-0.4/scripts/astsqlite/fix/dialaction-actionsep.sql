SELECT actionarg1
FROM tmp_dialaction
WHERE action = 'custom'
AND actionarg1 LIKE '%,%';

UPDATE phone
SET model = CONCAT('67',model)
WHERE vendor = 'aastra'
AND model IN('51i','53i','55i','57i');

UPDATE phone
SET model = '67'||model
WHERE vendor = 'aastra'
AND model IN('51i','53i','55i','57i');

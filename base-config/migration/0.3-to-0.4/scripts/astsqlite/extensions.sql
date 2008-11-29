UPDATE extensions
SET context = 'xivo-features'
WHERE context = 'features';

UPDATE extensions
SET context = 'xivo-handynumbers'
WHERE context = 'handynumbers';

UPDATE extensions
SET appdata = 'agentstaticlogoff|${EXTEN:3}'
WHERE name = 'agentstaticlogoff';

UPDATE extensions
SET appdata = 'recsnd|wav'
WHERE name = 'recsnd';

UPDATE extensions
SET appdata = 'vmdelete'
WHERE name = 'vmdelete';

INSERT INTO callerid (
	mode,
	callerdisplay,
	type,
	typeval)
SELECT
	'append',
	callfilter.callerdisplay,
	'callfilter',
	callfilter.id
FROM callfilter;

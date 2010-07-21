
function xivo_cti_rdid_oncontextchange()
{
	var context    = dwho_eid('it-rdid-contexts');
	var list       = dwho_eid('it-rdid-incalls-in');
	var listout    = dwho_eid('it-rdid-incalls-out');
	// clear lists
	for(i = list.length; i > 0; i--)
	{ list.remove(0); }
	for(i = listout.length; i > 0; i--)
	{ listout.remove(0); }

	// FAKE *ALL* CONTEXTS
	var opt = document.createElement("option");
	opt.value = '*';
	opt.text  = i18n_xivo_cti_rdid_all;

	if(context.value !== '*')
	{
		list.add(opt, null);
		xivo_cti_rdid_oncontextchange_sub(list, context.value, null); return; 
	}
	
	list.add(opt, null);
}

function xivo_cti_rdid_oncontextchange_sub(list, scontext, display)
{
	context        = scontext.replace("-", "__", "g");

	// fill list with context extens
	var table = eval('context_' + context + '_extens');
	for(var i = 0; i < table.length; i++)
	{
		var opt = document.createElement("option");
		opt.value = table[i][1];
		opt.text  = table[i][1];

		if(display != null)
		{ opt.text = display + '/' + opt.text; }

		list.add(opt, null);
	}
}

function xivo_cti_rdid_onload()
{
	// install hooks
	dwho.dom.add_event('change',
		dwho_eid('it-rdid-contexts'),
		xivo_cti_rdid_oncontextchange);
	
//	xivo_cti_rdid_oncontextchange();
}

dwho.dom.set_onload(xivo_cti_rdid_onload);


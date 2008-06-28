var xivo_elt_dialaction = new Array();
xivo_elt_dialaction['answer'] = new Array();
xivo_elt_dialaction['noanswer'] = new Array();
xivo_elt_dialaction['congestion'] = new Array();
xivo_elt_dialaction['busy'] = new Array();
xivo_elt_dialaction['chanunavail'] = new Array();
xivo_elt_dialaction['inschedule'] = new Array();
xivo_elt_dialaction['outschedule'] = new Array();

var xivo_fm_dialaction = new Array();
xivo_fm_dialaction = xivo_clone(xivo_elt_dialaction);

var xivo_dialaction_actiontype = new Array();
xivo_dialaction_actiontype['none'] = ['actiontype'];
xivo_dialaction_actiontype['endcall'] = ['action','actiontype','busy-actionarg1','congestion-actionarg1'];
xivo_dialaction_actiontype['user'] = ['actiontype','actionarg1','actionarg2'];
xivo_dialaction_actiontype['group'] = ['actiontype','actionarg1','actionarg2'];
xivo_dialaction_actiontype['queue'] = ['actiontype','actionarg1','actionarg2'];
xivo_dialaction_actiontype['meetme'] = ['actiontype','actionarg1'];
xivo_dialaction_actiontype['voicemail'] = ['actiontype','actionarg1'];
xivo_dialaction_actiontype['voicemenu'] = ['actiontype','actionarg1'];
xivo_dialaction_actiontype['schedule'] = ['actiontype','actionarg1'];
xivo_dialaction_actiontype['application'] = ['action',
					     'actiontype',
					     'callback-actionarg1',
					     'callback-actionarg2',
					     'callbackdisa-actionarg1',
					     'callbackdisa-actionarg2',
					     'directory-actionarg1',
					     'faxtomail-actionarg1',
					     'voicemailmain-actionarg1'];
xivo_dialaction_actiontype['custom'] = ['actiontype','actionarg1'];
xivo_dialaction_actiontype['sound'] = ['actiontype',
				       'actionarg1',
				       'actionarg2-skip',
				       'actionarg2-noanswer',
				       'actionarg2-j'];

var xivo_dialaction_actionarg = new Array();
xivo_dialaction_actionarg['endcall'] = new Array();
xivo_dialaction_actionarg['endcall']['hangup'] = [{display: false, name: 'busy-actionarg1'},
						  {display: false, name: 'congestion-actionarg1'}];
xivo_dialaction_actionarg['endcall']['busy'] = [{display: true, name: 'busy-actionarg1'},
						{display: false, name: 'congestion-actionarg1'}];
xivo_dialaction_actionarg['endcall']['congestion'] = [{display: false, name: 'busy-actionarg1'},
						      {display: true, name: 'congestion-actionarg1'}];
xivo_dialaction_actionarg['application'] = new Array();
xivo_dialaction_actionarg['application']['callback'] = [{display: true, name: 'callback-actionarg1'},
							{display: true, name: 'callback-actionarg2'},
							{display: false, name: 'callbackdisa-actionarg1'},
							{display: false, name: 'callbackdisa-actionarg2'},
							{display: false, name: 'directory-actionarg1'},
							{display: false, name: 'faxtomail-actionarg1'},
							{display: false, name: 'voicemailmain-actionarg1'}];
xivo_dialaction_actionarg['application']['callbackdisa'] = [{display: false, name: 'callback-actionarg1'},
							    {display: false, name: 'callback-actionarg2'},
							    {display: true, name: 'callbackdisa-actionarg1'},
							    {display: true, name: 'callbackdisa-actionarg2'},
							    {display: false, name: 'directory-actionarg1'},
							    {display: false, name: 'faxtomail-actionarg1'},
							    {display: false, name: 'voicemailmain-actionarg1'}];
xivo_dialaction_actionarg['application']['directory'] = [{display: false, name: 'callback-actionarg1'},
							 {display: false, name: 'callback-actionarg2'},
							 {display: false, name: 'callbackdisa-actionarg1'},
							 {display: false, name: 'callbackdisa-actionarg2'},
							 {display: true, name: 'directory-actionarg1'},
							 {display: false, name: 'faxtomail-actionarg1'},
							 {display: false, name: 'voicemailmain-actionarg1'}];
xivo_dialaction_actionarg['application']['faxtomail'] = [{display: false, name: 'callback-actionarg1'},
							 {display: false, name: 'callback-actionarg2'},
							 {display: false, name: 'callbackdisa-actionarg1'},
							 {display: false, name: 'callbackdisa-actionarg2'},
							 {display: false, name: 'directory-actionarg1'},
							 {display: true, name: 'faxtomail-actionarg1'},
							 {display: false, name: 'voicemailmain-actionarg1'}];
xivo_dialaction_actionarg['application']['voicemailmain'] = [{display: false, name: 'callback-actionarg1'},
							     {display: false, name: 'callback-actionarg2'},
							     {display: false, name: 'callbackdisa-actionarg1'},
							     {display: false, name: 'callbackdisa-actionarg2'},
							     {display: false, name: 'directory-actionarg1'},
							     {display: false, name: 'faxtomail-actionarg1'},
							     {display: true, name: 'voicemailmain-actionarg1'}];

function xivo_ast_build_dialaction_array(dialevent)
{
	if(xivo_is_undef(xivo_elt_dialaction[dialevent]) == true
	|| xivo_is_array(xivo_elt_dialaction[dialevent]) == false
	|| xivo_is_undef(xivo_fm_dialaction[dialevent]) == true)
		return(false);

	xivo_elt_dialaction[dialevent]['links'] = new Array();
	xivo_elt_dialaction[dialevent]['links']['link'] = new Array();

	var i = 0;

	for(property in xivo_dialaction_actiontype)
	{
		ref = xivo_dialaction_actiontype[property];

		if(xivo_is_array(ref) === false || (nb = ref.length) === 0)
			continue;

		for(var j = 0;j < nb;j++)
		{
			key = 'fd-dialaction-'+dialevent+'-'+property+'-'+ref[j];
			xivo_elt_dialaction[dialevent][key] = new Array();
			xivo_elt_dialaction[dialevent][key]['style'] = 'display:none';
			xivo_elt_dialaction[dialevent]['links']['link'][i++] = new Array(key,0,1);

			key = 'it-dialaction-'+dialevent+'-'+property+'-'+ref[j];
			xivo_elt_dialaction[dialevent][key] = new Array();
			xivo_elt_dialaction[dialevent][key]['property'] = 'disabled|true:boolean;className|it-disabled';
			xivo_elt_dialaction[dialevent]['links']['link'][i++] = new Array(key,0,1);
		}
	}

	for(property in xivo_dialaction_actiontype)
	{
		ref = xivo_dialaction_actiontype[property];

		xivo_fm_dialaction[dialevent][property] = xivo_clone(xivo_elt_dialaction[dialevent]);

		if(xivo_is_array(ref) === false || (nb = ref.length) === 0)
		{
			xivo_attrib_register('fm_dialaction-'+dialevent+'-'+property,xivo_fm_dialaction[dialevent][property]);
			continue;
		}

		for(var j = 0;j < nb;j++)
		{
			keyfd = 'fd-dialaction-'+dialevent+'-'+property+'-'+ref[j];
			keyit = 'it-dialaction-'+dialevent+'-'+property+'-'+ref[j];

			xivo_fm_dialaction[dialevent][property][keyfd]['style'] = 'display:block';
			xivo_fm_dialaction[dialevent][property][keyit]['property'] = 'disabled|false:boolean;className|it-enabled';
		}

		xivo_attrib_register('fm_dialaction-'+dialevent+'-'+property,xivo_fm_dialaction[dialevent][property]);
	}
}

function xivo_ast_chg_dialaction(dialevent,actiontype)
{
	if(xivo_is_array(xivo_fm_dialaction[dialevent]) == false
	|| xivo_is_undef(actiontype.value) == true
	|| xivo_is_array(xivo_fm_dialaction[dialevent][actiontype.value]) == false
	|| (xivo_is_undef(actiontype.disabled) == false && actiontype.disabled == true) === true)
		return(false);

	xivo_chg_attrib('fm_dialaction-'+dialevent+'-'+actiontype.value,'links',0,1);
	xivo_ast_chg_dialaction_actionarg(dialevent,actiontype.value);
}

function xivo_ast_chg_dialaction_actionarg(dialevent,actiontype)
{
	if(xivo_is_array(xivo_fm_dialaction[dialevent]) === false
	|| xivo_is_array(xivo_dialaction_actionarg[actiontype]) === false
	|| (nb = xivo_dialaction_actionarg[actiontype]) === 0
	|| (action = xivo_eid('it-dialaction-'+dialevent+'-'+actiontype+'-action')) === false
	|| xivo_is_undef(action.value) === true
	|| xivo_is_array(xivo_dialaction_actionarg[actiontype][action.value]) === false
	|| (nb = xivo_dialaction_actionarg[actiontype][action.value].length) === 0)
		return(false);

	ref = xivo_dialaction_actionarg[actiontype][action.value];

	for(var i = 0;i < nb;i++)
	{
		if((fdactionarg = xivo_eid('fd-dialaction-'+dialevent+'-'+actiontype+'-'+ref[i].name)) !== false)
			fdactionarg.style.display = (ref[i].display === false ? 'none' : 'block');

		if((itactionarg = xivo_eid('it-dialaction-'+dialevent+'-'+actiontype+'-'+ref[i].name)) !== false)
		{
			if(ref[i].display === false)
				xivo_chg_property_attrib(itactionarg,'disabled|true:boolean;className|it-disabled');
			else
				xivo_chg_property_attrib(itactionarg,'disabled|false:boolean;className|it-enabled');
		}
	}
}

function xivo_ast_dialaction_onload()
{
	for(dialevent in xivo_elt_dialaction)
	{
		if((action = xivo_eid('it-dialaction-'+dialevent+'-actiontype')) !== false)
			xivo_ast_chg_dialaction(dialevent,action);
	}
}

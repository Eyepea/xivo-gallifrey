var xivo_fm = document.forms;

function xivo_fm_onfocus_onblur()
{
	var arr = new Array();
	arr[0] = 'input';
	arr[1] = 'select';
	arr[2] = 'textarea';

	if(xivo_is_undef(xivo_fm_onfocus_class) == true
	|| xivo_is_undef(xivo_fm_onblur_class) == true)
		return(false);

	for(var i = 0;i < 3;i++)
	{
		if((tag = xivo_etag(arr[i])) == false
		|| (len = tag.length) == 0)
			continue;

		for(var j = 0;j < len;j++)
		{
			if(arr[i] == 'input'
			&& tag[j].type != 'text'
			&& tag[j].type != 'file'
			&& tag[j].type != 'password')
				continue;

			if(xivo_is_undef(tag[j].onfocus) == true || tag[j].onfocus === null)
				tag[j].onfocus = function() { this.className = xivo_fm_onfocus_class; }

			if(xivo_is_undef(tag[j].onblur) == true || tag[j].onblur === null)
				tag[j].onblur = function() { this.className = xivo_fm_onblur_class; }
		}
	}

	return(true);
}

function xivo_fm_move_selected(from,to)
{
	if((from = xivo_eid(from)) == false
	|| (to = xivo_eid(to)) == false
	|| from.type != 'select-multiple'
	|| to.type != 'select-multiple')
		return(false);

	var len = to.options.length;

	for(var i = 0;i < len;i++)
		to.options[i].selected = false;

	len = from.options.length - 1;
	
	for(i = len; i >= 0; i--)
	{
		if(from.options[i].selected != true)
			continue;

		to.options[to.options.length] = new Option(from.options[i].text,from.options[i].value);
		to.options[to.options.length-1].selected = true;
		from.options[i] = null;	
	}

	return(true);
}

function xivo_fm_copy_select(from,to)
{
	if ((from = xivo_eid(from)) == false || (to = xivo_eid(to)) == false)
		return(false);

	if(to.selectedIndex == -1 || xivo_is_undef(to.options[to.selectedIndex]) == true)
		var selected = false;
	else
		var selected = to.options[to.selectedIndex].text;

	var len = to.options.length;

	for(var i = len; i >= 0; i--)
		to.options[i] = null;

	for(i = 0; i < from.options.length; i++)
	{
		to.options[to.options.length] = new Option(from.options[i].text,from.options[i].value);

		if(selected == from.options[i].text)
			to.options[to.options.length-1].selected = true;
	}

	return(true);
}

function xivo_fm_unshift_opt_select(from,text,value)
{
	if((from = xivo_eid(from)) == false
	|| (from.type != 'select-one'
	   && from.type != 'select-multiple') == true)
		return(false);

	if(xivo_is_undef(text) == true)
		text = '';

	if(xivo_is_undef(value) == true)
		value = text;

	var len = from.options.length;

	var noptions = new Array();

	for(var i = 0;i < len;i++)
		noptions[i] = from.options[i];

	len = noptions.length;	

	from.options[0] = new Option(text,value);

	for(i = 0;i < len;i++)
		from.options[i+1] = noptions[i];

	return(true);
}

function xivo_fm_pop_opt_select(from)
{
	if((from = xivo_eid(from)) == false
	|| (from.type != 'select-one'
	   && from.type != 'select-multiple') == true)
		return(false);

	from.options[0] = null;

	return(true);
}

function xivo_fm_select(from,select)
{
	if((from = xivo_eid(from)) == false || from.type != 'select-multiple')
		return(false);

	select = select == false ? false : true;

	var len = from.options.length;

	for(var i = 0;i < len;i++)
	{
		from.options[i].selected = select;
	}

	return(true);
}

function xivo_fm_order_selected(from,order)
{
	var r = false;
	order = Number(order);

	if ((from = xivo_eid(from)) == false || from.type != 'select-multiple')
		return(r);

	var len = from.length;
	var selected = from.selectedIndex;

	if(len < 2 || selected == -1 || xivo_is_undef(from.options[selected]) == true)
		return(r);

	if(order == -1)
	{
		if(selected == len-1 || xivo_is_undef(from.options[selected+1]) == true)
			return(r);

		var noption = from.options[selected+1];
		var soption = from.options[selected];

		from.options[selected+1] = new Option(soption.text,soption.value);
		from.options[selected] = new Option(noption.text,noption.value);
		
		xivo_fm_select(from.id,false);

		from.options[selected+1].selected = true;
	}
	else
	{
		if(selected == 0 || xivo_is_undef(from.options[selected-1]) == true)
			return(r);

		var noption = from.options[selected-1];
		var soption = from.options[selected];

		from.options[selected-1] = new Option(soption.text,soption.value);
		from.options[selected] = new Option(noption.text,noption.value);

		xivo_fm_select(from.id,false);

		from.options[selected-1].selected = true;
	}
}

function xivo_fm_unshift_pop_opt_select(from,text,value,chk,num)
{
	if((from = xivo_eid(from)) == false || (from.type != 'select-one' && from.type != 'select-multiple') == true)
		return(false);

	if(xivo_is_undef(text) == true)
		text = '';

	if(xivo_is_undef(value) == true)
		value = null;

	if(xivo_is_undef(chk) == true)
		chk = '';

	if(xivo_is_undef(num) == true)
		num = 0;

	num = Number(num);

	if(from.value == chk)
		xivo_fm_unshift_opt_select(from.id,text,value);

	if(from.value == chk && xivo_is_undef(from.options[num]) == false && from.options[num].value == 'null')
		xivo_fm_pop_opt_select(from.id);

	return(true);
}

function xivo_fm_field_disabled(obj,disable)
{
	var i = 0;

	if(xivo_is_undef(disable) == true)
		disable = false;

	disable = Boolean(disable);

	var tag_input = false;
	var tag_select = false;
	var tag_textarea = false;

	if(xivo_is_object(obj) == false)
		return(false);

	if((tag_input = xivo_etag('input',obj)) != false)
	{
		var tag_input_nb = tag_input.length;

		for(i = 0;i < tag_input_nb;i++)
			tag_input[i].disabled = disable;
	}

	if((tag_select = xivo_etag('select',obj)) != false)
	{
		var tag_select_nb = tag_select.length;

		for(i = 0;i < tag_select_nb;i++)
			tag_select[i].disabled = disable;
	}

	if((tag_textarea = xivo_etag('textarea',obj)) != false)
	{
		var tag_textarea_nb = tag_textarea.length;

		for(i = 0;i < tag_textarea_nb;i++)
			tag_textarea[i].disabled = disable;
	}

	return(true);
}

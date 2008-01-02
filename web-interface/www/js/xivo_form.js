var xivo_fm = document.forms;
var xivo_fm_error = new Array();

function xivo_fm_show_error()
{
	if(xivo_is_undef(xivo_fm_error) == true
	|| xivo_is_array(xivo_fm_error) == false
	|| xivo_is_undef(xivo_fm_error_class) == true)
		return(false);

	for(var key in xivo_fm_error)
	{
		var val = new Boolean(xivo_fm_error[key]);

		if((obj = xivo_eid(key)) == false || val == false)
			continue;

		switch(obj.tagName.toLowerCase())
		{
			case 'input':
				if(obj.type != 'text'
				&& obj.type != 'file'
				&& obj.type != 'password')
					continue;
			default:
				obj.className = xivo_fm_error_class;
		}
	}

	return(true);
}

function xivo_fm_set_onfocus(obj)
{
	var arr = new Array();
	arr['input'] = 1;
	arr['select'] = 1;
	arr['textarea'] = 1;

	if(xivo_is_undef(xivo_fm_onfocus_class) == true
	|| xivo_is_undef(xivo_fm_error_class) == true
	|| xivo_is_undef(obj.tagName) == true
	|| xivo_is_undef(arr[obj.tagName.toLowerCase()]) == true)
		return(false);

	if((obj.tagName.toLowerCase() == 'input'
	&& obj.type != 'text'
	&& obj.type != 'file'
	&& obj.type != 'password') == true
	|| obj.className == xivo_fm_error_class
	|| obj.readOnly == true
	|| obj.disabled == true)
		return(false);

	obj.className = xivo_fm_onfocus_class;

	return(true);
}

function xivo_fm_set_onblur(obj)
{
	var arr = new Array();
	arr['input'] = 1;
	arr['select'] = 1;
	arr['textarea'] = 1;

	if(xivo_is_undef(xivo_fm_onblur_class) == true
	|| xivo_is_undef(xivo_fm_error_class) == true
	|| xivo_is_undef(obj.tagName) == true
	|| xivo_is_undef(arr[obj.tagName.toLowerCase()]) == true)
		return(false);

	if((obj.tagName.toLowerCase() == 'input'
	&& obj.type != 'text'
	&& obj.type != 'file'
	&& obj.type != 'password') == true
	|| obj.className == xivo_fm_error_class
	|| obj.readOnly == true
	|| obj.disabled == true)
		return(false);

	obj.className = xivo_fm_onblur_class;

	return(true);
}

function xivo_fm_onfocus_onblur()
{
	var arr = new Array();
	arr[0] = 'input';
	arr[1] = 'select';
	arr[2] = 'textarea';

	if(xivo_is_undef(xivo_fm_error_class) == true)
		return(false);

	for(var i = 0;i < 3;i++)
	{
		if((tag = xivo_etag(arr[i])) == false
		|| (len = tag.length) == 0)
			continue;

		for(var j = 0;j < len;j++)
		{
			if((arr[i] == 'input'
			&& tag[j].type != 'text'
			&& tag[j].type != 'file'
			&& tag[j].type != 'password') == true)
				continue;

			if(xivo_is_undef(tag[j].onfocus) == true || tag[j].onfocus === null)
				tag[j].onfocus = function() { xivo_fm_set_onfocus(this); }

			if(xivo_is_undef(tag[j].onblur) == true || tag[j].onblur === null)
				tag[j].onblur = function() { xivo_fm_set_onblur(this); }
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

function xivo_fm_select_add_entry(id,text,value)
{
	if ((obj = xivo_eid(id)) == false
	|| (obj.type != 'select-multiple'
	   && obj.type != 'select-one') == true)
		return(false);

	if(xivo_is_undef(text) == true)
		text = '';

	if(xivo_is_undef(value) == true)
		value = null;

	var len = obj.options.length;

	obj.options[len] = new Option(text,value);

	return(true);
}

function xivo_fm_select_delete_entry(id)
{
	if ((obj = xivo_eid(id)) == false
	|| (obj.type != 'select-multiple'
	   && obj.type != 'select-one') == true)
		return(false);
	
	var len = obj.options.length - 1;

	for(i = len; i >= 0; i--)
	{
		if(obj.options[i].selected == true)
			obj.options[i] = null;
	}

	return(true);
}

function xivo_fm_unshift_pop_opt_select(from,text,value,chk,num)
{
	if((from = xivo_eid(from)) == false
	|| (from.type != 'select-one'
	   && from.type != 'select-multiple') == true)
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
	if(xivo_is_object(obj) == false)
		return(false);

	if(xivo_is_undef(disable) == true)
		disable = false;

	disable = Boolean(disable);

	var i = 0;
	var tag_input = false;
	var tag_select = false;
	var tag_textarea = false;

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

function xivo_fm_field_id_counter(obj,cnt)
{
	if(xivo_is_object(obj) == false || xivo_is_int(cnt) == false)
		return(false);

	var i = 0;
	var tag_input = false;
	var tag_select = false;
	var tag_textarea = false;

	if((tag_input = xivo_etag('input',obj)) != false)
	{
		var tag_input_nb = tag_input.length;

		for(i = 0;i < tag_input_nb;i++)
		{
			if(xivo_is_undef(tag_input[i].id) == false
			&& tag_input[i].id.length > 0)
				tag_input[i].id += '-'+cnt;
		}
	}

	if((tag_select = xivo_etag('select',obj)) != false)
	{
		var tag_select_nb = tag_select.length;

		for(i = 0;i < tag_select_nb;i++)
		{
			if(xivo_is_undef(tag_select[i].id) == false
			&& tag_select[i].id.length > 0)
				tag_select[i].id += '-'+cnt;
		}
	}

	if((tag_textarea = xivo_etag('textarea',obj)) != false)
	{
		var tag_textarea_nb = tag_textarea.length;

		for(i = 0;i < tag_textarea_nb;i++)
		{
			if(xivo_is_undef(tag_textarea[i].id) == false
			&& tag_textarea[i].id.length > 0)
				tag_textarea[i].id += '-'+cnt;
		}
	}

	return(true);
}

function xivo_fm_field_name_counter(obj,cnt)
{
	if(xivo_is_object(obj) == false || xivo_is_int(cnt) == false)
		return(false);

	var i = 0;
	var tag_input = false;
	var tag_select = false;
	var tag_textarea = false;

	if((tag_input = xivo_etag('input',obj)) != false)
	{
		var tag_input_nb = tag_input.length;

		for(i = 0;i < tag_input_nb;i++)
		{
			if(xivo_is_undef(tag_input[i].name) == false)
				tag_input[i].name += '['+cnt+']';
		}
	}

	if((tag_select = xivo_etag('select',obj)) != false)
	{
		var tag_select_nb = tag_select.length;

		for(i = 0;i < tag_select_nb;i++)
		{
			if(xivo_is_undef(tag_select[i].name) == false)
				tag_select[i].name += '['+cnt+']';
		}
	}

	if((tag_textarea = xivo_etag('textarea',obj)) != false)
	{
		var tag_textarea_nb = tag_textarea.length;

		for(i = 0;i < tag_textarea_nb;i++)
		{
			if(xivo_is_undef(tag_textarea[i].name) == false)
				tag_textarea[i].name += '['+cnt+']';
		}
	}

	return(true);
}

function xivo_fm_mk_acl(tree)
{
        var ref = tree.form[tree.name];
        var value = tree.value;
        var len = value.length;
        var nb = ref.length
        var sub = 0;
        var rs = false;

        for(i = 0;i < nb;i++)
        {
                sub = ref[i].value.substring(0,len);
                rs = tree.checked == true ? true : false;

                if(value == sub)
                        ref[i].checked = rs;
        }
}

function xivo_fm_readonly(list,enable)
{
	if(xivo_is_array(list) == false)
		list = new Array(list);

	enable = new Boolean(enable);

	nb = list.length;

	for(var i = 0;i < nb;i++)
	{
		if((element = xivo_eid(list[i])) == false)
			continue;

		if(enable == true)
		{
			element.disabled = false;
			element.readOnly = false;
			element.className = xivo_fm_enabled_class;
		}
		else
		{
			if(element.tagName.toLowerCase() == 'select')
				element.disabled = true;
			else
				element.readOnly = true;
			element.className = xivo_fm_readonly_class;
		}
	}
}

function xivo_fm_select_add_host_ipv4_subnet(id,value)
{
	if(xivo_chk_ipv4_strict(value) == false
	&& xivo_chk_host(value) == false
	&& xivo_chk_ipv4_subnet(value) == false)
		return(false);

	return(xivo_fm_select_add_entry(id,value,value));
}

function xivo_fm_checked_all(form,name,mode)
{
	if(xivo_is_undef(form) == true
	|| xivo_is_undef(name) == true
	|| xivo_is_string(form) == false
	|| xivo_is_string(name) == false
	|| xivo_is_undef(xivo_fm[form]) == true
	|| xivo_is_undef(xivo_fm[form][name]) == true)
		return(false);

	if(xivo_is_undef(mode) == true)
		mode = true;
	else if(mode != 'reverse')
		mode = Boolean(mode);

	ref = xivo_fm[form][name];

	if(xivo_is_undef(ref.length) == false)
		len = ref.length;
	else if(xivo_is_undef(ref.type) == true
	|| (ref.type != 'checkbox' && ref.type != 'radio') == true)
		return(false);
	else
	{
		if(mode != 'reverse')
			ref.checked = mode;
		else if(ref.checked == true)
			ref.checked = false;
		else
			ref.checked = true;

		return(true);
	}

	for(var i = 0;i < len;i++)
	{
		if(ref[i].type != 'checkbox' && ref[i].type != 'radio')
			continue;

		if(mode != 'reverse')
			ref[i].checked = mode;
		else if(ref[i].checked == true)
			ref[i].checked = false;
		else
			ref[i].checked = true;
	}

	return(true);
}

function xivo_fm_get_checked(form,name)
{
	if(xivo_is_undef(form) == true
	|| xivo_is_undef(name) == true
	|| xivo_is_string(form) == false
	|| xivo_is_string(name) == false
	|| xivo_is_undef(xivo_fm[form]) == true
	|| xivo_is_undef(xivo_fm[form][name]) == true)
		return(false);

	ref = xivo_fm[form][name];

	if(xivo_is_undef(ref.length) == false)
		len = ref.length;
	else if(xivo_is_undef(ref.type) == true
	|| (ref.type != 'checkbox' && ref.type != 'radio') == true)
		return(false);
	else
		return(0);

	for(var i = 0;i < len;i++)
	{
		if(ref[i].type != 'checkbox' && ref[i].type != 'radio')
			continue;

		if(ref[i].checked == true)
			return(i);
	}

	return(false);
}

function xivo_fm_get_value_from_key(form,name,key)
{
	if(xivo_is_undef(form) == true
	|| xivo_is_undef(name) == true
	|| xivo_is_string(form) == false
	|| xivo_is_string(name) == false
	|| xivo_is_undef(xivo_fm[form]) == true
	|| xivo_is_undef(xivo_fm[form][name]) == true
	|| xivo_is_undef(xivo_fm[form][name][key]) == true
	|| xivo_is_undef(xivo_fm[form][name][key].value) == true)
		return(false);

	return(xivo_fm[form][name][key].value);
}

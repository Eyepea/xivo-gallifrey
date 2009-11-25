/*
 * XiVO Web-Interface
 * Copyright (C) 2006-2009  Proformatique <technique@proformatique.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

if(typeof(dwho) === 'undefined')
	var dwho = {'form': {}};
else if(dwho_is_undef(dwho.form) === true)
	dwho.form = {};

dwho.fm = document.forms;
dwho.form.error = {};
dwho.form.text_helper = {};

dwho.form.set_events_text_helper = function(id)
{
	dwho.dom.add_cssclass(dwho_eid(id),'it-helper');
	dwho.dom.add_event('focus',dwho_eid(id),dwho.form.focus_text_helper);
	dwho.dom.add_event('blur',dwho_eid(id),dwho.form.blur_text_helper);
}

dwho.form.focus_text_helper = function()
{
	if(dwho_has_len(this.id) === false)
		return(false);
	else if(dwho_has_len(dwho.form.text_helper[this.id]) === false)
	{
		dwho.form.text_helper[this.id] = this.value;
		dwho.dom.remove_cssclass(this,'it-helper');
		this.value = '';
	}
}

dwho.form.blur_text_helper = function()
{
	if(dwho_has_len(this.id) === false)
		return(false);
	else if(dwho_is_undef(dwho.form.text_helper[this.id]) === false
	&& dwho_has_len(this.value) === false)
	{
		this.value = dwho.form.text_helper[this.id];
		dwho.dom.add_cssclass(this,'it-helper');
		dwho.form.text_helper[this.id] = '';
	}
}

dwho.form.show_error = function()
{
	if(dwho_is_undef(dwho.form.error) === true
	|| dwho_type_object(dwho.form.error) === false
	|| dwho_is_undef(dwho_form_class_error) === true)
		return(false);

	for(var key in dwho.form.error)
	{
		var val = Boolean(dwho.form.error[key]);

		if((obj = dwho_eid(key)) === false || val === false)
			continue;

		switch(obj.tagName.toLowerCase())
		{
			case 'input':
				if(obj.type !== 'text'
				&& obj.type !== 'file'
				&& obj.type !== 'password')
					continue;
			default:
				obj.className = dwho_form_class_error;
		}
	}

	return(true);
}

dwho.form.set_onfocus = function(obj)
{
	var list = {'input': 1,'select': 1, 'textarea': 1};

	if(dwho_is_undef(dwho_form_class_onfocus) === true
	|| dwho_is_undef(dwho_form_class_error) === true
	|| dwho_is_undef(dwho_form_class_disabled) === true
	|| dwho_is_undef(obj.tagName) === true
	|| dwho_is_undef(list[obj.tagName.toLowerCase()]) === true)
		return(false);
	else if((obj.tagName.toLowerCase() === 'input'
	&& obj.type !== 'text'
	&& obj.type !== 'file'
	&& obj.type !== 'password') === true
	|| obj.className === dwho_form_class_error
	|| obj.className === dwho_form_class_disabled
	|| obj.readOnly === true
	|| obj.disabled === true)
		return(false);

	obj.className = dwho_form_class_onfocus;

	return(true);
}

dwho.form.set_onblur = function(obj)
{
	var list = {'input': 1,'select': 1, 'textarea': 1};

	if(dwho_is_undef(dwho_form_class_onblur) === true
	|| dwho_is_undef(dwho_form_class_error) === true
	|| dwho_is_undef(dwho_form_class_disabled) === true
	|| dwho_is_undef(obj.tagName) === true
	|| dwho_is_undef(list[obj.tagName.toLowerCase()]) === true)
		return(false);
	else if((obj.tagName.toLowerCase() === 'input'
	&& obj.type !== 'text'
	&& obj.type !== 'file'
	&& obj.type !== 'password') === true
	|| obj.className === dwho_form_class_error
	|| obj.className === dwho_form_class_disabled
	|| obj.readOnly === true
	|| obj.disabled === true)
		return(false);

	obj.className = dwho_form_class_onblur;

	return(true);
}

dwho.form.onfocus_onblur = function(obj)
{
	var arr = ['input', 'select', 'textarea'];

	var focus	= function() { dwho.form.set_onfocus(this); };
	var blur	= function() { dwho.form.set_onblur(this); }

	for(var i = 0;i < 3;i++)
	{
		if((tag = dwho.dom.etag(arr[i],obj)) === false
		|| (len = tag.length) === 0)
			continue;

		for(var j = 0;j < len;j++)
		{
			if(arr[i] === 'input'
			&& tag[j].type !== 'text'
			&& tag[j].type !== 'file'
			&& tag[j].type !== 'password')
				continue;

			dwho.dom.add_event('focus',tag[j],focus);
			dwho.dom.add_event('blur',tag[j],blur);
		}
	}

	return(true);
}

dwho.form.set_disable_submit_onenter = function(form)
{
	if((tag = dwho.dom.etag('input',form)) === false
	|| (len = tag.length) === 0)
		return(null);

	for(var i = 0;i < len;i++)
	{
		if(tag[i].type === 'text'
		   || tag[i].type === 'file'
		   || tag[i].type === 'password'
		   || tag[i].type === 'checkbox'
		   || tag[i].type === 'radio')
			dwho.dom.add_event('keypress',
					   tag[i],
					   dwho.form.disable_submit_onenter);
	}

	return(true);
}

dwho.form.move_selected = function(from,to)
{
	if((from = dwho_eid(from)) === false
	|| (to = dwho_eid(to)) === false
	|| from.type !== 'select-multiple'
	|| to.type !== 'select-multiple')
		return(false);

	var len = to.options.length;

	for(var i = 0;i < len;i++)
		to.options[i].selected = false;

	len = from.options.length;

	for(i = 0;i < len;i++)
	{
		if(from.options[i].selected !== true)
			continue;

		to.options[to.options.length] = new Option(from.options[i].text,from.options[i].value);
		to.options[to.options.length-1].selected = true;
		from.options[i--] = null;
		len--;
	}

	return(true);
}

dwho.form.copy_select = function(from,to)
{
	if((from = dwho_eid(from)) === false
	|| (to = dwho_eid(to)) === false
	|| (from.type !== 'select-one'
	   && from.type !== 'select-multiple') === true
	|| (to.type !== 'select-one'
	   && to.type !== 'select-multiple') === true)
		return(false);
	else if(to.selectedIndex === -1 || dwho_is_undef(to.options[to.selectedIndex]) === true)
		var selected = false;
	else
		var selected = to.options[to.selectedIndex].text;

	var len = to.options.length;

	for(var i = len; i >= 0; i--)
		to.options[i] = null;

	for(i = 0; i < from.options.length; i++)
	{
		to.options[to.options.length] = new Option(from.options[i].text,from.options[i].value);

		if(selected === from.options[i].text)
			to.options[to.options.length-1].selected = true;
	}

	return(true);
}

dwho.form.unshift_opt_select = function(from,text,value)
{
	if((from = dwho_eid(from)) === false
	|| (from.type !== 'select-one'
	   && from.type !== 'select-multiple') === true)
		return(false);
	else if(dwho_is_undef(text) === true)
		text = '';

	if(dwho_is_undef(value) === true)
		value = text;

	var len = from.options.length;

	var noptions = [];

	for(var i = 0;i < len;i++)
		noptions[i] = from.options[i];

	len = noptions.length;

	from.options[0] = new Option(text,value);

	for(i = 0;i < len;i++)
		from.options[i+1] = noptions[i];

	return(true);
}

dwho.form.pop_opt_select = function(from)
{
	if((from = dwho_eid(from)) === false
	|| (from.type !== 'select-one'
	   && from.type !== 'select-multiple') === true)
		return(false);

	from.options[0] = null;

	return(true);
}

dwho.form.get_text_opt_select = function(from,value,chk)
{
	if((from = dwho_eid(from)) === false
	|| from.type !== 'select-one'
	|| dwho_is_scalar(value) === false)
		return(false);

	var r = false;
	var sltindex = from.selectedIndex;

	from.value = value;
	var valueindex = from.selectedIndex;

	if(dwho_is_undef(from.options[valueindex]) === false)
		r = from.options[valueindex].text;

	if(Boolean(chk) === true && dwho_string(from.value) !== dwho_string(value))
		r = false;

	from.selectedIndex = sltindex;

	return(r);
}

dwho.form.select = function(from,select)
{
	if((from = dwho_eid(from)) === false || from.type !== 'select-multiple')
		return(false);

	select = dwho_is_undef(select) === true ? true : Boolean(select);

	var len = from.options.length;

	for(var i = 0;i < len;i++)
		from.options[i].selected = select;

	return(true);
}

dwho.form.order_selected = function(from,order,num)
{
	order = Number(order);

	if((from = dwho_eid(from)) === false || from.type !== 'select-multiple')
		return(false);

	var len = from.length;
	var selected = from.selectedIndex;

	if(len < 2 || selected === -1 || dwho_is_undef(from.options[selected]) === true)
		return(false);
	else if(order === -1)
	{
		if(selected === len-1 || dwho_is_undef(from.options[selected+1]) === true)
			return(false);

		var noption = from.options[selected+1];
		var soption = from.options[selected];

		if(Boolean(num) === false)
		{
			ntext = noption.text;
			stext = soption.text;
		}
		else
		{
			if((rs = noption.text.match(/^(\d+)\. (.*)$/)) !== null)
				ntext = rs[2];
			else
				ntext = noption.text;

			ntext = (selected+1)+'. '+ntext;

			if((rs = soption.text.match(/^(\d+)\. (.*)$/)) !== null)
				stext = rs[2];
			else
				stext = soption.text;

			stext = (selected+2)+'. '+stext;
		}

		from.options[selected+1] = new Option(stext,soption.value);
		from.options[selected] = new Option(ntext,noption.value);

		dwho.form.select(from.id,false);

		from.options[selected+1].selected = true;
	}
	else
	{
		if(selected === 0 || dwho_is_undef(from.options[selected-1]) === true)
			return(false);

		var noption = from.options[selected-1];
		var soption = from.options[selected];

		if(Boolean(num) === false)
		{
			ntext = noption.text;
			stext = soption.text;
		}
		else
		{
			if((rs = noption.text.match(/^(\d+)\. (.*)$/)) !== null)
				ntext = rs[2];
			else
				ntext = noption.text;

			ntext = (selected+1)+'. '+ntext;

			if((rs = soption.text.match(/^(\d+)\. (.*)$/)) !== null)
				stext = rs[2];
			else
				stext = soption.text;

			stext = selected+'. '+stext;
		}

		from.options[selected-1] = new Option(stext,soption.value);
		from.options[selected] = new Option(ntext,noption.value);

		dwho.form.select(from.id,false);

		from.options[selected-1].selected = true;
	}
}

dwho.form.select_add_entry = function(id,text,value,num)
{
	if ((obj = dwho_eid(id)) === false
	|| (obj.type !== 'select-multiple'
	   && obj.type !== 'select-one') === true)
		return(false);
	else if(dwho_is_undef(text) === true)
		text = '';

	if(dwho_is_undef(value) === true)
		value = null;

	var len = obj.options.length;

	if(Boolean(num) === true)
		text = (len + 1)+'. '+text;

	obj.options[len] = new Option(text,value);
	obj.scrollTop = obj.scrollHeight;

	return(true);
}

dwho.form.select_delete_entry = function(id,num)
{
	if ((obj = dwho_eid(id)) === false
	|| (obj.type !== 'select-multiple'
	   && obj.type !== 'select-one') === true)
		return(false);

	var len = obj.options.length;

	if(Boolean(num) === false)
	{
		for(var i = len-1;i >= 0;i--)
		{
			if(obj.options[i].selected === true)
				obj.options[i] = null;
		}

		return(true);
	}

	for(var i = 0;i < len;i++)
	{
		if(obj.options[i].selected === true)
		{
			obj.options[i--] = null;
			len--;
			continue;
		}
		else if((rs = obj.options[i].text.match(/^(\d+)\. (.*)$/)) !== null)
			text = rs[2];
		else
			text = obj.options[i].text;

		obj.options[i].text = (i+1)+'. '+text;
	}

	return(true);
}

dwho.form.field_disabled = function(obj,disable)
{
	if(dwho_is_object(obj) === false)
		return(false);
	else if(dwho_is_undef(disable) === true)
		disable = false;

	disable = Boolean(disable);

	if((tag_input = dwho.dom.etag('input',obj)) !== false)
	{
		var tag_input_nb = tag_input.length;

		for(var i = 0;i < tag_input_nb;i++)
			tag_input[i].disabled = disable;
	}

	if((tag_select = dwho.dom.etag('select',obj)) !== false)
	{
		var tag_select_nb = tag_select.length;

		for(var i = 0;i < tag_select_nb;i++)
			tag_select[i].disabled = disable;
	}

	if((tag_textarea = dwho.dom.etag('textarea',obj)) !== false)
	{
		var tag_textarea_nb = tag_textarea.length;

		for(var i = 0;i < tag_textarea_nb;i++)
			tag_textarea[i].disabled = disable;
	}

	return(true);
}

dwho.form.field_id_counter = function(obj,cnt)
{
	if(dwho_is_object(obj) === false || dwho_is_int(cnt) === false)
		return(false);

	var taglist = ['input','select','textarea','a','span'];
	var len = taglist.length;

	for(var i = 0;i < len;i++)
	{
		if((tagobj = dwho.dom.etag(taglist[i],obj)) === false)
			continue;

		tagobj_nb = tagobj.length;

		for(var j = 0;j < tagobj_nb;j++)
		{
			if(dwho_is_undef(tagobj[j].id) === false
			&& tagobj[j].id.length > 0)
				tagobj[j].id += '-'+cnt;
		}
	}

	return(true);
}

dwho.form.field_name_counter = function(obj,cnt)
{
	if(dwho_is_object(obj) === false || dwho_is_int(cnt) === false)
		return(false);
	else if((tag_input = dwho.dom.etag('input',obj)) !== false)
	{
		var tag_input_nb = tag_input.length;

		for(var i = 0;i < tag_input_nb;i++)
		{
			if(dwho_is_undef(tag_input[i].name) === false)
				tag_input[i].name += '['+cnt+']';
		}
	}

	if((tag_select = dwho.dom.etag('select',obj)) !== false)
	{
		var tag_select_nb = tag_select.length;

		for(var i = 0;i < tag_select_nb;i++)
		{
			if(dwho_is_undef(tag_select[i].name) === false)
				tag_select[i].name += '['+cnt+']';
		}
	}

	if((tag_textarea = dwho.dom.etag('textarea',obj)) !== false)
	{
		var tag_textarea_nb = tag_textarea.length;

		for(var i = 0;i < tag_textarea_nb;i++)
		{
			if(dwho_is_undef(tag_textarea[i].name) === false)
				tag_textarea[i].name += '['+cnt+']';
		}
	}

	return(true);
}

dwho.form.readonly = function(list,enable)
{
	if(dwho_is_array(list) === false)
		list = new Array(list);

	enable = Boolean(enable);

	nb = list.length;

	for(var i = 0;i < nb;i++)
	{
		if((element = dwho_eid(list[i])) === false)
			continue;
		else if(enable === true)
		{
			element.disabled = false;
			element.readOnly = false;
			element.className = dwho_form_class_enabled;
		}
		else
		{
			if(element.tagName.toLowerCase() === 'select')
				element.disabled = true;
			else
				element.readOnly = true;
			element.className = dwho_form_class_readonly;
		}
	}
}

dwho.form.checked_all = function(form,name,mode)
{
	if(dwho_is_string(form) === false
	|| dwho_is_string(name) === false
	|| dwho_is_undef(dwho.fm[form]) === true
	|| dwho_is_undef(dwho.fm[form][name]) === true)
		return(false);
	else if(dwho_is_undef(mode) === true)
		mode = true;
	else if(mode !== 'reverse')
		mode = Boolean(mode);

	ref = dwho.fm[form][name];

	if(dwho_is_undef(ref.length) === false)
		len = ref.length;
	else if(dwho_is_undef(ref.type) === true
	|| (ref.type !== 'checkbox' && ref.type !== 'radio') === true)
		return(false);
	else
	{
		if(mode !== 'reverse')
			ref.checked = mode;
		else if(ref.checked === true)
			ref.checked = false;
		else
			ref.checked = true;

		return(true);
	}

	for(var i = 0;i < len;i++)
	{
		if(ref[i].type !== 'checkbox' && ref[i].type !== 'radio')
			continue;
		else if(ref[i].disabled === true)
			continue;
		else if(mode !== 'reverse')
			ref[i].checked = mode;
		else if(ref[i].checked === true)
			ref[i].checked = false;
		else
			ref[i].checked = true;
	}

	return(true);
}

dwho.form.get_checked = function(form,name)
{
	if(dwho_is_string(form) === false
	|| dwho_is_string(name) === false
	|| dwho_is_undef(dwho.fm[form]) === true
	|| dwho_is_undef(dwho.fm[form][name]) === true)
		return(false);

	ref = dwho.fm[form][name];

	if(dwho_is_undef(ref.length) === false)
		len = ref.length;
	else if(dwho_is_undef(ref.type) === true
	|| (ref.type !== 'checkbox' && ref.type !== 'radio') === true)
		return(false);
	else
		return(0);

	for(var i = 0;i < len;i++)
	{
		if((ref[i].type === 'checkbox' || ref[i].type === 'radio') === true
		&& ref[i].checked === true)
			return(i);
	}

	return(false);
}

dwho.form.get_value_from_key = function(form,name,key)
{
	if(dwho_is_string(form) === false
	|| dwho_is_string(name) === false
	|| dwho_is_undef(dwho.fm[form]) === true
	|| dwho_is_undef(dwho.fm[form][name]) === true
	|| dwho_is_undef(dwho.fm[form][name][key]) === true
	|| dwho_is_undef(dwho.fm[form][name][key].value) === true)
		return(false);

	return(dwho.fm[form][name][key].value);
}

dwho.form.toggle_enable_field = function(form,name,disable,exform,exformtag)
{
	if(dwho_is_object(form) === false
	|| dwho_is_string(name) === false
	|| dwho_is_undef(form[name]) === true)
		return(false);

	if(dwho_is_string(exform) === true && dwho_is_string(exformtag) === true)
		var disableparent = false;
	else
		var disableparent = true;

	if((disable = Boolean(disable) !== false))
		var classname = dwho_form_class_disabled;
	else
		var classname = dwho_form_class_enabled;

	var ref = form[name];

	if(dwho_is_undef(ref.tagName) === true)
	{
		if(dwho_is_undef(ref.item) === true
		|| dwho_is_undef(ref.length) === true)
			return(false);

		var nb = ref.length;

		for(var i = 0;i < nb;i++)
		{
			if(dwho_is_undef(ref[i]) === false)
			{
				if(disableparent === false
				&& dwho.dom.get_parent_by_tag(ref[i],exformtag).id === exform)
					continue;

				ref[i].disabled = disable;
				ref[i].className = classname;
			}
		}

		return(true);
	}

	switch(ref.tagName.toLowerCase())
	{
		case 'input':
		case 'select':
		case 'textarea':
			if(disableparent === false
			&& dwho.dom.get_parent_by_tag(ref,exformtag).id === exform)
				return(false);

			ref.disabled = disable;
			ref.className = classname;
		default:
			return(false);
	}

	return(true);
}

dwho.form.reset_field = function(obj,empty)
{
	if(dwho_is_undef(obj.tagName) === true
	|| dwho_is_undef(obj.type) === true)
		return(false);

	switch(obj.tagName.toLowerCase())
	{
		case 'input':
			if(obj.type === 'checkbox'
			|| obj.type === 'radio')
				obj.checked = obj.defaultChecked;
			else if(obj.type === 'text'
			|| obj.type === 'file'
			|| obj.type === 'password')
				obj.value = Boolean(empty) === false ? obj.defaultValue : '';
			break;
		case 'textarea':
				obj.value = Boolean(empty) === false ? obj.defaultValue : '';
			break;
		case 'select':
			if(obj.type !== 'select-multiple' && obj.type !== 'select-one')
				return(false);

			var len = obj.options.length;

			for(var i = 0; i < len; i++)
				obj.options[i].selected = obj.options[i].defaultSelected;
			break;
	}

	return(true);
}

dwho.form.reset_child_field = function(obj,empty)
{
	if(dwho_is_object(obj) === false)
		return(false);

	if((tag_input = dwho.dom.etag('input',obj)) !== false)
	{
		var tag_input_nb = tag_input.length;

		for(var i = 0;i < tag_input_nb;i++)
			dwho.form.reset_field(tag_input[i],empty);
	}

	if((tag_select = dwho.dom.etag('select',obj)) !== false)
	{
		var tag_select_nb = tag_select.length;

		for(var i = 0;i < tag_select_nb;i++)
			dwho.form.reset_field(tag_select[i]);
	}

	if((tag_textarea = dwho.dom.etag('textarea',obj)) !== false)
	{
		var tag_textarea_nb = tag_textarea.length;

		for(var i = 0;i < tag_textarea_nb;i++)
			dwho.form.reset_field(tag_textarea[i],empty);
	}

	return(true);
}

dwho.form.disable_submit_onenter = function(e)
{
	var keycode = dwho_is_undef(window.event) === false ? window.event.keyCode : e.keyCode;

	if(keycode === 13)
	{
		if(dwho_is_function(e.preventDefault) === true)
			e.preventDefault();
		return(false);
	}

	return(true);
}

dwho.dom.set_onload(dwho.form.onfocus_onblur);
dwho.dom.set_onload(dwho.form.show_error);

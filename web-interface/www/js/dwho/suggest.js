/*
 * XiVO Web-Interface
 * Copyright (C) 2009-2010  Proformatique <technique@proformatique.com>
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
	var dwho = {'suggest': {}};
else if(typeof(dwho.suggest) === 'undefined')
	dwho.suggest = {};

dwho.suggest = function(options,id)
{
	this._namespace		= '__dwho-suggest__';
	this._suggests		= [];
	this._field		= null;
	this._resfield		= null;
	this._resfieldcleared	= false;
	this._autocomplete	= null;
	this._suggestid		= null;
	this._highlightedid	= null;
	this._delayid		= null;
	this._timeoutid		= null;
	this._dwsid		= null;
	this._dwsresid		= null;
	this._dwsrestitleid	= null;
	this._dwsresdescid	= null;
	this._highlightclass	= 'dws-highlight';
	this._searchval		= '';
	this._searchlen		= 0;
	this._vk		= {'TAB':	9,
				   'RETURN':	13,
				   'ESCAPE':	27,
				   'UP':	38,
				   'DOWN':	40};
	this._options		= {
		'requestor':			null,
		'request_delay':		0,
		'result_minlen':		1,
		'result_maxentries':		25,
		'result_field':			'',
		'result_onsetfield':		null,
		'result_onclearfield':		null,
		'result_fielddefaultvalue':	false,
		'result_fulldisplay':		false,
		'result_cache':			false,
		'result_class':			'dwho-suggest',
		'result_topoffset':		-10,
		'result_callback':		null,
		'result_displaycallback':	null,
		'result_emptycallback':		null};

	this.set_options(options);

	var dwsptr = this;

	this.fnkeypress	= function(e) { return(dwsptr.onkeypress(e)); }
	this.fnkeyup	= function(e) { return(dwsptr.onkeyup(e)); }
	this.fnfocus	= function() { dwsptr.deletetimeout(); }
	this.fnblur	= function() { dwsptr.onblur(); }
	this.fnchange	= function() { dwsptr.onchange(); }

	if(dwho_is_string(id) === true)
		this.set_field(id);
}

dwho.suggest.prototype.set_field = function(id)
{
	if(dwho_is_object(this._field) === true)
	{
		if(this._field.id === id)
			return(true);

		dwho.dom.remove_event('keypress',
				      this._field,
				      this.fnkeypress);

		dwho.dom.remove_event('keyup',
				      this._field,
				      this.fnkeyup);

		dwho.dom.remove_event('focus',
				      this._field,
				      this.fnfocus);

		dwho.dom.remove_event('mouseover',
				      this._field,
				      this.fnfocus);

		dwho.dom.remove_event('blur',
				      this._field,
				      this.fnblur);

		dwho.dom.remove_event('change',
				      this._field,
				      this.fnchange);

		this._field.setAttribute('autocomplete',this._autocomplete);

		this.clear();
	}

	if((this._field = dwho_eid(id,true)) === false)
		return(false);

	this._autocomplete = this._field.getAttribute('autocomplete');
	this._field.setAttribute('autocomplete','off');

	dwho.dom.add_event('keypress',
			   this._field,
			   this.fnkeypress);

	dwho.dom.add_event('keyup',
			   this._field,
			   this.fnkeyup);

	dwho.dom.add_event('focus',
			   this._field,
			   this.fnfocus);

	dwho.dom.add_event('mouseover',
			   this._field,
			   this.fnfocus);

	dwho.dom.add_event('blur',
			   this._field,
			   this.fnblur);

	dwho.dom.add_event('change',
			   this._field,
			   this.fnchange);

	this._searchval		= this._field.value;
	this._searchlen		= this._field.value.length;
	this._dwsid		= this._namespace + this._field.id;
	this._dwsresid		= this._dwsid + '-res';
	this._dwsrestitleid	= this._dwsresid + '-title-';
	this._dwsresdescid	= this._dwsresid + '-desc-';
	this._resfieldcleared	= false;
}

dwho.suggest.prototype.set_option = function(name,value)
{
	if(dwho_is_undef(this._options[name]) === true)
		return(false);

	switch(name)
	{
		case 'requestor':
		case 'result_onsetfield':
		case 'result_onclearfield':
		case 'result_callback':
		case 'result_displaycallback':
		case 'result_emptycallback':
			if(dwho_is_function(value) === false)
				return(false);
			break;
		case 'result_field':
			if(dwho_has_len(value) === true
			&& (resfield = dwho_eid(value,true)) !== false
			&& dwho_is_undef(resfield.value) === false)
			{
				this._resfield = resfield;
				break;
			}

			this._resfield = null;
			return(false);
		default:
			if(dwho_is_boolean(this._options[name]) === true)
			{
				if(dwho_is_boolean(value) === false)
					return(false);
				else
					break;
			}

			if(dwho_is_int(this._options[name]) === true)
			{
				if(dwho_is_int(value) === false)
					return(false);
				else
					break;
			}

			if(dwho_is_float(this._options[name]) === true)
			{
				if(dwho_is_float(value) === false)
					return(false);
				else
					break;
			}

			if(dwho_is_string(this._options[name]) === true)
			{
				if(dwho_is_string(value) === false)
					return(false);
				else
					break;
			}
	}

	this._options[name] = value;

	return(true);
}

dwho.suggest.prototype.set_options = function(options)
{
	if(dwho_is_object(options) === false)
		return(false);

	for(var property in options)
		this.set_option(property,options[property]);
}

dwho.suggest.prototype.get_option = function(name)
{
	if(dwho_is_undef(this._options[name]) === true)
		return(false);

	return(this._options[name]);
}

dwho.suggest.prototype.get_search_value = function()
{
	return(dwho_string(this._searchval));
}

dwho.suggest.prototype.set = function(request,value)
{
	if(this._field.value !== value)
		return(false);

	this._suggests = [];

	if(dwho_has_len(request.responseText) === true)
	{
		var jsondata = eval('(' + request.responseText + ')');
		var cnt = jsondata.length;

		for(var i = 0;i < cnt;i++)
			this._suggests.push({'id':	jsondata[i].id,
					     'value':	jsondata[i].identity,
					     'info':	jsondata[i].info});
	}

	this.display_result();
}

dwho.suggest.prototype.get = function(value)
{
	if(this._searchval === value)
		return(null);

	var prevlength = this._searchlen;
	this._searchval = value;
	this._searchlen = value.length;

	if(this._options.result_minlen > value.length)
	{
		this._suggests = [];
		this.clear();
		return(false);
	}

	var cnt = this._suggests.length;

	if(this._options.result_cache === true
	&& this._searchlen > prevlength
	&& cnt > 0
	&& cnt < this._options.result_maxentries)
	{
		var arr = [];

		for(var i = 0;i < cnt;i++)
		{
			if(this._options.result_fulldisplay === true
			&& dwho_has_len(this._suggests[i].info) === true)
				var suggest = this._suggests[i].value +
				' (' + this._suggests[i].info + ')';
			else
				var suggest = this._suggests[i].value;

			if(dwho_strcasecmp(suggest,value,value.length) === 0)
				arr.push(this._suggests[i]);
		}

		this._suggests = arr;
		this.display_result();
	}
	else
	{
		var dwsptr = this;

		if(this._delayid !== null)
		{
			window.clearTimeout(this._delayid);
			this._delayid = null;
		}

		if(this._options.request_delay > 0)
		{
			this._delayid = window.setTimeout(
						function() { dwsptr.get_option('requestor')(dwsptr); },
						this._options.request_delay);
		}
		else
			dwsptr.get_option('requestor')(dwsptr);
	}

	return(true);
}

dwho.suggest.prototype.clear = function()
{
	if(dwho_is_object(this._resfield) === true
	&& this._resfieldcleared === true)
	{
		if(this._options.result_fielddefaultvalue === true)
		{
			this._field.value = this._field.defaultValue;

			if(dwho_is_function(this._options.result_onclearfield) === true)
				this._options.result_onclearfield(this);
		}
		else if(dwho_has_len(this._field.value) === true)
		{
			this._field.value = '';

			if(dwho_is_function(this._options.result_onclearfield) === true)
				this._options.result_onclearfield(this);
		}
	}

	this._resfieldcleared = false;

	this.deletetimeout();
	this._highlightedid	= null;
	this._suggestid		= null;
	dwho.dom.remove_element(dwho_eid(this._dwsid,true));
}

dwho.suggest.prototype.display_result = function()
{
	if(this._options.result_displaycallback !== null)
	{
		this._options.result_displaycallback(this);
		return(true);
	}
	else if((cnt = this._suggests.length) === 0)
	{
		if(this._options.result_emptycallback !== null)
		{
			this._options.result_emptycallback(this);
			return(true);
		}

		this.clear();
		return(false);
	}

	this.deletetimeout();

	var dwsptr = this;

	var div = dwho.dom.create_element('div',
					  {'id': this._dwsid,
					   'className': this._options.result_class});

	var dl = dwho.dom.create_element('dl',{'id': this._dwsresid});

	for(var i = 0, j = 1;i < cnt;i++,j++)
	{
		var dtid = this._dwsrestitleid + j;
		var ddid = this._dwsresdescid + j;

		var dt = dwho.dom.create_element('dt',
						 {'id': dtid},
						 this._suggests[i].value);

		dt.onclick = function() { dwsptr.setselectedvalue(); return(false); }
		dt.onmouseover = function() { dwsptr.sethighlight(this.id); }
		dl.appendChild(dt);

		if(dwho_has_len(this._suggests[i].info) === true)
		{
			var dd = dwho.dom.create_element('dd',
							 {'id': ddid},
							 this._suggests[i].info);

			dd.onclick = function() { dwsptr.setselectedvalue(); return(false); }
			dd.onmouseover = function() { dwsptr.sethighlight(this.id); }
			dl.appendChild(dd);
		}
	}

	div.appendChild(dl);

	var pos = dwho.dom.get_offset_position(this._field);

	div.style.left	= pos.x + 'px';
	div.style.top	= (pos.y + this._field.offsetHeight + this._options.result_topoffset) + 'px';
	div.style.width	= this._field.offsetWidth + 'px';

	div.onmouseover	= function() { dwsptr.deletetimeout(); }
	div.onmouseout	= function() { dwsptr.starttimeout(); }

	dwho.dom.remove_element(dwho_eid(this._dwsid,true));

	if(dwho_is_object(this._field.parentNode) === false)
		document.getElementsByTagName('body')[0].appendChild(div);
	else
		this._field.parentNode.appendChild(div);

	this._field.focus();

	this._highlightedid	= null;
	this._suggestid		= null;

	this.starttimeout();

	return(true);
}

dwho.suggest.prototype.onkeypress = function(e)
{
	var key = dwho_is_undef(window.event) === false ? window.event.keyCode : e.keyCode;

	switch(key)
	{
		case this._vk.TAB:
			this.setselectedvalue();
			return(true);
		case this._vk.RETURN:
			this.setselectedvalue();

			if(dwho_is_function(e.preventDefault) === true)
				e.preventDefault();
			return(false);
		case this._vk.ESCAPE:
			this.clear();
			break;
	}

	return(true);
}

dwho.suggest.prototype.onkeyup = function(e)
{
	var key = dwho_is_undef(window.event) === false ? window.event.keyCode : e.keyCode;

	switch(key)
	{
		case this._vk.UP:
		case this._vk.DOWN:
			this.changehighlight(key);

			if(dwho_is_function(e.preventDefault) === true)
				e.preventDefault();
			return(false);
		default:
			if(dwho_is_object(this._resfield) === true
			&& this._resfield.value !== ''
			&& this._searchval !== this._field.value)
			{
				this._resfieldcleared = true;
				this._resfield.value = '';

				if(dwho_is_function(this._options.result_onsetfield) === true)
					this._options.result_onsetfield(this._resfield);
			}

			this.get(this._field.value);
	}

	return(true);
}

dwho.suggest.prototype.onblur = function()
{
	if(dwho_is_object(this._resfield) === true
	&& this._resfield.value === '')
	{
		this._resfieldcleared = true;

		if(dwho_is_function(this._options.result_onsetfield) === true)
			this._options.result_onsetfield(this._resfield);
	}

	this.starttimeout();
}

dwho.suggest.prototype.onchange = function()
{
	if(dwho_is_object(this._resfield) === true
	&& this._searchval !== this._field.value)
	{
		this._resfieldcleared = true;
		this._resfield.value = '';

		if(dwho_is_function(this._options.result_onsetfield) === true)
			this._options.result_onsetfield(this._resfield);

		this.clear();
	}
}

dwho.suggest.prototype.sethighlight = function(id)
{
	if(this._highlightedid !== null)
		this.resethighlight();

	if(dwho_strcmp(id,this._dwsrestitleid,this._dwsrestitleid.length) === 0)
	{
		if((title = dwho_eid(id,true)) === false)
			return(false);

		this._suggestid		= dwho_substr(id,this._dwsrestitleid.length - id.length);
		this._highlightedid	= id;
		title.className		= this._highlightclass;
		this.deletetimeout();

		if((desc = dwho_eid(this._dwsresdescid + this._suggestid,true)) === false)
			return(true);

		desc.className = this._highlightclass;

		return(true);
	}
	else if(dwho_strcmp(id,this._dwsresdescid,this._dwsresdescid.length) === 0)
	{
		if((desc = dwho_eid(id,true)) === false)
			return(false);

		this._suggestid		= dwho_substr(id,this._dwsresdescid.length - id.length);
		this._highlightedid	= id;
		desc.className		= this._highlightclass;
		this.deletetimeout();

		if((title = dwho_eid(this._dwsrestitleid + this._suggestid,true)) === false)
			return(false);

		title.className = this._highlightclass;

		return(true);
	}

	return(false);
}

dwho.suggest.prototype.changehighlight = function(key)
{
	switch(key)
	{
		case this._vk.DOWN:
			var suggestid = Number(this._suggestid) + 1;
			break;
		case this._vk.UP:
			var suggestid = Number(this._suggestid) - 1;
			break;
		default:
			return(false);
	}

	if(suggestid > this._suggests.length)
		suggestid = 1;

	if(suggestid < 1)
		suggestid = this._suggests.length;

	this.sethighlight(this._dwsrestitleid + suggestid);
}

dwho.suggest.prototype.resethighlight = function()
{
	if(this._highlightedid === null)
		return(null);
	else if(dwho_strcmp(this._highlightedid,this._dwsrestitleid,this._dwsrestitleid.length) === 0)
	{
		if((title = dwho_eid(this._highlightedid,true)) === false)
			return(false);

		title.className		= '';
		var suggestid		= dwho_substr(this._highlightedid,
						      this._dwsrestitleid.length - this._highlightedid.length);
		this._suggestid		= null;
		this._highlightedid	= null;

		if((desc = dwho_eid(this._dwsresdescid + suggestid,true)) === false)
			return(true);

		desc.className = '';

		return(true);
	}
	else if(dwho_strcmp(this._highlightedid,this._dwsresdescid,this._dwsresdescid.length) === 0)
	{
		if((desc = dwho_eid(this._highlightedid,true)) === false)
			return(false);

		desc.className		= '';
		var suggestid		= dwho_substr(this._highlightedid,
						      this._dwsresdescid.length - this._highlightedid.length);
		this._suggestid		= null;
		this._highlightedid	= null;

		if((title = dwho_eid(this._dwsrestitleid + suggestid,true)) === false)
			return(false);

		title.className = '';

		return(true);
	}
}

dwho.suggest.prototype.setselectedvalue = function()
{
	if(this._suggestid === null)
		return(null);

	if(this._options.result_fulldisplay === true
	&& dwho_has_len(this._suggests[this._suggestid - 1].info) === true)
		var value = this._suggests[this._suggestid - 1].value +
			' (' + this._suggests[this._suggestid - 1].info + ')';
	else
		var value = this._suggests[this._suggestid - 1].value;

	this._searchval = this._field.value = value;
	this._searchlen = value.length;

	if(dwho_is_object(this._resfield) === true)
	{
		this._resfieldcleared = false;
		this._resfield.value = this._suggests[this._suggestid - 1].id;

		if(dwho_is_function(this._options.result_onsetfield) === true)
			this._options.result_onsetfield(this._resfield);
	}

	this._field.focus();

	if(this._field.selectionStart > 0)
		this._field.setSelectionRange(this._searchval.length,this._searchval.length);

	this.clear();

	if(this._options.result_callback !== null)
		this._options.result_callback(this._suggests[this._suggestid - 1]);
}

dwho.suggest.prototype.starttimeout = function()
{
	this.deletetimeout();

	var dwsptr = this;
	this._timeoutid = window.setTimeout(function() { dwsptr.clear(); },2000);
}

dwho.suggest.prototype.deletetimeout = function()
{
	if(this._timeoutid !== null)
		window.clearTimeout(this._timeoutid);

	this._timeoutid = null;
}

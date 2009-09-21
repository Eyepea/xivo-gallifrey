/*
 * XiVO Web-Interface
 * Copyright (C) 2009  Proformatique <technique@proformatique.com>
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

if(typeof(xivo) === 'undefined')
	xivo = {'suggest': {}};
else if(xivo_is_undef(xivo.suggest) === true)
	xivo.suggest = {};

xivo.suggest = function(options,id)
{
	this._namespace		= '__xivo-suggest__';
	this._suggests		= [];
	this._field		= null;
	this._resfield		= null;
	this._resfieldcleared	= false;
	this._autocomplete	= null;
	this._suggestid		= null;
	this._highlightedid	= null;
	this._delayid		= null;
	this._timeoutid		= null;
	this._xsid		= null;
	this._xsresid		= null;
	this._xsrestitleid	= null;
	this._xsresdescid	= null;
	this._highlightclass	= 'xs-highlight';
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
		'result_fulldisplay':		false,
		'result_cache':			false,
		'result_class':			'xivo-suggest',
		'result_topoffset':		-10,
		'result_callback':		null,
		'result_displaycallback':	null,
		'result_emptycallback':		null};

	this.set_options(options);

	var xsptr = this;

	this.fnkeypress	= function(e) { return(xsptr.onkeypress(e)); }
	this.fnkeyup	= function(e) { return(xsptr.onkeyup(e)); }
	this.fnfocus	= function() { xsptr.deletetimeout(); }
	this.fnblur	= function() { xsptr.onblur(); }
	this.fnchange	= function() { xsptr.onchange(); }

	if(xivo_is_string(id) === true)
		this.set_field(id);
}

xivo.suggest.prototype.set_field = function(id)
{
	if(xivo_is_object(this._field) === true)
	{
		if(this._field.id === id)
			return(true);

		xivo.dom.remove_event('keypress',
				      this._field,
				      this.fnkeypress);

		xivo.dom.remove_event('keyup',
				      this._field,
				      this.fnkeyup);

		xivo.dom.remove_event('focus',
				      this._field,
				      this.fnfocus);

		xivo.dom.remove_event('blur',
				      this._field,
				      this.fnblur);

		xivo.dom.remove_event('change',
				      this._field,
				      this.fnchange);

		this._field.setAttribute('autocomplete',this._autocomplete);

		this.clear();
	}

	if((this._field = xivo_eid(id,true)) === false)
		return(false);


	xivo.dom.add_event('keypress',
			   this._field,
			   this.fnkeypress);

	xivo.dom.add_event('keyup',
			   this._field,
			   this.fnkeyup);

	xivo.dom.add_event('focus',
			   this._field,
			   this.fnfocus);

	xivo.dom.add_event('blur',
			   this._field,
			   this.fnblur);

	xivo.dom.add_event('change',
			   this._field,
			   this.fnchange);

	this._xsid		= this._namespace + this._field.id;
	this._xsresid		= this._xsid + '-res';
	this._xsrestitleid	= this._xsresid + '-title-';
	this._xsresdescid	= this._xsresid + '-desc-';
	this._autocomplete	= this._field.getAttribute('autocomplete');
	this._resfieldcleared	= false;

	this._field.setAttribute('autocomplete','off');
}

xivo.suggest.prototype.set_options = function(options)
{
	if(xivo_is_object(options) === false)
		return(false);

	for(var property in options)
		this.set_option(property,options[property]);
}

xivo.suggest.prototype.set_option = function(name,value)
{
	if(xivo_is_undef(this._options[name]) === true)
		return(false);

	switch(name)
	{
		case 'requestor':
		case 'result_callback':
		case 'result_displaycallback':
		case 'result_emptycallback':
			if(xivo_is_function(value) === false)
				return(false);
			break;
		case 'result_field':
			if(xivo_has_len(value) === true
			&& (resfield = xivo_eid(value,true)) !== false
			&& xivo_is_undef(resfield.value) === false)
			{
				this._resfield = resfield;
				break;
			}

			this._resfield = null;
			return(false);
		default:
			if(xivo_is_boolean(this._options[name]) === true)
			{
				if(xivo_is_boolean(value) === false)
					return(false);
				else
					break;
			}

			if(xivo_is_int(this._options[name]) === true)
			{
				if(xivo_is_int(value) === false)
					return(false);
				else
					break;
			}

			if(xivo_is_float(this._options[name]) === true)
			{
				if(xivo_is_float(value) === false)
					return(false);
				else
					break;
			}

			if(xivo_is_string(this._options[name]) === true)
			{
				if(xivo_is_string(value) === false)
					return(false);
				else
					break;
			}
	}

	this._options[name] = value;

	return(true);
}

xivo.suggest.prototype.get_option = function(name)
{
	if(xivo_is_undef(this._options[name]) === true)
		return(false);

	return(this._options[name]);
}

xivo.suggest.prototype.get_search_value = function()
{
	return(xivo_string(this._searchval));
}

xivo.suggest.prototype.set = function(request,value)
{
	if(this._field.value !== value)
		return(false);

	this._suggests = [];

	if(xivo_has_len(request.responseText) === true)
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

xivo.suggest.prototype.get = function(value)
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
			&& xivo_has_len(this._suggests[i].info) === true)
				var suggest = this._suggests[i].value +
				' (' + this._suggests[i].info + ')';
			else
				var suggest = this._suggests[i].value;

			if(xivo_strcasecmp(suggest,value,value.length) === 0)
				arr.push(this._suggests[i]);
		}

		this._suggests = arr;
		this.display_result();
	}
	else
	{
		var xsptr = this;

		if(this._delayid !== null)
		{
			window.clearTimeout(this._delayid);
			this._delayid = null;
		}

		if(this._options.request_delay > 0)
		{
			this._delayid = window.setTimeout(
						function() { xsptr.get_option('requestor')(xsptr); },
						this._options.request_delay);
		}
		else
			xsptr.get_option('requestor')(xsptr);
	}

	return(true);
}

xivo.suggest.prototype.clear = function()
{
	if(xivo_is_object(this._resfield) === true
	&& this._resfieldcleared === true)
		this._field.value = '';
	this._resfieldcleared = false;

	this.deletetimeout();
	this._highlightedid	= null;
	this._suggestid		= null;
	xivo.dom.remove_element(xivo_eid(this._xsid,true));
}

xivo.suggest.prototype.display_result = function()
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

	var xsptr = this;

	var div = xivo.dom.create_element('div',
					  {'id': this._xsid,
					   'className': this._options.result_class});

	var dl = xivo.dom.create_element('dl',{'id': this._xsresid});

	for(var i = 0, j = 1;i < cnt;i++,j++)
	{
		var dtid = this._xsrestitleid + j;
		var ddid = this._xsresdescid + j;

		var dt = xivo.dom.create_element('dt',
						 {'id': dtid},
						 this._suggests[i].value);

		dt.onclick = function() { xsptr.setselectedvalue(); return(false); }
		dt.onmouseover = function() { xsptr.sethighlight(this.id); }
		dl.appendChild(dt);

		if(xivo_has_len(this._suggests[i].info) === true)
		{
			var dd = xivo.dom.create_element('dd',
							 {'id': ddid},
							 this._suggests[i].info);

			dd.onclick = function() { xsptr.setselectedvalue(); return(false); }
			dd.onmouseover = function() { xsptr.sethighlight(this.id); }
			dl.appendChild(dd);
		}
	}

	div.appendChild(dl);

	var pos = xivo.dom.get_offset_position(this._field);

	div.style.left	= pos.x + 'px';
	div.style.top	= (pos.y + this._field.offsetHeight + this._options.result_topoffset) + 'px';
	div.style.width	= this._field.offsetWidth + 'px';

	div.onmouseover	= function() { xsptr.deletetimeout(); }
	div.onmouseout	= function() { xsptr.starttimeout(); }

	xivo.dom.remove_element(xivo_eid(this._xsid,true));
	if(xivo_is_object(this._field.parentNode) === false)
		document.getElementsByTagName('body')[0].appendChild(div);
	else
		this._field.parentNode.appendChild(div);

	this._field.focus();

	this._highlightedid	= null;
	this._suggestid		= null;

	this.starttimeout();

	return(true);
}

xivo.suggest.prototype.onkeypress = function(e)
{
	var key = xivo_is_undef(window.event) === false ? window.event.keyCode : e.keyCode;

	switch(key)
	{
		case this._vk.TAB:
			this.setselectedvalue();
			return(true);
		case this._vk.RETURN:
			this.setselectedvalue();

			if(xivo_is_function(e.preventDefault) === true)
				e.preventDefault();
			return(false);
		case this._vk.ESCAPE:
			this.clear();
			break;
	}

	return(true);
}

xivo.suggest.prototype.onkeyup = function(e)
{
	var key = xivo_is_undef(window.event) === false ? window.event.keyCode : e.keyCode;

	switch(key)
	{
		case this._vk.UP:
		case this._vk.DOWN:
			this.changehighlight(key);

			if(xivo_is_function(e.preventDefault) === true)
				e.preventDefault();
			return(false);
		default:
			if(xivo_is_object(this._resfield) === true
			&& this._resfield.value !== ''
			&& this._searchval !== this._field.value)
			{
				this._resfieldcleared = true;
				this._resfield.value = '';
			}

			this.get(this._field.value);
	}

	return(true);
}

xivo.suggest.prototype.onblur = function()
{
	if(xivo_is_object(this._resfield) === true
	&& this._resfield.value === '')
	{
		this._resfieldcleared = true;
		this._resfield.value = '';
	}

	this.starttimeout();
}

xivo.suggest.prototype.onchange = function()
{
	if(xivo_is_object(this._resfield) === true
	&& this._searchval !== this._field.value)
	{
		this._resfieldcleared = true;
		this._resfield.value = '';
		this.clear();
	}
}

xivo.suggest.prototype.sethighlight = function(id)
{
	if(this._highlightedid !== null)
		this.resethighlight();

	if(xivo_strcmp(id,this._xsrestitleid,this._xsrestitleid.length) === 0)
	{
		if((title = xivo_eid(id,true)) === false)
			return(false);

		this._suggestid		= xivo_substr(id,this._xsrestitleid.length - id.length);
		this._highlightedid	= id;
		title.className		= this._highlightclass;
		this.deletetimeout();

		if((desc = xivo_eid(this._xsresdescid + this._suggestid,true)) === false)
			return(true);

		desc.className = this._highlightclass;

		return(true);
	}
	else if(xivo_strcmp(id,this._xsresdescid,this._xsresdescid.length) === 0)
	{
		if((desc = xivo_eid(id,true)) === false)
			return(false);

		this._suggestid		= xivo_substr(id,this._xsresdescid.length - id.length);
		this._highlightedid	= id;
		desc.className		= this._highlightclass;
		this.deletetimeout();

		if((title = xivo_eid(this._xsrestitleid + this._suggestid,true)) === false)
			return(false);

		title.className = this._highlightclass;

		return(true);
	}

	return(false);
}

xivo.suggest.prototype.changehighlight = function(key)
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

	this.sethighlight(this._xsrestitleid + suggestid);
}

xivo.suggest.prototype.resethighlight = function()
{
	if(this._highlightedid === null)
		return(null);
	else if(xivo_strcmp(this._highlightedid,this._xsrestitleid,this._xsrestitleid.length) === 0)
	{
		if((title = xivo_eid(this._highlightedid,true)) === false)
			return(false);

		title.className		= '';
		var suggestid		= xivo_substr(this._highlightedid,
						      this._xsrestitleid.length - this._highlightedid.length);
		this._suggestid		= null;
		this._highlightedid	= null;

		if((desc = xivo_eid(this._xsresdescid + suggestid,true)) === false)
			return(true);

		desc.className = '';

		return(true);
	}
	else if(xivo_strcmp(this._highlightedid,this._xsresdescid,this._xsresdescid.length) === 0)
	{
		if((desc = xivo_eid(this._highlightedid,true)) === false)
			return(false);

		desc.className		= '';
		var suggestid		= xivo_substr(this._highlightedid,
						      this._xsresdescid.length - this._highlightedid.length);
		this._suggestid		= null;
		this._highlightedid	= null;

		if((title = xivo_eid(this._xsrestitleid + suggestid,true)) === false)
			return(false);

		title.className = '';

		return(true);
	}
}

xivo.suggest.prototype.setselectedvalue = function()
{
	if(this._suggestid === null)
		return(null);

	if(this._options.result_fulldisplay === true
	&& xivo_has_len(this._suggests[this._suggestid - 1].info) === true)
		var value = this._suggests[this._suggestid - 1].value +
			' (' + this._suggests[this._suggestid - 1].info + ')';
	else
		var value = this._suggests[this._suggestid - 1].value;

	this._searchval = this._field.value = value;
	this._searchlen = value.length;

	if(xivo_is_object(this._resfield) === true)
	{
		this._resfieldcleared = false;
		this._resfield.value = this._suggests[this._suggestid - 1].id;
	}

	this._field.focus();

	if(this._field.selectionStart > 0)
		this._field.setSelectionRange(this._searchval.length,this._searchval.length);

	this.clear();

	if(this._options.result_callback !== null)
		this._options.result_callback(this._suggests[this._suggestid - 1]);
}

xivo.suggest.prototype.starttimeout = function()
{
	this.deletetimeout();

	var xsptr = this;
	this._timeoutid = window.setTimeout(function() { xsptr.clear(); },2000);
}

xivo.suggest.prototype.deletetimeout = function()
{
	if(this._timeoutid !== null)
		window.clearTimeout(this._timeoutid);

	this._timeoutid = null;
}

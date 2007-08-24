var xivo_conf = new Array();
xivo_conf['attrib'] = new Array();
xivo_conf['eid'] = new Array();

var xivo_smenu = new Array();
xivo_smenu['click'] = new Array();
xivo_smenu['click']['id'] = '';
xivo_smenu['click']['class'] = '';
xivo_smenu['bak'] = new Array();
xivo_smenu['before'] = new Array();
xivo_smenu['before']['id'] = '';
xivo_smenu['before']['class'] = '';
xivo_smenu['display'] = '';
xivo_smenu['tab'] = 'smenu-tab-1';
xivo_smenu['class'] = 'moc';
xivo_smenu['part'] = 'sb-part-first';
xivo_smenu['last'] = false;

var xivo_dprog = new Array();
xivo_dprog['current'] = new Array();
xivo_dprog['bak'] = new Array();
xivo_dprog['display'] = new Array();

var xivo_tlist = new Array();

var xivo_fm = document.forms;
var xivo_winload = '';

function xivo_bwcheck()
{
	this.ver = navigator.appVersion;
	this.agent = navigator.userAgent;
	this.dom = document.getElementById ? 1 : 0;
	this.opera5 = (navigator.userAgent.indexOf('Opera') >-1 && document.getElementById) ? 1 : 0;
	this.ie5 = (this.ver.indexOf('MSIE 5') >-1 && this.dom && !this.opera5) ? 1 : 0;
	this.ie6 = (this.ver.indexOf('MSIE 6') >-1 && this.dom && !this.opera5) ? 1 : 0;
	this.ie4 = (document.all && !this.dom && !this.opera5) ? 1 : 0;
	this.ie = this.ie4 || this.ie5 || this.ie6;
	this.mac = this.agent.indexOf('Mac') > -1;
	this.ns6 = (this.dom && parseInt(this.ver) >= 5) ? 1 : 0; 
	this.ns4 = (document.layers && !this.dom) ? 1 : 0;
	this.bw = (this.ie6 || this.ie5 || this.ie4 || this.ns4 || this.ns6 || this.opera5);
	return(this);
}

var xivo_bw = xivo_bwcheck();

function xivo_open_center(url,name,width,height,param)
{
	var x=0;
	var y=0;
	var w=0;
	var h=0;

	if (bw.ie)
	{
		x = 0;
		y = 0;
		w = screen.width;
		h = screen.height;
	}
	else
	{
		x = window.screenX;
		y = window.screenY;
		w = window.outerWidth;
		h = window.outerHeight;
	}

	var cx = x;
	if(w > width)
		cx += Math.round((w - width) / 2);

	var cy = y;
	if(h > height)
		cy += Math.round((h - height) / 2);

	return open(url, name,'left=' + cx + 'px,top=' + cy + 'px,width=' + width + 'px,height=' + height +'px'+ param);
}

function xivo_str_repeat(str,len)
{
	var r = '';
	str = String(str);
	len = Number(len);

	if(len < 1)
		return(r);

	for(var i = 0;i < len;i++)
		r += str;

	return(r);
}

function xivo_eid(id)
{
	if(xivo_is_undef(xivo_conf['eid'][id]) == false)
		return(xivo_conf['eid'][id]);
	else if(xivo_bw.dom && (get = document.getElementById(id)))
	{
		xivo_conf['eid'][id] = get;
		return(get);
	}
	else
		return(false);
}

function xivo_etag(tag,obj,nb)
{
	if(xivo_is_string(tag) == false)
		return(false);

	if(xivo_is_undef(obj) == true)
		obj = document;
	
	if(xivo_is_object(obj) == false)
		return(false);

	if(xivo_is_undef(obj.getElementsByTagName) == true)
		return(false);

	if(xivo_is_undef(nb) == true)
		return(obj.getElementsByTagName(tag));

	if(xivo_is_uint(nb) == false)
		nb = 0;

	if(xivo_is_undef(obj.getElementsByTagName(tag)[nb]) == true)
		return(false);
		
	return(obj.getElementsByTagName(tag)[nb]);
}

function xivo_attrib_register(id,arr)
{
	if(xivo_is_undef(xivo_conf['attrib'][id]) == true)
		xivo_conf['attrib'][id] = arr;
}

function xivo_chg_style_attrib(elem,arr,type)
{
	type = type == 'undefined' ? 0 : type;

	if(typeof(arr) == 'string')
		var ref_style = arr;
	else if(xivo_is_array(arr) == true && xivo_is_undef(arr[type]) == false)
		var ref_style = arr[type];
	else
		return(false);

	try
	{
		var styles = '';

		if(xivo_is_undef(elem['style']) == false)
		{
			var astyles = xivo_split(ref_style,';');
			var astyle = new Array();

			for(i = 0;i < astyles.length;i++)
			{
				styles = astyles[i].replace(/\s/g,'');

				if(styles.length == 0)
					continue;

				astyle = xivo_split(styles,':');

				if(astyle.length == 1)
					continue;

				if((pos = astyle[0].search(/-/)) != -1)
				{
					var tmp = astyle[0];
					astyle[0]  = tmp.substring(0,pos);
					astyle[0] += tmp.substr(pos+1,1).toUpperCase();
					astyle[0] += tmp.substr(pos+2,(tmp.length - pos-2));
				}

				elem['style'][astyle[0]] = astyle[1];
			}
		}
		else if(xivo_is_undef(elem.style.setAttribute) == false)
		{
			var astyles = xivo_split(ref_style,';');
			var astyle = new Array();

			for(i = 0;i < astyles.length;i++)
			{
				styles = astyles[i].replace(/\s/g,'');

				if(styles.length == 0)
					continue;

				astyle = xivo_split(styles,':');

				if(astyle.length == 1)
					continue;

				if((pos = astyle[0].search(/-/)) != -1)
				{
					var tmp = astyle[0];
					astyle[0]  = tmp.substring(0,pos);
					astyle[0] += tmp.substr(pos+1,1).toUpperCase();
					astyle[0] += tmp.substr(pos+2,(tmp.length - pos-2));
				}

				elem.style.setAttribute(astyle[0],astyle[1]);
			}
		}
		else
		{
			styles = elem.style.cssText.replace(/\s/g,'');

			if(styles.charAt(styles.length-1) != ';')
				styles += ';'+ref_style;
			else
				styles += ref_style;

			elem.style.cssText = styles;
		}
	}
	catch(e) {}
}

function xivo_split(str,delimit)
{
	str = String(str);

	if(str.match(/\\/) == null || delimit.length != 1)
		return(str.split(delimit));

	var len = str.length;

	var norm = 0;
	var lit = 1;
	var strip = 2;

	var st = norm;
	var c = out = '';
	var r = new Array();

	for(i = 0;i < len;i++)
	{
		c = str.charAt(i);

		switch(st)
		{
			case norm:
				if(c == '\\')
					st = strip;
				else if(c == delimit)
				{
					r.push(out);
					out = '';
				}
				else
					out += c;
				break;
			case strip:
				if(c == delimit)
					out += c;
				else
					out += '\\'+c;
				st = norm;
				break;
		}
	}

	if(out.length != 0)
		r.push(out);

	return(r);
}

function xivo_chg_property_attrib(elem,arr,type)
{
	if(typeof(arr) == 'string')
		var aproperties = xivo_split(arr,';');
	else if(xivo_is_array(arr) == true && xivo_is_undef(arr[type]) == false)
		var aproperties = xivo_split(arr[type],';');
	else
		return(false);

	var properties = '';
	var nproperty = '';
	var aproperty = new Array();
	var len = aproperties.length;

	for(i = 0;i < len;i++)
	{
		properties = aproperties[i];

		if(properties.length == 0)
			continue;

		nproperty = xivo_split(properties,'|');

		if(nproperty.length > 2 || xivo_is_undef(elem[nproperty[0]]) == true)
			continue;

		if(nproperty[0] == 'style')
		{
			xivo_chg_style_attrib(elem,nproperty[1],type);
			continue;
		}

		aproperty = xivo_split(nproperty[1],':');
					
		if(xivo_is_undef(aproperty[1]) == false)
		{
			var vtype = aproperty[1].toLowerCase();
					
			switch(vtype)
			{
				case 'boolean':
					if(aproperty[0] == 'false')
						aproperty[0] = false;
					else
						aproperty[0] = Boolean(aproperty[0]);
					break;
				case 'number':
						aproperty[0] = Number(aproperty[0]);
					break;
				case 'string':
						aproperty[0] = String(aproperty[0]);
					break;
			}		
		}

		elem[nproperty[0]] = aproperty[0];
	}
}

function xivo_chg_attrib(name,id,type,link)
{
	link = link == 1 || link == true ? true : false;
	type = type == 'undefined' ? 0 : type;

	var i = 0;

	if(xivo_is_undef(xivo_conf['attrib'][name]) == true || xivo_is_undef(xivo_conf['attrib'][name][id]) == true)
		return(false);

	var ref_elem = xivo_conf['attrib'][name][id];

	if((get = xivo_eid(id)))
	{
		if(xivo_is_undef(ref_elem['switch']) == false && xivo_is_array(ref_elem['switch']) == true
		&& xivo_is_undef(ref_elem['switch'][type]) == false && xivo_is_undef(ref_elem['switch_var']) == false)
		{
			if(ref_elem['switch_var'] == ref_elem['switch'][type])
				type = ref_elem['switch_var'];

			ref_elem['switch_var'] = ref_elem['switch'][type];
		}
		
		if(xivo_is_undef(ref_elem['img']) == false && xivo_is_undef(ref_elem['img'][type]) == false)
			get.src = ref_elem['img'][type];

		if(xivo_is_undef(ref_elem['property']) == false)
			xivo_chg_property_attrib(get,ref_elem['property'],type);

		if(xivo_is_undef(ref_elem['style']) == false)
			xivo_chg_style_attrib(get,ref_elem['style'],type);
	}

	if(xivo_is_undef(ref_elem['link']) == false)
	{
		if(xivo_is_array(ref_elem['link']) == true && link == true)
		{
			var len = ref_elem['link'].length;

			for(i = 0;i < len; i++)
			{
				nlink = xivo_is_undef(ref_elem['link'][i][2]) == false ? ref_elem['link'][i][2] : 2;
				xivo_chg_attrib(name,ref_elem['link'][i][0],ref_elem['link'][i][1],nlink);
			}
		}
		else
			xivo_chg_attrib(name,ref_elem['link'],type,2);
	}
}

function xivo_is_string(s)
{
	var r = false;

	if(typeof(s) == 'string')
		r = true;

	return(r);
}

function xivo_is_array(a)
{
	return((a instanceof Array));
}

function xivo_is_object(o)
{
	var r = false;

	if(typeof(o) == 'object' && xivo_is_array(o) == false)
		r = true;

	return(r);
}

function xivo_is_undef(v)
{
	var r = false;

	if(typeof(v) == 'undefined')
		r = true;

	return(r);
}

function xivo_is_number(n)
{
	var r = false;

	if(xivo_is_undef(Number(n)) == false)
		r = true;

	return(r);
}

function xivo_is_int(i)
{
	var r = false;
	var y = parseInt(i);

	if(isNaN(y) == true)
		return(r);

	if(i == y && i.toString() == y.toString())
		r = true;

	return(r);
}

function xivo_is_uint(i)
{
	var r = false;

	if(xivo_is_int(i) == false)
		return(r);

	if(i >= 0)
		r = true;

	return(r);
}

function xivo_set_dprog(id,h,t,pid)
{
	h = Number(h);
	t = Number(t);

	if((get = xivo_eid(id)) == false
	|| xivo_is_undef(xivo_dprog['current'][id]) == true
	|| xivo_display_dprog(id) != 1)
		return(false);

	var mt = Number(t)*1000;
	var oh = Number(get.offsetHeight);
	var cmt = Math.round((mt * h) / oh);

	get.style.height = '0px';
	get.style.overflow = 'hidden';
	get.style.visibility = 'visible';
	get.style.position = 'static';

	if(pid != '')
		xivo_set_dprog_height(pid,h,t,oh,id);

	xivo_dprog['current'][id] = new Array();
	xivo_dprog['current'][id]['timer'] = window.setInterval('xivo_mk_dprog(\''+id+'\','+h+')',cmt);
	xivo_dprog['current'][id]['mheight'] = oh;
	xivo_dprog['current'][id]['cheight'] = 0;
}

function xivo_display_dprog(id)
{
	if(xivo_is_undef(xivo_dprog['display'][id]) == true || xivo_dprog['display'][id] == 2)
		xivo_dprog['display'][id] = 1;
	else
		xivo_dprog['display'][id] = 2;

	return(xivo_dprog['display'][id]);
}

function xivo_set_dprog_height(id,h,t,oh,cid)
{
	if((get = xivo_eid(id)) == false || xivo_is_undef(xivo_dprog['current'][id]) == true)
		return(false);

	var mt = Number(t)*1000;
	var oh = Number(get.offsetHeight + oh);
	var cmt = Math.round((mt * h) / oh);

	if(xivo_is_undef(xivo_dprog['bak'][id]) == true)
		xivo_dprog['bak'][id] = new Array();

	xivo_dprog['bak'][id][cid] = new Array();
	xivo_dprog['bak'][id][cid]['oh'] = get.offsetHeight;

	get.style.height = '0px';
	get.style.overflow = 'hidden';
	get.style.visibility = 'visible';
	get.style.position = 'static';

	xivo_dprog['current'][id] = new Array();

	xivo_dprog['current'][id]['timer'] = window.setInterval('xivo_mk_dprog(\''+id+'\','+h+')',cmt);
	xivo_dprog['current'][id]['mheight'] = oh;
	xivo_dprog['current'][id]['cheight'] = 0;
}

function xivo_mk_dprog(id,h)
{
	if((get = xivo_eid(id)) == false || xivo_is_undef(xivo_dprog['current'][id]) == true)
		return(false);

	if(xivo_dprog['current'][id]['cheight'] >= xivo_dprog['current'][id]['mheight'])
	{
		window.clearInterval(xivo_dprog['current'][id]['timer']);
		delete(xivo_dprog['current'][id]);
		return(null);
	}
		
	xivo_dprog['current'][id]['cheight'] += Number(h);
	get.style.height = xivo_dprog['current'][id]['cheight']+'px';
}

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
			if(arr[i] == 'input' && tag[j].type != 'text'
			&& tag[j].type != 'file' == true)
				continue;

			if(xivo_is_undef(tag[j].onfocus) == true)
				tag[j].onfocus = function() { this.className = xivo_fm_onfocus_class; }

			if(xivo_is_undef(tag[j].onblur) == true)
				tag[j].onblur = function() { this.className = xivo_fm_onblur_class; }
		}
	}

	return(true);
}

function xivo_fm_move_selected(from,to)
{
	if ((from = xivo_eid(from)) == false || (to = xivo_eid(to)) == false || from.type != 'select-multiple' || to.type != 'select-multiple')
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
	if((from = xivo_eid(from)) == false || (from.type != 'select-one' && from.type != 'select-multiple') == true)
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
	if((from = xivo_eid(from)) == false || (from.type != 'select-one' && from.type != 'select-multiple') == true)
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

function xivo_substr(str,beg,end)
{
	var r = '';

	var len = str.length;

	if(len == 0)
		return(r);

	beg = Number(beg);
	end = Number(end);

	if(beg < 0 || end < 0)
	{
		if(beg < 0)
			beg = len + beg;

		if(end < 0)
		{
			end = len + end;

			if(beg != 0 && str.substr(beg,len).length >= end)
				return(r);
		}

		r = str.substring(beg,end);
	}
	else
		r = str.substr(beg,end);

	return(r);
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

function xivo_clone(obj)
{
	if(typeof(obj) != 'object')
		return(false);

	var r = new obj.constructor();

  	for(var property in obj)
	{
    		if(typeof(obj[property]) == 'object')
			r[property] = xivo_clone(obj[property]);
		else
			r[property] = obj[property];
	}
	
	return(r);
}

function xivo_leadzero(n)
{
	if (n < 10)
		n = '0' + n;

	return(n);
}

function xivo_smenu_click(obj,cname,part,last)
{
	if(xivo_is_undef(obj.id) == true || obj.id == '' || xivo_smenu['click']['id'] == obj.id)
		return(false);

	last = Boolean(last) != false ? true : false;

	var disobj = click = before = '';
	var num = 0;
	var id = obj.id;

	if(xivo_smenu['display'] != '' && (disobj = xivo_eid(xivo_smenu['display'])) != false)
		disobj.style.display = 'none';

	if((disobj = xivo_eid(part)) != false)
	{
		xivo_smenu['display'] = part;
		disobj.style.display = 'block';
	}

	if(xivo_smenu['click']['id'] != '' && (click = xivo_eid(xivo_smenu['click']['id'])) != false)
		click.className = xivo_smenu['click']['class'];

	if(xivo_is_undef(xivo_smenu['bak'][id]) == false)
	{
		xivo_smenu['click']['id'] = id;
		xivo_smenu['click']['class'] = xivo_smenu['bak'][id];
	}

	if(xivo_smenu['before']['id'] != '' && (before = xivo_eid(xivo_smenu['before']['id'])) != false)
		before.className = xivo_smenu['before']['class'];

	var rs = id.match(/^([a-z0-9-_]*)(\d+)$/i);

	if(rs != null)
	{
		var vid = rs[1];
		var num = Number(rs[2]);
		var nid = vid+(num-1);
		var get = '';

		if(num > 1 && (get = xivo_eid(nid)) != false)
		{
			if(xivo_is_undef(xivo_smenu['bak'][nid]) == true)
				xivo_smenu['bak'][nid] = get.className;

			xivo_smenu['before']['id'] = nid;
			xivo_smenu['before']['class'] = get.className;
			get.className = cname+'-before';
		}
		else
			xivo_smenu['before']['id'] = xivo_smenu['before']['class'] = '';
	}

	obj.className = last == false ? cname : cname+'-last';
}

function xivo_smenu_fmsubmit(obj)
{
	if(xivo_is_object(obj) == false
	|| obj.nodeName.toLowerCase() != 'form'
	|| xivo_is_undef(obj['fm_smenu-tab']) == true
	|| xivo_is_undef(obj['fm_smenu-part']) == true
	|| xivo_smenu['click']['id'] == ''
	|| xivo_smenu['display'] == '')
		return(false);

	obj['fm_smenu-tab'].value = xivo_smenu['click']['id'];
	obj['fm_smenu-part'].value = xivo_smenu['display'];

	return(true);
}

function xivo_smenu_out(obj,cname,last)
{
	if(xivo_is_undef(obj.id) == true || obj.id == '' || xivo_smenu['click']['id'] == obj.id)
		return(false);

	var num = 0;
	var id = obj.id;

	last = Boolean(last) != false ? true : false;

	if(last == true)
	{
		obj.className = cname+'-last';
		return(true);
	}

	var rs = id.match(/^([a-z0-9-_]*)(\d+)$/i);

	if(rs != null)
	{
		var vid = rs[1];
		var num = Number(rs[2]);
		var nid = vid+(num+1);

		if(xivo_smenu['click']['id'] == nid)
		{
			obj.className = cname+'-before';
			return(true);
		}
	}

	obj.className = cname;

	return(true);
}

function xivo_smenu_over(obj,cname,last)
{
	if(xivo_is_undef(obj.id) == true || obj.id == '' || xivo_smenu['click']['id'] == obj.id)
		return(false);

	var num = 0;
	var id = obj.id;

	if(xivo_is_undef(xivo_smenu['bak'][id]) == true)
		xivo_smenu['bak'][id] = obj.className;

	last = Boolean(last) != false ? true : false;

	if(last == true)
	{
		obj.className = cname+'-last';
		return(true);
	}

	var rs = id.match(/^([a-z0-9-_]*)(\d+)$/i);

	if(rs != null)
	{
		var vid = rs[1];
		var num = Number(rs[2]);
		var nid = vid+(num+1);

		if(xivo_smenu['click']['id'] == nid)
		{
			obj.className = cname+'-before';
			return(true);
		}
	}

	obj.className = cname;

	return(true);
}

function xivo_table_list(name,obj,del)
{
	del = xivo_is_undef(del) == true || del == 0 ? 0 : 1;

	if(xivo_is_undef(xivo_tlist[name]) == true
	|| xivo_is_array(xivo_tlist[name]) == false)
	{
		xivo_tlist[name] = new Array();
		xivo_tlist[name]['cnt'] = 0;
		xivo_tlist[name]['node'] = '';
	}

	if(xivo_is_undef(xivo_tlist[name]['cnt']) == true)
		xivo_tlist[name]['cnt'] = 0;

	if(xivo_is_undef(xivo_tlist[name]['node']) == true)
		xivo_tlist[name]['node'] = '';

	var ref = xivo_tlist[name];

	if(ref['node'] === false)
		return(false);

	if(ref['node'] == ''
	&& (ref['node'] = xivo_etag('tr',xivo_eid('ex-'+name),0)) == false)
		return(false);

	if(del == 1)
	{
		if(xivo_is_object(obj) == false
		|| xivo_is_object(obj.parentNode) == false
		|| xivo_is_object(obj.parentNode.parentNode) == false
		|| xivo_is_object(obj.parentNode.parentNode.parentNode) == false)
			return(false);

		node = obj.parentNode.parentNode;
		node.parentNode.removeChild(node);

		ref['cnt']--;

		if(ref['cnt'] == 0)
			xivo_eid('no-'+name).style.display = 'table-row';
	}
	else
	{
		if(xivo_eid('ex-'+name) == false)
			return(false);

		ref['cnt']++;

		if(ref['cnt'] > 0)
			xivo_eid('no-'+name).style.display = 'none';

		var xivo_node_clone = ref['node'].cloneNode(true);

		xivo_fm_field_disabled(xivo_node_clone,false);

		xivo_eid(name).appendChild(xivo_node_clone);
	}

	return(true);
}

function xivo_menu_active()
{
	var xivo_menu_active = 'mn-'+xivo_api_path_info.replace(/\//g,'--');
	xivo_menu_active = xivo_menu_active.replace(/_/g,'-');

	if((xivo_menu_active = xivo_eid(xivo_menu_active)) != false)
		xivo_menu_active.className = 'mn-active';
}

function xivo_form_success(str)
{
	str = String(str);

	if(str == 'undefined' || str.length == 0 || xivo_eid('tooltips') == false)
		var property = 'innerHTML|&nbsp\\;';
	else
	{
		str = str.replace(/;/g,'\\;');
		str = str.replace(/\|/g,'\\|');
		str = str.replace(/:/g,'\\:');

		var property = 'className|c-green;innerHTML|'+str;
	}

	xivo_chg_property_attrib(xivo_eid('tooltips'),property);
}

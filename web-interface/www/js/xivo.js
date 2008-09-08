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

var xivo_tlist = new Array();
var xivo_winload = new Array();

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
	this.cookie = (typeof navigator.cookieEnabled !== 'undefined') ? navigator.cookieEnabled : 0;

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

function xivo_eid(id,forcereload)
{
	if(Boolean(forcereload) === false && xivo_is_undef(xivo_conf['eid'][id]) == false)
		return(xivo_conf['eid'][id]);
	else if(xivo_bw.dom && (get = document.getElementById(id)))
		return((xivo_conf['eid'][id] = get));

	return(false);
}

function xivo_etag(tag,obj,nb)
{
	if(xivo_is_string(tag) == false)
		return(false);
	else if(obj === null || xivo_is_undef(obj) == true)
		obj = document;

	if(xivo_is_object(obj) == false
	|| xivo_is_undef(obj.getElementsByTagName) == true)
		return(false);
	else if(xivo_is_undef(nb) == true)
		return(obj.getElementsByTagName(tag));
	else if(xivo_is_uint(nb) == false)
		nb = 0;

	if(xivo_is_undef(obj.getElementsByTagName(tag)[nb]) == true)
		return(false);
		
	return(obj.getElementsByTagName(tag)[nb]);
}

function xivo_ename(name,obj,nb)
{
	if(xivo_is_string(name) == false)
		return(false);
	else if(xivo_is_undef(obj) == true)
		obj = document;
	
	if(xivo_is_object(obj) == false
	|| xivo_is_undef(obj.getElementsByName) == true)
		return(false);
	else if(xivo_is_undef(nb) == true)
		return(obj.getElementsByName(name));
	else if(xivo_is_uint(nb) == false)
		nb = 0;

	if(xivo_is_undef(obj.getElementsByName(name)[nb]) == true)
		return(false);
		
	return(obj.getElementsByName(name)[nb]);
}

function xivo_attrib_register(id,arr)
{
	if(xivo_is_undef(xivo_conf['attrib'][id]) == true)
		xivo_conf['attrib'][id] = arr;
}

function xivo_chg_style_attrib(elem,obj,type)
{
	type = xivo_is_undef(type) === true ? 0 : type;

	if(xivo_is_array(obj) === true && xivo_is_object(obj[type]) === true)
		obj = obj[type];
	else if(xivo_is_object(obj) === false)
		return(false);
	else if(xivo_is_undef(elem['type']) === false && elem['type'] === 'hidden')
		return(null);

	var style = '';
	var styles = '';

	try
	{
		if(xivo_is_undef(elem['style']) === false)
			var styletype = 1;
		else if(xivo_is_undef(elem.style.setAttribute) === false)
			var styletype = 2;
		else
			var styletype = 3;

		for(property in obj)
		{
			style = obj[property];

			if(xivo_is_scalar(style) === false)
				continue;

			style = String(style);
			style = style.replace(/\s/g,'');

			if(style.length === 0)
				continue;
			else if(styletype !== 3 && (pos = property.search(/-/)) != -1)
			{
				tmp = property;
				property  = tmp.substring(0,pos);
				property += tmp.substr(pos+1,1).toUpperCase();
				property += tmp.substr(pos+2,(tmp.length - pos-2));
			}

			if(styletype === 1)
				elem['style'][property] = style;
			else if(styletype === 2)
				elem.style.setAttribute(property,style);
			else
				styles += property+':'+obj[style]+';';
		}

		if(styletype === 3 && styles.length > 2)
		{
			elemstyle = elem.style.cssText.replace(/\s/g,'');

			if(elemstyle.charAt(elemstyle.length-1) !== ';')
				elemstyle += ';';

			elem.style.cssText = elemstyle+styles;
		}
	}
	catch(e)
	{
		return(false);
	}

	return(true);
}

function xivo_trunc(str,nb,end,chr)
{
	var r = String(str);
	nb = Number(nb);
	end = String(end);
	chr = chr === null || xivo_is_undef(chr) === true ? ' ' : String(chr);
	
	if(nb < 1 || nb > r.length || (sub = xivo_substr(r,0,nb)) === '')
		return(r);

	r = sub;

	if(chr !== '' && (spos = sub.lastIndexOf(chr)) > -1)
		r = xivo_substr(r,0,spos);

	if(end.length > 0)
		r += end;

	return(r);
}

function xivo_htmlen(str,quote_style)
{
	var span = document.createElement('span');
	span.appendChild(document.createTextNode(str));

	return(xivo_htmlsc(span.innerHTML,quote_style));
}

function xivo_htmlsc(str,quote_style)
{
	str = String(str);
	quote_style = String(quote_style);

	str = str.replace(/</g,'&lt;');
	str = str.replace(/>/g,'&gt;');

	switch(quote_style.toUpperCase())
	{
		default:
			str = str.replace(/'/g,'&#039;');
		case '2':
		case 'ENT_COMPAT':
			str = str.replace(/"/g,'&quot;');
		case '0':
		case 'ENT_NOQUOTES':
	}

	return(str);
}

function xivo_split(str,delimit)
{
	str = String(str);

	if(str.match(/\\/) === null || delimit.length != 1)
		return(str.split(delimit));

	var len = str.length;

	var norm = 0;
	var lit = 1;
	var strip = 2;

	var st = norm;
	var c = out = '';
	var r = new Array();

	for(var i = 0;i < len;i++)
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

function xivo_chg_property_attrib(elem,obj,type)
{
	if(xivo_is_array(obj) === true && xivo_is_object(obj[type]) === true)
		obj = obj[type];
	else if(xivo_is_object(obj) === false)
		return(false);

	for(property in obj)
	{
		if(property === 'style')
			xivo_chg_style_attrib(elem,obj,type);
		else
			elem[property] = obj[property];
	}

	return(true);
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

			for(var i = 0;i < len; i++)
			{
				nlink = xivo_is_undef(ref_elem['link'][i][2]) == false ? ref_elem['link'][i][2] : 2;
				xivo_chg_attrib(name,ref_elem['link'][i][0],ref_elem['link'][i][1],nlink);
			}
		}
		else
			xivo_chg_attrib(name,ref_elem['link'],type,2);
	}
}

function xivo_bool(bool)
{
	if(bool === null)
		return(false);

	switch(typeof(bool))
	{
		case 'object':
			return(true);
		case 'undefined':
			return(false);
	}

	switch(bool.toString().toLowerCase())
	{
		case 'y':
		case '1':
		case 'on':
		case 'yes':
		case 'true':
			return(true);
		case 'n':
		case '0':
		case 'off':
		case 'no':
		case 'false':
			return(false);
	}

	return(Boolean(bool));
}

function xivo_is_string(s)
{
	return((typeof(s) === 'string'));
}

function xivo_is_array(a)
{
	return((a instanceof Array));
}

function xivo_is_object(o)
{
	return((typeof(o) === 'object' && xivo_is_array(o) === false));
}

function xivo_is_undef(v)
{
	return((typeof(v) === 'undefined'));
}

function xivo_is_number(n)
{
	return((xivo_is_undef(Number(n))));
}

function xivo_is_int(i)
{
	var y = parseInt(i);

	if(isNaN(y) === true)
		return(false);

	return((i == y && i.toString() == y.toString()));
}

function xivo_is_float(i)
{
	var y = parseFloat(i);

	if(isNaN(y) === true)
		return(false);

	return((i == y && i.toString() == y.toString()));
}

function xivo_is_uint(i)
{
	return(((xivo_is_int(i) === true && i >= 0)));
}

function xivo_is_ufloat(i)
{
	return(((xivo_is_float(i) === true && i >= 0)));
}

function xivo_is_scalar(val)
{
	switch(typeof(val))
	{
		case 'object':
		case 'undefined':
			return(false);
	}

	return(true);
}

function xivo_chk_ipv4_strict(value)
{
	if(xivo_is_string(value) === false
	|| (value === xivo_long2ip(xivo_ip2long(value))) === false)
		return(false);

	return(value);
}

function xivo_long2ip(value)
{
	if(xivo_is_int(value) === false || value > 0xFFFFFFFF)
		value = 0xFFFFFFFF;

	var r	= ((value >> 24) & 0xFF) + '.'
		+ ((value >> 16) & 0xFF) + '.'
		+ ((value >> 8) & 0xFF) + '.'
		+ ((value & 0xFF));

	return(r);
}

function xivo_ip2long(value)
{
	if(xivo_is_scalar(value) === false)
		return(false);

	var split = String(value).split('.');

	if((len = split.length) > 4)
		return(false);

	var rs = 0;

	for(var i = 0;i < len;i++)
	{
		if(split[i].match(/^(?:[1-9][0-9]*|0[0-7]*|0x[0-9A-F]*)$/i) === null)
			return(false);

		var lsn = parseInt(split[i]);

		if(len === 1)
		{
			if (lsn <= 0xFFFFFFFF)
				return(lsn | 0);
			else
				return(false);
		}
		else if(i < len-1)
			mul = 1 << 8;
		else
			mul = 1 << (8 * (4-i));

		if (lsn >= mul)
			return(false);

		rs = rs * mul + lsn;
	}

	return(rs | 0);
}

function xivo_chk_host(value)
{
	if(xivo_is_string(value) === false
	|| value.length < 4
	|| value.length > 255)
		return(false);
	
	if(value.match(/^[a-z0-9-]+(?:\.[a-z0-9-]+)*\.[a-z]{2,4}$/i) === null)
		return(false);

	return(value);
}

function xivo_chk_ipv4_subnet(value)
{
	if(xivo_is_string(value) === false
	|| (pos = value.indexOf('/')) < 7)
		return(false);

	var mask = xivo_substr(value,pos+1);
	var ip = xivo_substr(value,0,pos);

	if(xivo_chk_ipv4_netmask(mask) === false
	|| xivo_chk_ipv4_strict(ip) === false)
		return(false);

	return(true);
}

function xivo_chk_ipv4_netmask(nm)
{
	if(xivo_chk_ipv4_netmask_bit(nm) === false
	&& xivo_chk_ipv4_netmask_dotdec(nm) === false)
		return(false);

	return(true);
}

function xivo_chk_ipv4_netmask_bit(bit)
{
	if(xivo_is_uint(bit) === false
	|| bit > 32)
		return(false);

	return(true);
}

function xivo_chk_ipv4_netmask_dotdec(nm)
{
	if((nm = xivo_ip2long(nm)) === 0)
		return(0);
	else if(nm === -1)
		return(32);
	else if(nm === false
	|| (inv = (nm ^ 0xFFFFFFFF)) === 0
	|| (inv & (inv + 1)) !== 0)
		return(false);

	return((32 - parseInt(Math.log(inv + 1,2))));
}

function xivo_strcmp(str1,str2,len)
{
	if(xivo_is_scalar(str1) === false
	|| xivo_is_scalar(str2) === false)
		return(false);

	str1 = str1.toString();
	str2 = str2.toString();

	if(xivo_is_uint(len) === true)
		str1 = str1.substring(0,len);

 	if(str1 > str2)
		return(1);
	else if(str1 == str2)
		return(0);

	return(-1);
}

function xivo_strcasecmp(str1,str2,len)
{
	if(xivo_is_scalar(str1) === false
	|| xivo_is_scalar(str2) === false)
		return(false);

	str1 = str1.toString().toLowerCase();
	str2 = str2.toString().toLowerCase();
 
	if(xivo_is_uint(len) === true)
		str1 = str1.substring(0,len);

 	if(str1 > str2)
		return(1);
	else if(str1 == str2)
		return(0);

	return(-1);
}

function xivo_substr(str,beg,end)
{
	var r = '';

	if(xivo_is_scalar(str) == false)
		return(r);

	var len = str.length;

	if(len == 0 || isNaN(beg) == true)
		return(r);

	if(isNaN(end) == true)
		end = len;

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

		return(str.substring(beg,end));
	}

	return(str.substr(beg,end));
}

function xivo_clone(obj)
{
	if(xivo_is_array(obj) === true)
		var r = [];
	else if(xivo_is_object(obj) === true)
		var r = {};
	else
		return(obj);

	for (var property in obj) r[property] = xivo_clone(obj[property]);

	return(r);
}

function xivo_debug(obj)
{
	if(typeof(obj) != 'object')
		return(false);

	var r = '';

  	for(var property in obj)
	{
		if(property == 'selectionStart' || property == 'selectionEnd')
			continue;

    		if(typeof(obj[property]) != 'object')
		{
			if(typeof(obj[property]) != 'function')
				r += property+':'+obj[property]+'\n';
			else
				r += 'function::'+property+'\n';
		}
	}

	return(r);
}

function xivo_leadzero(n)
{
	if (n < 10)
		n = '0' + n;

	return(n);
}

function xivo_free_focus()
{
	xivo_etag('a',null,0).focus();

	return(false);
}

function xivo_smenu_click(obj,cname,part,last)
{
	if(xivo_is_undef(obj.id) == true || obj.id == '')
		return(false);

	last = Boolean(last) != false;

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

	xivo_free_focus();
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
	if((ul = xivo_etag('ul',obj,0)) !== false)
		ul.style.display = 'none';

	if(xivo_is_undef(obj.id) == true || obj.id == '' || xivo_smenu['click']['id'] == obj.id)
		return(false);

	var num = 0;
	var id = obj.id;

	last = Boolean(last) != false;

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
	if((ul = xivo_etag('ul',obj,0)) !== false)
		ul.style.display = 'block';

	if(xivo_is_undef(obj.id) == true || obj.id == '' || xivo_smenu['click']['id'] == obj.id)
		return(false);

	var num = 0;
	var id = obj.id;

	if(xivo_is_undef(xivo_smenu['bak'][id]) == true)
		xivo_smenu['bak'][id] = obj.className;

	last = Boolean(last) != false;

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

function xivo_table_list(name,obj,del,idcnt)
{
	del = xivo_is_undef(del) == true || del == 0 ? 0 : 1;
	idcnt = idcnt == true ? true : false;

	if(xivo_is_undef(xivo_tlist[name]) == true
	|| xivo_is_array(xivo_tlist[name]) == false)
	{
		xivo_tlist[name] = new Array();
		xivo_tlist[name]['cnt'] = 0;
		xivo_tlist[name]['node'] = '';
	}

	if(xivo_is_undef(xivo_tlist[name]['cnt']) == true)
		xivo_tlist[name]['cnt'] = 0;

	if(xivo_is_undef(xivo_tlist[name]['idcnt']) == true)
		xivo_tlist[name]['idcnt'] = xivo_tlist[name]['cnt'];

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

		if(idcnt == true)
			xivo_fm_field_id_counter(xivo_node_clone,++xivo_tlist[name]['idcnt']);

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
		var property = {innerHTML: '&nbsp;'};
	else
	{
		str = str.replace(/;/g,'\\;');
		str = str.replace(/\|/g,'\\|');
		str = str.replace(/:/g,'\\:');

		var property = {className: 'c-green', innerHTML: str};
	}

	xivo_chg_property_attrib(xivo_eid('tooltips'),property);
}

function xivo_get_parent_by_tagname(obj,tag)
{
	if(xivo_is_object(obj) == false
	|| xivo_is_string(tag) == false)
		return(false);

	tag = tag.toLowerCase();

	for(var i = 0;i < 10;i++)
	{
		if(xivo_is_undef(obj.parentNode) == true
		|| xivo_is_undef(obj.tagName) == true)
			return(false);

		obj = obj.parentNode;

		if(obj.tagName.toLowerCase() == tag)
			return(obj);
	}

	return(false);
}

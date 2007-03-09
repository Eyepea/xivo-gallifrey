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
var xivo_fm = document.forms;

function xivo_open_center(url,name,width,height,param)
{
	var x=0;
	var y=0;
	var w=0;
	var h=0;

	if (bw.ie)
	{
		//x = window.screenLeft;
		//y = window.screenTop;
		//w = window.document.body.offsetWidth;
		//h = window.document.body.offsetHeight;
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

var xivo_conf = new Array();
xivo_conf['attrib'] = new Array();
xivo_conf['eid'] = new Array();

var xivo_dprog = new Array();
xivo_dprog['current'] = new Array();
xivo_dprog['bak'] = new Array();
xivo_dprog['display'] = new Array();

function xivo_eid(id)
{
	if(typeof(xivo_conf['eid'][id]) != 'undefined')
		return(xivo_conf['eid'][id]);
	else if(xivo_bw.dom && (get = document.getElementById(id)))
	{
		xivo_conf['eid'][id] = get;
		return(get);
	}
	else
		return(false);
}

function xivo_attrib_constructor(id,arr)
{
	if(typeof(xivo_conf['attrib'][id]) == 'undefined')
		xivo_conf['attrib'][id] = arr;
}

function xivo_chg_attrib(name,id,type,link)
{
	link = link == 1 || link == true ? true : false;
	type = type == 'undefined' ? 0 : type;

	if(typeof(xivo_conf['attrib'][name]) != 'undefined' && typeof(xivo_conf['attrib'][name][id]) != 'undefined')
	{
		var ref_elem = xivo_conf['attrib'][name][id];

		if((get = xivo_eid(id)))
		{
			if(typeof(ref_elem['switch']) != 'undefined' && xivo_is_array(ref_elem['switch']) == true
			&& typeof(ref_elem['switch'][type]) != 'undefined' && typeof(ref_elem['switch_var']) != 'undefined')
			{

				if(ref_elem['switch_var'] == ref_elem['switch'][type])
					type = ref_elem['switch_var'];

				ref_elem['switch_var'] = ref_elem['switch'][type];
			}
		
			if(typeof(ref_elem['img']) != 'undefined' && typeof(ref_elem['img'][type]) != 'undefined')
			{
				get.src = ref_elem['img'][type];
			}

			do
			{
				if(typeof(ref_elem['property']) == 'undefined')
					break;

				if(typeof(ref_elem['property']) == 'string')
					var aproperties = ref_elem['property'].split(';');
				else if(xivo_is_array(ref_elem['property']) == true && typeof(ref_elem['property'][type]) != 'undefined')
					var aproperties = ref_elem['property'][type].split(';');
				else
					break;

				var properties = '';
				var aproperty = new Array();

				for(var i = 0;i < aproperties.length;i++)
				{
					properties = aproperties[i].replace(/\s/g,'');

					if(properties.length == 0)
						continue;

					aproperty = properties.split(':');
					
					if(aproperty.length == 1 || typeof(get[aproperty[0]]) == 'undefined')
						continue;

					if(typeof(aproperty[2]) != 'undefined')
					{
						var vtype = aproperty[2].toLowerCase();
						
						switch(vtype)
						{
							case 'boolean':
								if(aproperty[1] == 'false')
									aproperty[1] = false;
								else
									aproperty[1] = Boolean(aproperty[1]);
								break;
							case 'number':
								aproperty[1] = Number(aproperty[1]);
								break;
							case 'string':
								aproperty[1] = String(aproperty[1]);
								break;
						}		
					}

					get[aproperty[0]] = aproperty[1];
				}
			}
			while(false);

			do
			{
				if(typeof(ref_elem['style']) == 'undefined')
					break;

				if(typeof(ref_elem['style']) == 'string')
					var ref_style = ref_elem['style'];
				else if(xivo_is_array(ref_elem['style']) == true && typeof(ref_elem['style'][type]) != 'undefined')
					var ref_style = ref_elem['style'][type];
				else
					break;

				try
				{
					var styles = '';

					if(typeof(get['style']) != 'undefined')
					{
						var astyles = ref_style.split(';');
						var astyle = new Array();

						for(var i = 0;i < astyles.length;i++)
						{
							styles = astyles[i].replace(/\s/g,'');

							if(styles.length == 0)
								continue;

							astyle = styles.split(':');

							if(astyle.length == 1)
								continue;

							if((pos = astyle[0].search(/-/)) != -1)
							{
								var tmp = astyle[0];
								astyle[0]  = tmp.substring(0,pos);
								astyle[0] += tmp.substr(pos+1,1).toUpperCase();
								astyle[0] += tmp.substr(pos+2,(tmp.length - pos-2));
							}

							get['style'][astyle[0]] = astyle[1];
						}
					}
					else if(typeof(get.style.setAttribute) != 'undefined')
					{
						var astyles = ref_style.split(';');
						var astyle = new Array();

						for(var i = 0;i < astyles.length;i++)
						{
							styles = astyles[i].replace(/\s/g,'');

							if(styles.length == 0)
								continue;

							astyle = styles.split(':');

							if(astyle.length == 1)
								continue;

							if((pos = astyle[0].search(/-/)) != -1)
							{
								var tmp = astyle[0];
								astyle[0]  = tmp.substring(0,pos);
								astyle[0] += tmp.substr(pos+1,1).toUpperCase();
								astyle[0] += tmp.substr(pos+2,(tmp.length - pos-2));
							}

							get.style.setAttribute(astyle[0],astyle[1]);
						}
					}
					else
					{
						styles = get.style.cssText.replace(/\s/g,'');

						if(styles.charAt(styles.length-1) != ';')
							styles += ';'+ref_style;
						else
							styles += ref_style;

						get.style.cssText = styles;
					}
				}
				catch(e) {}
			}
			while(false);
		}

		if(typeof(ref_elem['link']) != 'undefined')
		{
			if(xivo_is_array(ref_elem['link']) == true && link == true)
			{
				for(var i = 0;i < ref_elem['link'].length; i++)
				{
					nlink = typeof(ref_elem['link'][i][2]) != 'undefined' ? ref_elem['link'][i][2] : 2;
					xivo_chg_attrib(name,ref_elem['link'][i][0],ref_elem['link'][i][1],nlink);
				}
			}
			else xivo_chg_attrib(name,ref_elem['link'],type,2);
		}
	}
}

function xivo_is_array(a)
{
	return((a instanceof Array));
}

function xivo_is_object(o)
{
	var r = false;

	if(typeof(o) == 'object' && xivo_is_array(a) == false)
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

function xivo_set_dprog(id,h,t,pid)
{
	h = Number(h);
	t = Number(t);

	if((get = xivo_eid(id)) != false && typeof(xivo_dprog['current'][id]) == 'undefined')
	{
		if(xivo_display_dprog(id) == 1)
		{
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
	}
}

function xivo_display_dprog(id)
{
	if(typeof(xivo_dprog['display'][id]) == 'undefined' || xivo_dprog['display'][id] == 2)
		xivo_dprog['display'][id] = 1;
	else
		xivo_dprog['display'][id] = 2;

	return(xivo_dprog['display'][id]);
}

function xivo_set_dprog_height(id,h,t,oh,cid)
{
	if((get = xivo_eid(id)) != false && typeof(xivo_dprog['current'][id]) == 'undefined')
	{
		var mt = Number(t)*1000;
		var oh = Number(get.offsetHeight + oh);
		var cmt = Math.round((mt * h) / oh);

		if(typeof(xivo_dprog['bak'][id]) == 'undefined')
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
}

function xivo_mk_dprog(id,h)
{
	if((get = xivo_eid(id)) != false && typeof(xivo_dprog['current'][id]) != 'undefined')
	{
		if(xivo_dprog['current'][id]['cheight'] >= xivo_dprog['current'][id]['mheight'])
		{
			window.clearInterval(xivo_dprog['current'][id]['timer']);
			delete(xivo_dprog['current'][id]);
			return(null);
		}
		
		xivo_dprog['current'][id]['cheight'] += Number(h);
		get.style.height = xivo_dprog['current'][id]['cheight']+'px';
	}
}

function xivo_fm_move_selected(from,to)
{
	if ((from = xivo_eid(from)) == false || (to = xivo_eid(to)) == false || from.type != 'select-multiple' || to.type != 'select-multiple')
		return(false);

	var len = from.options.length - 1;
	
	for(var i = len; i >= 0; i--)
	{
		if(from.options[i].selected == true)
		{
			to.options[to.options.length] = new Option(from.options[i].text,from.options[i].value);
			from.options[i] = null;	
		}
	}

	return(true);
}

function xivo_fm_copy_select(from,to)
{
	if ((from = xivo_eid(from)) == false || (to = xivo_eid(to)) == false)
		return(false);

	if(to.selectedIndex == -1 || typeof(to.options[to.selectedIndex]) == 'undefined')
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

function xivo_fm_new_opt_select(text,value)
{
	if(xivo_is_undef(value) == true)
		value = text;

	titi = xivo_eid('it-trunk-host');
	titi.options[titi.options.length] = new Option(text,value);
	tutu = xivo_eid('tutu');
	tutu.style.display = 'none';
	titi.style.display = 'inline';
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

	if(len < 2 || selected == -1 || typeof(from.options[selected]) == 'undefined')
		return(r);

	if(order == -1)
	{
		if(selected == len-1 || typeof(from.options[selected+1]) == 'undefined')
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
		if(selected == 0 || typeof(from.options[selected-1]) == 'undefined')
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

function xivo_fm_select_to_text()
{
	toto = xivo_eid('it-trunk-host');
	toto.style.display = 'none';
	tutu = xivo_eid('tutu');
	tutu.type = 'text';
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

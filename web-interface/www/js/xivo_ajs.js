var xivo_ajs_errors = new Array();

function xivo_ajs_load_script(url,urlparam,id,sess)
{
	if(xivo_is_undef(xivo_script_root) == true)
		return(xivo_ajs_set_error('Undefined variable: xivo_script_root'));
	else if((head = xivo_eid('t-head')) == false)
		return(xivo_ajs_set_error('Undefined element: t-head'));

	var setid = false;

	if(id != 'undefined' && id.length > 0)
	{
		if((obj = xivo_eid(id,true)) != false)
		{
			if(xivo_is_undef(obj.src) == true
			|| xivo_is_undef(obj.type) == true
			|| obj.type != 'text/javascript' == true)
				return(xivo_ajs_set_error('Unable to load script'));

			head.removeChild(obj);
		}

		setid = true;
	}

	var uri = xivo_script_root+url+'?'+new Date().getTime();

	if(xivo_type_object(urlparam) === true)
	{
		for(property in urlparam)
			uri += '&'+property+'='+urlparam[property];
	}

	if(Boolean(sess) === true)
		uri += '&'+xivo_sess_str;

	var script = document.createElement('script');
	script.type = 'text/javascript';
	script.src = uri;

	if(setid === true)
		script.id = id;

	return(head.appendChild(script));
}

function xivo_ajs_reload_script(id)
{
	if((head = xivo_eid('t-head')) == false
	|| (obj = xivo_eid(id)) == false
	|| xivo_is_undef(obj.src) == true
	|| xivo_is_undef(obj.type) == true
	|| obj.type != 'text/javascript')
		return(xivo_ajs_set_error('Unable to reload script'));

	if(xivo_eid(id,true) != false)
		head.removeChild(obj);

	var script = document.createElement('script');
	script.type = 'text/javascript';
	script.src = obj.src;
	script.id = obj.id;

	return(head.appendChild(script));
}

function xivo_ajs_set_error(error)
{
	xivo_ajs_errors.push(error);

	return(false);
}

function xivo_ajs_last_error()
{
	if(xivo_ajs_errors.length > 0)
		return(xivo_ajs_errors[xivo_ajs_errors.length-1]);

	return(null);
}

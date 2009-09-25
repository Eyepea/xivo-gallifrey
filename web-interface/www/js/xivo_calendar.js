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

function xivo_calendar_calc(d)
{
	if((d instanceof Date) === false)
		d = new Date();

	var r = {'slt':
			{'year':	d.getFullYear(),
			'month':	d.getMonth(),
			'realmonth':	d.getMonth() + 1,
			'day':		d.getDate()}};

	r['slt']['str'] = r['slt']['year'] + '-' +
			  dwho_leadzero(r['slt']['realmonth']) + '-' +
			  dwho_leadzero(r['slt']['day']);

	r['time'] = d.getTime();

	r['cal'] = new Array();

	d.setDate(1);
	var day = d.getDay();

	if(day === 0)
		day = -5;
	else
		day -= ((day - 1) * 2);

	d.setDate(day);

	var dateb = d.getDate();
	var time = d.getTime();

	for(i = 0;i < 42;i++)
	{
		d.setDate(dateb + i);

		r['cal'][i] = {
				'year':		d.getFullYear(),
				'month':	d.getMonth(),
				'realmonth':	d.getMonth() + 1,
				'day':		d.getDate()};

		r['cal'][i]['str'] = r['cal'][i]['year'] + '-' +
				     dwho_leadzero(r['cal'][i]['realmonth']) + '-' +
				     dwho_leadzero(r['cal'][i]['day']);

		d.setTime(time);
	}

	return(r);
}

function xivo_calendar_html(disid,inid,str)
{
	if(dwho_is_undef(xivo_date_month) === true
	|| dwho_is_undef(xivo_date_day) === true
	|| dwho_type_object(xivo_date_month) === false
	|| dwho_is_array(xivo_date_day) === false
	|| dwho_eid(disid) === false
	|| dwho_eid(inid) === false)
		return(false);

	var t = new Date();
	var d = new Date();
	var rstr = '';

	if(dwho_is_undef(str) === false && dwho_is_string(str) === true)
	{
		var result = str.match(/^(2[0-9]{3})(?:-(0?[1-9]|1[0-2])(?:-(0?[1-9]|1[0-9]|2[0-9]|3[0-1]))?)?$/);

		if(result !== null && dwho_is_undef(result[1]) === false && result[1] !== '')
		{
			d.setYear(result[1]);
			if(dwho_is_undef(result[2]) === false && result[2] !== '')
			{
				d.setMonth(result[2]-1);

				if(dwho_is_undef(result[3]) === false && result[3] !== '')
					d.setDate(result[3]);
				else
					d.setDate(1);
			}
			else
				d.setMonth(0);
		}

		if(d === 'Invalid Date')
			d = new Date();
		else if(result !== null && dwho_is_undef(result[3]) === false && result[3] !== '')
			rstr =	d.getFullYear() + '-' +
				dwho_leadzero(d.getMonth()+1) + '-' +
				dwho_leadzero(d.getDate());
		else
			rstr = '';
	}

	var arr = xivo_calendar_calc(d);
	var classname = '';
	var len = arr['cal'].length;

	var r = '<div class="clearboth xivo-calendar">';

	var prev = new Date();
	prev.setTime(arr['time']);

	if(prev.getMonth() === 0)
	{
		prev.setYear(prev.getFullYear()-1);
		prev.setMonth(11);
	}
	else
		prev.setMonth(prev.getMonth()-1);

	var next = new Date();
	next.setTime(arr['time']);

	if(next.getMonth() === 11)
	{
		next.setYear(next.getFullYear()+1);
		next.setMonth(0);
	}
	else
		next.setMonth(next.getMonth()+1);

	var valprev = prev.getFullYear()+'-'+dwho_leadzero(prev.getMonth()+1);
	var valnext = next.getFullYear()+'-'+dwho_leadzero(next.getMonth()+1);

	var onclick = 'xivo_calendar_prevnext(\''+disid+'\',\''+inid+'\',\''+valprev+'\'); return(false);';

	r += '<span class="cal-prev"><a href="#" onclick="'+onclick+'">&laquo;<\/a><\/span>';

	onclick = 'xivo_calendar_select(\''+disid+'\',\''+inid+'\',\''+arr['slt']['year']+'-'+dwho_leadzero(arr['slt']['realmonth'])+'\');';

	r += '<span class="cal-month-year"><a href="#" onclick="'+onclick+'">';
	r += xivo_date_month[arr['slt']['month']]+' '+arr['slt']['year'];
	r += '<\/a><\/span>';

	onclick = 'xivo_calendar_prevnext(\''+disid+'\',\''+inid+'\',\''+valnext+'\'); return(false);';

	r += '<span class="cal-next" onclick="'+onclick+'"><a href="#">&raquo;<\/a><\/span>';

	r += '<table cellspacing="0" cellpadding="0" border="0" class="clearboth">';

	r += '<tr>';
	r += '<th class="cal-day-0">'+xivo_date_day[1].charAt(0).toUpperCase()+'<\/th>';
	r += '<th class="cal-day-1">'+xivo_date_day[2].charAt(0).toUpperCase()+'<\/th>';
	r += '<th class="cal-day-2">'+xivo_date_day[3].charAt(0).toUpperCase()+'<\/th>';
	r += '<th class="cal-day-3">'+xivo_date_day[4].charAt(0).toUpperCase()+'<\/th>';
	r += '<th class="cal-day-4">'+xivo_date_day[5].charAt(0).toUpperCase()+'<\/th>';
	r += '<th class="cal-day-5">'+xivo_date_day[6].charAt(0).toUpperCase()+'<\/th>';
	r += '<th class="cal-day-6">'+xivo_date_day[0].charAt(0).toUpperCase()+'<\/th>';
	r += '<\/tr>';

	onmov = 'onmouseover="this.tmp = this.className; this.className = \'cal-day-focus\';"';
	onmoo = 'onmouseout="this.className = this.tmp;"';

	for(i = 0,mod = 0;i < len;i++,mod = i % 7)
	{
		if(arr['cal'][i]['month'] < arr['slt']['month'])
			classname = 'cal-prev-month';
		else if(arr['cal'][i]['month'] > arr['slt']['month'])
			classname = 'cal-next-month';
		else
			classname = 'cal-cur-month';

		classname += ' cal-day-'+mod;

		if(rstr === arr['cal'][i]['str'])
			classname += ' cal-day-slt';

		onclick = 'onclick="xivo_calendar_select(\''+disid+'\',\''+inid+'\',\''+arr['cal'][i]['str']+'\');"';

		if(mod === 0)
			r += '<tr>';

			r += '<td '+onmov+' '+onmoo+' class="'+classname+'" '+onclick+'>';
			r += arr['cal'][i]['day'];
			r += '<\/td>';

		if(mod === 6)
			r += '<\/tr>';
	}

	r += '<\/table><\/div>';

	return(r);
}

function xivo_calendar_select(disid,inid,str)
{
	if(dwho_is_undef(str) === false)
		dwho_eid(inid).value = str;

	dwho_eid(disid).style.display = 'none';
}


function xivo_calendar_prevnext(disid,inid,str)
{
	dwho_eid(disid).innerHTML = xivo_calendar_html(disid,inid,str);
	dwho_eid(disid).style.display = 'block';
}

function xivo_calendar_display(disid,inid,display)
{
	if((disobj = dwho_eid(disid)) === false
	|| (inobj = dwho_eid(inid)) === false
	|| dwho_is_undef(inobj.value) === true)
		return(false);

	if(dwho_is_undef(display) === true)
	{
		if(disobj.style.display === 'block')
			display = false;
		else
			display = true;
	}
	else
		display = Boolean(display);

	if(display === false)
		disobj.style.display = 'none';
	else
	{
		disobj.style.display = 'block';
		disobj.innerHTML = xivo_calendar_html(disid,inid,inobj.value);
	}

	return(true);
}

function xivo_calendar_body(disid,inid)
{
	if((body = dwho_eid('bc-body')) === false)
		return(false);

	if(dwho_is_undef(disid) === true
	|| dwho_is_undef(inid) === true)
		body.onclick = null;
	else
	{
		body.onclick = function ()
		{
			if(dwho_eid(disid).style.display === 'block')
				xivo_calendar_select(disid,inid);
		}
	}

	return(true);
}

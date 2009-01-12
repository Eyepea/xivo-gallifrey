/*
 * XiVO Web-Interface
 * Copyright (C) 2006, 2007, 2008  Proformatique <technique@proformatique.com>
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

xivo_menu_active();

var tooltips_display_enable = true;

function tooltips_display()
{
	if(tooltips_display_enable == false)
		return(false);
/*
	xivo_eid('tips-info-bindaddr').style.display = 'inline';
	xivo_eid('ancr-tooltips-bindaddr').focus();
*/
}

window.onload = function()
{
	if(xivo_winload.length > 0)
		eval(xivo_winload.join('\n'));

	xivo_fm_show_error();
	xivo_fm_onfocus_onblur();
/*
	if(xivo_eid('tooltips-bindaddr') == false && xivo_eid('lb-bindaddr') != false)
	{
		var nodechild = xivo_eid('lb-bindaddr').firstChild;

		var tchild = document.createElement('SPAN');
		tchild.id = 'tips-info-bindaddr';
		tchild.className = 'tooltips-info';
		tchild.textContent = '...';

		var achild = document.createElement('A');
		achild.id = 'ancr-tooltips-bindaddr';
		achild.name = 'ancr-tooltips-bindaddr';

		var nchild = document.createElement('SPAN');
		nchild.id = 'ah-tooltips-bindaddr';
		nchild.className = 'tooltips-over';
		nchild.textContent = nodechild.textContent;
		nodechild.textContent = '';

		nodechild.appendChild(tchild);
		nodechild.appendChild(achild);
		nodechild.appendChild(nchild);
	}

	xivo_eid('ah-tooltips-bindaddr').onmouseover = function ()
	{
		tooltips_display_enable = true;
		setTimeout('tooltips_display()',500);
	}

	xivo_eid('ah-tooltips-bindaddr').onclick = function ()
	{
		xivo_eid('tips-info-bindaddr').style.display = 'none';
	}

	xivo_eid('tips-info-bindaddr').onmouseover = function ()
	{
		this.style.display = 'inline';
	}

	xivo_eid('tips-info-bindaddr').onclick = function ()
	{
		return(false);
	}

	xivo_eid('ah-tooltips-bindaddr').onmouseout = function ()
	{
		tooltips_display_enable = false;
		xivo_eid('tips-info-bindaddr').style.display = 'none';
	}

	xivo_eid('tips-info-bindaddr').onmouseout = function ()
	{
		this.style.display = 'none';
	}
*/
}

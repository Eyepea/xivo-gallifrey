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

var xivo_fm_dcontext = {
	'fd-dcontext-custom':	{'style':	[{display: 'none'},{display: 'block'}],
				 'link':	'it-dcontext-custom'},
	'it-dcontext-custom':	{'property':	[{disabled: true},{disabled: false}]}};

xivo_attrib_register('fm_dcontext',xivo_fm_dcontext);

function xivo_cdr_onload()
{
	if((dcontext = xivo_eid('it-dcontext')) !== false)
		xivo_chg_attrib('fm_dcontext','fd-dcontext-custom',Number(dcontext.value === 'custom'));
}

xivo.dom.set_onload(xivo_cdr_onload);

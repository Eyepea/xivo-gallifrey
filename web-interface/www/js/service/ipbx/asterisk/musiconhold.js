/*
 * XiVO Web-Interface
 * Copyright (C) 2006-2010  Proformatique <technique@proformatique.com>
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

var xivo_fm_musiconhold = {
	'fd-application':	{'style':	[{display: 'none'},{display: 'block'}],
				 'link':	'it-application'},
	'it-application':	{'property':	[{disabled: true},{disabled: false}]}};

xivo_attrib_register('fm_musiconhold',xivo_fm_musiconhold);

function xivo_moh_onload()
{
	if((mode = dwho_eid('it-mode')) !== false)
		xivo_chg_attrib('fm_musiconhold','fd-application',Number(mode.value === 'custom'));
}

dwho.dom.set_onload(xivo_moh_onload);

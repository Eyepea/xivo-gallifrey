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

var xivo_fm_dcontext = new Array();

xivo_fm_dcontext['fd-dcontext-custom'] = new Array();
xivo_fm_dcontext['fd-dcontext-custom']['style'] = new Array({display: 'none'},{display: 'block'});
xivo_fm_dcontext['fd-dcontext-custom']['link'] = 'it-dcontext-custom';

xivo_fm_dcontext['it-dcontext-custom'] = new Array();
xivo_fm_dcontext['it-dcontext-custom']['property'] = new Array({disabled: true},{disabled: false});

xivo_attrib_register('fm_dcontext',xivo_fm_dcontext);

function xivo_cdr_onload()
{
	if((dcontext = xivo_eid('it-dcontext')) != false)
		xivo_chg_attrib('fm_dcontext','fd-dcontext-custom',Number(dcontext.value === 'custom'));
}

xivo_winload.push('xivo_cdr_onload();');

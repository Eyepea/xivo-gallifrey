/*
 * XiVO Web-Interface
 * Copyright (C) 2009-2010  Proformatique <technique@proformatique.com>
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

var xivo_fm_method = {
	'fd-address':	{'style':	[{display: 'none'},{display: 'block'}],
			 'link':	'it-address'},
	'it-address':	{'property':	[{disabled: true},{disabled: false}],
			 'link':	'fd-netmask'},
	'fd-netmask':	{'style':	[{display: 'none'},{display: 'block'}],
			 'link':	'it-netmask'},
	'it-netmask':	{'property':	[{disabled: true},{disabled: false}],
			 'link':	'fd-broadcast'},
	'fd-broadcast':	{'style':	[{display: 'none'},{display: 'block'}],
			 'link':	'it-broadcast'},
	'it-broadcast':	{'property':	[{disabled: true},{disabled: false}],
			 'link':	'fd-gateway'},
	'fd-gateway':	{'style':	[{display: 'none'},{display: 'block'}],
			 'link':	'it-gateway'},
	'it-gateway':	{'property':	[{disabled: true},{disabled: false}],
			 'link':	'fd-mtu'},
	'fd-mtu':	{'style':	[{display: 'none'},{display: 'block'}],
			 'link':	'it-mtu'},
	'it-mtu':	{'property':	[{disabled: true},{disabled: false}]}};

xivo_attrib_register('fm_method',xivo_fm_method);

var xivo_fm_vlanrawdevice = {
	'it-vlanid':	{'property':	[{disabled: true,className: 'it-disabled'},
					 {disabled: false,className: 'it-enabled'}]}};

xivo_attrib_register('fm_vlanrawdevice',xivo_fm_vlanrawdevice);

function xivo_network_chg_method()
{
	if((method = dwho_eid('it-method')) !== false)
		xivo_chg_attrib('fm_method','fd-address',Number(method.value === 'static'));
}

function xivo_network_chg_vlanrawdevice()
{
	if((vlanrawdevice = dwho_eid('it-vlanrawdevice')) !== false)
		xivo_chg_attrib('fm_vlanrawdevice','it-vlanid',Number(vlanrawdevice.value !== ''));
}

function xivo_network_onload()
{
	xivo_network_chg_method();
	xivo_network_chg_vlanrawdevice();

	dwho.dom.add_event('change',
			   dwho_eid('it-method'),
			   xivo_network_chg_method);
	
	dwho.dom.add_event('change',
			   dwho_eid('it-vlanrawdevice'),
			   xivo_network_chg_vlanrawdevice);
}

dwho.dom.set_onload(xivo_network_onload);

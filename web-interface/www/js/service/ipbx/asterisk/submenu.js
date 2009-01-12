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

function xivo_submenu_onload()
{
	if(xivo_eid(xivo_smenu['tab']) == false)
		return(false);

	xivo_smenu['bak'][xivo_smenu['tab']] = xivo_eid(xivo_smenu['tab']).className;
	xivo_smenu_click(xivo_eid(xivo_smenu['tab']),xivo_smenu['class'],xivo_smenu['part'],xivo_smenu['last']);
}

xivo_winload.push('xivo_submenu_onload();');

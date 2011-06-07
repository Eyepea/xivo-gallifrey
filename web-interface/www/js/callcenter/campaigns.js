/*
 * XiVO Web-Interface
 * Copyright (C) 2010  Proformatique <technique@proformatique.com>
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

function xivo_ast_campaign_chg_occasional(evt)
{
	end = dwho_eid('it-campaign-end');

	end.setAttribute('class', 'it-' + (evt.target.checked?'enabled':'disabled'));
	end.disabled = !evt.target.checked;
}

function xivo_ast_campaign_onload()
{
	if((occasional = dwho_eid('it-campaign_occasional')) !== false)
	{
		dwho.dom.add_event('change',occasional,xivo_ast_campaign_chg_occasional);
		
		if(!occasional.checked)
		{
			end = dwho_eid('it-campaign-end');
			end.setAttribute('class', 'it-disabled');
			end.disabled = true;
		}
	}
}

dwho.dom.set_onload(xivo_ast_campaign_onload);

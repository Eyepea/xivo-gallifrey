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

var xivo_list_elt_extenfeatures = new Array();
xivo_list_elt_extenfeatures['it-extenfeatures-enable-recsnd'] = new Array('it-extenfeatures-recsnd');
xivo_list_elt_extenfeatures['it-extenfeatures-enable-phonestatus'] = new Array('it-extenfeatures-phonestatus');
xivo_list_elt_extenfeatures['it-extenfeatures-enable-enablednd'] = new Array('it-extenfeatures-enablednd');
xivo_list_elt_extenfeatures['it-extenfeatures-enable-incallrec'] = new Array('it-extenfeatures-incallrec');
xivo_list_elt_extenfeatures['it-extenfeatures-enable-incallfilter'] = new Array('it-extenfeatures-incallfilter');
xivo_list_elt_extenfeatures['it-extenfeatures-enable-pickup'] = new Array('it-extenfeatures-pickup','it-extenfeatures-list-pickup');
xivo_list_elt_extenfeatures['it-extenfeatures-enable-calllistening'] = new Array('it-extenfeatures-calllistening');
xivo_list_elt_extenfeatures['it-extenfeatures-enable-directoryaccess'] = new Array('it-extenfeatures-directoryaccess');
xivo_list_elt_extenfeatures['it-extenfeatures-enable-bsfilter'] = new Array('it-extenfeatures-bsfilter','it-extenfeatures-list-bsfilter');
xivo_list_elt_extenfeatures['it-extenfeatures-enable-guestprov'] = new Array('it-extenfeatures-guestprov');
xivo_list_elt_extenfeatures['it-extenfeatures-enable-enablevm'] = new Array('it-extenfeatures-enablevm');
xivo_list_elt_extenfeatures['it-extenfeatures-enable-voicemsg'] = new Array('it-extenfeatures-voicemsg');
xivo_list_elt_extenfeatures['it-extenfeatures-enable-vmdelete'] = new Array('it-extenfeatures-vmdelete');
xivo_list_elt_extenfeatures['it-extenfeatures-enable-fwdundoall'] = new Array('it-extenfeatures-fwdundoall');
xivo_list_elt_extenfeatures['it-extenfeatures-enable-fwdundorna'] = new Array('it-extenfeatures-fwdundorna');
xivo_list_elt_extenfeatures['it-extenfeatures-enable-fwdundobusy'] = new Array('it-extenfeatures-fwdundobusy');
xivo_list_elt_extenfeatures['it-extenfeatures-enable-fwdundounc'] = new Array('it-extenfeatures-fwdundounc');
xivo_list_elt_extenfeatures['it-extenfeatures-enable-fwdrna'] = new Array('it-extenfeatures-fwdrna','it-extenfeatures-list-fwdrna');
xivo_list_elt_extenfeatures['it-extenfeatures-enable-fwdbusy'] = new Array('it-extenfeatures-fwdbusy','it-extenfeatures-list-fwdbusy');
xivo_list_elt_extenfeatures['it-extenfeatures-enable-fwdunc'] = new Array('it-extenfeatures-fwdunc','it-extenfeatures-list-fwdunc');
xivo_list_elt_extenfeatures['it-extenfeatures-enable-agentstaticlogin'] = new Array('it-extenfeatures-agentstaticlogin','it-extenfeatures-list-agentstaticlogin');
xivo_list_elt_extenfeatures['it-extenfeatures-enable-agentstaticlogoff'] = new Array('it-extenfeatures-agentstaticlogoff','it-extenfeatures-list-agentstaticlogoff');
xivo_list_elt_extenfeatures['it-extenfeatures-enable-agentdynamiclogin'] = new Array('it-extenfeatures-agentdynamiclogin');

function xivo_extenfeatures_onload()
{
	for(property in xivo_list_elt_extenfeatures)
	{
		if(xivo_eid(property) == false)
			continue;

		xivo_fm_readonly(xivo_list_elt_extenfeatures[property],xivo_eid(property).checked);
		xivo_eid(property).onchange = function () { xivo_fm_readonly(xivo_list_elt_extenfeatures[this.id],this.checked); }
	}

	if((elt = xivo_eid('it-extenfeatures-pickup')) != false
	&& (eltlist = xivo_eid('it-extenfeatures-list-pickup')) != false)
		eltlist.value = xivo_get_exten_buffer('X',elt.value);

	if((elt = xivo_eid('it-extenfeatures-bsfilter')) != false
	&& (eltlist = xivo_eid('it-extenfeatures-list-bsfilter')) != false)
		eltlist.value = xivo_get_exten_buffer('X',elt.value);

	if((elt = xivo_eid('it-extenfeatures-fwdrna')) != false
	&& (eltlist = xivo_eid('it-extenfeatures-list-fwdrna')) != false)
		eltlist.value = xivo_get_exten_buffer('X',elt.value);

	if((elt = xivo_eid('it-extenfeatures-fwdbusy')) != false
	&& (eltlist = xivo_eid('it-extenfeatures-list-fwdbusy')) != false)
		eltlist.value = xivo_get_exten_buffer('X',elt.value);

	if((elt = xivo_eid('it-extenfeatures-fwdunc')) != false
	&& (eltlist = xivo_eid('it-extenfeatures-list-fwdunc')) != false)
		eltlist.value = xivo_get_exten_buffer('X',elt.value);

	if((elt = xivo_eid('it-extenfeatures-agentstaticlogin')) != false
	&& (eltlist = xivo_eid('it-extenfeatures-list-agentstaticlogin')) != false)
		eltlist.value = xivo_get_exten_buffer('X',elt.value);

	if((elt = xivo_eid('it-extenfeatures-agentstaticlogoff')) != false
	&& (eltlist = xivo_eid('it-extenfeatures-list-agentstaticlogoff')) != false)
		eltlist.value = xivo_get_exten_buffer('X',elt.value);
}

xivo_winload.push('xivo_extenfeatures_onload();');

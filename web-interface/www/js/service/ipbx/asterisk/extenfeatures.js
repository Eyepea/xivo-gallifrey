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

var xivo_list_elt_extenfeatures = {
	'it-extenfeatures-enable-recsnd': ['it-extenfeatures-recsnd'],
	'it-extenfeatures-enable-phonestatus': ['it-extenfeatures-phonestatus'],
	'it-extenfeatures-enable-enablednd': ['it-extenfeatures-enablednd'],
	'it-extenfeatures-enable-incallrec': ['it-extenfeatures-incallrec'],
	'it-extenfeatures-enable-incallfilter': ['it-extenfeatures-incallfilter'],
	'it-extenfeatures-enable-pickup': ['it-extenfeatures-pickup',
					   'it-extenfeatures-list-pickup'],
	'it-extenfeatures-enable-calllistening': ['it-extenfeatures-calllistening'],
	'it-extenfeatures-enable-directoryaccess': ['it-extenfeatures-directoryaccess'],
	'it-extenfeatures-enable-bsfilter': ['it-extenfeatures-bsfilter',
					     'it-extenfeatures-list-bsfilter'],
	'it-extenfeatures-enable-guestprov': ['it-extenfeatures-guestprov'],
	'it-extenfeatures-enable-enablevm': ['it-extenfeatures-enablevm'],
	'it-extenfeatures-enable-enablevmslt': ['it-extenfeatures-enablevmslt',
						'it-extenfeatures-list-enablevmslt'],
	'it-extenfeatures-enable-enablevmbox': ['it-extenfeatures-enablevmbox'],
	'it-extenfeatures-enable-enablevmboxslt': ['it-extenfeatures-enablevmboxslt',
						   'it-extenfeatures-list-enablevmboxslt'],
	'it-extenfeatures-enable-vmusermsg': ['it-extenfeatures-vmusermsg'],
	'it-extenfeatures-enable-vmboxmsgslt': ['it-extenfeatures-vmboxmsgslt',
						'it-extenfeatures-list-vmboxmsgslt'],
	'it-extenfeatures-enable-vmuserslt': ['it-extenfeatures-vmuserslt',
					      'it-extenfeatures-list-vmuserslt'],
	'it-extenfeatures-enable-vmboxslt': ['it-extenfeatures-vmboxslt',
					     'it-extenfeatures-list-vmboxslt'],
	'it-extenfeatures-enable-vmuserpurge': ['it-extenfeatures-vmuserpurge'],
	'it-extenfeatures-enable-vmuserpurgeslt': ['it-extenfeatures-vmuserpurgeslt',
						   'it-extenfeatures-list-vmuserpurgeslt'],
	'it-extenfeatures-enable-vmboxpurgeslt': ['it-extenfeatures-vmboxpurgeslt',
						   'it-extenfeatures-list-vmboxpurgeslt'],
	'it-extenfeatures-enable-fwdundoall': ['it-extenfeatures-fwdundoall'],
	'it-extenfeatures-enable-fwdundorna': ['it-extenfeatures-fwdundorna'],
	'it-extenfeatures-enable-fwdundobusy': ['it-extenfeatures-fwdundobusy'],
	'it-extenfeatures-enable-fwdundounc': ['it-extenfeatures-fwdundounc'],
	'it-extenfeatures-enable-fwdrna': ['it-extenfeatures-fwdrna',
					   'it-extenfeatures-list-fwdrna'],
	'it-extenfeatures-enable-fwdbusy': ['it-extenfeatures-fwdbusy',
					    'it-extenfeatures-list-fwdbusy'],
	'it-extenfeatures-enable-fwdunc': ['it-extenfeatures-fwdunc',
					   'it-extenfeatures-list-fwdunc'],
	'it-extenfeatures-enable-agentstaticlogin': ['it-extenfeatures-agentstaticlogin',
						     'it-extenfeatures-list-agentstaticlogin'],
	'it-extenfeatures-enable-agentstaticlogoff': ['it-extenfeatures-agentstaticlogoff',
						      'it-extenfeatures-list-agentstaticlogoff'],
	'it-extenfeatures-enable-agentdynamiclogin': ['it-extenfeatures-agentdynamiclogin',
						      'it-extenfeatures-list-agentdynamiclogin']}

function xivo_extenfeatures_onload()
{
	for(property in xivo_list_elt_extenfeatures)
	{
		if(xivo_eid(property) === false)
			continue;

		xivo_fm_readonly(xivo_list_elt_extenfeatures[property],xivo_eid(property).checked);
		xivo_eid(property).onchange = function () { xivo_fm_readonly(xivo_list_elt_extenfeatures[this.id],this.checked); }

		if(xivo_list_elt_extenfeatures[property].length === 2
		&& (elt = xivo_eid(xivo_list_elt_extenfeatures[property][0])) !== false
		&& (eltlist = xivo_eid(xivo_list_elt_extenfeatures[property][1])) !== false)
			eltlist.value = xivo_get_exten_buffer('X',elt.value);
	}
}

xivo_winload.push('xivo_extenfeatures_onload();');

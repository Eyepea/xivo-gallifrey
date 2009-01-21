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

xivo_fm_format = new Array();
xivo_fm_format['it-voicemail-attachformat'] = new Array();
xivo_fm_format['it-voicemail-attachformat']['property'] = new Array({disabled: false, className: 'it-enabled'},
								    {disabled: true, className: 'it-disabled'});

xivo_attrib_register('fm_format',xivo_fm_format);

function xivo_informat()
{
	xivo_fm_move_selected('it-voicemail-formatlist','it-voicemail-format');
	xivo_fm_copy_select('it-voicemail-format','it-voicemail-attachformat');

	if(xivo_is_undef('it-voicemail-attachformat') == true)
		return(false);
	else if(xivo_eid('it-voicemail-attachformat').length == 0)
		xivo_chg_attrib('fm_format','it-voicemail-attachformat',1);
	else
		xivo_chg_attrib('fm_format','it-voicemail-attachformat',0);

	return(true);
}

function xivo_outformat()
{
	xivo_fm_move_selected('it-voicemail-format','it-voicemail-formatlist');
	xivo_fm_copy_select('it-voicemail-format','it-voicemail-attachformat');

	if(xivo_is_undef('it-voicemail-attachformat') == true)
		return(false);
	else if(xivo_eid('it-voicemail-attachformat').length == 0)
		xivo_chg_attrib('fm_format','it-voicemail-attachformat',1);
	else
		xivo_chg_attrib('fm_format','it-voicemail-attachformat',0);

	return(true);
}

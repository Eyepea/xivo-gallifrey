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

xivo_fm_voicemail_format = {
		'it-voicemail-attachformat':
			{property:	[{disabled: false, className: 'it-enabled'},
					 {disabled: true, className: 'it-disabled'}]}};

xivo_attrib_register('fm_voicemail_format',xivo_fm_voicemail_format);

function xivo_voicemail_format(action)
{
	if(action === 'out')
	{
		dwho.form.move_selected('it-voicemail-format','it-voicemail-formatlist');
		dwho.form.copy_select('it-voicemail-format','it-voicemail-attachformat');
	}
	else
	{
		dwho.form.move_selected('it-voicemail-formatlist','it-voicemail-format');
		dwho.form.copy_select('it-voicemail-format','it-voicemail-attachformat');
	}

	if(dwho_eid('it-voicemail-attachformat') === false)
		return(false);

	xivo_chg_attrib('fm_voicemail_format',
			'it-voicemail-attachformat',
			Number((dwho_eid('it-voicemail-attachformat').length === 0)));

	return(true);
}

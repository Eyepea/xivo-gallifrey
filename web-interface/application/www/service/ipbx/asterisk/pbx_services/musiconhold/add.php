<?php

#
# XiVO Web-Interface
# Copyright (C) 2006-2009  Proformatique <technique@proformatique.com>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#

$fm_save = null;

do
{
	if(isset($_QR['fm_send']) === false)
		break;

	unset($_QR['filename']);

	if(($result = $musiconhold->chk_values($_QR)) === false
	|| ($result['mode'] === 'custom' && (string) $result['application'] === '') === true)
	{
		$fm_save = false;
		$info = $musiconhold->get_filter_result();
		break;
	}

	if($musiconhold->add_category($result) !== false)
		$_QRY->go($_TPL->url('service/ipbx/pbx_services/musiconhold'),$param);
	else
		$fm_save = false;
}
while(false);

$_TPL->set_var('fm_save',$fm_save);

$element = $musiconhold->get_element();

?>

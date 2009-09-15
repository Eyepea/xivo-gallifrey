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

$info['filename'] = '';
$info['category'] = $cat;

$fm_save = null;

if(isset($_QR['fm_send'],$_QR['category']) === false
|| ($infos = $musiconhold->get_category($_QR['category'])) === false
|| ($fileuploaded = $musiconhold->get_upload('filename')) === false
|| ($info['category'] = $musiconhold->chk_value('category',$infos['cat']['category'])) === false
|| ($info['filename'] = $musiconhold->chk_value('filename',$fileuploaded['name'])) === false)
{
	if(isset($fileuploaded) === true)
	{
		$fm_save = false;

		if(is_array($fileuploaded) === true)
			xivo_file::rm($fileuploaded['tmp_name']);
	}
}
else
{
	$filename = xivo_file::joinpath($info['category'],$fileuploaded['name']);

	if($musiconhold->add_file($filename,$fileuploaded['tmp_name']) === true)
	{
		$param['cat'] = $info['category'];
		$_QRY->go($_TPL->url('service/ipbx/pbx_services/musiconhold'),$param);
	}
	else
		$fm_save = false;
}

$_TPL->set_var('info',$info);
$_TPL->set_var('fm_save',$fm_save);
$_TPL->set_var('option',$musiconhold->get_option());

?>

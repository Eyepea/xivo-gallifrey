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

$param['page'] = $page;

if(($infos = $musiconhold->get_category($cat)) === false)
	$_QRY->go($_TPL->url('service/ipbx/pbx_services/musiconhold'),'act=list');

$cat = $info['category'] = $infos['cat']['category'];

if(isset($_QR['id']) === false || ($info['file'] = $musiconhold->get_file($_QR['id'],$infos['cat']['category'])) === false)
	$_QRY->go($_TPL->url('service/ipbx/pbx_services/musiconhold'),$param);

$info['filename'] = $info['file']['basename'];
$id = $info['file']['filename'];

$fm_save = null;

do
{
	if(isset($_QR['fm_send'],$_QR['filename'],$_QR['category']) === false
	|| ($infos = $musiconhold->get_category($_QR['category'])) === false)
		break;

	$info['category'] = $infos['cat']['category'];
	$info['filename'] = strval($_QR['filename']).'.'.$info['file']['extension'];

	if(($info['category'] = $musiconhold->chk_value('category',$info['category'])) === false
	|| ($info['filename'] = $musiconhold->chk_value('filename',$info['filename'])) === false)
	{
		$fm_save = false;
		break;
	}

	$filename = dwho_file::joinpath($info['file']['dirname'],$info['file']['filename']);
	$newfilename = dwho_file::joinpath($info['category'],$info['filename']);

	if($musiconhold->edit_file($filename,$newfilename) === true)
		$_QRY->go($_TPL->url('service/ipbx/pbx_services/musiconhold'),$param);
	else
		$fm_save = false;

	$info['filename'] = $info['file']['basename'];
}
while(false);

$_TPL->set_var('id',$id);
$_TPL->set_var('info',$info);
$_TPL->set_var('fm_save',$fm_save);

?>

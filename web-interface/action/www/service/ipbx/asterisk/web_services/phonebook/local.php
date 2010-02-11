<?php

#
# XiVO Web-Interface
# Copyright (C) 2006-2010  Proformatique <technique@proformatique.com>
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

include(dwho_file::joinpath(dirname(__FILE__),'_common.php'));

define('XIVO_PHONEBOOK_URL',$_TPL->url('service/ipbx/web_services/phonebook/search',true));

$vendor = isset($_QR['vendor']) === true ? $phonebook->chk_vendor($_QR['vendor']) : false;

if($vendor === false)
{
	$http_response->set_status_line(400);
	$http_response->send(false);
	dwho_die('Error/Invalid Vendor');
}

if(isset($_QR['name']) === false || dwho_has_len($_QR['name']) === false)
{
	$http_response->set_status_line(400);
	$http_response->send(false);
	dwho_die('Error/Invalid name');
}

$rs = array();

if(($rsp = $phonebook->get_phonebook_search($_QR['name'])) !== false)
	$rs = $rsp;

if(($rsu = $phonebook->get_user_search($_QR['name'],false)) !== false)
	$rs = array_merge($rs,$rsu);

if(($nb = count($rs)) === 0)
{
	$http_response->set_status_line(204);
	$http_response->send(true);
}

$_TPL->set_var('act','list');
$_TPL->set_var('list',$rs);
$_TPL->display('/service/ipbx/'.$ipbx->get_name().'/generic');

?>

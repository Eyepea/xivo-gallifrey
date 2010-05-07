<?php

#
# XiVO Web-Interface
# Copyright (C) 2010  Proformatique <technique@proformatique.com>
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

#
# main menu is used by cisco phones to append XiVO directory to local directories
#

include(dwho_file::joinpath(dirname(__FILE__),'_common.php'));

$xmlphone = &$_TPL->get_module('xmlphone');

$pos = isset($_QR['pos']) === true ? dwho_uint($_QR['pos']) : 0;
$node = isset($_QR['node']) === true ? dwho_uint($_QR['node']) : 0;
$directory = isset($_QR['directory']) === true && $node === 1 ? true : false;
$vendor = isset($_QR['vendor']) === true ? $phonebook->chk_vendor($_QR['vendor']) : false;

if($vendor === false && ($vendor = $phonebook->get_vendor_from_useragent()) === false)
{
	$http_response->set_status_line(400);
	$http_response->send(false);
	dwho_die('Error/Invalid Vendor and User-Agent');
}

$_TPL->set_var('path'  , '/bloc/service/ipbx/'.$ipbx->get_name().'/web_services/phonebook');
$_TPL->set_var('vendor', $vendor);
$_TPL->set_var('act'   , 'mainmenu');

$_TPL->display('/bloc/service/ipbx/'.$ipbx->get_name().'/web_services/phonebook/phone');
die();

?>

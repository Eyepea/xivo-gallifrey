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

$phonebook = &$ipbx->get_appcustom('webservices','phonebook');

if(isset($_SERVER['REMOTE_ADDR']) === false
|| $phonebook->chk_host_access($_SERVER['REMOTE_ADDR']) === false)
	dwho_die('Error/403');

define('XIVO_PHONEBOOK_URL',$_TPL->url('service/ipbx/web_services/phonebook/search',true));

$vendor = isset($_QR['vendor']) === true ? $phonebook->chk_vendor($_QR['vendor']) : false;

if($vendor === false)
	dwho_die('Error/Invalid Vendor');

if(isset($_QR['name']) === false || dwho_has_len($_QR['name']) === false)
	dwho_die('Error/Invalid name');

$rs = array();

if(($rsp = $phonebook->get_phonebook_search($_QR['name'])) !== false)
	$rs = $rsp;

if(($rsu = $phonebook->get_user_search($_QR['name'],false)) !== false)
	$rs = array_merge($rs,$rsu);

if(($nb = count($rs)) === 0)
	dwho_die('no-data');

echo dwho_msg('beg-data')."\n";

for($i = 0;$i < $nb;$i++)
{
	$ref = &$rs[$i];

	echo	'"'.str_replace('"','""',$ref['name']).'"|'.
		'"'.str_replace('"','""',$ref['phone']).'"|'.
		'"'.str_replace('"','""',$ref['type']).'"'."\n";
}

dwho_die('end-data')."\n";

?>

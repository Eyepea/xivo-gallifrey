<?php

#
# XiVO Web-Interface
# Copyright (C) 2009-2010  Proformatique <technique@proformatique.com>
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

if(defined('XIVO_LOC_UI_ACL_CATEGORY') === true
&& defined('XIVO_LOC_UI_ACL_SUBCATEGORY') === true)
{
	$access_category = XIVO_LOC_UI_ACL_CATEGORY;
	$access_subcategory = XIVO_LOC_UI_ACL_SUBCATEGORY;
}
else
{
	$access_category = 'pbx_settings';
	$access_subcategory = 'voicemail';
}

include(dwho_file::joinpath(dirname(__FILE__),'..','_common.php'));

if(defined('XIVO_LOC_UI_ACTION') === true)
	$act = XIVO_LOC_UI_ACTION;
else
	$act = $_QRY->get('act');

switch($act)
{
	case 'view':
		$appvoicemail = &$ipbx->get_application('voicemail');

		$nocomponents = array('contextmember'	=> true);

		if(($info = $appvoicemail->get($_QRY->get('id'),
					       null,
					       $nocomponents)) === false)
		{
			$http_response->set_status_line(404);
			$http_response->send(true);
		}

		$_TPL->set_var('info',$info);
		break;
	case 'search':
	default:
		$act = 'search';
		$appvoicemail = &$ipbx->get_application('voicemail',null,false);

		if(($list = $appvoicemail->get_voicemail_search($_QRY->get('search'))) === false)
		{
			$http_response->set_status_line(204);
			$http_response->send(true);
		}

		$_TPL->set_var('list',$list);
		$_TPL->set_var('except',$_QRY->get('except'));
		break;
}

$_TPL->set_var('act',$act);
$_TPL->display('/service/ipbx/'.$ipbx->get_name().'/pbx_settings/voicemail');

?>

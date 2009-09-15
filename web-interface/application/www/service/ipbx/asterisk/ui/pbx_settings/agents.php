
<?php

#
# XiVO Web-Interface
# Copyright (C) 2009  Proformatique <technique@proformatique.com>
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

if(defined('XIVO_TPL_UI_ACL_CATEGORY') === true
&& defined('XIVO_TPL_UI_ACL_SUBCATEGORY') === true)
{
	$access_category = XIVO_TPL_UI_ACL_CATEGORY;
	$access_subcategory = XIVO_TPL_UI_ACL_SUBCATEGORY;
}
else
{
	$access_category = 'pbx_settings';
	$access_subcategory = 'agents';
}

include(xivo_file::joinpath(dirname(__FILE__),'..','_common.php'));

if(defined('XIVO_TPL_UI_ACTION') === true)
	$act = XIVO_TPL_UI_ACTION;
else
	$act = $_QRY->get('act');

switch($act)
{
	case 'search':
	default:
		$act = 'search';
		$appagent = &$ipbx->get_application('agent',null,false);

		if(($list = $appagent->get_agents_search($_QRY->get('search'),false)) === false)
		{
			$http->set_status(204);
			$http->send(true);
		}

		$_TPL->set_var('list',$list);
		$_TPL->set_var('except',$_QRY->get('except'));
		break;
}

$_TPL->set_var('act',$act);
$_TPL->display('/service/ipbx/'.$ipbx->get_name().'/pbx_settings/agents');

?>

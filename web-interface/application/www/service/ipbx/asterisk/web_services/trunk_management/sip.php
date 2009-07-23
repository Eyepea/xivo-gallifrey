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

xivo::load_class('xivo_http');
$http = new xivo_http();

if(defined('XIVO_TPL_WEBSERVICES_MODE') === false
|| (XIVO_TPL_WEBSERVICES_MODE !== 'private'
   && XIVO_TPL_WEBSERVICES_MODE !== 'restricted') === true)
{
	$http->set_status(403);
	$http->send(true);
}

$access_category = 'trunk_management';
$access_subcategory = 'sip';

include(xivo_file::joinpath(dirname(__FILE__),'..','_'.XIVO_TPL_WEBSERVICES_MODE.'.php'));

switch($_QRY->get_qs('act'))
{
	case 'add':
		$apptrunk = &$ipbx->get_application('trunk',
						    array('protocol' => XIVO_SRE_IPBX_AST_PROTO_SIP));

		$status = $apptrunk->add_from_json() === true ? 200 : 400;

		$http->set_status($status);
		$http->send(true);
		break;
	case 'delete':
		$apptrunk = &$ipbx->get_application('trunk',
						    array('protocol' => XIVO_SRE_IPBX_AST_PROTO_SIP));

		if($apptrunk->get($_QRY->get_qs('id')) !== false
		&& $apptrunk->delete() === true)
			$status = 200;
		else
			$status = 400;

		$http->set_status($status);
		$http->send(true);
		break;
	case 'search':
		$apptrunk = &$ipbx->get_application('trunk',
						    array('protocol' => XIVO_SRE_IPBX_AST_PROTO_SIP),
						    false);

		if(($trunks = $apptrunk->get_trunks_search($_QRY->get_qs('search'),true)) === false)
		{
			$http->set_status(204);
			$http->send(true);
		}

		$_HTML->set_var('trunks',$trunks);
		$_HTML->set_var('sum',$_QRY->get_qs('sum'));
		$_HTML->display('/service/ipbx/'.$ipbx->get_name().'/trunk_management/sip');
		break;
	case 'list':
	default:
		$apptrunk = &$ipbx->get_application('trunk',
						    array('protocol' => XIVO_SRE_IPBX_AST_PROTO_SIP),
						    false);

		if(($trunks = $apptrunk->get_trunks_list(true)) === false)
		{
			$http->set_status(204);
			$http->send(true);
		}

		$_HTML->set_var('trunks',$trunks);
		$_HTML->set_var('sum',$_QRY->get_qs('sum'));
		$_HTML->display('/service/ipbx/'.$ipbx->get_name().'/trunk_management/sip');
}

?>

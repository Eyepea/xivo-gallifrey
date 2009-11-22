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

define('XIVO_PATH_ROOT',dirname(__FILE__));

require_once(XIVO_PATH_ROOT.DIRECTORY_SEPARATOR.'xivo.inc');

$_XIVO = &dwho_gct::set_get(new xivo());

$_CF = &dwho_gat::get('_CF');
$_QR = &dwho_gat::get('_QR');
$_URL = &dwho_gat::load_get('url',XIVO_PATH_OBJECTCONF);
$_API_MISC = &dwho_gat::load_get('api_misc',XIVO_PATH_OBJECTCONF);

$_I18N = &dwho_gct::get('dwho_i18n');
$_I18N->load_file('xivo');

$_QRY = &dwho_gct::get('dwho_query');
$_XOBJ = &dwho_gct::get('xivo_object');
$_SRE = &dwho_gct::get('xivo_service');
$_USR = &dwho_gct::get('_USR');

if(defined('XIVO_TPL_SPACE') === false)
	define('XIVO_TPL_SPACE','www');

$tpl_space = XIVO_TPL_SPACE;

if(isset($_CF['tpl'][$tpl_space]) === false)
	$tpl_space = false;

switch($tpl_space)
{
	case 'www':
		dwho::load_class('dwho_tpl');
		$_TPL = &new dwho_tpl($_CF['tpl']['www'],array('menu','url','dhtml'),$_URL,$_API_MISC);
		break;
	case 'ui':
	case 'json':
		dwho::load_class('dwho_tpl');
		$_TPL = &new dwho_tpl($_CF['tpl'][$tpl_space],array('json','url','dhtml'),$_URL,$_API_MISC);

		if(dwho::load_class('dwho_json') === false)
		{
			dwho::load_class('dwho_http');
			$http_response = dwho_http::factory('response');
			$http_response->set_status_line(500);
			$http_response->send(true);
		}
		break;
	default:
		dwho_die('Invalid TPL SPACE');
}

if(($prepend = $_TPL->get_prepend()) !== false)
	include($prepend);

?>

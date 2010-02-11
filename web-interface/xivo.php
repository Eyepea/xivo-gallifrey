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

define('XIVO_PATH_ROOT',dirname(__FILE__));

require_once(XIVO_PATH_ROOT.DIRECTORY_SEPARATOR.'xivo.inc');

$_XIVO = &dwho_gct::set_get(new xivo());

$_CF = &dwho_gat::get('_CF');
$_QR = &dwho_gat::get('_QR');
$_URL = &dwho_gat::load_get('url',XIVO_PATH_OBJECTCONF);
$_ACTION_MISC = &dwho_gat::load_get('action_misc',XIVO_PATH_OBJECTCONF);

$_I18N = &dwho_gct::get('dwho_i18n');
$_I18N->load_file('xivo');

$_QRY = &dwho_gct::get('dwho_query');

dwho::load_class('location');
$_LOC = &dwho_gct::set_get(new dwho_location($_CF['location'],$_ACTION_MISC));

$_XOBJ = &dwho_gct::get('xivo_object');
$_SRE = &dwho_gct::get('xivo_service');
$_USR = &dwho_gct::get('_USR');

if(defined('XIVO_TPL_AREA') === false)
	define('XIVO_TPL_AREA','www');

$tpl_area = XIVO_TPL_AREA;

if(isset($_CF['template'][$tpl_area]) === false)
	$tpl_area = false;

switch($tpl_area)
{
	case 'www':
		if(dwho_constant('XIVO_WEBI_CONFIGURED',false) === false
		&& $_LOC->cmp_to_current_location('/index.php') === false)
			dwho_die('XIVO is not configured');

		dwho::load_class('dwho_tpl');
		$_TPL = &new dwho_tpl($_CF['template']['www']['path'],
				      array('menu','url','dhtml'),
				      $_URL);
		break;
	case 'ui':
	case 'json':
		dwho::load_class('dwho_tpl');
		$_TPL = &new dwho_tpl($_CF['template'][$tpl_area]['path'],
				      array('json','url','dhtml'),
				      $_URL);

		if(dwho::load_class('dwho_json') === false
		|| dwho_constant('XIVO_WEBI_CONFIGURED',false) === false)
		{
			dwho::load_class('dwho_http');
			$http_response = dwho_http::factory('response');
			$http_response->set_status_line(500);
			$http_response->send(true);
		}
		break;
	default:
		dwho_die('Invalid TPL AREA');
}

?>

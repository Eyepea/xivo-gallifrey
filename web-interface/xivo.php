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

require_once(XIVO_PATH_ROOT.DIRECTORY_SEPARATOR.'object'.DIRECTORY_SEPARATOR.'xivo.inc');

$_XIVO = &xivo_gct::set_get(new xivo());

$_CF = &xivo_gat::get('_CF');
$_QR = &xivo_gat::get('_QR');
$_URL = &xivo_gat::load_get('url',XIVO_PATH_OBJECTCONF);

$_I18N = &xivo_gct::get('xivo_i18n');
$_I18N->load_file('xivo');

$_QRY = &xivo_gct::get('xivo_query');
$_SRE = &xivo_gct::get('xivo_service');
$_USR = &xivo_gct::get('_USR');

if(defined('XIVO_TPL_SPACE') === false)
	define('XIVO_TPL_SPACE','www');

$tpl_space = XIVO_TPL_SPACE;

if(isset($_CF['tpl'][$tpl_space]) === false)
	$tpl_space = false;

switch($tpl_space)
{
	case 'www':
		xivo::load_class('xivo_tpl');
		$_TPL = &new xivo_tpl($_CF['tpl']['www'],array('menu','url','dhtml'),$_URL);
		break;
	case 'ui':
	case 'json':
		xivo::load_class('xivo_tpl');
		$_TPL = &new xivo_tpl($_CF['tpl'][$tpl_space],array('json','url','dhtml'),$_URL);

		if(xivo::load_class('xivo_json') === false)
		{
			xivo::load_class('xivo_http');
			$http = new xivo_http();
			$http->set_status(500);
			$http->send(true);
		}
		break;
	default:
		xivo_die('Invalid TPL SPACE');
}

if(($prepend = $_TPL->get_prepend()) !== false)
	include($prepend);

?>

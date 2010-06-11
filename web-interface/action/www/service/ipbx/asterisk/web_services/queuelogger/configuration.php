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

$access_category = 'queuelogger';
$access_subcategory = 'configuration';

include(dwho_file::joinpath(dirname(__FILE__),'..','_common.php'));

$act = $_QRY->get('act');
define("CLIENT_VERSION", '5956');

switch($act)
{
	case 'view':
	default:
		$act = 'view';

		// load queuelogger database uri
		$config         = dwho::load_init(XIVO_PATH_CONF.DWHO_SEP_DIR.'cti.ini');
		$db_queuelogger = $config['queuelogger']['datastorage'];

		$_TPL->set_var('info', array('db_uri' => $db_queuelogger));
		break;
}

$_TPL->set_var('act',$act);
$_TPL->set_var('sum',$_QRY->get('sum'));
$_TPL->display('/service/ipbx/'.$ipbx->get_name().'/generic');

?>

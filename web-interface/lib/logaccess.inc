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


if(($ipbx_ini = dwho::load_init(XIVO_PATH_CONF.DWHO_SEP_DIR.'ipbx.ini')) === false)
	die('ERR: Missing ipbx init file');

if(dwho_issa('logaccess',$ipbx_ini) === true
&& isset($ipbx_ini['logaccess']['file']) === true
&& dwho_empty($ipbx_ini['logaccess']['file']) === false)
	define('XIVO_LOG_FILE',$ipbx_ini['logaccess']['file']);
else
	die('ERR: [logaccess] in ipbx.ini not configured');

if ((($_QR['act'] === 'add'
|| $_QR['act'] === 'edit')
&& $_QR['fm_send'] === '1')
|| $_QR['act'] === 'del'
|| $_QR['act'] === 'enables'
|| $_QR['act'] === 'disables')
	dwho_logw($action_path,&$_USR->_info['login'],$_QR['act']);

?>

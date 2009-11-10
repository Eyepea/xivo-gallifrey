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

$appentity = &$_XOBJ->get_application('entity',null,false);

xivo::load_class('xivo_server',XIVO_PATH_OBJECT,null,false);
$_SVR = new xivo_server();

xivo::load_class('xivo_ldapserver',XIVO_PATH_OBJECT,null,false);
$_LDAPSVR = new xivo_ldapserver();

$userstat = $entitystat = $serverstat = $ldapserver = array();
$userstat['enable'] = $entitystat['enable'] = $serverstat['enable'] = $ldapserverstat['enable'] =  0;
$userstat['disable'] = $entitystat['disable'] = $serverstat['disable'] = $ldapserverstat['disable'] = 0;

if(($enableuser = $_USR->get_nb(null,true)) !== false)
	$userstat['enable'] = $enableuser;

if(($disableuser = $_USR->get_nb(null,false)) !== false)
	$userstat['disable'] = $disableuser;

if(($enableentity = $appentity->get_nb(null,false)) !== false)
	$entitystat['enable'] = $enableentity;

if(($disableentity = $appentity->get_nb(null,true)) !== false)
	$entitystat['disable'] = $disableentity;

$entitystat['total'] = $entitystat['enable'] + $entitystat['disable'];

$userstat['total'] = $userstat['enable'] + $userstat['disable'];

if(($enableserver = $_SVR->get_nb(null,false)) !== false)
	$serverstat['enable'] = $enableserver;

if(($disableserver = $_SVR->get_nb(null,true)) !== false)
	$serverstat['disable'] = $disableserver;

$serverstat['total'] = $serverstat['enable'] + $serverstat['disable'];

if(($enableldapserver = $_LDAPSVR->get_nb(null,false)) !== false)
	$ldapserverstat['enable'] = $enableldapserver;

if(($disableldapserver = $_LDAPSVR->get_nb(null,true)) !== false)
	$ldapserverstat['disable'] = $disableldapserver;

$ldapserverstat['total'] = $ldapserverstat['enable'] + $ldapserverstat['disable'];

$_TPL->set_var('userstat',$userstat);
$_TPL->set_var('entitystat',$entitystat);
$_TPL->set_var('serverstat',$serverstat);
$_TPL->set_var('ldapserverstat',$ldapserverstat);

$menu = &$_TPL->get_module('menu');
$menu->set_top('top/user/'.$_USR->get_info('meta'));
$menu->set_left('left/xivo/configuration');

$_TPL->set_bloc('main','xivo/configuration/index');
$_TPL->set_struct('xivo/configuration');
$_TPL->display('index');

?>

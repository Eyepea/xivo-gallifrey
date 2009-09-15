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

$appagents = &$ipbx->get_apprealstatic('agents');
$appgeneralagents = &$appagents->get_module('general');

$appqueues = &$ipbx->get_apprealstatic('queues');
$appgeneralqueues = &$appqueues->get_module('general');

$appmeetme = &$ipbx->get_apprealstatic('meetme');
$appgeneralmeetme = &$appmeetme->get_module('general');

$appuserguest = &$ipbx->get_application('user',array('internal' => 1),false);

$info = array();
$info['generalagents'] = $appgeneralagents->get_all_by_category();
$info['generalqueues'] = $appgeneralqueues->get_all_by_category();
$info['generalmeetme'] = $appgeneralmeetme->get_all_by_category();

$info['userinternal'] = array();
$info['userinternal']['guest'] = $appuserguest->get_where(array('name' => 'guest'),null,true);

$element = array();
$element['generalagents'] = $appgeneralagents->get_elements();
$element['generalqueues'] = $appgeneralqueues->get_elements();
$element['generalmeetme'] = $appgeneralmeetme->get_elements();

$error = array();
$error['generalagents'] = array();
$error['generalqueues'] = array();
$error['generalmeetme'] = array();

$fm_save = null;

if(isset($_QR['fm_send']) === true)
{
	if(xivo_issa('generalagents',$_QR) === false)
		$_QR['generalagents'] = array();

	if(($rs = $appgeneralagents->set_save_all($_QR['generalagents'])) !== false)
	{
		$info['generalagents'] = $rs['result'];
		$error['generalagents'] = $rs['error'];

		if(isset($rs['error'][0]) === true)
			$fm_save = false;
		else if($fm_save !== false)
			$fm_save = true;
	}

	if(xivo_issa('generalqueues',$_QR) === false)
		$_QR['generalqueues'] = array();

	if(($rs = $appgeneralqueues->set_save_all($_QR['generalqueues'])) !== false)
	{
		$info['generalqueues'] = $rs['result'];
		$error['generalqueues'] = $rs['error'];

		if(isset($rs['error'][0]) === true)
			$fm_save = false;
		else if($fm_save !== false)
			$fm_save = true;
	}

	if(xivo_issa('generalmeetme',$_QR) === true
	&& ($rs = $appgeneralmeetme->set_save_all($_QR['generalmeetme'])) !== false)
	{
		$info['generalmeetme'] = $rs['result'];
		$error['generalmeetme'] = $rs['error'];

		if(isset($rs['error'][0]) === true)
			$fm_save = false;
		else if($fm_save !== false)
			$fm_save = true;
	}

	if(xivo_issa('userinternal',$_QR) === false)
		$_QR['userinternal'] = array();

	if($info['userinternal']['guest'] !== false)
	{
		if(isset($_QR['userinternal']['guest']) === true)
		{
			if($appuserguest->enable() === true)
				$info['userinternal']['guest']['userfeatures']['commented'] = false;
		}
		else
		{
			if($appuserguest->disable() === true)
				$info['userinternal']['guest']['userfeatures']['commented'] = true;
		}
	}
}

$_TPL->set_var('fm_save',$fm_save);
$_TPL->set_var('error',$error);
$_TPL->set_var('generalagents',$info['generalagents']);
$_TPL->set_var('generalqueues',$info['generalqueues']);
$_TPL->set_var('generalmeetme',$info['generalmeetme']);
$_TPL->set_var('userinternal',$info['userinternal']);
$_TPL->set_var('element',$element);

$dhtml = &$_TPL->get_module('dhtml');
$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/submenu.js');

$menu = &$_TPL->get_module('menu');
$menu->set_top('top/user/'.$_USR->get_info('meta'));
$menu->set_left('left/service/ipbx/'.$ipbx->get_name());

$_TPL->set_bloc('main','service/ipbx/'.$ipbx->get_name().'/general_settings/advanced');
$_TPL->set_struct('service/ipbx/'.$ipbx->get_name());
$_TPL->display('index');

?>

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

$info = $error = array();

$return = &$info;

$apphnumbersemergency = $ipbx->get_application('handynumbers',array('type' => 'emergency'));
$info['emergency'] = $apphnumbersemergency->get();
$elements          = $apphnumbersemergency->get_elements();

$apphnumbersspecial = $ipbx->get_application('handynumbers',array('type' => 'special'));
$info['special'] = $apphnumbersspecial->get();

$fm_save = null;
$dwsm_form_tab = $dwsm_form_part = '';

if(isset($_QR['fm_send']) === true)
{
	$fm_save = true;

	if(isset($_QR['dwsm-form-tab'],$_QR['dwsm-form-part']) === true)
	{
		$dwsm_form_tab = strval($_QR['dwsm-form-tab']);
		$dwsm_form_part = strval($_QR['dwsm-form-part']);
	}

	$return = &$result;

	if(dwho_issa('emergency',$_QR) === false
	|| ($emergency = dwho_group_array('trunkfeaturesid',$_QR['emergency'])) === false)
		$emergency = array();

	$apphnumbersemergency = $ipbx->get_application('handynumbers',array('type' => 'emergency'));
	if($apphnumbersemergency->set_save_all($emergency) === false)
		$result['emergency'] = false;
	else
		$result['emergency'] = $apphnumbersemergency->get_result();

	if($apphnumbersemergency->get_errnb() === 0)
		$error['emergency'] = false;
	else
	{
		$fm_save = false;
		$error['emergency'] = $apphnumbersemergency->get_error();
	}

	if(dwho_issa('special',$_QR) === false
	|| ($special = dwho_group_array('trunkfeaturesid',$_QR['special'])) === false)
		$special = array();

	$apphnumbersspecial = $ipbx->get_application('handynumbers',array('type' => 'special'));
	if($apphnumbersspecial->set_save_all($special) === false)
		$result['special'] = false;
	else
		$result['special'] = $apphnumbersspecial->get_result();

	if($apphnumbersspecial->get_errnb() === 0)
		$error['special'] = false;
	else
	{
		$fm_save = false;
		$error['special'] = $apphnumbersspecial->get_error();
	}
}

if(dwho_issa('emergency',$return) === true
&& isset($return['emergency']['handynumbers'],$return['emergency']['handynumbers'][0]) === true)
	$return['emergency'] = $return['emergency']['handynumbers'];
else
	$return['emergency'] = false;

if(dwho_issa('special',$return) === true
&& isset($return['special']['handynumbers'],$return['special']['handynumbers'][0]) === true)
	$return['special'] = $return['special']['handynumbers'];
else
	$return['special'] = false;

$apptrunk = &$ipbx->get_application('trunk',null,false);
if(($trunkslist = $apptrunk->get_trunks_list(null,null,null,null,true)) !== false)
{
	dwho::load_class('dwho_sort');
	$trunksort = new dwho_sort(array('key' => 'identity'));
	uasort($trunkslist,array(&$trunksort,'str_usort'));
}

$_TPL->set_var('fm_save',$fm_save);
$_TPL->set_var('dwsm_form_tab',$dwsm_form_tab);
$_TPL->set_var('dwsm_form_part',$dwsm_form_part);
$_TPL->set_var('element',$elements);
$_TPL->set_var('info',$return);
$_TPL->set_var('error',$error);
$_TPL->set_var('trunkslist',$trunkslist);

$dhtml = &$_TPL->get_module('dhtml');
$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/general.js');
$dhtml->set_js('js/dwho/submenu.js');

$menu = &$_TPL->get_module('menu');
$menu->set_top('top/user/'.$_USR->get_info('meta'));
$menu->set_left('left/service/ipbx/'.$ipbx->get_name());

$_TPL->set_bloc('main','service/ipbx/'.$ipbx->get_name().'/pbx_services/handynumbers');
$_TPL->set_struct('service/ipbx/'.$ipbx->get_name());
$_TPL->display('index');

?>

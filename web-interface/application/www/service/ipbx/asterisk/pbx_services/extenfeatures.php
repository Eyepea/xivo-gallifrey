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

$appextenfeatures = &$ipbx->get_application('extenfeatures');

$appfeatures = &$ipbx->get_apprealstatic('features');
$appgeneralfeatures = &$appfeatures->get_module('general');
$appfeaturemap = &$appfeatures->get_module('featuremap');

$info = array();
$info['extenfeatures'] = $appextenfeatures->get_all_by_context();
$info['generalfeatures'] = $appgeneralfeatures->get_all_by_category();
$info['featuremap'] = $appfeaturemap->get_all_by_category();

$element = array();
$element['extenfeatures'] = $appextenfeatures->get_elements();
$element['generalfeatures'] = $appgeneralfeatures->get_elements();
$element['featuremap'] = $appfeaturemap->get_elements();

$error = array();
$error['extenfeatures'] = array();
$error['generalfeatures'] = array();
$error['featuremap'] = array();

$fm_save = null;

if(isset($_QR['fm_send']) === true)
{
	if(dwho_issa('extenfeatures',$_QR) === true)
	{
		if($info['extenfeatures'] === false)
			$info['extenfeatures'] = array();

		$extens = $appextenfeatures->get_config_exten();

		if(is_array($extens) === true && empty($extens) === false)
		{
			$rs = array();

			reset($extens);

			while(list($key) = each($extens))
			{
				if(dwho_issa($key,$_QR['extenfeatures']) === false)
					continue;
				else if(isset($_QR['extenfeatures'][$key]['exten']) === true)
					$exten = $_QR['extenfeatures'][$key]['exten'];
				else
					$exten = '';

				$rs['commented'] = isset($_QR['extenfeatures'][$key]['enable']) === false;
				$rs['name'] = $key;
				$rs['exten'] = $exten;

				if(isset($info['extenfeatures'][$key]) === false)
					$info['extenfeatures'][$key] = $rs;
				else
				{
					$info['extenfeatures'][$key]['exten'] = $rs['exten'];
					$info['extenfeatures'][$key]['commented'] = $rs['commented'];
				}

				if($appextenfeatures->set($rs) === false
				|| $appextenfeatures->save() === false)
				{
					$info['extenfeatures'][$key]['exten'] = '';
					$error['extenfeatures'][] = $key;
				}
			}

			if(isset($error['extenfeatures'][0]) === true)
				$fm_save = false;
			else if($fm_save !== false)
				$fm_save = true;
		}
	}

	if(dwho_issa('generalfeatures',$_QR) === true
	&& ($rs = $appgeneralfeatures->set_save_all($_QR['generalfeatures'],false)) !== false)
	{
		$info['generalfeatures'] = $rs['result'];
		$error['generalfeatures'] = $rs['error'];

		if(isset($rs['error'][0]) === true)
			$fm_save = false;
		else if($fm_save !== false)
			$fm_save = true;
	}

	if(dwho_issa('featuremap',$_QR) === true
	&& ($rs = $appfeaturemap->set_save_all($_QR['featuremap'],false)) !== false)
	{
		$info['featuremap'] = $rs['result'];
		$error['featuremap'] = $rs['error'];

		if(isset($rs['error'][0]) === true)
			$fm_save = false;
		else if($fm_save !== false)
			$fm_save = true;
	}
}

$_TPL->set_var('fm_save',$fm_save);
$_TPL->set_var('error',$error);
$_TPL->set_var('extenfeatures',$info['extenfeatures']);
$_TPL->set_var('generalfeatures',$info['generalfeatures']);
$_TPL->set_var('featuremap',$info['featuremap']);
$_TPL->set_var('sound_list',$appgeneralfeatures->get_sound());
$_TPL->set_var('element',$element);

$dhtml = &$_TPL->get_module('dhtml');
$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/extenfeatures.js');
$dhtml->set_js('js/dwho/submenu.js');

$menu = &$_TPL->get_module('menu');
$menu->set_top('top/user/'.$_USR->get_info('meta'));
$menu->set_left('left/service/ipbx/'.$ipbx->get_name());

$_TPL->set_bloc('main','service/ipbx/'.$ipbx->get_name().'/pbx_services/extenfeatures');
$_TPL->set_struct('service/ipbx/'.$ipbx->get_name());
$_TPL->display('index');

?>

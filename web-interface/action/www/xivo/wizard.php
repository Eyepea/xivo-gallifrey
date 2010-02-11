<?php

#
# XiVO Web-Interface
# Copyright (C) 2009-2010  Proformatique <technique@proformatique.com>
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

$appwizard = &$_XOBJ->get_application('wizard');

$previous = false;
$trysave = false;
$ressave = null;
$verify = (bool) $_QRY->get('verify');

if($_QRY->get('step') === $appwizard->get_current_step())
{
	if((bool) $_QRY->get('refresh') === true)
	{
		$previous = true;
		$trysave = true;
		$appwizard->save($_QR);
		$appwizard->set_previous_step();
	}
	else if((bool) $_QRY->get('previous') === true
	&& (bool) $_QRY->get('next') === false)
	{
		$previous = true;
		$appwizard->set_previous_step();
	}
	else if((bool) $_QRY->get('fm_send') === true && $verify === false)
	{
		$trysave = true;
		$ressave = $appwizard->save($_QR);
	}
}

$step = $appwizard->get_current_step();

switch($step)
{
	case 'license':
		$license_path = XIVO_PATH_ROOT.DWHO_SEP_DIR.'LICENSE';

		if(dwho_file::is_f_r($license_path) !== false)
			$license = file_get_contents($license_path);
		else
			$license = null;

		$_TPL->set_var('license',$license);
		break;
	case 'ipbxengine':
		$_TPL->set_var('info',$appwizard->step_ipbxengine());
		break;
	case 'dbconfig':
		if($previous === true
		|| $trysave === false
		|| $_QRY->get('step') !== $step)
			$info = $appwizard->step_dbconfig();
		else
			$info = $appwizard->get_result('dbconfig');

		$element = $appwizard->get_step_element();
		$dbbackend = dwho_array_intersect_key(array_flip($element['backend']['value']),
						      dwho_gat::get('datastorage'));

		$_TPL->set_var('info',$info);
		$_TPL->set_var('element',$element);
		$_TPL->set_var('dbbackend',$dbbackend);
		break;
	case 'checkcomponents':
		if(($info = $appwizard->step_checkcomponents($verify)) === false)
		{
			$appwizard->reset_wizard();
			$_QRY->go($_TPL->url('index'));
		}

		$_SYSINFO = new dwho_sysinfo();
		$_TPL->set_var('memstats',$_SYSINFO->memstats(true));

		$_TPL->set_var('network',$info['hardware']['network']);
		$_TPL->set_var('packages',$info['packages']);
		break;
	case 'mainconfig':
		if($previous === true
		|| $trysave === false
		|| $_QRY->get('step') !== $step)
			$info = $appwizard->step_mainconfig();
		else
		{
			$info = array();
			$info['mainconfig'] = $appwizard->get_result('mainconfig');
			$info['netiface'] = $appwizard->get_result('netiface');
			$info['resolvconf'] = $appwizard->get_result('resolvconf');
		}

		$_TPL->set_var('info',$info);
		$_TPL->set_var('element',$appwizard->get_step_element());
		break;
	case 'entitycontext':
		if($previous === true
		|| $trysave === false
		|| $_QRY->get('step') !== $step)
			$info = $appwizard->step_entitycontext();
		else
		{
			$info = array();
			$info['entity'] = $appwizard->get_result('entity');
			$info['context_incall'] = $appwizard->get_result('context_incall');
			$info['context_internal'] = $appwizard->get_result('context_internal');
			$info['context_outcall'] = $appwizard->get_result('context_outcall');
		}

		$_TPL->set_var('info',$info);
		$_TPL->set_var('element',$appwizard->get_step_element());
		break;
	case 'ipbximportuser':
		$_TPL->set_var('max_file_size',dwho_get_memory('upload_max_filesize'));
		break;
	case 'validate':
		if($ressave === true
		&& $_QRY->get('step') === $step)
		{
			session_destroy();

			if(($uri = $appwizard->discover_finish_uri()) !== false)
				$_QRY->go($uri.$_TPL->url('index'));
		}

		$_TPL->set_var('info',$appwizard->step_validate());
		$_TPL->set_var('ipbxengine',$appwizard->step_ipbxengine());
		$_TPL->set_var('dbbackend',dwho_gat::get('datastorage'));
		break;
	case 'welcome':
	default:
		$_LANG = &dwho_gat::load_get('language',XIVO_PATH_OBJECTCONF);
		$_TPL->set_var('language',dwho_array_intersect_key(
							$_LANG,
							dwho_i18n::get_language_translated_list()));
}

$_TPL->set_var('error',$appwizard->get_error());
$_TPL->set_var('step',$appwizard->get_current_step());
$_TPL->set_var('can_previous_step',$appwizard->can_previous_step());
$_TPL->set_var('can_next_step',$appwizard->can_next_step());

$menu = &$_TPL->get_module('menu');
$menu->set_top('top/xivo/wizard');
$menu->set_left('left/xivo/wizard');

$dhtml = &$_TPL->get_module('dhtml');
$dhtml->set_css('css/xivo/wizard.css');
$dhtml->set_js('js/xivo/wizard.js');

$_TPL->set_struct('xivo/wizard');
$_TPL->display('index');

?>

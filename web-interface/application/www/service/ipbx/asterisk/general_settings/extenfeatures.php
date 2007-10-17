<?php

$appextenfeatures = &$ipbx->get_application('extenfeatures');

$appfeatures = &$ipbx->get_application('features');
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
	if(xivo_issa('extenfeatures',$_QR) === true)
	{
		if($info['extenfeatures'] === false)
			$info['extenfeatures'] = array();

		$extens = $appextenfeatures->get_config_exten();

		if(($arr = xivo_get_aks($extens)) !== false)
		{
			$rs = array();

			for($i = 0;$i < $arr['cnt'];$i++)
			{
				$key = &$arr['keys'][$i];

				if(xivo_issa($key,$_QR['extenfeatures']) === false)
					continue;

				if(isset($_QR['extenfeatures'][$key]['exten']) === true)
					$exten = $_QR['extenfeatures'][$key]['exten'];
				else
					$exten = '';

				if(isset($_QR['extenfeatures'][$key]['enable']) === false)
					$rs['commented'] = true;
				else
					$rs['commented'] = false;

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

			if(isset($error['extenfeatures'][0]) === false)
				$fm_save = true;
		}
	}

	if(xivo_issa('generalfeatures',$_QR) === true)
	{
		if($info['generalfeatures'] === false)
			$info['generalfeatures'] = array();

		if(($arr = xivo_get_aks($element['generalfeatures'])) !== false)
		{
			$rs = array();

			for($i = 0;$i < $arr['cnt'];$i++)
			{
				$key = &$arr['keys'][$i];

				if(isset($_QR['generalfeatures'][$key]) === true)
					$val = $_QR['generalfeatures'][$key];
				else
					$val = '';

				$rs['var_name'] = $key;
				$rs['var_val'] = $val;
				$rs['commented'] = false;

				if(isset($info['generalfeatures'][$key]) === false)
					$info['generalfeatures'][$key] = $rs;
				else
				{
					$info['generalfeatures'][$key]['var_val'] = $rs['var_val'];
					$info['generalfeatures'][$key]['commented'] = $rs['commented'];
				}

				if($appgeneralfeatures->set($rs) === false
				|| $appgeneralfeatures->save() === false)
				{
					$info['generalfeatures'][$key]['var_val'] = '';
					$error['generalfeatures'][] = $key;
				}
			}

			if(isset($error['generalfeatures'][0]) === true)
				$fm_save = false;
			else if($fm_save !== false)
				$fm_save = true;
		}
	}

	if(xivo_issa('featuremap',$_QR) === true)
	{
		if($info['featuremap'] === false)
			$info['featuremap'] = array();

		if(($arr = xivo_get_aks($element['featuremap'])) !== false)
		{
			$rs = array();

			for($i = 0;$i < $arr['cnt'];$i++)
			{
				$key = &$arr['keys'][$i];

				if(isset($_QR['featuremap'][$key]) === true)
					$val = $_QR['featuremap'][$key];
				else
					$val = '';

				$rs['var_name'] = $key;
				$rs['var_val'] = $val;
				$rs['commented'] = false;

				if(isset($info['featuremap'][$key]) === false)
					$info['featuremap'][$key] = $rs;
				else
				{
					$info['featuremap'][$key]['var_val'] = $rs['var_val'];
					$info['featuremap'][$key]['commented'] = $rs['commented'];
				}

				if($appfeaturemap->set($rs) === false
				|| $appfeaturemap->save() === false)
				{
					$info['featuremap'][$key]['var_val'] = '';
					$error['featuremap'][] = $key;
				}
			}

			if(isset($error['featuremap'][0]) === true)
				$fm_save = false;
			else if($fm_save !== false)
				$fm_save = true;
		}
	}
}

$_HTML->assign('fm_save',$fm_save);
$_HTML->assign('error',$error);
$_HTML->assign('extenfeatures',$info['extenfeatures']);
$_HTML->assign('generalfeatures',$info['generalfeatures']);
$_HTML->assign('featuremap',$info['featuremap']);
$_HTML->assign('sound_list',$appgeneralfeatures->get_sound());
$_HTML->assign('element',$element);

$dhtml = &$_HTML->get_module('dhtml');
$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/extenfeatures.js');
$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/submenu.js');

$menu = &$_HTML->get_module('menu');
$menu->set_top('top/user/'.$_USR->get_info('meta'));
$menu->set_left('left/service/ipbx/'.$ipbx->get_name());

$_HTML->set_bloc('main','service/ipbx/'.$ipbx->get_name().'/general_settings/extenfeatures');
$_HTML->set_struct('service/ipbx/'.$ipbx->get_name());
$_HTML->display('index');

?>

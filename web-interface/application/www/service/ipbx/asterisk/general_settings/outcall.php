<?php

$generaloutcall = &$ipbx->get_module('generaloutcall');
$trunks_list = $ipbx->get_trunks_list();

$info = array();

$return = &$info;

$order = array('exten' => SORT_ASC,'rangebeg' => SORT_ASC);

$info['emergency'] = $generaloutcall->get_all_where(array('type' => 'emergency'),null,$order);

if(isset($_QR['fm_send']) === true)
{
	$return = &$result;

	$tfeatures = &$ipbx->get_module('trunkfeatures');

	$emergency_add = $emergency_edit = $emergency_del = array();

	if(xivo_issa('emergency',$_QR) === true
	&& ($arr_egency = xivo_group_array('trunkfeaturesid',$_QR['emergency'])) !== false)
	{
		$nb = count($arr_egency);

		for($i = 0;$i < $nb;$i++)
		{
			$arr_egency[$i]['type'] = 'emergency';

			if(isset($arr_egency[$i]['id']) === true)
				$egencyid = xivo_uint($arr_egency[$i]['id']);
			else
				$egencyid = 0;

			if(($eginfo = $generaloutcall->chk_values($arr_egency[$i])) === false
			|| $tfeatures->get($eginfo['trunkfeaturesid']) === false)
				continue;

			if($arr_egency[$i]['extenmode'] === 'extension')
				$eginfo['rangebeg'] = $eginfo['rangeend'] = '';
			else
				$eginfo['exten'] = '';

			if($egencyid !== 0)
				$emergency_edit[$egencyid] = $eginfo;
			else
				$emergency_add[] = $eginfo;
		}
	}

	if($info['emergency'] !== false && ($nb = count($info['emergency'])) !== 0)
	{
		for($i = 0;$i < $nb;$i++)
		{
			if(isset($emergency_edit[$info['emergency'][$i]['id']]) === false)
				$generaloutcall->delete($info['emergency'][$i]['id']);
		}
	}

	$result['emergency'] = array();

	$egencykeys = array_keys($emergency_edit);

	if(($nb = count($egencykeys)) !== 0)
	{
		for($i = 0;$i < $nb;$i++)
		{
			$egencyid = &$egencykeys[$i];
			$egencyval = &$emergency_edit[$egencyid];

			if($generaloutcall->edit($egencyid,$egencyval) === false)
				$generaloutcall->delete($egencyid);
			else
			{
				$egencyval['id'] = $egencyid;
				$result['emergency'][] = $egencyval;
			}
		}
	}

	if(($nb = count($emergency_add)) !== 0)
	{
		for($i = 0;$i < $nb;$i++)
		{
			if(($egencyid = $generaloutcall->add($emergency_add[$i])) !== false)
			{
				$emergency_add[$i]['id'] = $egencyid;
				$result['emergency'][] = $emergency_add[$i];
			}
		}
	}

	if(isset($result['emergency'][0]) === false)
		$result['emergency'] = false;
}

$_HTML->assign('element',$generaloutcall->get_element());
$_HTML->assign('info',$return);
$_HTML->assign('trunks_list',$trunks_list);

$dhtml = &$_HTML->get_module('dhtml');
$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/general.js');
$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/submenu.js');

$menu = &$_HTML->get_module('menu');
$menu->set_top('top/user/'.$_USR->get_infos('meta'));
$menu->set_left('left/service/ipbx/asterisk');

$_HTML->assign('bloc','general_settings/outcall');
$_HTML->assign('service_name',$service_name);
$_HTML->set_struct('service/ipbx/index');
$_HTML->display('index');

?>

<?php

$generaloutcall = &$ipbx->get_module('generaloutcall');
$trunks_list = $ipbx->get_trunks_list();

$info = array();

$return = &$info;

$order = array('exten' => SORT_ASC,'rangebeg' => SORT_ASC);

$info['emergency'] = $generaloutcall->get_all_where(array('type' => 'emergency'),null,$order);
$info['special'] = $generaloutcall->get_all_where(array('type' => 'special'),null,$order);

$fm_save = false;

if(isset($_QR['fm_send']) === true)
{
	$fm_save = true;
	$return = &$result;

	$tfeatures = &$ipbx->get_module('trunkfeatures');

	$arr_part = array('emergency','special');

	$i = 0;

	while($i < 2)
	{
		if(isset($arr_part[$i]) === false)
		{
			$i++;
			break;
		}

		$partname = $arr_part[$i];

		$genoutcall_add = $genoutcall_edit = $genoutcall_del = array();

		if(xivo_issa($partname,$_QR) === true
		&& ($arr_outcall = xivo_group_array('trunkfeaturesid',$_QR[$partname])) !== false)
		{
			$nb = count($arr_outcall);

			for($j = 0;$j < $nb;$j++)
			{
				$ref = &$arr_outcall[$j];
				$ref['type'] = $partname;

				if(isset($ref['id']) === true)
					$outcallid = xivo_uint($ref['id']);
				else
					$outcallid = 0;

				if(($eginfo = $generaloutcall->chk_values($ref)) === false)
				{
					$fm_save = false;
					$outcallerr['_error'] = true;
					$result[$partname][] = $generaloutcall->get_filter_result();
					continue;
				}
				
				if($tfeatures->get($eginfo['trunkfeaturesid']) === false)
				{
					$fm_save = false;
					$eginfo['_error'] = true;
					$result[$partname][] = $eginfo;
					continue;
				}

				if($ref['extenmode'] === 'extension')
					$eginfo['rangebeg'] = $eginfo['rangeend'] = '';
				else
					$eginfo['exten'] = '';

				if($outcallid !== 0)
					$genoutcall_edit[$outcallid] = $eginfo;
				else
					$genoutcall_add[] = $eginfo;
			}
		}

		if($info[$partname] !== false && ($nb = count($info[$partname])) !== 0)
		{
			for($j = 0;$j < $nb;$j++)
			{
				if(isset($genoutcall_edit[$info[$partname][$j]['id']]) === false)
					$generaloutcall->delete($info[$partname][$j]['id']);
			}
		}

		$result[$partname] = array();

		$outcallkeys = array_keys($genoutcall_edit);

		if(($nb = count($outcallkeys)) !== 0)
		{
			for($j = 0;$j < $nb;$j++)
			{
				$outcallid = &$outcallkeys[$j];
				$outcallval = &$genoutcall_edit[$outcallid];

				if($generaloutcall->edit($outcallid,$outcallval) === false)
				{
					$fm_save = false;
					$outcallval['_error'] = true;
					$generaloutcall->delete($outcallid);
				}
				else
					$outcallval['id'] = $outcallid;

				$result[$partname][] = $outcallval;
			}
		}

		if(($nb = count($genoutcall_add)) !== 0)
		{
			for($j = 0;$j < $nb;$j++)
			{
				$outcallval = &$genoutcall_add[$j];
				if(($outcallid = $generaloutcall->add($outcallval)) === false)
				{
					$fm_save = false;
					$outcallval['_error'] = true;
				}
				else
					$outcallval['id'] = $outcallid;

				$result[$partname][] = $outcallval;
			}
		}

		if(isset($result[$partname][0]) === false)
			$result[$partname] = false;

		$i++;
	}
}

$_HTML->assign('fm_save',$fm_save);
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

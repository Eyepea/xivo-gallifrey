<?php

$generaloutcall = &$ipbx->get_module('generaloutcall');
$extenumbers = &$ipbx->get_module('extenumbers');
$trunks_list = $ipbx->get_trunks_list();

$info = array();

$return = &$info;

$info['emergency'] = $ipbx->get_generaloutcall_list('emergency');
$info['special'] = $ipbx->get_generaloutcall_list('special');

$fm_save = false;
$fm_smenu_tab = $fm_smenu_part = '';

if(isset($_QR['fm_send']) === true)
{
	if(isset($_QR['fm_smenu-tab'],$_QR['fm_smenu-part']) === true)
	{
		$fm_smenu_tab = strval($_QR['fm_smenu-tab']);
		$fm_smenu_part = strval($_QR['fm_smenu-part']);
	}

	$fm_save = true;
	$return = &$result;

	$tfeatures = &$ipbx->get_module('trunkfeatures');

	$arr_part = array('emergency','special');
	$gen_outcall = array();
	$info_err = array('_error' => true);

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
		$result[$partname] = array();
		$gen_outcall['type'] = $partname;

		if(xivo_issa($partname,$_QR) === true
		&& ($arr_outcall = xivo_group_array('trunkfeaturesid',$_QR[$partname])) !== false)
		{
			$nb = count($arr_outcall);

			for($j = 0;$j < $nb;$j++)
			{
				$exten_numbers = $arr_outcall[$j];

				$genoutcallid = $gen_outcall['extenumid'] = 0;
				$gen_outcall['trunkfeaturesid'] = $exten_numbers['trunkfeaturesid'];

				if(isset($exten_numbers['id']) === true && $exten_numbers['id'] !== '')
				{
					$genoutcallid = xivo_uint($exten_numbers['id']);

					if($info[$partname] === false
					|| isset($info[$partname][$genoutcallid]) === false)
					{
						$fm_save = false;
						continue;
					}

					$gen_outcall['extenumid'] = $info[$partname][$genoutcallid]['generaloutcall']['extenumid'];
				}

				unset($exten_numbers['id'],$exten_numbers['trunkfeaturesid']);
				
				$exten_numbers['context'] = '';

				if(($genoutcall_rs = $generaloutcall->chk_values($gen_outcall)) === false)
				{
					$fm_save = false;
					continue;
				}

				$info_err['generaloutcall'] = $genoutcall_rs;

				if($genoutcallid !== 0)
					$info_err['generaloutcall']['id'] = $genoutcallid;

				if(($extenumbers_rs = $extenumbers->chk_values($exten_numbers)) === false)
				{
					$fm_save = false;
					$info_err['extenumbers'] = $extenumbers->get_filter_result();
					$result[$partname][] = $info_err;
					continue;
				}

				$info_err['extenumbers'] = $extenumbers_rs;

				if($tfeatures->get_id($genoutcall_rs['trunkfeaturesid']) === false)
				{
					$fm_save = false;
					$result[$partname][] = $info_err;
					continue;
				}

				if($genoutcallid === 0)
				{
					if($extenumbers->exists($extenumbers_rs) !== false)
					{
						$fm_save = false;
						$result[$partname][] = $info_err;
						continue;
					}

					$add = array();
					$add['extenumbers'] = $extenumbers_rs;
					$add['generaloutcall'] = $genoutcall_rs;

					$genoutcall_add[] = $add; 
				}
				else
				{
					if($extenumbers->exists($extenumbers_rs,$genoutcall_rs['extenumid']) !== false)
					{
						$fm_save = false;
						$result[$partname][] = $info_err;
						continue;
					}

					$genoutcall_edit[$genoutcallid]['extenumbers'] = $extenumbers_rs; 
					$genoutcall_edit[$genoutcallid]['generaloutcall'] = $genoutcall_rs; 
				}
			}
		}

		if(($partinfo = xivo_issa_val($partname,$info)) !== false)
		{
			$nb = count($partinfo);

			for($j = 0;$j < $nb;$j++)
			{
				$ref = &$partinfo[$j]['generaloutcall'];

				if(isset($genoutcall_edit[$ref['id']]) === true)
				{
					$refedit = &$genoutcall_edit[$ref['id']];
					$editgenoutcall = null;

					$info_rs = array();
					$info_rs['generaloutcall'] = $refedit['generaloutcall'];
					$info_rs['generaloutcall']['id'] = $ref['id'];
					$info_rs['extenumbers'] = $refedit['extenumbers'];

					if($extenumbers->edit($ref['extenumid'],$refedit['extenumbers']) === false
					|| ($editgenoutcall = $generaloutcall->edit($ref['id'],$refedit['generaloutcall'])) === false)
					{
						$fm_save = false;
						$info_rs['_error'] = true;

						if($editgenoutcall === false)
							$extenumbers->edit($ref['extenumid'],$partinfo[$j]['extenumbers']);
					}

					$result[$partname][] = $info_rs;

					continue;
				}
				
				if($extenumbers->delete($ref['extenumid']) === false)
					continue;
			
				if($generaloutcall->delete($ref['id']) === false)
					$extenumbers->add($partinfo[$j]['extenumbers'],$ref['extenumid']);
			}

		}

		if(($nb = count($genoutcall_add)) !== 0)
		{
			for($j = 0;$j < $nb;$j++)
			{
				$refadd = &$genoutcall_add[$j];

				$info_rs = array();
				$info_rs['generaloutcall'] = $refadd['generaloutcall'];
				$info_rs['extenumbers'] = $refadd['extenumbers'];

				if(($extenumid = $extenumbers->add($refadd['extenumbers'])) === false)
				{
					$fm_save = false;
					$info_rs['_error'] = true;
					$result[$partname][] = $info_rs;
					continue;
				}

				$refadd['generaloutcall']['extenumid'] = $extenumid;

				if(($genoutcallid = $generaloutcall->add($refadd['generaloutcall'])) === false)
				{
					$fm_save = false;
					$info_rs['_error'] = true;
					$result[$partname][] = $info_rs;

					$extenumbers->delete($extenumid);
					continue;
				}

				$info_rs['generaloutcall']['id'] = $genoutcallid;
				$result[$partname][] = $info_rs;
			}
		}

		$i++;
	}

	if(isset($result['emergency'][0]) === false)
		$result['emergency'] = false;

	if(isset($result['special'][0]) === false)
		$result['special'] = false;
}
else
{
	if($info['emergency'] !== false)
		$info['emergency'] = array_values($info['emergency']);

	if($info['special'] !== false)
		$info['special'] = array_values($info['special']);
}

$element = array();
$element['generaloutcall'] = $generaloutcall->get_element();
$element['extenumbers'] = $extenumbers->get_element();

$_HTML->assign('fm_save',$fm_save);
$_HTML->assign('fm_smenu_tab',$fm_smenu_tab);
$_HTML->assign('fm_smenu_part',$fm_smenu_part);
$_HTML->assign('element',$element);
$_HTML->assign('info',$return);
$_HTML->assign('trunks_list',$trunks_list);

$dhtml = &$_HTML->get_module('dhtml');
$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/general.js');
$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/submenu.js');

$menu = &$_HTML->get_module('menu');
$menu->set_top('top/user/'.$_USR->get_info('meta'));
$menu->set_left('left/service/ipbx/'.$ipbx->get_name());

$_HTML->set_bloc('main','service/ipbx/'.$ipbx->get_name().'/pbx_services/outcall');
$_HTML->set_struct('service/ipbx/'.$ipbx->get_name());
$_HTML->display('index');

?>

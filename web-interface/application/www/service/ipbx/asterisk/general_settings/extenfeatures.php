<?php

$extenfeatures = &$ipbx->get_module('extenfeatures');
$extenumbers = &$ipbx->get_module('extenumbers');

$element = $extenfeatures->get_element();

$info = $extenfeatures->get_name_exten();
$return = &$info;

$result = $error = array();

$exten_numbers = array();
$exten_numbers['context'] = '';

if(isset($_QR['fm_send']) === true && ($arr = xivo_get_aks($element)) !== false)
{
	$return = &$result;

	for($i = 0;$i < $arr['cnt'];$i++)
	{
		$key = &$arr['keys'][$i];

		if(isset($_QR[$key]) === false)
		{
			$result[$key] = '';
			$error[] = $key;
			continue;
		}
	
		$exten_numbers['exten'] = $_QR[$key];
		$exten_numbers['context'] = '';

		if(($extenumbers_rs = $extenumbers->chk_values($exten_numbers)) === false
		|| ($extenfeatures_rs = $extenfeatures->chk_value($key,$extenumbers_rs['exten'])) === false)
		{
			$result[$key] = '';
			$error[] = $key;
			continue;
		}

		$extenumbersid = false;

		if(isset($info[$key]) === false)
		{
			if($extenumbers->exists($extenumbers_rs) !== false)
			{
				$result[$key] = '';
				$error[] = $key;
				continue;
			}
		}
		else
		{	
			$exten_where = $exten_numbers;
			$exten_where['exten'] = $info[$key];

			if(($extenumbersid = $extenumbers->get_id($exten_where)) === false)
				$extenumbersid = null;

			if($extenumbers->exists($extenumbers_rs,$extenumbersid) !== false)
			{
				$result[$key] = '';
				$error[] = $key;
				continue;
			}
		}

		if($extenumbersid === false || $extenumbersid === null)
		{
			if($extenumbers->add($extenumbers_rs) === false)
			{
				$result[$key] = '';
				$error[] = $key;
				continue;
			}
		}
		else if($extenumbers->edit($extenumbersid,$extenumbers_rs) === false)
		{
			$result[$key] = '';
			$error[] = $key;
			continue;
		}

		$extenfeatures->replace_exten_by_name($key,$extenfeatures_rs);

		$result[$key] = $extenfeatures_rs;

		if(isset($result[$key]{0}) === true && $result[$key]{0} === '_')
			$result[$key] = substr($result[$key],1);
	}

	if(isset($error[0]) === false)
		$_HTML->assign('fm_save',true);
}
else $info = $extenfeatures->get_name_exten_for_display();

$_HTML->assign('info',$return);
$_HTML->assign('error',$error);
$_HTML->assign('element',$element);

$menu = &$_HTML->get_module('menu');
$menu->set_top('top/user/'.$_USR->get_infos('meta'));
$menu->set_left('left/service/ipbx/asterisk');

$_HTML->assign('bloc','general_settings/extenfeatures');
$_HTML->assign('service_name',$service_name);
$_HTML->set_struct('service/ipbx/index');
$_HTML->display('index');

?>

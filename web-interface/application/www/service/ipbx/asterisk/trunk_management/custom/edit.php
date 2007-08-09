<?php

$info = array();

$return = &$info;

if(isset($_QR['id']) === false
|| ($info['trunk'] = $trunkcustom->get($_QR['id'])) === false
|| ($info['tfeatures'] = $tfeatures->get_by_trunk($info['trunk']['id'],'custom')) === false)
	xivo_go($_HTML->url('service/ipbx/trunk_management/custom'),$param);

$edit = true;

do
{
	if(isset($_QR['fm_send']) === false || xivo_issa('trunk',$_QR) === false)
		break;

	$return = &$result;

	if(($result['trunk'] = $trunkcustom->chk_values($_QR['trunk'])) === false)
	{
		$edit = false;
		$result['trunk'] = $trunkcustom->get_filter_result();
	}

	$result['trunk']['commented'] = $info['trunk']['commented'];

	$info['tfeatures']['registerid'] = 0;
	$info['tfeatures']['registercommented'] = 0;

	if(($result['tfeatures'] = $tfeatures->chk_values($info['tfeatures'])) === false)
	{
		$edit = false;
		$result['tfeatures'] = $tfeatures->get_filter_result();
	}

	if($edit === false || $trunkcustom->edit($info['trunk']['id'],$result['trunk']) === false)
		break;

	if($tfeatures->edit($info['tfeatures']['id'],$result['tfeatures']) === false)
	{
		$trunkcustom->edit_origin();
		break;
	}

	xivo_go($_HTML->url('service/ipbx/trunk_management/custom'),$param);

} while(false);

$element['trunk'] = $trunkcustom->get_element();

$_HTML->assign('id',$info['trunk']['id']);
$_HTML->assign('info',$return);

?>

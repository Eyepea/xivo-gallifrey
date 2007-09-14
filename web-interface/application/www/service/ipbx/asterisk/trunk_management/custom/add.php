<?php

$result = null;

$add = true;

do
{
	if(isset($_QR['fm_send']) === false || xivo_issa('trunk',$_QR) === false)
		break;

	$result = array();

	if(($result['trunk'] = $trunkcustom->chk_values($_QR['trunk'])) === false)
	{
		$add = false;
		$result['trunk'] = $trunkcustom->get_filter_result();
	}

	$_QR['tfeatures'] = array();
	$_QR['tfeatures']['trunk'] = 'custom';
	$_QR['tfeatures']['trunkid'] = 0;
	$_QR['tfeatures']['registerid'] = 0;

	if(($result['tfeatures'] = $tfeatures->chk_values($_QR['tfeatures'])) === false)
	{
		$add = false;
		$result['tfeatures'] = $tfeatures->get_filter_result();
	}

	if($add === false || ($trunkid = $trunkcustom->add($result['trunk'])) === false)
		break;

	$result['tfeatures']['trunkid'] = $trunkid;

	if($tfeatures->add($result['tfeatures']) === false)
	{
		$trunkcustom->delete($trunkid);
		break;
	}

	$_QRY->go($_HTML->url('service/ipbx/trunk_management/custom'),$param);

} while(false);

$element['trunk'] = $trunkcustom->get_element();

$_HTML->assign('info',$result);

?>

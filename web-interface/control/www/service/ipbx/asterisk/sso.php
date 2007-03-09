<?php

$total = 0;

if(($users = $ipbx->get_users_list()) !== false)
{
	$total = count($users);
	
	for($i = 0;$i < $total;$i++)
	{
		$ref = &$users[$i];
		if($ref['protocol']['commented'] === true || $ref['protocol']['initialized'] === false)
			continue;

		echo	addcslashes($ref['ufeatures']['protocol'],'|'),'|',
			addcslashes($ref['protocol']['name'],'|'),'|',
			addcslashes($ref['protocol']['secret'],'|'),'|',
			addcslashes($ref['ufeatures']['popupwidget'],'|'),"\n";
	}
}

?>

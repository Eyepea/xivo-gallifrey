<?php

$total = 0;

if(($users = $ipbx->get_users_list()) !== false)
{
	$total = count($users);
	
	for($i = 0;$i < $total;$i++)
	{
		$ref = &$users[$i];

		echo	addcslashes($ref['ufeatures']['protocol'],'|'),'|',
			addcslashes($ref['protocol']['name'],'|'),'|',
			addcslashes($ref['protocol']['secret'],'|'),'|',
			addcslashes($ref['ufeatures']['popupwidget'],'|'),'|',
			addcslashes($ref['ufeatures']['number'],'|'),'|',
			(int) (bool) $ref['protocol']['initialized'],'|',
			(int) (bool) $ref['protocol']['commented'],"\n";
	}
}

?>

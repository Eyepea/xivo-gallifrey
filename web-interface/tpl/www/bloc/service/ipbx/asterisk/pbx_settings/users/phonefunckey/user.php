<?php

$form = &$this->get_module('form');

$list = $this->get_varra('fkdest_list','users');
$fkdata = $this->get_var('fkdata');

$select = array();
$select['field'] = false;
$select['name'] = 'phonefunckey[typeval][]';
$select['label'] = false;
$select['key'] = 'identity';
$select['altkey'] = 'id';
$select['id'] = 'it-phonefunckey-user-typeval';

if($fkdata['ex'] === false):
	$incr = xivo_uint($fkdata['incr']);
	$select['id'] .= '-'.$incr;

	if($fkdata['type'] === 'user'):
		$select['invalid'] = true;
		$select['value'] = $fkdata['user'];
	endif;

	$selectoptattr = '';

	$spanid = 'id="fd-phonefunckey-user-typeval-'.$incr.'"';
else:
	$select['disabled'] = true;

	$selectoptattr = ' onfocus="xivo_fm_set_onfocus(this);"'.
			 ' onblur="xivo_fm_set_onblur(this);"'.
			 ' style="display: none;"';

	$spanid = 'id="fd-phonefunckey-user-typeval" style="display: none;"';
endif;

if(empty($list) === false):
	echo	$form->select($select,$list,$selectoptattr);
else:
	echo	'<span ',$spanid,'>',$form->hidden($select),$this->bbf('phonefunckey_no-user'),'</span>';
endif;

?>

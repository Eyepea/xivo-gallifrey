<?php

$form = &$this->get_module('form');

$fkdata = $this->get_var('fkdata');
$typeelem = $this->get_var('typeelem');

$select = array();
$select['field'] = false;
$select['name'] = 'phonefunckey[type][]';
$select['label'] = false;
$select['key'] = false;
$select['bbf'] = array('concatkey','fm_phonefunckey_type-opt-');
$select['default'] = $typeelem['default'];
$select['id'] = 'it-phonefunckey-type';

if($fkdata['ex'] === false):
	$select['id'] .= '-'.xivo_uint($fkdata['incr']);
	$select['value'] = $fkdata['type'];

	$selectoptattr = 'onchange="xivo_chgphonefunckey(this);"';
else:
	$select['disabled'] = true;

	$selectoptattr = 'onfocus="xivo_fm_set_onfocus(this);" '.
			 'onblur="xivo_fm_set_onblur(this);" '.
			 'onchange="xivo_chgphonefunckey(this);"';
endif;

echo	$form->select($select,$typeelem['options'],$selectoptattr);

?>

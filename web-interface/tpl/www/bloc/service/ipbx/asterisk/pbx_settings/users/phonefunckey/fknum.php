<?php

$form = &$this->get_module('form');

$fkdata = $this->get_var('fkdata');
$fknumelem = $this->get_var('fknumelem');

$select = array();
$select['field'] = false;
$select['name'] = 'phonefunckey[fknum][]';
$select['id'] = false;
$select['label'] = false;
$select['key'] = false;
$select['default'] = $fknumelem['default'];

if($fkdata['ex'] === false):
	$selectoptattr = '';
	$select['value'] = $fknumelem['value'];
else:
	$select['disabled'] = true;

	$selectoptattr = 'onfocus="xivo_fm_set_onfocus(this);" '.
			 'onblur="xivo_fm_set_onblur(this);"';
endif;

echo	$form->select($select,$fknumelem['options'],$selectoptattr);

?>

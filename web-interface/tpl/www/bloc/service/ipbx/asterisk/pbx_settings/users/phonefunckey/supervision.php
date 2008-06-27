<?php

$form = &$this->get_module('form');

$fkdata = $this->get_var('fkdata');
$supelem = $this->get_var('supelem');

$select = array();
$select['field'] = false;
$select['name'] = 'phonefunckey[supervision][]';
$select['label'] = false;
$select['bbf'] = array('concatkey','fm_phonefunckey_supervision-opt-');
$select['id'] = 'it-phonefunckey-supervision';
$select['default'] = $supelem['default'];

$selectoptattr = 'onfocus="(this.className != xivo_fm_disabled_class ? xivo_fm_set_onfocus(this) : false)" '.
		 'onblur="(this.className != xivo_fm_disabled_class ? xivo_fm_set_onblur(this) : false)"';

if($fkdata['ex'] === false):
	$select['id'] .= '-'.xivo_uint($fkdata['incr']);
	$select['value'] = $supelem['value'];
else:
	$select['disabled'] = true;
endif;

echo	$form->select($select,$supelem['options'],$selectoptattr);

?>

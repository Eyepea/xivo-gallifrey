<?php

$form = &$this->get_module('form');

$fkdata = $this->get_var('fkdata');

$inputtxt = array();
$inputtxt['field'] = false;
$inputtxt['name'] = 'phonefunckey[typeval][]';
$inputtxt['label'] = false;
$inputtxt['size'] = 15;
$inputtxt['id'] = 'it-phonefunckey-custom-typeval';

if($fkdata['ex'] === false):
	$incr = xivo_uint($fkdata['incr']);
	$inputtxt['id'] .= '-'.$incr;

	if($fkdata['type'] === 'custom'):
		$inputtxt['value'] = $fkdata['custom'];
	endif;

	$inputtxtoptattr = '';
else:
	$inputtxt['disabled'] = true;
	$inputtxtoptattr = ' onfocus="xivo_fm_set_onfocus(this);"'.
			   ' onblur="xivo_fm_set_onblur(this);"'.
			   ' style="display: none;"';
endif;

echo	$form->text($inputtxt,$inputtxtoptattr);

?>

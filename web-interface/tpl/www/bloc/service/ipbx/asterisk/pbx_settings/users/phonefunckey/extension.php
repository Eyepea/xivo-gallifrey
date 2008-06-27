<?php

$form = &$this->get_module('form');

$list = $this->get_varra('fktype_list','extension');

if(empty($list) === false):
	$fkdata = $this->get_var('fkdata');

	$select = array();
	$select['field'] = false;
	$select['name'] = 'phonefunckey[typeval][]';
	$select['label'] = false;
	$select['key'] = true;
	$select['bbf'] = array('concatkey','fm_phonefunckey_extension-typeval-opt-');
	$select['id'] = 'it-phonefunckey-extension-typeval';

	if($fkdata['ex'] === false):
		$select['id'] .= '-'.xivo_uint($fkdata['incr']);

		if($fkdata['type'] === 'extension'):
			$select['invalid'] = true;
			$select['value'] = $fkdata['extension'];
		endif;

		$selectoptattr = '';
	else:
		$select['disabled'] = true;

		$selectoptattr = ' onfocus="xivo_fm_set_onfocus(this);"'.
				 ' onblur="xivo_fm_set_onblur(this);"'.
				 ' style="display: none;"';
	endif;

	echo	$form->select($select,$list,$selectoptattr);
endif;

?>

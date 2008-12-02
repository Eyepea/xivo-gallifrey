<?php

$form = &$this->get_module('form');
$url = &$this->get_module('url');

$list = $this->get_var('bsfilter_list');
$fkdata = $this->get_var('fkdata');

$select = array();
$select['field'] = false;
$select['name'] = 'phonefunckey[typeval][]';
$select['label'] = false;
$select['key'] = 'callfilteridentity';
$select['altkey'] = 'id';
$select['id'] = 'it-phonefunckey-bosssecretary-typeval';

if($fkdata['ex'] === false):
	$incr = xivo_uint($fkdata['incr']);
	$select['id'] .= '-'.$incr;

	if($fkdata['type'] === 'bosssecretary'):
		$select['invalid'] = true;
		$select['value'] = $fkdata['bosssecretary'];
	endif;

	$selectoptattr = '';

	$hrefstyle = 'id="fd-phonefunckey-bosssecretary-typeval-'.$incr.'"';
else:
	$select['disabled'] = true;

	$selectoptattr = ' onfocus="xivo_fm_set_onfocus(this);"'.
			 ' onblur="xivo_fm_set_onblur(this);"'.
			 ' style="display: none;"';

	$hrefstyle = 'id="fd-phonefunckey-bosssecretary-typeval" style="display: none;"';
endif;

if(empty($list) === false):
	echo	$form->select($select,$list,$selectoptattr);
else:
	echo	$form->hidden($select),
		$url->href_html($this->bbf('create_callfilter'),
				'service/ipbx/call_management/callfilter',
				'act=add',
				$hrefstyle);
endif;

?>

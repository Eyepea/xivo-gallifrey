<?php

$form = &$this->get_module('form');
$url = &$this->get_module('url');

$fkelem = $this->get_varra('element','phonefunckey');
$list = $this->get_var('fkdest_list');
$fkerror = $this->get_varra('error','phonefunckey');
$fkinfo = $this->get_varra('info','phonefunckey');

if(empty($fkinfo) === false):
	$nb = count($fkinfo);
else:
	$nb = 0;
endif;

$phonefunckey_js = array();
$phonefunckey_js[] = 'xivo_tlist[\'phonefunckey\'] = new Array();';
$phonefunckey_js[] = 'xivo_tlist[\'phonefunckey\'][\'cnt\'] = '.$nb.';';

$dhtml = &$this->get_module('dhtml');
$dhtml->write_js($phonefunckey_js);

?>
<div class="sb-list">
<table cellspacing="0" cellpadding="0" border="0">
	<thead>
	<tr class="sb-top">
		<th class="th-left"><?=$this->bbf('col_phonefunckey-fknum');?></th>
		<th class="th-center"><?=$this->bbf('col_phonefunckey-type');?></th>
		<th class="th-center"><?=$this->bbf('col_phonefunckey-typeval');?></th>
		<th class="th-center"><?=$this->bbf('col_phonefunckey-supervision');?></th>
		<th class="th-right"><?=$url->href_html($url->img_html('img/site/button/mini/orange/bo-add.gif',
								       $this->bbf('col_phonefunckey-add'),
								       'border="0"'),
							'#',
							null,
							'onclick="xivo_table_list(\'phonefunckey\',this,0,true);
							          xivo_chgphonefunckey(xivo_eid(\'it-phonefunckey-type-\'+
							 					xivo_tlist[\'phonefunckey\'][\'idcnt\']));
								  return(xivo_free_focus());"',
							$this->bbf('col_phonefunckey-add'));?></th>
	</tr>
	</thead>
	<tbody id="phonefunckey">
<?php

$fknumelem = array();
$fknumelem['options'] = $fkelem['fknum']['value'];
$fknumelem['default'] = $fkelem['fknum']['default'];

$typeelem = array();
$typeelem['options'] = $fkelem['type']['value'];
$typeelem['default'] = $fkelem['type']['default'];

$fkdata = array();

$supelem = array();
$supelem['options'] = $fkelem['supervision']['value'];
$supelem['default'] = $fkelem['supervision']['default'];

if($nb > 0):
	for($i = 0;$i < $nb;$i++):
		$ref = &$fkinfo[$i];

		$fknumelem['value'] = $ref['fknum'];

		$this->set_var('fknumelem',$fknumelem);

		$typeelem['value'] = $ref['type'];

		$this->set_var('typeelem',$typeelem);

		$fkdata['ex'] = false;
		$fkdata['type'] = $ref['type'];
		$fkdata[$ref['type']] = $ref[$ref['type']];
		$fkdata['incr'] = $i;

		$this->set_var('fkdata',$fkdata);

		$supelem['value'] = $ref['supervision'];

		$this->set_var('supelem',$supelem);

		if(isset($fkerror[$i]) === true):
			$errdisplay = ' l-infos-error';
		else:
			$errdisplay = '';
		endif;

		echo	'<tr class="fm-field',$errdisplay,'">',
			'<td class="td-left txt-center">';

		$this->file_include('bloc/service/ipbx/asterisk/pbx_settings/users/phonefunckey/fknum');

		echo	'</td><td>';

		$this->file_include('bloc/service/ipbx/asterisk/pbx_settings/users/phonefunckey/type');

		echo	'</td><td>';

		$this->file_include('bloc/service/ipbx/asterisk/pbx_settings/users/phonefunckey/user');
		$this->file_include('bloc/service/ipbx/asterisk/pbx_settings/users/phonefunckey/group');
		$this->file_include('bloc/service/ipbx/asterisk/pbx_settings/users/phonefunckey/queue');
		$this->file_include('bloc/service/ipbx/asterisk/pbx_settings/users/phonefunckey/meetme');
		$this->file_include('bloc/service/ipbx/asterisk/pbx_settings/users/phonefunckey/extension');
		$this->file_include('bloc/service/ipbx/asterisk/pbx_settings/users/phonefunckey/bosssecretary');
		$this->file_include('bloc/service/ipbx/asterisk/pbx_settings/users/phonefunckey/custom');

		echo	'</td><td>';

		$this->file_include('bloc/service/ipbx/asterisk/pbx_settings/users/phonefunckey/supervision');

		echo	'</td><td class="td-right">',
			$url->href_html($url->img_html('img/site/button/mini/blue/delete.gif',
						       $this->bbf('opt_phonefunckey-delete'),
						       'border="0"'),
					'#',
					null,
					'onclick="xivo_table_list(\'phonefunckey\',this,1); return(xivo_free_focus());"',
					$this->bbf('opt_phonefunckey-delete')),
			'</td></tr>';
	endfor;
endif;
?>
	</tbody>
	<tfoot>
	<tr id="no-phonefunckey"<?=($nb > 0 ? ' class="b-nodisplay"' : '')?>>
		<td colspan="5" class="td-single"><?=$this->bbf('no_phonefunckey');?></td>
	</tr>
	</tfoot>
</table>
<table class="b-nodisplay" cellspacing="0" cellpadding="0" border="0">
	<tbody id="ex-phonefunckey">
	<tr class="fm-field">
<?php

$this->set_var('typeelem',$typeelem);
$this->set_var('fknumelem',$fknumelem);
$this->set_var('fkdata',array('ex' => true));
$this->set_var('supelem',$supelem);

	echo	'<td class="td-left txt-center">';

	$this->file_include('bloc/service/ipbx/asterisk/pbx_settings/users/phonefunckey/fknum');

	echo	'</td><td>';

	$this->file_include('bloc/service/ipbx/asterisk/pbx_settings/users/phonefunckey/type');

	echo	'</td><td>';

	$this->file_include('bloc/service/ipbx/asterisk/pbx_settings/users/phonefunckey/user');
	$this->file_include('bloc/service/ipbx/asterisk/pbx_settings/users/phonefunckey/group');
	$this->file_include('bloc/service/ipbx/asterisk/pbx_settings/users/phonefunckey/queue');
	$this->file_include('bloc/service/ipbx/asterisk/pbx_settings/users/phonefunckey/meetme');
	$this->file_include('bloc/service/ipbx/asterisk/pbx_settings/users/phonefunckey/extension');
	$this->file_include('bloc/service/ipbx/asterisk/pbx_settings/users/phonefunckey/bosssecretary');
	$this->file_include('bloc/service/ipbx/asterisk/pbx_settings/users/phonefunckey/custom');

	echo	'</td><td>';

	$this->file_include('bloc/service/ipbx/asterisk/pbx_settings/users/phonefunckey/supervision');

	echo	'</td><td class="td-right">',
		$url->href_html($url->img_html('img/site/button/mini/blue/delete.gif',
					       $this->bbf('opt_phonefunckey-delete'),
					       'border="0"'),
				'#',
				null,
				'onclick="xivo_table_list(\'phonefunckey\',this,1);
					  return(xivo_free_focus());"',
				$this->bbf('opt_phonefunckey-delete'));
?>
		</td>
	</tr>
	</tbody>
</table>
</div>

<div id="fd-ipbxapplication-record" class="b-nodisplay">
<?php

$form = &$this->get_module('form');
$apparg_record = $this->get_var('apparg_record');

echo	$form->text(array('desc'	=> $this->bbf('fm_ipbxapplication_record-filename'),
		          'name'	=> 'ipbxapplication[record][filename]',
		          'labelid'	=> 'ipbxapplication-record-filename',
		          'size'	=> 15,
		          'default'	=> $apparg_record['filename']['default'])),

	$form->select(array('desc'	=> $this->bbf('fm_ipbxapplication_record-fileformat'),
			    'name'	=> 'ipbxapplication[record][fileformat]',
			    'labelid'	=> 'ipbxapplication-record-fileformat',
			    'key'	=> false,
			    'bbf'	=> 'ast_format_name_info-',
			    'default'	=> $apparg_record['fileformat']['default']),
		      $apparg_record['fileformat']['value']),

	$form->text(array('desc'	=> $this->bbf('fm_ipbxapplication_record-silence'),
		          'name'	=> 'ipbxapplication[record][silence]',
		          'labelid'	=> 'ipbxapplication-record-silence',
		          'size'	=> 10,
		          'default'	=> $apparg_record['silence']['default'])),

	$form->text(array('desc'	=> $this->bbf('fm_ipbxapplication_record-maxduration'),
		          'name'	=> 'ipbxapplication[record][maxduration]',
		          'labelid'	=> 'ipbxapplication-record-maxduration',
		          'size'	=> 10,
		          'default'	=> $apparg_record['maxduration']['default'])),

	$form->checkbox(array('desc'	=> $this->bbf('fm_ipbxapplication_record-a'),
			      'name'	=> 'ipbxapplication[record][a]',
			      'labelid'	=> 'ipbxapplication-record-a',
			      'default'	=> $apparg_record['a']['default'])),

	$form->checkbox(array('desc'	=> $this->bbf('fm_ipbxapplication_record-n'),
			      'name'	=> 'ipbxapplication[record][n]',
			      'labelid'	=> 'ipbxapplication-record-n',
			      'default'	=> $apparg_record['n']['default'])),

	$form->checkbox(array('desc'	=> $this->bbf('fm_ipbxapplication_record-q'),
			      'name'	=> 'ipbxapplication[record][q]',
			      'labelid'	=> 'ipbxapplication-record-q',
			      'default'	=> $apparg_record['q']['default'])),

	$form->checkbox(array('desc'	=> $this->bbf('fm_ipbxapplication_record-s'),
			      'name'	=> 'ipbxapplication[record][s]',
			      'labelid'	=> 'ipbxapplication-record-s',
			      'default'	=> $apparg_record['s']['default'])),

	$form->checkbox(array('desc'	=> $this->bbf('fm_ipbxapplication_record-t'),
			      'name'	=> 'ipbxapplication[record][t]',
			      'labelid'	=> 'ipbxapplication-record-t',
			      'default'	=> $apparg_record['t']['default'])),

	$form->button(array('name'	=> 'add-ipbxapplication-record',
			    'id'	=> 'it-add-ipbxapplication-record',
			    'value'	=> $this->bbf('fm_bt-add')),
		      'onclick="xivo_ast_application_record();"');

?>
</div>

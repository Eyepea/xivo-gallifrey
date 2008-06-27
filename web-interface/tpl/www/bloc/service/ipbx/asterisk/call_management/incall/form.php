<?php
	$form = &$this->get_module('form');
	$url = &$this->get_module('url');

	$incall = $this->get_var('incall');
	$element = $this->get_var('element');
	$rightcall = $this->get_var('rightcall');
	$list = $this->get_var('list');
	$context_list = $this->get_var('context_list');
?>

<div id="sb-part-first">

<?php

echo	$form->text(array('desc'	=> $this->bbf('fm_incall_exten'),
			  'name'	=> 'incall[exten]',
			  'labelid'	=> 'incall-exten',
			  'size'	=> 15,
			  'default'	=> $element['incall']['exten']['default'],
			  'value'	=> $this->get_varra('incall','exten')));

if($context_list !== false):
	echo	$form->select(array('desc'	=> $this->bbf('fm_incall_context'),
				    'name'	=> 'incall[context]',
				    'labelid'	=> 'incall-context',
				    'key'	=> 'identity',
				    'altkey'	=> 'name',
				    'default'	=> $element['incall']['context']['default'],
				    'value'	=> $this->get_varra('incall','context')),$context_list);
else:
	echo	'<div id="fd-incall-context" class="txt-center">',
		$url->href_html($this->bbf('create_context'),
				'service/ipbx/system_management/context',
				'act=add'),
		'</div>';
endif;

echo	$this->file_include('bloc/service/ipbx/asterisk/dialaction/all',array('event' => 'answer'));

?>

</div>

<div id="sb-part-last" class="b-nodisplay">

<?php
	if($rightcall['list'] !== false):
?>

	<div id="rightcalllist" class="fm-field fm-multilist">
		<div class="slt-outlist">
			<?=$form->select(array('name'		=> 'rightcalllist',
					       'label'		=> false,
					       'id'		=> 'it-rightcalllist',
					       'browse'		=> 'rightcall',
					       'key'		=> 'identity',
					       'altkey'		=> 'id',
					       'multiple'	=> true,
					       'size'		=> 5,
					       'field'		=> false),
					 $rightcall['list']);?>
		</div>
		<div class="inout-list">
			<a href="#"
			   onclick="xivo_fm_move_selected('it-rightcalllist','it-rightcall'); return(xivo_free_focus());"
			   title="<?=$this->bbf('bt_inrightcall');?>">
			   	<?=$url->img_html('img/site/button/row-left.gif',
			   			  $this->bbf('bt_inrightcall'),
						  'class="bt-inlist" id="bt-inrightcall" border="0"');?></a><br />

			<a href="#"
			   onclick="xivo_fm_move_selected('it-rightcall','it-rightcalllist'); return(xivo_free_focus());"
			   title="<?=$this->bbf('bt_outrightcall');?>">
			   	<?=$url->img_html('img/site/button/row-right.gif',
						  $this->bbf('bt_outrightcall'),
						  'class="bt-outlist" id="bt-outrightcall" border="0"');?></a>
		</div>
		<div class="slt-inlist">
			<?=$form->select(array('name'		=> 'rightcall[]',
					       'label'		=> false,
					       'id'		=> 'it-rightcall',
					       'browse'		=> 'rightcall',
					       'key'		=> 'identity',
					       'altkey'		=> 'id',
					       'multiple'	=> true,
					       'size'		=> 5,
					       'field'		=> false),
					 $rightcall['slt']);?>

		</div>
	</div>
	<div class="clearboth"></div>
<?php
	else:
		echo	'<div class="txt-center">',
			$url->href_html($this->bbf('create_rightcall'),
					'service/ipbx/call_management/rightcall',
					'act=add'),
			'</div>';
	endif;
?>

</div>

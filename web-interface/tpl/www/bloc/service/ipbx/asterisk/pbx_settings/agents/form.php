<?php

$form = &$this->get_module('form');
$url = &$this->get_module('url');

$element = $this->get_var('element');
$info = $this->get_var('info');

$agents = $this->get_var('agents');
$amember = $this->get_var('amember');

$queues = $this->get_var('queues');
$qmember = $this->get_var('qmember');

?>

<div id="sb-part-first">
<?php
	echo	$form->text(array('desc'	=> $this->bbf('fm_agentgroup_name'),
				  'name'	=> 'agentgroup[name]',
				  'labelid'	=> 'agentgroup-name',
				  'size'	=> 15,
				  'default'	=> $element['agentgroup']['name']['default'],
				  'value'	=> $info['agentgroup']['name']));

	if(is_array($agents) === true && empty($agents) === false):
?>
<div id="agentlist" class="fm-field fm-multilist">
	<p><label id="lb-agentlist" for="it-agentlist"><?=$this->bbf('fm_agents');?></label></p>
	<div class="slt-outlist">
<?php
		echo	$form->select(array('name'	=> 'agentlist',
					    'label'	=> false,
					    'id'	=> 'it-agentlist',
					    'multiple'	=> true,
					    'size'	=> 5,
					    'field'	=> false,
					    'browse'	=> 'afeatures',
					    'key'	=> 'identity',
					    'altkey'	=> 'id'),
				      $amember['list']);
?>
	</div>

	<div class="inout-list">
		<a href="#"
		   onclick="xivo_fm_move_selected('it-agentlist','it-agent');
			    return(xivo_free_focus());"
		   title="<?=$this->bbf('bt_inagent');?>">
			<?=$url->img_html('img/site/button/row-left.gif',
					  $this->bbf('bt_inagent'),
					  'class="bt-inlist" id="bt-inagent" border="0"');?></a><br />
		<a href="#"
		   onclick="xivo_fm_move_selected('it-agent','it-agentlist');
			    return(xivo_free_focus());"
		   title="<?=$this->bbf('bt_outagent');?>">
			<?=$url->img_html('img/site/button/row-right.gif',
					  $this->bbf('bt_outagent'),
					  'class="bt-outlist" id="bt-outagent" border="0"');?></a>
	</div>

	<div class="slt-inlist">
<?php
		echo	$form->select(array('name'	=> 'agent-select[]',
					    'label'	=> false,
					    'id'	=> 'it-agent',
					    'multiple'	=> true,
					    'size'	=> 5,
					    'field'	=> false,
					    'browse'	=> 'afeatures',
					    'key'	=> 'identity',
					    'altkey'	=> 'id'),
				      $amember['slt']);
?>
	</div>
</div>
<div class="clearboth"></div>
<?php
	endif;
?>
	<div class="fm-field fm-description">
		<p>
			<label id="lb-agentgroup-description" for="it-agentgroup-description"><?=$this->bbf('fm_agentgroup_description');?></label>
		</p>
		<?=$form->textarea(array('field'	=> false,
					 'label'	=> false,
					 'name'		=> 'agentgroup[description]',
					 'id'		=> 'it-agentgroup-description',
					 'cols'		=> 60,
					 'rows'		=> 5,
					 'default'	=> $element['agentgroup']['description']['default']),
				   $info['agentgroup']['description']);?>
	</div>
</div>

<div id="sb-part-last" class="b-nodisplay">
<?php
	if(is_array($queues) === true && empty($queues) === false):
?>
<div id="queuelist" class="fm-field fm-multilist">
	<div class="slt-outlist">
<?php
		echo	$form->select(array('name'	=> 'queuelist',
					    'label'	=> false,
					    'id'	=> 'it-queuelist',
					    'multiple'	=> true,
					    'size'	=> 5,
					    'field'	=> false,
					    'browse'	=> 'qfeatures',
					    'key'	=> 'name',
					    'altkey'	=> 'name'),
				      $qmember['list']);
?>
	</div>

	<div class="inout-list">
		<a href="#"
		   onclick="xivo_ast_inqueue();
			    return(xivo_free_focus());"
		   title="<?=$this->bbf('bt_inqueue');?>">
			<?=$url->img_html('img/site/button/row-left.gif',
					  $this->bbf('bt_inqueue'),
					  'class="bt-inlist" id="bt-inqueue" border="0"');?></a><br />
		<a href="#"
		   onclick="xivo_ast_outqueue();
			    return(xivo_free_focus());"
		   title="<?=$this->bbf('bt_outqueue');?>">
			<?=$url->img_html('img/site/button/row-right.gif',
					  $this->bbf('bt_outqueue'),
					  'class="bt-outlist" id="bt-outqueue" border="0"');?></a>

	</div>

	<div class="slt-inlist">
<?php
		echo	$form->select(array('name'	=> 'queue-select[]',
					    'label'	=> false,
					    'id'	=> 'it-queue',
					    'multiple'	=> true,
					    'size'	=> 5,
					    'field'	=> false,
					    'browse'	=> 'qfeatures',
					    'key'	=> 'name',
					    'altkey'	=> 'name'),
				      $qmember['slt']);
?>
	</div>
</div>
<div class="clearboth"></div>

<div class="sb-list">
	<table cellspacing="0" cellpadding="0" border="0">
		<tr class="sb-top">
			<th class="th-left"><?=$this->bbf('col_queue-name');?></th>
			<th class="th-right"><?=$this->bbf('col_queue-penalty');?></th>
		</tr>
<?php
		foreach($queues as $value):
			$name = $value['qfeatures']['name'];

			if(xivo_issa($value['qfeatures']['id'],$qmember['info']) === true):
				$class = '';
				$value['member'] = $qmember['info'][$value['qfeatures']['id']];
				$penalty = intval($value['member']['penalty']);
			else:
				$class = ' b-nodisplay';
				$value['member'] = null;
				$penalty = '';
			endif;

		echo	'<tr id="queue-',$name,'" class="fm-field',$class,'">',"\n",
			'<td class="td-left">',$name,'</td>',"\n",
			'<td class="td-right">',
			$form->select(array('field'	=> false,
					    'name'	=> 'queue['.$name.'][penalty]',
					    'id'	=> false,
					    'label'	=> false,
					    'default'	=> $element['qmember']['penalty']['default'],
					    'value'	=> $penalty),
				      $element['qmember']['penalty']['value']),
			'</td>',"\n",
			'</tr>',"\n";
		endforeach;
?>
		<tr id="no-queue"<?=(empty($qmember['slt']) === false ? ' class="b-nodisplay"' : '')?>>
			<td colspan="4" class="td-single"><?=$this->bbf('no_queue');?></td>
		</tr>
	</table>
</div>
<?php
	else:
		echo	'<div class="txt-center">',
			$url->href_html($this->bbf('create_queue'),
					'service/ipbx/pbx_settings/queues',
					'act=add'),
			'</div>';
	endif;
?>
</div>

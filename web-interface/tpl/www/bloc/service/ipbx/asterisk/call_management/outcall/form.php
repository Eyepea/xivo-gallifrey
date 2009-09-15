<?php

#
# XiVO Web-Interface
# Copyright (C) 2006-2009  Proformatique <technique@proformatique.com>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#

$form = &$this->get_module('form');
$url = &$this->get_module('url');

$info = $this->get_var('info');
$element = $this->get_var('element');
$context_list = $this->get_var('context_list');

$outcalltrunk = $this->get_var('outcalltrunk');
$rightcall = $this->get_var('rightcall');

if($this->get_var('fm_save') === false):
	$dhtml = &$this->get_module('dhtml');
	$dhtml->write_js('xivo_form_result(false,\''.$dhtml->escape($this->bbf('fm_error-save')).'\');');
endif;

?>

<div id="sb-part-first">
<?php
	echo	$form->text(array('desc'	=> $this->bbf('fm_outcall_name'),
				  'name'	=> 'outcall[name]',
				  'labelid'	=> 'outcall-name',
				  'size'	=> 15,
				  'default'	=> $element['outcall']['name']['default'],
				  'value'	=> $info['outcall']['name']));

if($context_list !== false):
	echo	$form->select(array('desc'	=> $this->bbf('fm_outcall_context'),
				    'name'	=> 'outcall[context]',
				    'labelid'	=> 'outcall-context',
				    'key'	=> 'identity',
				    'altkey'	=> 'name',
				    'default'	=> $element['outcall']['context']['default'],
				    'value'	=> $info['outcall']['context']),
			      $context_list);
else:
	echo	'<div id="fd-outcall-context" class="txt-center">',
		$url->href_html($this->bbf('create_context'),
				'service/ipbx/system_management/context',
				'act=add'),
		'</div>';
endif;

	echo	$form->text(array('desc'	=> $this->bbf('fm_outcall_externprefix'),
				  'name'	=> 'outcall[externprefix]',
				  'labelid'	=> 'outcall-externprefix',
				  'size'	=> 15,
				  'default'	=> $element['outcall']['externprefix']['default'],
				  'value'	=> $info['outcall']['externprefix'])),

		$form->select(array('desc'	=> $this->bbf('fm_outcall_stripnum'),
				    'name'	=> 'outcall[stripnum]',
				    'labelid'	=> 'outcall-stripnum',
				    'key'	=> false,
				    'default'	=> $element['outcall']['stripnum']['default'],
				    'value'	=> $info['outcall']['stripnum']),
			      $element['outcall']['stripnum']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_outcall_mode'),
				    'name'	=> 'outcall[mode]',
				    'labelid'	=> 'outcall-mode',
				    'key'	=> false,
				    'bbf'	=> array('concatkey','fm_outcall_mode-opt-'),
				    'default'	=> $element['outcall']['mode']['default'],
				    'value'	=> ''),
			      $element['outcall']['mode']['value']),

		$form->text(array('desc'	=> $this->bbf('fm_outcall_prefix'),
				  'name'	=> 'outcall[prefix]',
				  'labelid'	=> 'outcall-prefix',
				  'size'	=> 15,
				  'default'	=> $element['outcall']['prefix']['default'],
				  'value'	=> '')),

		$form->select(array('desc'	=> $this->bbf('fm_outcall_numlen'),
				    'name'	=> 'outcall[numlen]',
				    'labelid'	=> 'outcall-numlen',
				    'key'	=> false,
				    'empty'	=> true,
				    'default'	=> $element['outcall']['numlen']['default'],
				    'value'	=> ''),
			      $element['outcall']['numlen']['value']),

		$form->text(array('desc'	=> $this->bbf('fm_outcall_exten'),
				  'name'	=> 'outcall[exten]',
				  'labelid'	=> 'outcall-exten',
				  'size'	=> 15,
				  'default'	=> $element['outcall']['exten']['default'],
				  'value'	=> $info['outcall']['exten'])),

		$form->checkbox(array('desc'	=> $this->bbf('fm_outcall_setcallerid'),
				      'name'	=> 'outcall[setcallerid]',
				      'labelid'	=> 'setcallerid',
				      'checked'	=> $info['outcall']['setcallerid'],
				      'default'	=> $element['outcall']['setcallerid']['default'])),

		$form->text(array('desc'	=> $this->bbf('fm_outcall_callerid'),
				  'name'	=> 'outcall[callerid]',
				  'labelid'	=> 'outcall-callerid',
				  'size'	=> 15,
				  'notag'	=> false,
				  'default'	=> $element['outcall']['callerid']['default'],
				  'value'	=> $info['outcall']['callerid'])),

		$form->checkbox(array('desc'	=> $this->bbf('fm_outcall_useenum'),
				      'name'	=> 'outcall[useenum]',
				      'labelid'	=> 'useenum',
				      'checked'	=> $info['outcall']['useenum'],
				      'default'	=> $element['outcall']['useenum']['default'])),

		$form->checkbox(array('desc'	=> $this->bbf('fm_outcall_internal'),
				      'name'	=> 'outcall[internal]',
				      'labelid'	=> 'internal',
				      'checked'	=> $info['outcall']['internal'],
				      'default'	=> $element['outcall']['internal']['default'])),

		$form->text(array('desc'	=> $this->bbf('fm_outcall_preprocess-subroutine'),
				  'name'	=> 'outcall[preprocess_subroutine]',
				  'labelid'	=> 'outcall-preprocess-subroutine',
				  'size'	=> 15,
				  'default'	=> $element['outcall']['preprocess_subroutine']['default'],
				  'value'	=> $info['outcall']['preprocess_subroutine'])),

		$form->select(array('desc'	=> $this->bbf('fm_outcall_hangupringtime'),
				    'name'	=> 'outcall[hangupringtime]',
				    'labelid'	=> 'outcall-hangupringtime',
				    'key'	=> false,
				    'bbf'	=> 'fm_outcall_hangupringtime-opt',
				    'bbf_opt'	=> array('argmode'	=> 'mixkey',
				    			 'time'		=> array(
							 		'from'		=> 'second',
							 		'format'	=> '%M%s')),
				    'default'	=> $element['outcall']['hangupringtime']['default'],
				    'value'	=> $info['outcall']['hangupringtime']),
			      $element['outcall']['hangupringtime']['value']);

if($outcalltrunk['list'] !== false):
?>
<div id="outcalltrunklist" class="fm-field fm-multilist">
	<p>
		<label id="lb-outcalltrunklist" for="it-outcalltrunklist"><?=$this->bbf('fm_outcalltrunk');?></label>
	</p>
	<div class="slt-outlist">
		<?=$form->select(array('name'		=> 'outcalltrunklist',
				       'label'		=> false,
				       'id'		=> 'it-outcalltrunklist',
				       'key'		=> 'identity',
				       'altkey'		=> 'id',
				       'multiple'	=> true,
				       'size'		=> 5,
				       'field'		=> false),
				 $outcalltrunk['list']);?>
	</div>

	<div class="inout-list">
		<a href="#"
		   onclick="xivo_fm_move_selected('it-outcalltrunklist','it-outcalltrunk');
			    return(xivo.dom.free_focus());"
		   title="<?=$this->bbf('bt_inoutcalltrunk');?>">
			<?=$url->img_html('img/site/button/row-left.gif',
					  $this->bbf('bt_inoutcalltrunk'),
					  'class="bt-inlist" id="bt-inoutcalltrunk" border="0"');?></a><br />
		<a href="#"
		   onclick="xivo_fm_move_selected('it-outcalltrunk','it-outcalltrunklist');
			    return(xivo.dom.free_focus());"
		   title="<?=$this->bbf('bt_outoutcalltrunk');?>">
			<?=$url->img_html('img/site/button/row-right.gif',
					  $this->bbf('bt_outoutcalltrunk'),
					  'class="bt-outlist" id="bt-outoutcalltrunk" border="0"');?></a>
	</div>

	<div class="slt-inlist">
		<?=$form->select(array('name'		=> 'outcalltrunk[]',
				       'label'		=> false,
				       'id'		=> 'it-outcalltrunk',
				       'key'		=> 'identity',
				       'altkey'		=> 'id',
				       'multiple'	=> true,
				       'size'		=> 5,
				       'field'		=> false),
				 $outcalltrunk['slt']);?>
		<div class="bt-updown">
			<a href="#"
			   onclick="xivo_fm_order_selected('it-outcalltrunk',1);
				    return(xivo.dom.free_focus());"
			   title="<?=$this->bbf('bt_upoutcalltrunk');?>">
				<?=$url->img_html('img/site/button/row-up.gif',
						  $this->bbf('bt_upoutcalltrunk'),
						  'class="bt-uplist" id="bt-upoutcalltrunk" border="0"');?></a><br />
			<a href="#"
			   onclick="xivo_fm_order_selected('it-outcalltrunk',-1);
				    return(xivo.dom.free_focus());"
			   title="<?=$this->bbf('bt_downoutcalltrunk');?>">
				<?=$url->img_html('img/site/button/row-down.gif',
						  $this->bbf('bt_downoutcalltrunk'),
						  'class="bt-downlist" id="bt-downoutcalltrunk" border="0"');?></a>
		</div>
	</div>
</div>
<div class="clearboth"></div>
<?php
else:
	echo	'<div class="txt-center">',
		$url->href_html($this->bbf('create_trunk'),
				'service/ipbx/trunk_management/sip',
				'act=add'),
		'</div>';
endif;
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
			   onclick="xivo_fm_move_selected('it-rightcalllist','it-rightcall');
				    return(xivo.dom.free_focus());"
			   title="<?=$this->bbf('bt_inrightcall');?>">
				<?=$url->img_html('img/site/button/row-left.gif',
						  $this->bbf('bt_inrightcall'),
						  'class="bt-inlist" id="bt-inrightcall" border="0"');?></a><br />
			<a href="#"
			   onclick="xivo_fm_move_selected('it-rightcall','it-rightcalllist');
				    return(xivo.dom.free_focus());"
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

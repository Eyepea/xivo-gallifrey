<?php

#
# XiVO Web-Interface
# Copyright (C) 2006-2010  Proformatique <technique@proformatique.com>
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

$incall = $this->get_var('incall');
$callerid = $this->get_var('callerid');
$element = $this->get_var('element');
$rightcall = $this->get_var('rightcall');
$list = $this->get_var('list');
$context_list = $this->get_var('context_list');

if($this->get_var('fm_save') === false):
	$dhtml = &$this->get_module('dhtml');
	$dhtml->write_js('xivo_form_result(false,\''.$dhtml->escape($this->bbf('fm_error-save')).'\');');
endif;

?>

<div id="sb-part-first">
<?php
	echo	$form->text(array('desc'	=> $this->bbf('fm_incall_exten'),
				  'name'	=> 'incall[exten]',
				  'labelid'	=> 'incall-exten',
				  'size'	=> 15,
				  'default'	=> $element['incall']['exten']['default'],
				  'value'	=> $this->get_var('incall','exten'),
				  'error'	=> $this->bbf_args('incall-exten',
					   $this->get_var('error','incall','exten'))));

if($context_list !== false):
	echo	$form->select(array('desc'	=> $this->bbf('fm_incall_context'),
				    'name'	=> 'incall[context]',
				    'labelid'	=> 'incall-context',
				    'key'	=> 'identity',
				    'altkey'	=> 'name',
				    'default'	=> $element['incall']['context']['default'],
				    'selected'	=> $this->get_var('incall','context'),
					'error'	=> $this->bbf_args('context',
					   $this->get_var('error','contextnummember'))),
			      $context_list);
else:
	echo	'<div id="fd-incall-context" class="txt-center">',
		$url->href_html($this->bbf('create_context'),
				'service/ipbx/system_management/context',
				'act=add'),
		'</div>';
endif;

	$this->file_include('bloc/service/ipbx/asterisk/dialaction/all',
			    array('event'	=> 'answer'));

	echo	$form->select(array('desc'	=> $this->bbf('fm_callerid_mode'),
				    'name'	=> 'callerid[mode]',
				    'labelid'	=> 'callerid-mode',
				    'empty'	=> true,
				    'key'	=> false,
				    'bbf'	=> 'fm_callerid_mode-opt',
				    'bbfopt'	=> array('argmode' => 'paramvalue'),
				    'default'	=> $element['callerid']['mode']['default'],
				    'selected'	=> $callerid['mode']),
			      $element['callerid']['mode']['value'],
			      'onchange="xivo_ast_chg_callerid_mode(this);"'),

		$form->text(array('desc'	=> '&nbsp;',
				  'name'	=> 'callerid[callerdisplay]',
				  'labelid'	=> 'callerid-callerdisplay',
				  'size'	=> 15,
				  'notag'	=> false,
				  'default'	=> $element['callerid']['callerdisplay']['default'],
				  'value'	=> $callerid['callerdisplay'],
				  'error'	=> $this->bbf_args('callerid-callerdisplay',
					   $this->get_var('error','callerid','callerdisplay')))),

		$form->text(array('desc'	=> $this->bbf('fm_incall_preprocess-subroutine'),
				  'name'	=> 'incall[preprocess_subroutine]',
				  'labelid'	=> 'incall-preprocess-subroutine',
				  'size'	=> 15,
				  'default'	=> $element['incall']['preprocess_subroutine']['default'],
				  'value'	=> $this->get_var('incall','preprocess_subroutine'),
				  'error'	=> $this->bbf_args('incall-preprocess-subroutine',
					   $this->get_var('error','incall','preprocess_subroutine'))));
?>
	<div class="fm-paragraph fm-description">
		<p>
			<label id="lb-incall-description" for="it-incall-description"><?=$this->bbf('fm_incall_description');?></label>
		</p>
		<?=$form->textarea(array('paragraph'	=> false,
					 'label'	=> false,
					 'name'		=> 'incall[description]',
					 'id'		=> 'incall-description',
					 'cols'		=> 60,
					 'rows'		=> 5,
					 'default'	=> $element['incall']['description']['default'],
					 'error'	=> $this->bbf_args('error',
						   $this->get_var('error', 'incall', 'description')) ),
				   $this->get_var('incall','description'));?>
	</div>
</div>

<div id="sb-part-faxdetect" class="b-nodisplay">
<?php
	echo	$form->checkbox(array('desc'	=> $this->bbf('fm_incall_faxdetectenable'),
				      'name'	=> 'incall[faxdetectenable]',
				      'labelid'	=> 'incall-faxdetectenable',
				      'checked'	=> $this->get_var('incall','faxdetectenable'),
				      'default'	=> $element['incall']['faxdetectenable']['default']),
				'onchange="xivo_ast_enable_faxdetect();"'),

		$form->select(array('desc'	=> $this->bbf('fm_incall_faxdetecttimeout'),
				    'name'	=> 'incall[faxdetecttimeout]',
				    'labelid'	=> 'incall-faxdetecttimeout',
				    'key'	=> false,
				    'bbf'	=> 'fm_incall_faxdetecttimeout-opt',
				    'bbfopt'	=> array('argmode' => 'paramvalue'),
				    'default'	=> $element['incall']['faxdetecttimeout']['default'],
				    'selected'	=> $this->get_var('incall','faxdetecttimeout')),
			      $element['incall']['faxdetecttimeout']['value']),

		$form->text(array('desc'	=> $this->bbf('fm_incall_faxdetectemail'),
				  'name'	=> 'incall[faxdetectemail]',
				  'labelid'	=> 'incall-faxdetectemail',
				  'size'	=> 15,
				  'default'	=> $element['incall']['faxdetectemail']['default'],
				  'value'	=> $this->get_var('incall','faxdetectemail'),
				  'error'	=> $this->bbf_args('incall-faxdetectemail',
					   $this->get_var('error','incall','faxdetectemail'))));
?>
</div>

<div id="sb-part-last" class="b-nodisplay">
<?php
	if($rightcall['list'] !== false):
?>

	<div id="rightcalllist" class="fm-paragraph fm-multilist">
		<div class="slt-outlist">
			<?=$form->select(array('name'		=> 'rightcalllist',
					       'label'		=> false,
					       'id'		=> 'it-rightcalllist',
					       'browse'		=> 'rightcall',
					       'key'		=> 'identity',
					       'altkey'		=> 'id',
					       'multiple'	=> true,
					       'size'		=> 5,
					       'paragraph'	=> false),
					 $rightcall['list']);?>
		</div>
		<div class="inout-list">
			<a href="#"
			   onclick="dwho.form.move_selected('it-rightcalllist','it-rightcall'); return(dwho.dom.free_focus());"
			   title="<?=$this->bbf('bt_inrightcall');?>">
				<?=$url->img_html('img/site/button/arrow-left.gif',
						  $this->bbf('bt_inrightcall'),
						  'class="bt-inlist" id="bt-inrightcall" border="0"');?></a><br />

			<a href="#"
			   onclick="dwho.form.move_selected('it-rightcall','it-rightcalllist'); return(dwho.dom.free_focus());"
			   title="<?=$this->bbf('bt_outrightcall');?>">
				<?=$url->img_html('img/site/button/arrow-right.gif',
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
					       'paragraph'	=> false),
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

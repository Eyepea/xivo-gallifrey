<?php

#
# XiVO Web-Interface
# Copyright (C) 2010  Proformatique <technique@proformatique.com>
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

$element = $this->get_var('element');
$info    = $this->get_var('info');
$agents  = $this->get_var('agents');
$queues  = $this->get_var('queues');


$dhtml = &$this->get_module('dhtml');
$dhtml->write_js('var xivo_fm_user_suggest = \''.$dhtml->escape($user_suggest).'\';');

if($this->get_var('fm_save') === false):
	$dhtml = &$this->get_module('dhtml');
	$dhtml->write_js('xivo_form_result(false,\''.$dhtml->escape($this->bbf('fm_error-save')).'\');');
endif;

?>


<div id="sb-part-first">
<?php
	echo	$form->text(array('desc'	=> $this->bbf('fm_campaign_name'),
				  'name'	=> 'campaign[name]',
				  'labelid'	=> 'campaign-name',
				  'size'	=> 15,
				  'default'	=> $element['campaign']['name']['default'],
				  'value'	=> $info['campaign']['name'],
				  'error'	=> $this->bbf_args('error',
					   $this->get_var('error','name')))),

		    $form->checkbox(array('desc'	=> $this->bbf('fm_campaign_occasional'),
		      'name'		  => 'campaign_occasional',
		      'labelid'		=> 'campaign_occasional',
				  'help'	  	=> $this->bbf('hlp_fm_campaign_occasional'),
				  'required'	=> false,
		      'checked'		=> $info['campaign']['occasional'],
		      'default'		=> $element['campaign_occasional']['default'])),

	    	$form->text(array('desc'	=> $this->bbf('fm_campaign_start'),
				  'name'	=> 'campaign[start]',
				  'labelid'	=> 'campaign-start',
				  'size'	=> 20,
				  'default'	=> $element['campaign']['start']['default'],
				  'value'	=> $info['campaign']['start'],
				  'error'	=> $this->bbf_args('error',
					   $this->get_var('error','start')))),

	    	$form->text(array('desc'	=> $this->bbf('fm_campaign_end'),
				  'name'	=> 'campaign[end]',
				  'labelid'	=> 'campaign-end',
					'size'	=> 20,
				  'default'	=> $element['campaign']['end']['default'],
				  'value'	=> $info['campaign']['end'],
				  'error'	=> $this->bbf_args('error',
					   $this->get_var('error','end'))));

?>
<script type="text/javascript">
	/* <![CDATA[ */
		$(function() {
				  $('#it-campaign-start').datetime({
									userLang	: 'fr',
									americanMode: false,
								});

				  $('#it-campaign-end').datetime({
									userLang	: 'fr',
									americanMode: false,
								});								

			});
	/* ]]> */
</script>
</div>

<div id="sb-part-filters" class="b-nodisplay">
	<fieldset id="fld-agents">
		<legend><?=$this->bbf('fld-campaign-filter-agents');?></legend>

<?php
	if($user['list'] !== false):
?>
	<div id="agentlist" class="fm-paragraph fm-multilist">
		<div class="slt-outlist">
			<?=$form->select(array('name'		=> 'agentlist',
					       'label'		=> false,
					       'id'	     	=> 'it-agentlist',
					       'multiple'	=> true,
					       'size'	   	=> 5,
					       'paragraph'	=> false,
					       'key'		=> 'identity',
					       'altkey'		=> 'id'),
								$agents);?>
		</div>

		<div class="inout-list">
			<a href="#"
			   onclick="dwho.form.move_selected('it-agentlist','it-agent');
				    return(dwho.dom.free_focus());"
			   title="<?=$this->bbf('bt_inagent');?>">
				<?=$url->img_html('img/site/button/arrow-left.gif',
						  $this->bbf('bt_inagent'),
						  'class="bt-inlist" id="bt-inagent" border="0"');?></a><br />
			<a href="#"
			   onclick="dwho.form.move_selected('it-agent','it-agentlist');
				    return(dwho.dom.free_focus());"
			   title="<?=$this->bbf('bt_outagent');?>">
				<?=$url->img_html('img/site/button/arrow-right.gif',
						  $this->bbf('bt_outagent'),
						  'class="bt-outlist" id="bt-outagent" border="0"');?></a>
		</div>

		<div class="slt-inlist">
			<?=$form->select(array('name'		=> 'agents[]',
					       'label'		=> false,
					       'id'		=> 'it-agent',
					       'multiple'	=> true,
					       'size'		=> 5,
					       'paragraph'	=> false,
					       'key'		=> 'identity',
					       'altkey'		=> 'id'),
					 $info['agents']);?>
		</div>
	</div>
	<div class="clearboth"></div>
<?php
	else:
		echo	'<div class="txt-center">',
			$url->href_html($this->bbf('create_agent'),
					'service/ipbx/call_center/agents',
					'act=add'),
			'</div>';
	endif;
?>
	</fieldset>



	<fieldset id="fld-queues">
		<legend><?=$this->bbf('fld-campaign-filter-queues');?></legend>

<?php
	if($user['list'] !== false):
?>
	<div id="queuelist" class="fm-paragraph fm-multilist">
		<div class="slt-outlist">
			<?=$form->select(array('name'		=> 'queuelist',
					       'label'		=> false,
					       'id'		=> 'it-queuelist',
					       'multiple'	=> true,
					       'size'		=> 5,
					       'paragraph'	=> false,
					       'key'		=> 'identity',
					       'altkey'		=> 'id'),
								$queues);?>
		</div>

		<div class="inout-list">
			<a href="#"
			   onclick="dwho.form.move_selected('it-queuelist','it-queue');
				    return(dwho.dom.free_focus());"
			   title="<?=$this->bbf('bt_inqueue');?>">
				<?=$url->img_html('img/site/button/arrow-left.gif',
						  $this->bbf('bt_inqueue'),
						  'class="bt-inlist" id="bt-inqueue" border="0"');?></a><br />
			<a href="#"
			   onclick="dwho.form.move_selected('it-queue','it-queuelist');
				    return(dwho.dom.free_focus());"
			   title="<?=$this->bbf('bt_outqueue');?>">
				<?=$url->img_html('img/site/button/arrow-right.gif',
						  $this->bbf('bt_outqueue'),
						  'class="bt-outlist" id="bt-outqueue" border="0"');?></a>
		</div>

		<div class="slt-inlist">
			<?=$form->select(array('name'		=> 'queues[]',
					       'label'		=> false,
					       'id'		=> 'it-queue',
					       'multiple'	=> true,
					       'size'		=> 5,
					       'paragraph'	=> false,
					       'key'		=> 'identity',
					       'altkey'		=> 'id'),
					 $info['queues']);?>
		</div>
	</div>
	<div class="clearboth"></div>
<?php
	else:
		echo	'<div class="txt-center">',
			$url->href_html($this->bbf('create_queue'),
					'service/ipbx/call_center/queues',
					'act=add'),
			'</div>';
	endif;
?>
	</fieldset>




	<fieldset id="fld-skills">
		<legend><?=$this->bbf('fld-campaign-filter-skills');?></legend>

		<div class="fm-paragraph fm-description">
<?php
		echo $form->textarea(array('paragraph'	=> false,
					 'label'	=> false,
					 'name'		=> 'skills',
					 'id'		=> 'it-filter-skills',
					 'cols'		=> 60,
					 'rows'		=> 8),
				   $info['filters']['skills']);
?>
		</div>
	</fieldset>



	<fieldset id="fld-way">
		<legend><?=$this->bbf('fld-campaign-filter-way');?></legend>

<?php
	echo $form->checkbox(array('desc'	=> $this->bbf('fm_campaign_filter_way_in'),
		      'name'		  => 'way[in]',
		      'labelid'		=> 'campaign_filter_way_in',
				  'help'	  	=> $this->bbf('hlp_fm_campaign_filter_way_in'),
				  'required'	=> false,
		      'checked'		=> $info['filters']['way']['in'],
		      'default'		=> $element['campaign_filter_way_in']['default'])),

		    $form->checkbox(array('desc'	=> $this->bbf('fm_campaign_filter_way_out'),
		      'name'		  => 'way[out]',
		      'labelid'		=> 'campaign_filter_way_out',
				  'help'	  	=> $this->bbf('hlp_fm_campaign_filter_way_out'),
				  'required'	=> false,
		      'checked'		=> $info['filters']['way']['out'],
					'default'		=> $element['campaign_filter_way_out']['default']));
?>

	</fieldset>
</div>


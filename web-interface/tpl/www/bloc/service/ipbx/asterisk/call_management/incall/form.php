<?php
	$form = &$this->get_module('form');
	$url = &$this->get_module('url');

	$incall = $this->get_var('incall');
	$element = $this->get_var('element');
	$rightcall = $this->get_var('rightcall');
	$list = $this->get_var('list');

	$linked = $incall['linked'];
	$type = $incall['type'];
?>

<div id="sb-part-first">

<?php

echo $form->text(array('desc' => $this->bbf('fm_incall_exten'),'name' => 'incall[exten]','labelid' => 'incall-exten','size' => 15,'default' => $element['incall']['exten']['default'],'value' => $this->get_varra('incall','exten')));

echo $form->select(array('desc' => $this->bbf('fm_incall_type'),'name' => 'incall[type]','labelid' => 'incall-type','bbf' => 'fm_incall_type-opt-','key' => false,'default' => $element['incall']['type']['default'],'value' => $type),$element['incall']['type']['value'],'onchange="xivo_chgtype(this);"');

echo $form->select(array('desc' => $this->bbf('fm_incall_endcall-typeval'),'name' => 'incall[typeval]','labelid' => 'incall-endcall-typeval','bbf' => 'fm_incall_endcall-typeval-opt-','key' => false,'default' => $element['incall']['typeval']['default'],'value' => $incall['endcall']),$element['incall']['endcall']['value']);

if(empty($list['users']) === false):
	
	if($linked === false && $type === 'user'):
		$invalid = true;
	else:
		$invalid = false;
	endif;

	echo $form->select(array('desc' => $this->bbf('fm_incall_user-typeval'),'name' => 'incall[typeval]','labelid' => 'incall-user-typeval','key' => 'identity','altkey' => 'id','invalid' => $invalid,'default' => $element['incall']['typeval']['default'],'value' => $incall['user']),$list['users']);

else:
	echo '<div id="fd-incall-user-typeval" class="txt-center">',$url->href_html($this->bbf('create_user'),'service/ipbx/pbx_settings/users','act=add'),'</div>';
endif;

if(empty($list['groups']) === false):

	if($linked === false && $type === 'group'):
		$invalid = true;
	else:
		$invalid = false;
	endif;

echo $form->select(array('desc' => $this->bbf('fm_incall_group-typeval'),'name' => 'incall[typeval]','labelid' => 'incall-group-typeval','key' => 'identity','altkey' => 'id','invalid' => $invalid,'default' => $element['incall']['typeval']['default'],'value' => $incall['group']),$list['groups']);

else:
	echo '<div id="fd-incall-group-typeval" class="txt-center">',$url->href_html($this->bbf('create_group'),'service/ipbx/pbx_settings/groups','act=add'),'</div>';
endif;

if(empty($list['queues']) === false):

	if($linked === false && $type === 'queue'):
		$invalid = true;
	else:
		$invalid = false;
	endif;

	echo $form->select(array('desc' => $this->bbf('fm_incall_queue-typeval'),'name' => 'incall[typeval]','labelid' => 'incall-queue-typeval','key' => 'identity','altkey' => 'id','invalid' => $invalid,'default' => $element['incall']['typeval']['default'],'value' => $incall['queue']),$list['queues']);

else:
	echo '<div id="fd-incall-queue-typeval" class="txt-center">',$url->href_html($this->bbf('create_queue'),'service/ipbx/pbx_settings/queues','act=add'),'</div>';
endif;

if(empty($list['meetme']) === false):
	
	if($linked === false && $type === 'meetme'):
		$invalid = true;
	else:
		$invalid = false;
	endif;

	echo $form->select(array('desc' => $this->bbf('fm_incall_meetme-typeval'),'name' => 'incall[typeval]','labelid' => 'incall-meetme-typeval','key' => 'identity','altkey' => 'id','invalid' => $invalid,'default' => $element['incall']['typeval']['default'],'value' => $incall['meetme']),$list['meetme']);

else:
	echo '<div id="fd-incall-meetme-typeval" class="txt-center">',$url->href_html($this->bbf('create_meetme'),'service/ipbx/pbx_settings/meetme','act=add'),'</div>';
endif;

if(empty($list['schedule']) === false):
	
	if($linked === false && $type === 'schedule'):
		$invalid = true;
	else:
		$invalid = false;
	endif;

	echo $form->select(array('desc' => $this->bbf('fm_incall_schedule-typeval'),'name' => 'incall[typeval]','labelid' => 'incall-schedule-typeval','key' => 'name','altkey' => 'id','invalid' => $invalid,'default' => $element['incall']['typeval']['default'],'value' => $incall['schedule']),$list['schedule']);

else:
	echo '<div id="fd-incall-schedule-typeval" class="txt-center">',$url->href_html($this->bbf('create_schedule'),'service/ipbx/call_management/schedule','act=add'),'</div>';
endif;

echo	'<div id="fd-incall-application-typeval" class="fm-field">',
	$form->select(array('desc' => $this->bbf('fm_incall_application-typeval'),'name' => 'incall[typeval]','field' => false,'labelid' => 'incall-application-typeval','bbf' => 'fm_incall_application-typeval-opt-','key' => false,'default' => $element['incall']['typeval']['default'],'value' => $incall['application']),$element['incall']['application']['value']),
	$form->text(array('field' => false,'name' => 'incall[applicationval]','labelid' => 'incall-application-applicationval','size' => 15,'value' => $incall['applicationval'])),
	'</div>';

if($list['sounds'] !== false):

	echo $form->select(array('desc' => $this->bbf('fm_incall_sound-typeval'),'name' => 'incall[typeval]','labelid' => 'incall-sound-typeval','default' => $element['incall']['typeval']['default'],'value' => $incall['sound']),$list['sounds']);

else:

	echo '<div id="fd-incall-sound-typeval" class="txt-center">',$url->href_html($this->bbf('add_playback-sound'),'service/ipbx/pbx_services/sounds',array('act' => 'list','dir' => 'playback')),'</div>';

endif;

echo $form->text(array('desc' => $this->bbf('fm_incall_custom-typeval'),'name' => 'incall[typeval]','labelid' => 'incall-custom-typeval','size' => 15,'value' => $incall['custom']));

?>

</div>

<div id="sb-part-last" class="b-nodisplay">

<?php
	if($rightcall['list'] !== false):
?>
		<div id="rightcalllist" class="fm-field fm-multilist">
			<div class="slt-outlist">

		<?=$form->select(array('name' => 'rightcalllist','label' => false,'id' => 'it-rightcalllist','browse' => 'rightcall','key' => 'name','altkey' => 'id','multiple' => true,'size' => 5,'field' => false),$rightcall['list']);?>

			</div>
			<div class="inout-list">

		<a href="#" onclick="xivo_fm_move_selected('it-rightcalllist','it-rightcall'); return(false);" title="<?=$this->bbf('bt-inrightcall');?>"><?=$url->img_html('img/site/button/row-left.gif',$this->bbf('bt-inrightcall'),'class="bt-inlist" id="bt-inrightcall" border="0"');?></a><br />

		<a href="#" onclick="xivo_fm_move_selected('it-rightcall','it-rightcalllist'); return(false);" title="<?=$this->bbf('bt-outrightcall');?>"><?=$url->img_html('img/site/button/row-right.gif',$this->bbf('bt-outrightcall'),'class="bt-outlist" id="bt-outrightcall" border="0"');?></a>

			</div>
			<div class="slt-inlist">

		<?=$form->select(array('name' => 'rightcall[]','label' => false,'id' => 'it-rightcall','browse' => 'rightcall','key' => 'name','altkey' => 'id','multiple' => true,'size' => 5,'field' => false),$rightcall['slt']);?>

			</div>
		</div>
		<div class="clearboth"></div>
<?php
	else:
		echo '<div class="txt-center">',$url->href_html($this->bbf('create_rightcall'),'service/ipbx/call_management/rightcall','act=add'),'</div>';
	endif;
?>

</div>

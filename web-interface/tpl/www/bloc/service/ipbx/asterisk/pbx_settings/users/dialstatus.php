<?php
	$form = &$this->get_module('form');
	$url = &$this->get_module('url');

	$info = $this->vars('info');
	$element = $this->vars('element');
	$list = $this->vars('list');
	$status = $this->vars('status');

	$linked = $this->varra('dialstatus',array($status,'linked'));
	$type = $this->varra('dialstatus',array($status,'type'));

echo $form->select(array('desc' => $this->bbf('fm_dialstatus_type'),'name' => 'dialstatus['.$status.'][type]','labelid' => 'dialstatus-'.$status.'-type','bbf' => 'fm_dialstatus_type-opt-','key' => false,'default' => $element['dialstatus']['type']['default'],'value' => $type),$element['dialstatus']['type']['value'],'onchange="xivo_chgdialstatus(\''.$status.'\',this);"');

echo $form->select(array('desc' => $this->bbf('fm_dialstatus_endcall-typeval'),'name' => 'dialstatus['.$status.'][typeval]','labelid' => 'dialstatus-'.$status.'-endcall-typeval','bbf' => 'fm_dialstatus_endcall-typeval-opt-','key' => false,'default' => $element['dialstatus']['typeval']['default'],'value' => $this->varra('dialstatus',array($status,'endcall'))),$element['dialstatus']['endcall']['value']);

if(empty($list['users']) === false):

	if($linked === false && $type === 'user'):
		$invalid = true;
	else:
		$invalid = false;
	endif;

	echo $form->select(array('desc' => $this->bbf('fm_dialstatus_user-typeval'),'name' => 'dialstatus['.$status.'][typeval]','labelid' => 'dialstatus-'.$status.'-user-typeval','key' => 'identity','altkey' => 'id','invalid' => $invalid,'default' => $element['dialstatus']['typeval']['default'],'value' => $this->varra('dialstatus',array($status,'user'))),$list['users']);

else:

	echo '<div id="fd-dialstatus-'.$status.'-user-typeval" class="txt-center">',$url->href_html($this->bbf('create_user'),'service/ipbx/pbx_settings/users','act=add'),'</div>';

endif;

if(empty($list['groups']) === false):

	if($linked === false && $type === 'group'):
		$invalid = true;
	else:
		$invalid = false;
	endif;

	echo $form->select(array('desc' => $this->bbf('fm_dialstatus_group-typeval'),'name' => 'dialstatus['.$status.'][typeval]','labelid' => 'dialstatus-'.$status.'-group-typeval','key' => 'identity','altkey' => 'id','invalid' => $invalid,'default' => $element['dialstatus']['typeval']['default'],'value' => $this->varra('dialstatus',array($status,'group'))),$list['groups']);

else:

	echo '<div id="fd-dialstatus-'.$status.'-group-typeval" class="txt-center">',$url->href_html($this->bbf('create_group'),'service/ipbx/pbx_settings/groups','act=add'),'</div>';

endif;

if(empty($list['queues']) === false):

	if($linked === false && $type === 'queue'):
		$invalid = true;
	else:
		$invalid = false;
	endif;

	echo $form->select(array('desc' => $this->bbf('fm_dialstatus_queue-typeval'),'name' => 'dialstatus['.$status.'][typeval]','labelid' => 'dialstatus-'.$status.'-queue-typeval','key' => 'identity','altkey' => 'id','invalid' => $invalid,'default' => $element['dialstatus']['typeval']['default'],'value' => $this->varra('dialstatus',array($status,'queue'))),$list['queues']);

else:

		echo '<div id="fd-dialstatus-'.$status.'-queue-typeval" class="txt-center">',$url->href_html($this->bbf('create_queue'),'service/ipbx/pbx_settings/queues','act=add'),'</div>';

endif;

if(empty($list['meetme']) === false):

	if($linked === false && $type === 'meetme'):
		$invalid = true;
	else:
		$invalid = false;
	endif;

	echo $form->select(array('desc' => $this->bbf('fm_dialstatus_meetme-typeval'),'name' => 'dialstatus['.$status.'][typeval]','labelid' => 'dialstatus-'.$status.'-meetme-typeval','key' => 'identity','altkey' => 'id','invalid' => $invalid,'default' => $element['dialstatus']['typeval']['default'],'value' => $this->varra('dialstatus',array($status,'meetme'))),$list['meetme']);

else:

	echo '<div id="fd-dialstatus-'.$status.'-meetme-typeval" class="txt-center">',$url->href_html($this->bbf('create_meetme'),'service/ipbx/pbx_settings/meetme','act=add'),'</div>';

endif;

if(empty($list['schedule']) === false):

	if($linked === false && $type === 'schedule'):
		$invalid = true;
	else:
		$invalid = false;
	endif;

	echo $form->select(array('desc' => $this->bbf('fm_dialstatus_schedule-typeval'),'name' => 'dialstatus['.$status.'][typeval]','labelid' => 'dialstatus-'.$status.'-schedule-typeval','key' => 'name','altkey' => 'id','invalid' => $invalid,'default' => $element['dialstatus']['typeval']['default'],'value' => $this->varra('dialstatus',array($status,'schedule'))),$list['schedule']);

else:

	echo '<div id="fd-dialstatus-'.$status.'-schedule-typeval" class="txt-center">',$url->href_html($this->bbf('create_schedule'),'service/ipbx/call_management/schedule','act=add'),'</div>';

endif;

echo $form->select(array('desc' => $this->bbf('fm_dialstatus_application-typeval'),'name' => 'dialstatus['.$status.'][typeval]','labelid' => 'dialstatus-'.$status.'-application-typeval','bbf' => 'fm_dialstatus_application-typeval-opt-','key' => false,'default' => $element['dialstatus']['typeval']['default'],'value' => $this->varra('dialstatus',array($status,'application'))),$element['dialstatus']['application']['value']);

if($list['sounds'] !== false):

	echo $form->select(array('desc' => $this->bbf('fm_dialstatus_sound-typeval'),'name' => 'dialstatus['.$status.'][typeval]','labelid' => 'dialstatus-'.$status.'-sound-typeval','default' => $element['dialstatus']['typeval']['default'],'value' => $this->varra('dialstatus',array($status,'sound'))),$list['sounds']);

else:

	echo '<div class="txt-center">',$url->href_html($this->bbf('add_dialstatus-sound'),'service/ipbx/general_settings/sounds',array('act' => 'list','dir' => 'dialstatus')),'</div>';

endif;

	echo $form->text(array('desc' => $this->bbf('fm_dialstatus_custom-typeval'),'name' => 'dialstatus['.$status.'][typeval]','labelid' => 'dialstatus-'.$status.'-custom-typeval','size' => 15,'value' => $this->varra('dialstatus',array($status,'custom'))));

?>

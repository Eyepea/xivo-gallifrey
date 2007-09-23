<?php
	$form = &$this->get_module('form');
	$url = &$this->get_module('url');

	$info = $this->vars('info');
	$element = $this->vars('element');
	$list = $this->vars('list');
?>

<?=$form->text(array('desc' => $this->bbf('fm_extenumbers_exten'),'name' => 'extenumbers[exten]','labelid' => 'extenumbers-exten','size' => 15,'default' => $element['extenumbers']['exten']['default'],'value' => $info['extenumbers']['exten']));?>

<?php
	echo $form->select(array('desc' => $this->bbf('fm_incall_type'),'name' => 'incall[type]','labelid' => 'incall-type','bbf' => 'fm_incall_type-opt-','key' => false,'default' => $element['incall']['type']['default'],'value' => $info['incall']['type']),$element['incall']['type']['value'],'onchange="xivo_chgtype(this);"');

	echo $form->select(array('desc' => $this->bbf('fm_incall_endcall-typeval'),'name' => 'incall[typeval]','labelid' => 'incall-endcall-typeval','bbf' => 'fm_incall_endcall-typeval-opt-','key' => false,'default' => $element['incall']['typeval']['default'],'value' => $info['incall']['endcall']),$element['incall']['endcall']['value']);

	if(empty($list['users']) === false):
	
		if($info['incall']['linked'] === false && $info['incall']['type'] === 'user'):
			$invalid = true;
		else:
			$invalid = false;
		endif;

	echo $form->select(array('desc' => $this->bbf('fm_incall_user-typeval'),'name' => 'incall[typeval]','labelid' => 'incall-user-typeval','key' => 'identity','altkey' => 'id','invalid' => $invalid,'default' => $element['incall']['typeval']['default'],'value' => $info['incall']['user']),$list['users']);

	else:
		echo '<div id="fd-incall-user-typeval" class="txt-center">',$url->href_html($this->bbf('create_user'),'service/ipbx/pbx_settings/users','act=add'),'</div>';
	endif;

	if(empty($list['groups']) === false):
	
		if($info['incall']['linked'] === false && $info['incall']['type'] === 'group'):
			$invalid = true;
		else:
			$invalid = false;
		endif;

	echo $form->select(array('desc' => $this->bbf('fm_incall_group-typeval'),'name' => 'incall[typeval]','labelid' => 'incall-group-typeval','key' => 'identity','altkey' => 'id','invalid' => $invalid,'default' => $element['incall']['typeval']['default'],'value' => $info['incall']['group']),$list['groups']);

	else:
		echo '<div id="fd-incall-group-typeval" class="txt-center">',$url->href_html($this->bbf('create_group'),'service/ipbx/pbx_settings/groups','act=add'),'</div>';
	endif;

	if(empty($list['queues']) === false):
	
		if($info['incall']['linked'] === false && $info['incall']['type'] === 'queue'):
			$invalid = true;
		else:
			$invalid = false;
		endif;

	echo $form->select(array('desc' => $this->bbf('fm_incall_queue-typeval'),'name' => 'incall[typeval]','labelid' => 'incall-queue-typeval','key' => 'identity','altkey' => 'id','invalid' => $invalid,'default' => $element['incall']['typeval']['default'],'value' => $info['incall']['queue']),$list['queues']);

	else:
		echo '<div id="fd-incall-queue-typeval" class="txt-center">',$url->href_html($this->bbf('create_queue'),'service/ipbx/pbx_settings/queues','act=add'),'</div>';
	endif;

	if(empty($list['meetme']) === false):
	
		if($info['incall']['linked'] === false && $info['incall']['type'] === 'meetme'):
			$invalid = true;
		else:
			$invalid = false;
		endif;

	echo $form->select(array('desc' => $this->bbf('fm_incall_meetme-typeval'),'name' => 'incall[typeval]','labelid' => 'incall-meetme-typeval','key' => 'identity','altkey' => 'id','invalid' => $invalid,'default' => $element['incall']['typeval']['default'],'value' => $info['incall']['meetme']),$list['meetme']);

	else:
		echo '<div id="fd-incall-meetme-typeval" class="txt-center">',$url->href_html($this->bbf('create_meetme'),'service/ipbx/pbx_settings/meetme','act=add'),'</div>';
	endif;

	if(empty($list['schedule']) === false):
	
		if($info['incall']['linked'] === false && $info['incall']['type'] === 'schedule'):
			$invalid = true;
		else:
			$invalid = false;
		endif;

	echo $form->select(array('desc' => $this->bbf('fm_incall_schedule-typeval'),'name' => 'incall[typeval]','labelid' => 'incall-schedule-typeval','key' => 'name','altkey' => 'id','invalid' => $invalid,'default' => $element['incall']['typeval']['default'],'value' => $info['incall']['schedule']),$list['schedule']);

	else:
		echo '<div id="fd-incall-schedule-typeval" class="txt-center">',$url->href_html($this->bbf('create_schedule'),'service/ipbx/call_management/schedule','act=add'),'</div>';
	endif;

	echo $form->select(array('desc' => $this->bbf('fm_incall_application-typeval'),'name' => 'incall[typeval]','labelid' => 'incall-application-typeval','bbf' => 'fm_incall_application-typeval-opt-','key' => false,'default' => $element['incall']['typeval']['default'],'value' => $info['incall']['application']),$element['incall']['application']['value']);

	echo $form->text(array('desc' => $this->bbf('fm_incall_custom-typeval'),'name' => 'incall[typeval]','labelid' => 'incall-custom-typeval','size' => 15,'value' => $info['incall']['custom']));
?>

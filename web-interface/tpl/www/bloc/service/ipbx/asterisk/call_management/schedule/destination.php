<?php
	$form = &$this->get_module('form');
	$url = &$this->get_module('url');

	$info = $this->vars('info');
	$element = $this->vars('element');
	$list = $this->vars('list');
	$typename = $this->vars('typename');

	if($typename === 'true'):
		$typeval = 'typetrue';
		$typevalname = 'typevaltrue';
	else:
		$typeval = 'typefalse';
		$typevalname = 'typevalfalse';
	endif;

	$linked = $info['schedule']['linked'];
	$type = $info['schedule'][$typeval];
?>

<?=$form->select(array('desc' => $this->bbf('fm_schedule_type'),'name' => 'schedule['.$typeval.']','labelid' => 'schedule-'.$typeval,'bbf' => 'fm_schedule_type-opt-','key' => false,'default' => $element['schedule'][$typeval]['default'],'value' => $type),$element['schedule'][$typeval]['value'],'onchange="xivo_chg'.$typeval.'(this);"');?>

<?=$form->select(array('desc' => $this->bbf('fm_schedule_endcall-typeval'),'name' => 'schedule['.$typevalname.']','labelid' => 'schedule-endcall-'.$typevalname,'bbf' => 'fm_schedule_endcall-typeval-opt-','key' => false,'default' => $element['schedule'][$typevalname]['default'],'value' => $info['schedule']['endcall'][$typename]),$element['schedule']['endcall']['value']);?>

<?php

if(empty($list['users']) === false):

	if($linked === false && $type === 'user'):
		$invalid = true;
	else:
		$invalid = false;
	endif;

	echo $form->select(array('desc' => $this->bbf('fm_schedule_user-typeval'),'name' => 'schedule['.$typevalname.']','labelid' => 'schedule-user-'.$typevalname,'key' => 'identity','altkey' => 'id','invalid' => $invalid,'default' => $element['schedule'][$typevalname]['default'],'value' => $info['schedule']['user'][$typename]),$list['users']);

else:

	echo '<div id="fd-schedule-user-'.$typevalname.'" class="txt-center">',$url->href_html($this->bbf('create_user'),'service/ipbx/pbx_settings/users','act=add'),'</div>';

endif;

if(empty($list['groups']) === false):

	if($linked === false && $type === 'group'):
		$invalid = true;
	else:
		$invalid = false;
	endif;

	echo $form->select(array('desc' => $this->bbf('fm_schedule_group-typeval'),'name' => 'schedule['.$typevalname.']','labelid' => 'schedule-group-'.$typevalname,'key' => 'identity','altkey' => 'id','invalid' => $invalid,'default' => $element['schedule'][$typevalname]['default'],'value' => $info['schedule']['group'][$typename]),$list['groups']);

else:

		echo '<div id="fd-schedule-group-'.$typevalname.'" class="txt-center">',$url->href_html($this->bbf('create_group'),'service/ipbx/pbx_settings/groups','act=add'),'</div>';

endif;

if(empty($list['queues']) === false):

	if($linked === false && $type === 'queue'):
		$invalid = true;
	else:
		$invalid = false;
	endif;

	echo $form->select(array('desc' => $this->bbf('fm_schedule_queue-typeval'),'name' => 'schedule['.$typevalname.']','labelid' => 'schedule-queue-'.$typevalname,'key' => 'identity','altkey' => 'id','invalid' => $invalid,'default' => $element['schedule'][$typevalname]['default'],'value' => $info['schedule']['queue'][$typename]),$list['queues']);

else:

	echo '<div id="fd-schedule-queue-'.$typevalname.'" class="txt-center">',$url->href_html($this->bbf('create_queue'),'service/ipbx/pbx_settings/queues','act=add'),'</div>';

endif;

if(empty($list['meetme']) === false):

	if($linked === false && $type === 'meetme'):
		$invalid = true;
	else:
		$invalid = false;
	endif;

	echo $form->select(array('desc' => $this->bbf('fm_schedule_meetme-typeval'),'name' => 'schedule['.$typevalname.']','labelid' => 'schedule-meetme-'.$typevalname,'key' => 'identity','altkey' => 'id','invalid' => $invalid,'default' => $element['schedule'][$typevalname]['default'],'value' => $info['schedule']['meetme'][$typename]),$list['meetme']);

else:

	echo '<div id="fd-schedule-meetme-'.$typevalname.'" class="txt-center">',$url->href_html($this->bbf('create_meetme'),'service/ipbx/pbx_settings/meetme','act=add'),'</div>';

endif;

if(empty($list['schedule']) === false):

	if($linked === false && $type === 'schedule'):
		$invalid = true;
	else:
		$invalid = false;
	endif;

	echo $form->select(array('desc' => $this->bbf('fm_schedule_schedule-typeval'),'name' => 'schedule['.$typevalname.']','labelid' => 'schedule-schedule-'.$typevalname,'key' => 'name','altkey' => 'id','invalid' => $invalid,'default' => $element['schedule'][$typevalname]['default'],'value' => $info['schedule']['schedule'][$typename]),$list['schedule']);

else:

	echo '<div id="fd-schedule-schedule-'.$typevalname.'" class="txt-center">',$this->bbf('no_schedule'),'</div>';

endif;

echo $form->select(array('desc' => $this->bbf('fm_schedule_application-typeval'),'name' => 'schedule['.$typevalname.']','labelid' => 'schedule-application-'.$typevalname,'bbf' => 'fm_schedule_application-typeval-opt-','key' => false,'default' => $element['schedule'][$typevalname]['default'],'value' => $info['schedule']['application'][$typename]),$element['schedule']['application']['value']);

echo $form->text(array('desc' => $this->bbf('fm_schedule_custom-typeval'),'name' => 'schedule['.$typevalname.']','labelid' => 'schedule-custom-'.$typevalname,'size' => 15,'value' => $info['schedule']['custom'][$typename]));

?>

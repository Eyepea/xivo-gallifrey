<?php
	$form = &$this->get_module('form');
	$url = &$this->get_module('url');

	$info = $this->vars('info');
	$element = $this->vars('element');
	$list = $this->vars('list');
?>

<?=$form->text(array('desc' => $this->bbf('fm_schedule_name'),'name' => 'schedule[name]','labelid' => 'schedule-name','size' => 15,'default' => $element['schedule']['name']['default'],'value' => $info['schedule']['name']));?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_schedule_publicholiday'),'name' => 'schedule[publicholiday]','labelid' => 'publicholiday','checked' => $info['schedule']['publicholiday'],'default' => $element['schedule']['publicholiday']['default']));?>

<div class="fm-field fm-multifield">
<?=$form->select(array('field' => false,'desc' => $this->bbf('fm_schedule_timebeg'),'name' => 'schedule[timebeg][hour]','labelid' => 'schedule-timehourbeg','empty' => true,'key' => false,'default' => $element['schedule']['timehourbeg']['default'],'value' => $info['schedule']['timehourbeg']),$element['schedule']['timehourbeg']['value']);?>
<?=$form->select(array('field' => false,'name' => 'schedule[timebeg][min]','labelid' => 'schedule-timeminbeg','empty' => true,'key' => false,'default' => $element['schedule']['timeminbeg']['default'],'value' => $info['schedule']['timeminbeg']),$element['schedule']['timeminbeg']['value']);?>
</div>

<div class="fm-field fm-multifield">
<?=$form->select(array('field' => false,'desc' => $this->bbf('fm_schedule_timeend'),'name' => 'schedule[timeend][hour]','labelid' => 'schedule-timehourend','empty' => true,'key' => false,'default' => $element['schedule']['timehourend']['default'],'value' => $info['schedule']['timehourend']),$element['schedule']['timehourend']['value']);?>
<?=$form->select(array('field' => false,'name' => 'schedule[timeend][min]','labelid' => 'schedule-timeminend','empty' => true,'key' => false,'default' => $element['schedule']['timeminend']['default'],'value' => $info['schedule']['timeminend']),$element['schedule']['timeminend']['value']);?>
</div>

<div class="fm-field fm-desc-inline">
<div class="fm-multifield">
<?=$form->select(array('field' => false,'desc' => $this->bbf('fm_schedule_daynamebeg'),'name' => 'schedule[daynamebeg]','labelid' => 'schedule-daynamebeg','empty' => true,'key' => false,'bbf' => 'date_Day_','default' => $element['schedule']['daynamebeg']['default'],'value' => $info['schedule']['daynamebeg']),$element['schedule']['daynamebeg']['value']);?>
</div>

<div class="fm-multifield">
<?=$form->select(array('field' => false,'desc' => $this->bbf('fm_schedule_daynameend'),'name' => 'schedule[daynameend]','labelid' => 'schedule-daynameend','empty' => true,'key' => false,'bbf' => 'date_Day_','default' => $element['schedule']['daynameend']['default'],'value' => $info['schedule']['daynameend']),$element['schedule']['daynameend']['value']);?>
</div>
</div>

<div class="fm-field fm-desc-inline">
<div class="fm-multifield">
<?=$form->select(array('field' => false,'desc' => $this->bbf('fm_schedule_daynumbeg'),'name' => 'schedule[daynumbeg]','labelid' => 'schedule-daynumbeg','empty' => true,'key' => false,'default' => $element['schedule']['daynumbeg']['default'],'value' => $info['schedule']['daynumbeg']),$element['schedule']['daynumbeg']['value']);?>
</div>

<div class="fm-multifield">
<?=$form->select(array('field' => false,'desc' => $this->bbf('fm_schedule_daynumend'),'name' => 'schedule[daynumend]','labelid' => 'schedule-daynumend','empty' => true,'key' => false,'default' => $element['schedule']['daynumend']['default'],'value' => $info['schedule']['daynumend']),$element['schedule']['daynumend']['value']);?>
</div>
</div>

<div class="fm-field fm-desc-inline">
<div class="fm-multifield">
<?=$form->select(array('field' => false,'desc' => $this->bbf('fm_schedule_monthbeg'),'name' => 'schedule[monthbeg]','labelid' => 'schedule-monthbeg','empty' => true,'key' => false,'bbf' => 'date_Month_','default' => $element['schedule']['monthbeg']['default'],'value' => $info['schedule']['monthbeg']),$element['schedule']['monthbeg']['value']);?>
</div>

<div class="fm-multifield">
<?=$form->select(array('field' => false,'desc' => $this->bbf('fm_schedule_monthend'),'name' => 'schedule[monthend]','labelid' => 'schedule-monthend','empty' => true,'key' => false,'bbf' => 'date_Month_','default' => $element['schedule']['monthend']['default'],'value' => $info['schedule']['monthend']),$element['schedule']['monthend']['value']);?>
</div>
</div>

<fieldset id="fld-typetrue">
	<legend><?=$this->bbf('fld-inschedule');?></legend>

<?php
	echo $form->select(array('desc' => $this->bbf('fm_schedule_typetrue'),'name' => 'schedule[typetrue]','labelid' => 'schedule-typetrue','bbf' => 'fm_schedule_typetrue-opt-','key' => false,'default' => $element['schedule']['typetrue']['default'],'value' => $info['schedule']['typetrue']),$element['schedule']['typetrue']['value'],'onchange="xivo_chgtypetrue(this);"');

	echo $form->select(array('desc' => $this->bbf('fm_schedule_endcall-typevaltrue'),'name' => 'schedule[typevaltrue]','labelid' => 'schedule-endcall-typevaltrue','bbf' => 'fm_schedule_endcall-typevaltrue-opt-','key' => false,'default' => $element['schedule']['typevaltrue']['default'],'value' => $info['schedule']['endcall']['true']),$element['schedule']['endcall']['value']);

	if(empty($list['users']) === false):

		if($info['schedule']['linked'] === false && $info['schedule']['typetrue'] === 'user'):
			$invalid = true;
		else:
			$invalid = false;
		endif;
	
	echo $form->select(array('desc' => $this->bbf('fm_schedule_user-typevaltrue'),'name' => 'schedule[typevaltrue]','labelid' => 'schedule-user-typevaltrue','key' => 'identity','altkey' => 'id','invalid' => $invalid,'default' => $element['schedule']['typevaltrue']['default'],'value' => $info['schedule']['user']['true']),$list['users']);

	else:
		echo '<div id="fd-schedule-user-typevaltrue" class="txt-center">',$url->href_html($this->bbf('create_user'),'service/ipbx/pbx_settings/users','act=add'),'</div>';
	endif;

	if(empty($list['groups']) === false):

		if($info['schedule']['linked'] === false && $info['schedule']['typetrue'] === 'group'):
			$invalid = true;
		else:
			$invalid = false;
		endif;

	echo $form->select(array('desc' => $this->bbf('fm_schedule_group-typevaltrue'),'name' => 'schedule[typevaltrue]','labelid' => 'schedule-group-typevaltrue','key' => 'identity','altkey' => 'id','invalid' => $invalid,'default' => $element['schedule']['typevaltrue']['default'],'value' => $info['schedule']['group']['true']),$list['groups']);

	else:
		echo '<div id="fd-schedule-group-typevaltrue" class="txt-center">',$url->href_html($this->bbf('create_group'),'service/ipbx/pbx_settings/groups','act=add'),'</div>';
	endif;

	if(empty($list['queues']) === false):

		if($info['schedule']['linked'] === false && $info['schedule']['typetrue'] === 'queue'):
			$invalid = true;
		else:
			$invalid = false;
		endif;

	echo $form->select(array('desc' => $this->bbf('fm_schedule_queue-typevaltrue'),'name' => 'schedule[typevaltrue]','labelid' => 'schedule-queue-typevaltrue','key' => 'identity','altkey' => 'id','invalid' => $invalid,'default' => $element['schedule']['typevaltrue']['default'],'value' => $info['schedule']['queue']['true']),$list['queues']);

	else:
		echo '<div id="fd-schedule-queue-typevaltrue" class="txt-center">',$url->href_html($this->bbf('create_queue'),'service/ipbx/pbx_settings/queues','act=add'),'</div>';
	endif;

	if(empty($list['meetme']) === false):

		if($info['schedule']['linked'] === false && $info['schedule']['typetrue'] === 'meetme'):
			$invalid = true;
		else:
			$invalid = false;
		endif;

	echo $form->select(array('desc' => $this->bbf('fm_schedule_meetme-typevaltrue'),'name' => 'schedule[typevaltrue]','labelid' => 'schedule-meetme-typevaltrue','key' => 'identity','altkey' => 'id','invalid' => $invalid,'default' => $element['schedule']['typevaltrue']['default'],'value' => $info['schedule']['meetme']['true']),$list['meetme']);

	else:
		echo '<div id="fd-schedule-meetme-typevaltrue" class="txt-center">',$url->href_html($this->bbf('create_meetme'),'service/ipbx/pbx_settings/meetme','act=add'),'</div>';
	endif;

	if(empty($list['schedule']) === false):

		if($info['schedule']['linked'] === false && $info['schedule']['typetrue'] === 'schedule'):
			$invalid = true;
		else:
			$invalid = false;
		endif;

	echo $form->select(array('desc' => $this->bbf('fm_schedule_schedule-typevaltrue'),'name' => 'schedule[typevaltrue]','labelid' => 'schedule-schedule-typevaltrue','key' => 'name','altkey' => 'id','invalid' => $invalid,'default' => $element['schedule']['typevaltrue']['default'],'value' => $info['schedule']['schedule']['true']),$list['schedule']);

	else:
		echo '<div id="fd-schedule-schedule-typevaltrue" class="txt-center">',$this->bbf('no_schedule'),'</div>';
	endif;

	echo $form->select(array('desc' => $this->bbf('fm_schedule_application-typevaltrue'),'name' => 'schedule[typevaltrue]','labelid' => 'schedule-application-typevaltrue','bbf' => 'fm_schedule_application-typevaltrue-opt-','key' => false,'default' => $element['schedule']['typevaltrue']['default'],'value' => $info['schedule']['application']['true']),$element['schedule']['application']['value']);

	echo $form->text(array('desc' => $this->bbf('fm_schedule_custom-typevaltrue'),'name' => 'schedule[typevaltrue]','labelid' => 'schedule-custom-typevaltrue','size' => 15,'value' => $info['schedule']['custom']['true']));
?>

</fieldset>

<fieldset id="fld-typefalse">
	<legend><?=$this->bbf('fld-outschedule');?></legend>

<?=$form->select(array('desc' => $this->bbf('fm_schedule_typefalse'),'name' => 'schedule[typefalse]','labelid' => 'schedule-typefalse','bbf' => 'fm_schedule_typefalse-opt-','key' => false,'default' => $element['schedule']['typefalse']['default'],'value' => $info['schedule']['typefalse']),$element['schedule']['typefalse']['value'],'onchange="xivo_chgtypefalse(this);"');?>

<?=$form->select(array('desc' => $this->bbf('fm_schedule_endcall-typevalfalse'),'name' => 'schedule[typevalfalse]','labelid' => 'schedule-endcall-typevalfalse','bbf' => 'fm_schedule_endcall-typevalfalse-opt-','key' => false,'default' => $element['schedule']['typevalfalse']['default'],'value' => $info['schedule']['endcall']['false']),$element['schedule']['endcall']['value']);?>

<?php
	if(empty($list['users']) === false):

		if($info['schedule']['linked'] === false && $info['schedule']['typefalse'] === 'user'):
			$invalid = true;
		else:
			$invalid = false;
		endif;

	echo $form->select(array('desc' => $this->bbf('fm_schedule_user-typevalfalse'),'name' => 'schedule[typevalfalse]','labelid' => 'schedule-user-typevalfalse','key' => 'identity','altkey' => 'id','invalid' => $invalid,'default' => $element['schedule']['typevalfalse']['default'],'value' => $info['schedule']['user']['false']),$list['users']);

	else:
		echo '<div id="fd-schedule-user-typevalfalse" class="txt-center">',$url->href_html($this->bbf('create_user'),'service/ipbx/pbx_settings/users','act=add'),'</div>';
	endif;

	if(empty($list['groups']) === false):

		if($info['schedule']['linked'] === false && $info['schedule']['typefalse'] === 'group'):
			$invalid = true;
		else:
			$invalid = false;
		endif;

	echo $form->select(array('desc' => $this->bbf('fm_schedule_group-typevalfalse'),'name' => 'schedule[typevalfalse]','labelid' => 'schedule-group-typevalfalse','key' => 'identity','altkey' => 'id','invalid' => $invalid,'default' => $element['schedule']['typevalfalse']['default'],'value' => $info['schedule']['group']['false']),$list['groups']);

	else:
		echo '<div id="fd-schedule-group-typevalfalse" class="txt-center">',$url->href_html($this->bbf('create_group'),'service/ipbx/pbx_settings/groups','act=add'),'</div>';
	endif;

	if(empty($list['queues']) === false):

		if($info['schedule']['linked'] === false && $info['schedule']['typefalse'] === 'queue'):
			$invalid = true;
		else:
			$invalid = false;
		endif;

	echo $form->select(array('desc' => $this->bbf('fm_schedule_queue-typevalfalse'),'name' => 'schedule[typevalfalse]','labelid' => 'schedule-queue-typevalfalse','key' => 'identity','altkey' => 'id','invalid' => $invalid,'default' => $element['schedule']['typevalfalse']['default'],'value' => $info['schedule']['queue']['false']),$list['queues']);

	else:
		echo '<div id="fd-schedule-queue-typevalfalse" class="txt-center">',$url->href_html($this->bbf('create_queue'),'service/ipbx/pbx_settings/queues','act=add'),'</div>';
	endif;

	if(empty($list['meetme']) === false):

		if($info['schedule']['linked'] === false && $info['schedule']['typefalse'] === 'meetme'):
			$invalid = true;
		else:
			$invalid = false;
		endif;

	echo $form->select(array('desc' => $this->bbf('fm_schedule_meetme-typevalfalse'),'name' => 'schedule[typevalfalse]','labelid' => 'schedule-meetme-typevalfalse','key' => 'identity','altkey' => 'id','invalid' => $invalid,'default' => $element['schedule']['typevalfalse']['default'],'value' => $info['schedule']['meetme']['false']),$list['meetme']);

	else:
		echo '<div id="fd-schedule-meetme-typevalfalse" class="txt-center">',$url->href_html($this->bbf('create_meetme'),'service/ipbx/pbx_settings/meetme','act=add'),'</div>';
	endif;

	if(empty($list['schedule']) === false):

		if($info['schedule']['linked'] === false && $info['schedule']['typefalse'] === 'schedule'):
			$invalid = true;
		else:
			$invalid = false;
		endif;

	echo $form->select(array('desc' => $this->bbf('fm_schedule_schedule-typevalfalse'),'name' => 'schedule[typevalfalse]','labelid' => 'schedule-schedule-typevalfalse','key' => 'name','altkey' => 'id','invalid' => $invalid,'default' => $element['schedule']['typevalfalse']['default'],'value' => $info['schedule']['schedule']['false']),$list['schedule']);

	else:
		echo '<div id="fd-schedule-schedule-typevalfalse" class="txt-center">',$this->bbf('no_schedule'),'</div>';
	endif;

	echo $form->select(array('desc' => $this->bbf('fm_schedule_application-typevalfalse'),'name' => 'schedule[typevalfalse]','labelid' => 'schedule-application-typevalfalse','bbf' => 'fm_schedule_application-typevalfalse-opt-','key' => false,'default' => $element['schedule']['typevalfalse']['default'],'value' => $info['schedule']['application']['false']),$element['schedule']['application']['value']);

	echo $form->text(array('desc' => $this->bbf('fm_schedule_custom-typevalfalse'),'name' => 'schedule[typevalfalse]','labelid' => 'schedule-custom-typevalfalse','size' => 15,'value' => $info['schedule']['custom']['false']));

?>

</fieldset>

<?php
	$form = &$this->get_module('form');
	$url = &$this->get_module('url');

	$info = $this->get_var('info');
	$element = $this->get_var('element');
	$list = $this->get_var('list');
	$context_list = $this->get_var('context_list');

	$linked = $info['schedule']['linked'];
	$typetrue = $info['schedule']['typetrue'];
	$typefalse = $info['schedule']['typefalse'];
?>

<?=$form->text(array('desc' => $this->bbf('fm_schedule_name'),'name' => 'schedule[name]','labelid' => 'schedule-name','size' => 15,'default' => $element['schedule']['name']['default'],'value' => $info['schedule']['name']));?>

<?php

if($context_list !== false):
	echo $form->select(array('desc' => $this->bbf('fm_schedule_context'),'name' => 'schedule[context]','labelid' => 'schedule-context','key' => 'identity','altkey' => 'name','default' => $element['schedule']['context']['default'],'value' => $info['schedule']['context']),$context_list);
else:
	echo '<div id="fd-schedule-context" class="txt-center">',$url->href_html($this->bbf('create_context'),'service/ipbx/system_management/context','act=add'),'</div>';
endif;

?>

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
	<?=$this->file_include('bloc/service/ipbx/asterisk/call_management/schedule/destination',array('typename' => 'true'));?>
</fieldset>

<fieldset id="fld-typefalse">
	<legend><?=$this->bbf('fld-outschedule');?></legend>
	<?=$this->file_include('bloc/service/ipbx/asterisk/call_management/schedule/destination',array('typename' => 'false'));?>
</fieldset>

<?php
	$form = &$this->get_module('form');

	$moh_list = $this->vars('moh_list');
	$info = $this->vars('info');
	$element = $this->vars('element');
?>

<?=$form->text(array('desc' => $this->bbf('fm_meetmefeatures_name'),'name' => 'mfeatures[name]','labelid' => 'mfeatures-pin','size' => 25,'default' => $element['mfeatures']['name']['default'],'value' => $info['mfeatures']['name']));?>

<?=$form->text(array('desc' => $this->bbf('fm_meetmeroom_number'),'name' => 'meetmeroom[number]','labelid' => 'meetmeroom-number','label' => 'lb-meetmeroom-number','size' => 25,'default' => $element['meetmeroom']['number']['default'],'value' => $info['meetmeroom']['number']));?>

<?=$form->text(array('desc' => $this->bbf('fm_meetmeroom_pin'),'name' => 'meetmeroom[pin]','labelid' => 'meetmeroom-pin','size' => 25,'default' => $element['meetmeroom']['pin']['default'],'value' => $info['meetmeroom']['pin']));?>

<?=$form->text(array('desc' => $this->bbf('fm_meetmeroom_admin-pin'),'name' => 'meetmeroom[admin-pin]','labelid' => 'meetmeroom-admin-pin','size' => 25,'default' => $element['meetmeroom']['admin-pin']['default'],'value' => $info['meetmeroom']['admin-pin']));?>

<?=$form->select(array('desc' => $this->bbf('fm_meetmefeatures_mode'),'name' => 'mfeatures[mode]','labelid' => 'mfeatures-mode','bbf' => 'fm_meetmefeatures_mode-','key' => false,'default' => $element['mfeatures']['mode']['default'],'value' => $info['mfeatures']['mode']),$element['mfeatures']['mode']['value']);?>

<?php
	if($moh_list !== false):
		echo $form->select(array('desc' => $this->bbf('fm_meetmefeatures_musiconhold'),'name' => 'mfeatures[musiconhold]','labelid' => 'mfeatures-musiconhold','key' => 'category','empty' => true,'default' => $element['mfeatures']['musiconhold']['default'],'value' => $info['mfeatures']['musiconhold']),$moh_list);
	endif;
?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_meetmefeatures_exit'),'name' => 'mfeatures[exit]','labelid' => 'mfeatures-exit','default' => $element['mfeatures']['exit']['default'],'checked' => $info['mfeatures']['exit']));?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_meetmefeatures_quiet'),'name' => 'mfeatures[quiet]','labelid' => 'mfeatures-quiet','default' => $element['mfeatures']['quiet']['default'],'checked' => $info['mfeatures']['quiet']));?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_meetmefeatures_record'),'name' => 'mfeatures[record]','labelid' => 'mfeatures-record','default' => $element['mfeatures']['record']['default'],'checked' => $info['mfeatures']['record']));?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_meetmefeatures_video'),'name' => 'mfeatures[video]','labelid' => 'mfeatures-video','default' => $element['mfeatures']['video']['default'],'checked' => $info['mfeatures']['video']));?>


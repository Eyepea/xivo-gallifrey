<?php
	$form = &$this->get_module('form');

	$moh_list = $this->vars('moh_list');
	$info = $this->vars('info');
	$element = $this->vars('element');
?>

<?=$form->text(array('desc' => $this->bbf('fm_meetmefeatures_name'),'name' => 'mfeatures[name]','labelid' => 'mfeatures-pin','size' => 25,'default' => $element['mfeatures']['name']['default'],'value' => $info['mfeatures']['name']),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->text(array('desc' => $this->bbf('fm_meetme_number'),'name' => 'meetme[number]','labelid' => 'meetme-number','label' => 'lb-meetme-number','size' => 25,'default' => $element['meetme']['number']['default'],'value' => $info['meetme']['number']),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->text(array('desc' => $this->bbf('fm_meetme_pin'),'name' => 'meetme[pin]','labelid' => 'meetme-pin','size' => 25,'default' => $element['meetme']['pin']['default'],'value' => $info['meetme']['pin']),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->text(array('desc' => $this->bbf('fm_meetme_admin-pin'),'name' => 'meetme[admin-pin]','labelid' => 'meetme-admin-pin','size' => 25,'default' => $element['meetme']['admin-pin']['default'],'value' => $info['meetme']['admin-pin']),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->slt(array('desc' => $this->bbf('fm_meetmefeatures_mode'),'name' => 'mfeatures[mode]','labelid' => 'mfeatures-mode','bbf' => 'fm_meetmefeatures_mode-','key' => false,'default' => $element['mfeatures']['mode']['default'],'value' => $info['mfeatures']['mode']),$element['mfeatures']['mode']['value'],'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?php
	if($moh_list !== false):
		echo $form->slt(array('desc' => $this->bbf('fm_meetmefeatures_musiconhold'),'name' => 'mfeatures[musiconhold]','labelid' => 'mfeatures-musiconhold','key' => 'category','empty' => true,'default' => $element['mfeatures']['musiconhold']['default'],'value' => $info['mfeatures']['musiconhold']),$moh_list,'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');
	endif;
?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_meetmefeatures_exit'),'name' => 'mfeatures[exit]','labelid' => 'mfeatures-exit','default' => $element['mfeatures']['exit']['default'],'checked' => $info['mfeatures']['exit']),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_meetmefeatures_quiet'),'name' => 'mfeatures[quiet]','labelid' => 'mfeatures-quiet','default' => $element['mfeatures']['quiet']['default'],'checked' => $info['mfeatures']['quiet']),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_meetmefeatures_record'),'name' => 'mfeatures[record]','labelid' => 'mfeatures-record','default' => $element['mfeatures']['record']['default'],'checked' => $info['mfeatures']['record']),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_meetmefeatures_video'),'name' => 'mfeatures[video]','labelid' => 'mfeatures-video','default' => $element['mfeatures']['video']['default'],'checked' => $info['mfeatures']['video']),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>


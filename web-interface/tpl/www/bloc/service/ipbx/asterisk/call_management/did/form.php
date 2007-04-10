<?php
	$form = &$this->get_module('form');
	$info = $this->vars('info');
	$element = $this->vars('element');
	$list = $this->vars('list');
?>

<?=$form->text(array('desc' => $this->bbf('fm_extenumbers_number'),'name' => 'extenumbers[number]','labelid' => 'extenumbers-number','size' => 15,'default' => $element['extenumbers']['number']['default'],'value' => $info['extenumbers']['number']),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->slt(array('desc' => $this->bbf('fm_dfeatures_type'),'name' => 'dfeatures[type]','labelid' => 'dfeatures-type','key' => false,'default' => $element['dfeatures']['type']['default'],'value' => $info['dfeatures']['type']),$element['dfeatures']['type']['value'],'onchange="xivo_chgtype(this);" onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->slt(array('desc' => $this->bbf('fm_dfeatures_typeid'),'name' => 'dfeatures[typeid]','labelid' => 'dfeatures-user-typeid','key' => 'number','key_val' => 'id','default' => $element['dfeatures']['typeid']['default'],'value' => $info['dfeatures']['typeid']),$list['users'],'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->slt(array('desc' => $this->bbf('fm_dfeatures_typeid'),'name' => 'dfeatures[typeid]','labelid' => 'dfeatures-group-typeid','key' => 'number','key_val' => 'id','default' => $element['dfeatures']['typeid']['default'],'value' => $info['dfeatures']['typeid']),$list['groups'],'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->slt(array('desc' => $this->bbf('fm_dfeatures_typeid'),'name' => 'dfeatures[typeid]','labelid' => 'dfeatures-meetme-typeid','key' => 'number','key_val' => 'id','default' => $element['dfeatures']['typeid']['default'],'value' => $info['dfeatures']['typeid']),$list['meetme'],'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->text(array('desc' => $this->bbf('fm_dfeatures_custom'),'name' => 'dfeatures[custom]','labelid' => 'dfeatures-custom','size' => 15,'value' => $info['dfeatures']['custom']),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>


<?php
	$form = &$this->get_module('form');
	$url = &$this->get_module('url');

	$info = $this->vars('info');
	$element = $this->vars('element');
	$list = $this->vars('list');
?>

<?=$form->text(array('desc' => $this->bbf('fm_extenumbers_number'),'name' => 'extenumbers[number]','labelid' => 'extenumbers-number','size' => 15,'default' => $element['extenumbers']['number']['default'],'value' => $info['extenumbers']['number']),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->slt(array('desc' => $this->bbf('fm_dfeatures_type'),'name' => 'dfeatures[type]','labelid' => 'dfeatures-type','key' => false,'bbf' => array('concatvalue','fm_dfeatures_type-'),'default' => $element['dfeatures']['type']['default'],'value' => $info['dfeatures']['type']),$element['dfeatures']['type']['value'],'onchange="xivo_chgtype(this);" onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?php
	if(empty($list['users']) === false):
?>

<?=$form->slt(array('desc' => $this->bbf('fm_dfeatures_typeid'),'name' => 'dfeatures[typeid]','labelid' => 'dfeatures-user-typeid','key' => 'number-context','overkey' => 'id','default' => $element['dfeatures']['typeid']['default'],'value' => $info['dfeatures']['typeid']),$list['users'],'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?php
	else:
		echo '<div id="fd-dfeatures-user-typeid" class="txt-center">',$url->href_html($this->bbf('create_user'),'service/ipbx/pbx_settings/users','act=add'),'</div>';
	endif;

	if(empty($list['groups']) === false):
?>

<?=$form->slt(array('desc' => $this->bbf('fm_dfeatures_typeid'),'name' => 'dfeatures[typeid]','labelid' => 'dfeatures-group-typeid','key' => 'number-context','overkey' => 'id','default' => $element['dfeatures']['typeid']['default'],'value' => $info['dfeatures']['typeid']),$list['groups'],'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?php
	else:
		echo '<div id="fd-dfeatures-group-typeid" class="txt-center">',$url->href_html($this->bbf('create_group'),'service/ipbx/pbx_settings/groups','act=add'),'</div>';
	endif;

	if(empty($list['meetme']) === false):
?>

<?=$form->slt(array('desc' => $this->bbf('fm_dfeatures_typeid'),'name' => 'dfeatures[typeid]','labelid' => 'dfeatures-meetme-typeid','key' => 'number','overkey' => 'id','default' => $element['dfeatures']['typeid']['default'],'value' => $info['dfeatures']['typeid']),$list['meetme'],'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?php
	else:
		echo '<div id="fd-dfeatures-meetme-typeid" class="txt-center">',$url->href_html($this->bbf('create_meetme'),'service/ipbx/pbx_settings/meetme','act=add'),'</div>';
	endif;
?>

<?=$form->text(array('desc' => $this->bbf('fm_dfeatures_custom'),'name' => 'dfeatures[custom]','labelid' => 'dfeatures-custom','size' => 15,'value' => $info['dfeatures']['custom']),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>


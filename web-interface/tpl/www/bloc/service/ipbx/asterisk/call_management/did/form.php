<?php
	$form = &$this->get_module('form');
	$url = &$this->get_module('url');

	$info = $this->vars('info');
	$element = $this->vars('element');
	$list = $this->vars('list');
?>

<?=$form->text(array('desc' => $this->bbf('fm_extenumbers_exten'),'name' => 'extenumbers[exten]','labelid' => 'extenumbers-exten','size' => 15,'default' => $element['extenumbers']['exten']['default'],'value' => $info['extenumbers']['exten']));?>

<?=$form->select(array('desc' => $this->bbf('fm_dfeatures_type'),'name' => 'dfeatures[type]','labelid' => 'dfeatures-type','key' => false,'bbf' => array('concatvalue','fm_dfeatures_type-'),'default' => $element['dfeatures']['type']['default'],'value' => $info['dfeatures']['type']),$element['dfeatures']['type']['value'],'onchange="xivo_chgtype(this);"');?>

<?php
	if(empty($list['users']) === false):
?>

<?=$form->select(array('desc' => $this->bbf('fm_dfeatures_typeid'),'name' => 'dfeatures[typeid]','labelid' => 'dfeatures-user-typeid','key' => 'number-context','altkey' => 'id','default' => $element['dfeatures']['typeid']['default'],'value' => $info['dfeatures']['typeid']),$list['users']);?>

<?php
	else:
		echo '<div id="fd-dfeatures-user-typeid" class="txt-center">',$url->href_html($this->bbf('create_user'),'service/ipbx/pbx_settings/users','act=add'),'</div>';
	endif;

	if(empty($list['groups']) === false):
?>

<?=$form->select(array('desc' => $this->bbf('fm_dfeatures_typeid'),'name' => 'dfeatures[typeid]','labelid' => 'dfeatures-group-typeid','key' => 'number-context','altkey' => 'id','default' => $element['dfeatures']['typeid']['default'],'value' => $info['dfeatures']['typeid']),$list['groups']);?>

<?php
	else:
		echo '<div id="fd-dfeatures-group-typeid" class="txt-center">',$url->href_html($this->bbf('create_group'),'service/ipbx/pbx_settings/groups','act=add'),'</div>';
	endif;

	if(empty($list['meetme']) === false):
?>

<?=$form->select(array('desc' => $this->bbf('fm_dfeatures_typeid'),'name' => 'dfeatures[typeid]','labelid' => 'dfeatures-meetme-typeid','key' => 'number','altkey' => 'id','default' => $element['dfeatures']['typeid']['default'],'value' => $info['dfeatures']['typeid']),$list['meetme']);?>

<?php
	else:
		echo '<div id="fd-dfeatures-meetme-typeid" class="txt-center">',$url->href_html($this->bbf('create_meetme'),'service/ipbx/pbx_settings/meetme','act=add'),'</div>';
	endif;
?>

<?=$form->text(array('desc' => $this->bbf('fm_dfeatures_custom'),'name' => 'dfeatures[custom]','labelid' => 'dfeatures-custom','size' => 15,'value' => $info['dfeatures']['custom']));?>


<?php
	$form = &$this->get_module('form');
	$url = &$this->get_module('url');

	$element = $this->vars('element');
	$info = $this->vars('info');
	$user_slt = $this->vars('user_slt');
	$user_list = $this->vars('user_list');
?>

<?=$form->text(array('desc' => $this->bbf('fm_gfeatures_name'),'name' => 'gfeatures[name]','labelid' => 'gfeatures-name','size' => 25,'default' => $element['gfeatures']['name']['default'],'value' => $info['gfeatures']['name']));?>

<?=$form->text(array('desc' => $this->bbf('fm_gfeatures_number'),'name' => 'gfeatures[number]','labelid' => 'gfeatures-number','size' => 25,'default' => $element['gfeatures']['number']['default'],'value' => $info['gfeatures']['number']));?>

<?=$form->text(array('desc' => $this->bbf('fm_gfeatures_context'),'name' => 'gfeatures[context]','labelid' => 'gfeatures-context','size' => 25,'default' => $element['gfeatures']['context']['default'],'value' => $info['gfeatures']['context']));?>

<?=$form->select(array('desc' => $this->bbf('fm_queue_timeout'),'name' => 'queue[timeout]','labelid' => 'queue-timeout','bbf' => array('paramkey','fm_queue_timeout-opt'),'key' => false,'default' => $element['queue']['timeout']['default'],'value' => (isset($info['queue']['timeout']) === true ? (int) $info['queue']['timeout'] : null)),$element['queue']['timeout']['value']);?>

<?php
	if($user_list !== false):
?>
<div id="userlist" class="fm-field fm-multilist"><p><label id="lb-userlist" for="it-userlist"><?=$this->bbf('fm_user');?></label></p>
	<div class="slt-outlist">

	<?=$form->select(array('name' => 'userlist','label' => false,'id' => 'it-userlist','multiple' => true,'size' => 5,'field' => false,'key' => 'identity'),$user_list);?>

	</div>
	<div class="inout-list">

		<a href="#" onclick="xivo_fm_move_selected('it-userlist','it-user'); return(false);" title="<?=$this->bbf('bt-inuser');?>"><?=$url->img_html('img/site/button/row-left.gif',$this->bbf('bt-inuser'),'class="bt-inlist" id="bt-inuser" border="0"');?></a><br />

		<a href="#" onclick="xivo_fm_move_selected('it-user','it-userlist'); return(false);" title="<?=$this->bbf('bt-outuser');?>"><?=$url->img_html('img/site/button/row-right.gif',$this->bbf('bt-outuser'),'class="bt-outlist" id="bt-outuser" border="0"');?></a>

	</div>
	<div class="slt-inlist">

		<?=$form->select(array('name' => 'user[]','label' => false,'id' => 'it-user','multiple' => true,'size' => 5,'field' => false,'key' => 'identity'),$user_slt);?>

	</div>
</div>
<div class="clearboth"></div>
<?php
	else:
		echo '<div class="txt-center">',$url->href_html($this->bbf('create_user'),'service/ipbx/pbx_settings/users','act=add'),'</div>';
	endif;
?>

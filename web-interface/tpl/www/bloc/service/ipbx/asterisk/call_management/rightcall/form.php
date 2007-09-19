<?php
	$form = &$this->get_module('form');
	$url = &$this->get_module('url');

	$info = $this->vars('info');
	$element = $this->vars('element');

	$user_slt = $this->vars('user_slt');
	$user_list = $this->vars('user_list');
?>

<?=$form->text(array('desc' => $this->bbf('fm_rightcall_name'),'name' => 'rightcall[name]','labelid' => 'rightcall-name','size' => 15,'default' => $element['rightcall']['name']['default'],'value' => $info['rightcall']['name']));?>

<?=$form->text(array('desc' => $this->bbf('fm_rightcall_passwd'),'name' => 'rightcall[passwd]','labelid' => 'rightcall-passwd','size' => 15,'default' => $element['rightcall']['passwd']['default'],'value' => $info['rightcall']['passwd']));?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_rightcall_permit'),'name' => 'rightcall[permit]','labelid' => 'permit','checked' => $info['rightcall']['permit'],'default' => $element['rightcall']['permit']['default']));?>

<div id="description" class="fm-field"><p><label id="lb-rightcall-description" for="it-rightcall-description"><?=$this->bbf('fm_rightcall_description');?></label></p>
<?=$form->textarea(array('field' => false,'label' => false,'name' => 'rightcall[description]','id' => 'it-rightcall-description','cols' => 60,'rows' => 5,'default' => $element['rightcall']['description']['default']),$info['rightcall']['description']);?>
</div>

<?php
	if($user_list !== false):
?>
		<div id="userlist" class="fm-field fm-multilist">
			<div class="slt-outlist">

		<?=$form->select(array('name' => 'userlist','label' => false,'id' => 'it-userlist','multiple' => true,'size' => 5,'field' => false),$user_list);?>

			</div>
			<div class="inout-list">

		<a href="#" onclick="xivo_fm_move_selected('it-userlist','it-user'); return(false);" title="<?=$this->bbf('bt-inuser');?>"><?=$url->img_html('img/site/button/row-left.gif',$this->bbf('bt-inuser'),'class="bt-inlist" id="bt-inuser" border="0"');?></a><br />

		<a href="#" onclick="xivo_fm_move_selected('it-user','it-userlist'); return(false);" title="<?=$this->bbf('bt-outuser');?>"><?=$url->img_html('img/site/button/row-right.gif',$this->bbf('bt-outuser'),'class="bt-outlist" id="bt-outuser" border="0"');?></a>

			</div>
			<div class="slt-inlist">

		<?=$form->select(array('name' => 'rightcalluser[]','label' => false,'id' => 'it-user','multiple' => true,'size' => 5,'field' => false),$user_slt);?>

			</div>
		</div>
		<div class="clearboth"></div>
<?php
	else:
		echo '<div class="txt-center">',$url->href_html($this->bbf('create_user'),'service/ipbx/pbx_settings/users','act=add'),'</div>';
	endif;
?>


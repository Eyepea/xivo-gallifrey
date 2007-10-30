<?php
	$form = &$this->get_module('form');
	$url = &$this->get_module('url');

	$info = $this->vars('info');
	$element = $this->vars('element');

	$rcalluser = $this->vars('rcalluser');
	$rcallgroup = $this->vars('rcallgroup');
	$rcalloutcall = $this->vars('rcalloutcall');
	$rcallexten = $this->vars('rcallexten');
?>

<div id="sb-part-first">

<?=$form->text(array('desc' => $this->bbf('fm_rightcall_name'),'name' => 'rightcall[name]','labelid' => 'rightcall-name','size' => 15,'default' => $element['rightcall']['name']['default'],'value' => $info['rightcall']['name']));?>

<?=$form->text(array('desc' => $this->bbf('fm_rightcall_passwd'),'name' => 'rightcall[passwd]','labelid' => 'rightcall-passwd','size' => 15,'default' => $element['rightcall']['passwd']['default'],'value' => $info['rightcall']['passwd']));?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_rightcall_permit'),'name' => 'rightcall[permit]','labelid' => 'permit','checked' => $info['rightcall']['permit'],'default' => $element['rightcall']['permit']['default']));?>

<div id="extenlist" class="fm-field fm-multilist">

	<p><label id="lb-exten" for="it-exten"><?=$this->bbf('fm_rightcallexten_exten');?></label></p>

	<div class="slt-list">
		<?=$form->select(array('name' => 'rightcallexten[]','label' => false,'id' => 'it-exten','key' => true,'altkey' => 'exten','multiple' => true,'size' => 5,'field' => false),$rcallexten);?>
		<div class="bt-adddelete">

			<a href="#" onclick="xivo_fm_select_add_exten('it-exten',prompt('<?=xivo_stript($this->bbf('rightcallexten_add-extension'));?>')); return(false);" title="<?=$this->bbf('bt-addexten');?>"><?=$url->img_html('img/site/button/mini/blue/add.gif',$this->bbf('bt-addexten'),'class="bt-addlist" id="bt-addexten" border="0"');?></a><br />

			<a href="#" onclick="xivo_fm_select_delete_entry('it-exten'); return(false);" title="<?=$this->bbf('bt-deleteexten');?>"><?=$url->img_html('img/site/button/mini/blue/delete.gif',$this->bbf('bt-deleteexten'),'class="bt-deletelist" id="bt-deleteexten" border="0"');?></a>

		</div>
	</div>
</div>
<div class="clearboth"></div>

<div class="fm-field fm-description"><p><label id="lb-rightcall-description" for="it-rightcall-description"><?=$this->bbf('fm_rightcall_description');?></label></p>
<?=$form->textarea(array('field' => false,'label' => false,'name' => 'rightcall[description]','id' => 'it-rightcall-description','cols' => 60,'rows' => 5,'default' => $element['rightcall']['description']['default']),$info['rightcall']['description']);?>
</div>

</div>

<div id="sb-part-rightcalluser" class="b-nodisplay">

<?php
	if($rcalluser['list'] !== false):
?>
		<div id="userlist" class="fm-field fm-multilist">
			<div class="slt-outlist">

		<?=$form->select(array('name' => 'userlist','label' => false,'id' => 'it-userlist','browse' => 'ufeatures','key' => 'identity','altkey' => 'id','multiple' => true,'size' => 5,'field' => false),$rcalluser['list']);?>

			</div>
			<div class="inout-list">

		<a href="#" onclick="xivo_fm_move_selected('it-userlist','it-user'); return(false);" title="<?=$this->bbf('bt-inuser');?>"><?=$url->img_html('img/site/button/row-left.gif',$this->bbf('bt-inuser'),'class="bt-inlist" id="bt-inuser" border="0"');?></a><br />

		<a href="#" onclick="xivo_fm_move_selected('it-user','it-userlist'); return(false);" title="<?=$this->bbf('bt-outuser');?>"><?=$url->img_html('img/site/button/row-right.gif',$this->bbf('bt-outuser'),'class="bt-outlist" id="bt-outuser" border="0"');?></a>

			</div>
			<div class="slt-inlist">

		<?=$form->select(array('name' => 'rightcalluser[]','label' => false,'id' => 'it-user','browse' => 'ufeatures','key' => 'identity','altkey' => 'id','multiple' => true,'size' => 5,'field' => false),$rcalluser['slt']);?>

			</div>
		</div>
		<div class="clearboth"></div>
<?php
	else:
		echo '<div class="txt-center">',$url->href_html($this->bbf('create_user'),'service/ipbx/pbx_settings/users','act=add'),'</div>';
	endif;
?>

</div>

<div id="sb-part-rightcallgroup" class="b-nodisplay">

<?php
	if($rcallgroup['list'] !== false):
?>
		<div id="grouplist" class="fm-field fm-multilist">
			<div class="slt-outlist">

		<?=$form->select(array('name' => 'grouplist','label' => false,'id' => 'it-grouplist','browse' => 'gfeatures','key' => 'name','altkey' => 'id','multiple' => true,'size' => 5,'field' => false),$rcallgroup['list']);?>

			</div>
			<div class="inout-list">

		<a href="#" onclick="xivo_fm_move_selected('it-grouplist','it-group'); return(false);" title="<?=$this->bbf('bt-ingroup');?>"><?=$url->img_html('img/site/button/row-left.gif',$this->bbf('bt-ingroup'),'class="bt-inlist" id="bt-ingroup" border="0"');?></a><br />

		<a href="#" onclick="xivo_fm_move_selected('it-group','it-grouplist'); return(false);" title="<?=$this->bbf('bt-outgroup');?>"><?=$url->img_html('img/site/button/row-right.gif',$this->bbf('bt-outgroup'),'class="bt-outlist" id="bt-outgroup" border="0"');?></a>

			</div>
			<div class="slt-inlist">

		<?=$form->select(array('name' => 'rightcallgroup[]','label' => false,'id' => 'it-group','browse' => 'gfeatures','key' => 'name','altkey' => 'id','multiple' => true,'size' => 5,'field' => false),$rcallgroup['slt']);?>

			</div>
		</div>
		<div class="clearboth"></div>
<?php
	else:
		echo '<div class="txt-center">',$url->href_html($this->bbf('create_group'),'service/ipbx/pbx_settings/groups','act=add'),'</div>';
	endif;
?>

</div>

<div id="sb-part-last" class="b-nodisplay">

<?php
	if($rcalloutcall['list'] !== false):
?>
		<div id="outcalllist" class="fm-field fm-multilist">
			<div class="slt-outlist">

		<?=$form->select(array('name' => 'outcalllist','label' => false,'id' => 'it-outcalllist','browse' => 'outcall','key' => 'name','altkey' => 'id','multiple' => true,'size' => 5,'field' => false),$rcalloutcall['list']);?>

			</div>
			<div class="inout-list">

		<a href="#" onclick="xivo_fm_move_selected('it-outcalllist','it-outcall'); return(false);" title="<?=$this->bbf('bt-inoutcall');?>"><?=$url->img_html('img/site/button/row-left.gif',$this->bbf('bt-inoutcall'),'class="bt-inlist" id="bt-inoutcall" border="0"');?></a><br />

		<a href="#" onclick="xivo_fm_move_selected('it-outcall','it-outcalllist'); return(false);" title="<?=$this->bbf('bt-outoutcall');?>"><?=$url->img_html('img/site/button/row-right.gif',$this->bbf('bt-outoutcall'),'class="bt-outlist" id="bt-outoutcall" border="0"');?></a>

			</div>
			<div class="slt-inlist">

		<?=$form->select(array('name' => 'rightcalloutcall[]','label' => false,'id' => 'it-outcall','browse' => 'outcall','key' => 'name','altkey' => 'id','multiple' => true,'size' => 5,'field' => false),$rcalloutcall['slt']);?>

			</div>
		</div>
		<div class="clearboth"></div>
<?php
	else:
		echo '<div class="txt-center">',$url->href_html($this->bbf('create_outcall'),'service/ipbx/call_management/outcall','act=add'),'</div>';
	endif;
?>

</div>

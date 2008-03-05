<?php
	$form = &$this->get_module('form');
	$url = &$this->get_module('url');
	$dhtml = &$this->get_module('dhtml');

	$info = $this->get_var('info');
	$element = $this->get_var('element');
?>

<div id="sb-part-first">

<?=$form->text(array('desc' => $this->bbf('fm_ldapserver_name'),'name' => 'ldapserver[name]','labelid' => 'ldapserver-name','size' => 15,'default' => $element['ldapserver']['name']['default'],'value' => $info['ldapserver']['name']));?>

<?php
	if(($xldapservers = $this->get_var('xldapservers')) !== false):
		echo $form->select(array('desc' => $this->bbf('fm_ldapserver_ldapserverid'),'name' => 'ldapserver[ldapserverid]','labelid' => 'ldapserver-ldapserverid','invalid' => ($this->get_var('act') === 'edit'),'key' => 'identity','altkey' => 'id','default' => $element['ldapserver']['ldapserverid']['default'],'value' => $info['ldapserver']['ldapserverid']),$xldapservers);
	else:
		echo '<div class="txt-center">',$url->href_html($this->bbf('create_ldapserver'),'xivo/configuration/ldapservers','act=add'),'</div>';
	endif;
?>

<?=$form->text(array('desc' => $this->bbf('fm_ldapserver_user'),'name' => 'ldapserver[user]','labelid' => 'ldapserver-user','size' => 15,'default' => $element['ldapserver']['user']['default'],'value' => $info['ldapserver']['user']));?>

<?=$form->text(array('desc' => $this->bbf('fm_ldapserver_passwd'),'name' => 'ldapserver[passwd]','labelid' => 'ldapserver-passwd','size' => 15,'default' => $element['ldapserver']['passwd']['default'],'value' => $info['ldapserver']['passwd']));?>

<?=$form->text(array('desc' => $this->bbf('fm_ldapserver_basedn'),'name' => 'ldapserver[basedn]','labelid' => 'ldapserver-basedn','size' => 30,'default' => $element['ldapserver']['basedn']['default'],'value' => $info['ldapserver']['basedn']));?>

<?=$form->text(array('desc' => $this->bbf('fm_ldapserver_filter'),'name' => 'ldapserver[filter]','labelid' => 'ldapserver-filter','size' => 30,'notag' => false,'default' => $element['ldapserver']['filter']['default'],'value' => $info['ldapserver']['filter']));?>

<?=$form->select(array('desc' => $this->bbf('fm_ldapserver_additionaltype'),'name' => 'ldapserver[additionaltype]','labelid' => 'ldapserver-additionaltype','bbf' => array('concatkey','fm_ldapserver_additionaltype-opt-'),'key' => false,'default' => $element['ldapserver']['additionaltype']['default'],'value' => $info['ldapserver']['additionaltype']),$element['ldapserver']['additionaltype']['value'],'onchange="xivo_chg_additionaltype(this.value);"');?>

<?=$form->text(array('desc' => '&nbsp;','name' => 'ldapserver[additionaltext]','labelid' => 'ldapserver-additionaltext','size' => 15,'default' => $element['ldapserver']['additionaltext']['default'],'value' => $info['ldapserver']['additionaltext']));?>

<div class="fm-field fm-description"><p><label id="lb-ldapserver-description" for="it-ldapserver-description"><?=$this->bbf('fm_ldapserver_description');?></label></p>
<?=$form->textarea(array('field' => false,'label' => false,'name' => 'ldapserver[description]','id' => 'it-ldapserver-description','cols' => 60,'rows' => 5,'default' => $element['ldapserver']['description']['default']),$info['ldapserver']['description']);?>
</div>

</div>

<div id="sb-part-last" class="b-nodisplay">

<fieldset id="fld-ldapserver-attrdisplayname">
	<legend><?=$this->bbf('fld-ldapserver-attrdisplayname');?></legend>

	<div class="fm-field fm-multilist">

		<div class="slt-list">

			<div class="bt-adddelete">

					<a href="#" onclick="xivo_fm_select_add_attrldap('it-ldapserver-attrdisplayname',prompt('<?=$dhtml->escape($this->bbf('add_ldapserver-attrdisplayname'));?>')); return(false);" title="<?=$this->bbf('bt_ldapserver-attrdisplayname-add');?>"><?=$url->img_html('img/site/button/mini/orange/add.gif',$this->bbf('bt_ldapserver-attrdisplayname-add'),'class="bt-addlist" id="bt-ldapserver-attrdisplayname-add" border="0"');?></a><br />

					<a href="#" onclick="xivo_fm_select_delete_entry('it-ldapserver-attrdisplayname'); return(false);" title="<?=$this->bbf('bt_delete_ldapserver-attrdisplayname');?>"><?=$url->img_html('img/site/button/mini/orange/delete.gif',$this->bbf('bt_delete_ldapserver-attrdisplayname'),'class="bt-deletelist" id="bt-ldapserver-attrdisplayname-delete" border="0"');?></a>

			</div>

			<?=$form->select(array('name' => 'ldapserver[attrdisplayname][]','label' => false,'id' => 'it-ldapserver-attrdisplayname','key' => false,'multiple' => true,'size' => 5,'field' => false),$info['ldapserver']['attrdisplayname']);?>

		</div>

		<div class="bt-updown">

			<a href="#" onclick="xivo_fm_order_selected('it-ldapserver-attrdisplayname',1); return(false);" title="<?=$this->bbf('bt_up_ldapserver-attrdisplayname');?>"><?=$url->img_html('img/site/button/row-up.gif',$this->bbf('bt_up_ldapserver-attrdisplayname'),'class="bt-uplist" id="bt-ldapserver-attrdisplayname-up" border="0"');?></a><br />

			<a href="#" onclick="xivo_fm_order_selected('it-ldapserver-attrdisplayname',-1); return(false);" title="<?=$this->bbf('bt_down_ldapserver-attrdisplayname');?>"><?=$url->img_html('img/site/button/row-down.gif',$this->bbf('bt_down_ldapserver-attrdisplayname'),'class="bt-downlist" id="bt-ldapserver-attrdisplayname-down" border="0"');?></a>

		</div>

	</div>
	<div class="clearboth"></div>

</fieldset>

<fieldset id="fld-ldapserver-attrphonenumber">
	<legend><?=$this->bbf('fld-ldapserver-attrphonenumber');?></legend>

	<div class="fm-field fm-multilist">

		<div class="slt-list">

			<div class="bt-adddelete">

					<a href="#" onclick="xivo_fm_select_add_attrldap('it-ldapserver-attrphonenumber',prompt('<?=$dhtml->escape($this->bbf('add_ldapserver-attrphonenumber'));?>')); return(false);" title="<?=$this->bbf('bt_ldapserver-attrphonenumber-add');?>"><?=$url->img_html('img/site/button/mini/orange/add.gif',$this->bbf('bt_ldapserver-attrphonenumber-add'),'class="bt-addlist" id="bt-ldapserver-attrphonenumber-add" border="0"');?></a><br />

					<a href="#" onclick="xivo_fm_select_delete_entry('it-ldapserver-attrphonenumber'); return(false);" title="<?=$this->bbf('bt_delete_ldapserver-attrphonenumber');?>"><?=$url->img_html('img/site/button/mini/orange/delete.gif',$this->bbf('bt_delete_ldapserver-attrphonenumber'),'class="bt-deletelist" id="bt-ldapserver-attrphonenumber-delete" border="0"');?></a>

			</div>

			<?=$form->select(array('name' => 'ldapserver[attrphonenumber][]','label' => false,'id' => 'it-ldapserver-attrphonenumber','key' => false,'multiple' => true,'size' => 5,'field' => false),$info['ldapserver']['attrphonenumber']);?>

		</div>

		<div class="bt-updown">

			<a href="#" onclick="xivo_fm_order_selected('it-ldapserver-attrphonenumber',1); return(false);" title="<?=$this->bbf('bt_up_ldapserver-attrphonenumber');?>"><?=$url->img_html('img/site/button/row-up.gif',$this->bbf('bt_up_ldapserver-attrphonenumber'),'class="bt-uplist" id="bt-ldapserver-attrphonenumber-up" border="0"');?></a><br />

			<a href="#" onclick="xivo_fm_order_selected('it-ldapserver-attrphonenumber',-1); return(false);" title="<?=$this->bbf('bt_down_ldapserver-attrphonenumber');?>"><?=$url->img_html('img/site/button/row-down.gif',$this->bbf('bt_down_ldapserver-attrphonenumber'),'class="bt-downlist" id="bt-ldapserver-attrphonenumber-down" border="0"');?></a>

		</div>

	</div>
	<div class="clearboth"></div>

</fieldset>

</div>

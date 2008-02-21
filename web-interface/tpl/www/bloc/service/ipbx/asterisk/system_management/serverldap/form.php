<?php
	$form = &$this->get_module('form');
	$url = &$this->get_module('url');
	$dhtml = &$this->get_module('dhtml');

	$info = $this->get_var('info');
	$element = $this->get_var('element');
?>

<div id="sb-part-first">

<?=$form->text(array('desc' => $this->bbf('fm_serverldap_name'),'name' => 'serverldap[name]','labelid' => 'serverldap-name','size' => 15,'default' => $element['serverldap']['name']['default'],'value' => $info['serverldap']['name']));?>

<?=$form->text(array('desc' => $this->bbf('fm_serverldap_host'),'name' => 'serverldap[host]','labelid' => 'serverldap-host','size' => 15,'default' => $element['serverldap']['host']['default'],'value' => $info['serverldap']['host']));?>

<?=$form->text(array('desc' => $this->bbf('fm_serverldap_port'),'name' => 'serverldap[port]','labelid' => 'serverldap-port','default' => $element['serverldap']['port']['default'],'value' => $info['serverldap']['port']));?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_serverldap_ssl'),'name' => 'serverldap[ssl]','labelid' => 'serverldap-ssl','default' => $element['serverldap']['ssl']['default'],'checked' => $info['serverldap']['ssl']));?>

<?=$form->select(array('desc' => $this->bbf('fm_serverldap_protocolversion'),'name' => 'serverldap[protocolversion]','labelid' => 'serverldap-protocolversion','bbf' => array('paramkey','fm_serverldap_protocolversion-opt'),'key' => false,'default' => $element['serverldap']['protocolversion']['default'],'value' => $info['serverldap']['protocolversion']),$element['serverldap']['protocolversion']['value']);?>

<?=$form->text(array('desc' => $this->bbf('fm_serverldap_user'),'name' => 'serverldap[user]','labelid' => 'serverldap-user','size' => 15,'default' => $element['serverldap']['user']['default'],'value' => $info['serverldap']['user']));?>

<?=$form->text(array('desc' => $this->bbf('fm_serverldap_passwd'),'name' => 'serverldap[passwd]','labelid' => 'serverldap-passwd','size' => 15,'default' => $element['serverldap']['passwd']['default'],'value' => $info['serverldap']['passwd']));?>

<?=$form->text(array('desc' => $this->bbf('fm_serverldap_basedn'),'name' => 'serverldap[basedn]','labelid' => 'serverldap-basedn','size' => 30,'default' => $element['serverldap']['basedn']['default'],'value' => $info['serverldap']['basedn']));?>

<?=$form->text(array('desc' => $this->bbf('fm_serverldap_filter'),'name' => 'serverldap[filter]','labelid' => 'serverldap-filter','size' => 30,'notag' => false,'default' => $element['serverldap']['filter']['default'],'value' => $info['serverldap']['filter']));?>

<?=$form->select(array('desc' => $this->bbf('fm_serverldap_additionaltype'),'name' => 'serverldap[additionaltype]','labelid' => 'serverldap-additionaltype','bbf' => array('concatkey','fm_serverldap_additionaltype-opt-'),'key' => false,'default' => $element['serverldap']['additionaltype']['default'],'value' => $info['serverldap']['additionaltype']),$element['serverldap']['additionaltype']['value'],'onchange="xivo_chg_additionaltype(this.value);"');?>

<?=$form->text(array('desc' => '&nbsp;','name' => 'serverldap[additionaltext]','labelid' => 'serverldap-additionaltext','size' => 15,'default' => $element['serverldap']['additionaltext']['default'],'value' => $info['serverldap']['additionaltext']));?>

<div class="fm-field fm-description"><p><label id="lb-serverldap-description" for="it-serverldap-description"><?=$this->bbf('fm_serverldap_description');?></label></p>
<?=$form->textarea(array('field' => false,'label' => false,'name' => 'serverldap[description]','id' => 'it-serverldap-description','cols' => 60,'rows' => 5,'default' => $element['serverldap']['description']['default']),$info['serverldap']['description']);?>
</div>

</div>

<div id="sb-part-last" class="b-nodisplay">

<fieldset id="fld-serverldap-attrdisplayname">
	<legend><?=$this->bbf('fld-serverldap-attrdisplayname');?></legend>

	<div class="fm-field fm-multilist">

		<div class="slt-list">

			<div class="bt-adddelete">

					<a href="#" onclick="xivo_fm_select_add_attrldap('it-serverldap-attrdisplayname',prompt('<?=$dhtml->escape($this->bbf('add_serverldap-attrdisplayname'));?>')); return(false);" title="<?=$this->bbf('bt_serverldap-attrdisplayname-add');?>"><?=$url->img_html('img/site/button/mini/orange/add.gif',$this->bbf('bt_serverldap-attrdisplayname-add'),'class="bt-addlist" id="bt-serverldap-attrdisplayname-add" border="0"');?></a><br />

					<a href="#" onclick="xivo_fm_select_delete_entry('it-serverldap-attrdisplayname'); return(false);" title="<?=$this->bbf('bt_delete_serverldap-attrdisplayname');?>"><?=$url->img_html('img/site/button/mini/orange/delete.gif',$this->bbf('bt_delete_serverldap-attrdisplayname'),'class="bt-deletelist" id="bt-serverldap-attrdisplayname-delete" border="0"');?></a>

			</div>

			<?=$form->select(array('name' => 'serverldap[attrdisplayname][]','label' => false,'id' => 'it-serverldap-attrdisplayname','key' => false,'multiple' => true,'size' => 5,'field' => false),$info['serverldap']['attrdisplayname']);?>

		</div>

		<div class="bt-updown">

			<a href="#" onclick="xivo_fm_order_selected('it-serverldap-attrdisplayname',1); return(false);" title="<?=$this->bbf('bt_up_serverldap-attrdisplayname');?>"><?=$url->img_html('img/site/button/row-up.gif',$this->bbf('bt_up_serverldap-attrdisplayname'),'class="bt-uplist" id="bt-serverldap-attrdisplayname-up" border="0"');?></a><br />

			<a href="#" onclick="xivo_fm_order_selected('it-serverldap-attrdisplayname',-1); return(false);" title="<?=$this->bbf('bt_down_serverldap-attrdisplayname');?>"><?=$url->img_html('img/site/button/row-down.gif',$this->bbf('bt_down_serverldap-attrdisplayname'),'class="bt-downlist" id="bt-serverldap-attrdisplayname-down" border="0"');?></a>

		</div>

	</div>
	<div class="clearboth"></div>

</fieldset>

<fieldset id="fld-serverldap-attrphonenumber">
	<legend><?=$this->bbf('fld-serverldap-attrphonenumber');?></legend>

	<div class="fm-field fm-multilist">

		<div class="slt-list">

			<div class="bt-adddelete">

					<a href="#" onclick="xivo_fm_select_add_attrldap('it-serverldap-attrphonenumber',prompt('<?=$dhtml->escape($this->bbf('add_serverldap-attrphonenumber'));?>')); return(false);" title="<?=$this->bbf('bt_serverldap-attrphonenumber-add');?>"><?=$url->img_html('img/site/button/mini/orange/add.gif',$this->bbf('bt_serverldap-attrphonenumber-add'),'class="bt-addlist" id="bt-serverldap-attrphonenumber-add" border="0"');?></a><br />

					<a href="#" onclick="xivo_fm_select_delete_entry('it-serverldap-attrphonenumber'); return(false);" title="<?=$this->bbf('bt_delete_serverldap-attrphonenumber');?>"><?=$url->img_html('img/site/button/mini/orange/delete.gif',$this->bbf('bt_delete_serverldap-attrphonenumber'),'class="bt-deletelist" id="bt-serverldap-attrphonenumber-delete" border="0"');?></a>

			</div>

			<?=$form->select(array('name' => 'serverldap[attrphonenumber][]','label' => false,'id' => 'it-serverldap-attrphonenumber','key' => false,'multiple' => true,'size' => 5,'field' => false),$info['serverldap']['attrphonenumber']);?>

		</div>

		<div class="bt-updown">

			<a href="#" onclick="xivo_fm_order_selected('it-serverldap-attrphonenumber',1); return(false);" title="<?=$this->bbf('bt_up_serverldap-attrphonenumber');?>"><?=$url->img_html('img/site/button/row-up.gif',$this->bbf('bt_up_serverldap-attrphonenumber'),'class="bt-uplist" id="bt-serverldap-attrphonenumber-up" border="0"');?></a><br />

			<a href="#" onclick="xivo_fm_order_selected('it-serverldap-attrphonenumber',-1); return(false);" title="<?=$this->bbf('bt_down_serverldap-attrphonenumber');?>"><?=$url->img_html('img/site/button/row-down.gif',$this->bbf('bt_down_serverldap-attrphonenumber'),'class="bt-downlist" id="bt-serverldap-attrphonenumber-down" border="0"');?></a>

		</div>

	</div>
	<div class="clearboth"></div>

</fieldset>

</div>

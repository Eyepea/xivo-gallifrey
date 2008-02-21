<?php
	$url = &$this->get_module('url');
	$form = &$this->get_module('form');
	$dhtml = &$this->get_module('dhtml');

	$info = $this->get_var('info');
	$element = $this->get_var('element');

	if($this->get_var('fm_save') === true):
		$dhtml->write_js('xivo_form_success(\''.$dhtml->escape($this->bbf('fm_success-save')).'\');');
	endif;
?>
<div class="b-infos b-form">
	<h3 class="sb-top xspan"><span class="span-left">&nbsp;</span><span class="span-center"><?=$this->bbf('title_content_name');?></span><span class="span-right">&nbsp;</span></h3>

<div class="sb-smenu">
	<ul>
		<li id="smenu-tab-1" class="moo" onclick="xivo_smenu_click(this,'moc','sb-part-first');" onmouseout="xivo_smenu_out(this,'moo');" onmouseover="xivo_smenu_over(this,'mov');">
			<div class="tab"><span class="span-center"><a href="#" onclick="return(false);"><?=$this->bbf('smenu_access');?></a></span></div><span class="span-right">&nbsp;</span>
		</li>
		<li id="smenu-tab-2" class="moo" onclick="xivo_smenu_click(this,'moc','sb-part-serverxivo');" onmouseout="xivo_smenu_out(this,'moo');" onmouseover="xivo_smenu_over(this,'mov');">
			<div class="tab"><span class="span-center"><a href="#" onclick="return(false);"><?=$this->bbf('smenu_serversxivo');?></a></span></div><span class="span-right">&nbsp;</span>
		</li>
		<li id="smenu-tab-3" class="moo-last" onclick="xivo_smenu_click(this,'moc','sb-part-last',1);" onmouseout="xivo_smenu_out(this,'moo',1);" onmouseover="xivo_smenu_over(this,'mov',1);">
			<div class="tab"><span class="span-center"><a href="#" onclick="return(false);"><?=$this->bbf('smenu_serversldap');?></a></span></div><span class="span-right">&nbsp;</span>
		</li>
	</ul>
</div>

<div class="sb-content">
<form action="#" method="post" accept-charset="utf-8" onsubmit="xivo_fm_select('it-access'); xivo_fm_select('it-serverxivo'); xivo_fm_select('it-serverldap');">

<?=$form->hidden(array('name' => XIVO_SESS_NAME,'value' => XIVO_SESS_ID));?>
<?=$form->hidden(array('name' => 'fm_send','value' => 1));?>

<div id="sb-part-first">

<div id="accesslist" class="fm-field fm-multilist">

	<div class="slt-list">
		<?=$form->select(array('name' => 'accessfeatures[]','label' => false,'id' => 'it-access','key' => true,'altkey' => 'host','multiple' => true,'size' => 5,'field' => false),$info['accessfeatures']);?>
		<div class="bt-adddelete">

			<a href="#" onclick="xivo_fm_select_add_host_ipv4_subnet('it-access',prompt('<?=$dhtml->escape($this->bbf('accessfeatures_add'));?>')); return(false);" title="<?=$this->bbf('bt_addaccess');?>"><?=$url->img_html('img/site/button/mini/blue/add.gif',$this->bbf('bt_addaccess'),'class="bt-addlist" id="bt-addaccess" border="0"');?></a><br />

			<a href="#" onclick="xivo_fm_select_delete_entry('it-access'); return(false);" title="<?=$this->bbf('bt_deleteaccess');?>"><?=$url->img_html('img/site/button/mini/blue/delete.gif',$this->bbf('bt_deleteaccess'),'class="bt-deletelist" id="bt-deleteaccess" border="0"');?></a>

		</div>
	</div>
</div>
<div class="clearboth"></div>

</div>

<div id="sb-part-serverxivo" class="b-nodisplay">

<?php
	if($info['serverxivo']['list'] !== false):
?>
		<div id="serverxivolist" class="fm-field fm-multilist">
			<div class="slt-outlist">

		<?=$form->select(array('name' => 'serverxivolist','label' => false,'id' => 'it-serverxivolist','key' => 'identity','altkey' => 'id','multiple' => true,'size' => 5,'field' => false),$info['serverxivo']['list']);?>

			</div>
			<div class="inout-list">

		<a href="#" onclick="xivo_fm_move_selected('it-serverxivolist','it-serverxivo'); return(false);" title="<?=$this->bbf('bt_inserverxivo');?>"><?=$url->img_html('img/site/button/row-left.gif',$this->bbf('bt_inserverxivo'),'class="bt-inlist" id="bt-inserverxivo" border="0"');?></a><br />

		<a href="#" onclick="xivo_fm_move_selected('it-serverxivo','it-serverxivolist'); return(false);" title="<?=$this->bbf('bt_outserverxivo');?>"><?=$url->img_html('img/site/button/row-right.gif',$this->bbf('bt_outserverxivo'),'class="bt-outlist" id="bt-outserverxivo" border="0"');?></a>

			</div>
			<div class="slt-inlist">

		<?=$form->select(array('name' => 'serverxivo[]','label' => false,'id' => 'it-serverxivo','key' => 'identity','altkey' => 'id','multiple' => true,'size' => 5,'field' => false),$info['serverxivo']['slt']);?>

			</div>
		</div>
		<div class="clearboth"></div>
<?php
	else:
		echo '<div class="txt-center">',$url->href_html($this->bbf('create_serverxivo'),'xivo/configuration/servers','act=add'),'</div>';
	endif;
?>

</div>

<div id="sb-part-last" class="b-nodisplay">

<?php
	if($info['serverldap']['list'] !== false):
?>
		<div id="serverldaplist" class="fm-field fm-multilist">
			<div class="slt-outlist">

		<?=$form->select(array('name' => 'serverldaplist','label' => false,'id' => 'it-serverldaplist','key' => 'identity','altkey' => 'id','multiple' => true,'size' => 5,'field' => false),$info['serverldap']['list']);?>

			</div>
			<div class="inout-list">

		<a href="#" onclick="xivo_fm_move_selected('it-serverldaplist','it-serverldap'); return(false);" title="<?=$this->bbf('bt_inserverldap');?>"><?=$url->img_html('img/site/button/row-left.gif',$this->bbf('bt_inserverldap'),'class="bt-inlist" id="bt-inserverldap" border="0"');?></a><br />

		<a href="#" onclick="xivo_fm_move_selected('it-serverldap','it-serverldaplist'); return(false);" title="<?=$this->bbf('bt_outserverldap');?>"><?=$url->img_html('img/site/button/row-right.gif',$this->bbf('bt_outserverldap'),'class="bt-outlist" id="bt-outserverldap" border="0"');?></a>

			</div>
			<div class="slt-inlist">

		<?=$form->select(array('name' => 'serverldap[]','label' => false,'id' => 'it-serverldap','key' => 'identity','altkey' => 'id','multiple' => true,'size' => 5,'field' => false),$info['serverldap']['slt']);?>

			</div>
		</div>
		<div class="clearboth"></div>
<?php
	else:
		echo '<div class="txt-center">',$url->href_html($this->bbf('create_serverldap'),'service/ipbx/system_management/serverldap','act=add'),'</div>';
	endif;
?>

</div>

<?=$form->submit(array('name' => 'submit','id' => 'it-submit','value' => $this->bbf('fm_bt-save')));?>

</form>
	</div>
	<div class="sb-foot xspan"><span class="span-left">&nbsp;</span><span class="span-center">&nbsp;</span><span class="span-right">&nbsp;</span></div>
</div>

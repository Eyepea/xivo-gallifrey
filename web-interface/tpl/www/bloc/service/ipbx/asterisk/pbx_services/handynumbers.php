<?php
	$url = &$this->get_module('url');
	$form = &$this->get_module('form');
	$dhtml = &$this->get_module('dhtml');

	$info = $this->get_var('info');
	$smenu = $this->get_var('fm_smenu');

	$handynumbers_js = array();

	if($this->get_var('fm_save') === true):
		$handynumbers_js[] = 'xivo_form_success(\''.$dhtml->escape($this->bbf('fm_success-save')).'\');';
	endif;

	if(($smenu_tab = $this->get_var('fm_smenu_tab')) !== '' && ($smenu_part = $this->get_var('fm_smenu_part')) !== ''):
		$handynumbers_js[] = 'xivo_smenu[\'tab\'] = \''.$dhtml->escape($smenu_tab).'\';';
		$handynumbers_js[] = 'xivo_smenu[\'part\'] = \''.$dhtml->escape($smenu_part).'\';';

		if($smenu_part === 'sb-part-last'):
			$handynumbers_js[] = 'xivo_smenu[\'last\'] = true;';
		endif;
	endif;

	if($info['emergency'] !== false):
		$egency_nb = count($info['emergency']);
		$handynumbers_js[] = 'xivo_tlist[\'emergency\'] = new Array();';
		$handynumbers_js[] = 'xivo_tlist[\'emergency\'][\'cnt\'] = '.$egency_nb.';';
	else:
		$egency_nb = 0;
	endif;

	if($info['special'] !== false):
		$special_nb = count($info['special']);
		$handynumbers_js[] = 'xivo_tlist[\'special\'] = new Array();';
		$handynumbers_js[] = 'xivo_tlist[\'special\'][\'cnt\'] = '.$special_nb.';';
	else:
		$special_nb = 0;
	endif;

	$dhtml->write_js($handynumbers_js);
?>
<div class="b-infos b-form">
	<h3 class="sb-top xspan"><span class="span-left">&nbsp;</span><span class="span-center"><?=$this->bbf('title_content_name');?></span><span class="span-right">&nbsp;</span></h3>

<div class="sb-smenu">
	<ul>
		<li id="smenu-tab-1" class="moo" onclick="xivo_smenu_click(this,'moc','sb-part-first');" onmouseout="xivo_smenu_out(this,'moo');" onmouseover="xivo_smenu_over(this,'mov');">
			<div class="tab"><span class="span-center"><a href="#" onclick="return(false);"><?=$this->bbf('smenu_emergency');?></a></span></div><span class="span-right">&nbsp;</span>
		</li>
		<li id="smenu-tab-2" class="moo-last" onclick="xivo_smenu_click(this,'moc','sb-part-last',1);" onmouseout="xivo_smenu_out(this,'moo',1);" onmouseover="xivo_smenu_over(this,'mov',1);">
			<div class="tab"><span class="span-center"><a href="#" onclick="return(false);"><?=$this->bbf('smenu_special');?></a></span></div><span class="span-right">&nbsp;</span>
		</li>
	</ul>
</div>
	
	<div class="sb-content">

<?php

if($this->get_var('trunkslist') === false):
	echo '<div class="txt-center">',$url->href_html($this->bbf('create_trunk'),'service/ipbx/trunk_management/sip','act=add'),'</div>';
else:

?>

<form action="#" method="post" accept-charset="utf-8" onsubmit="xivo_smenu_fmsubmit(this);">

<?=$form->hidden(array('name' => XIVO_SESS_NAME,'value' => XIVO_SESS_ID));?>
<?=$form->hidden(array('name' => 'fm_send','value' => 1));?>
<?=$form->hidden(array('name' => 'fm_smenu-tab','value' => 'smenu-tab-1'));?>
<?=$form->hidden(array('name' => 'fm_smenu-part','value' => 'sb-part-first'));?>

<div id="sb-part-first" class="b-nodisplay">
	<div class="sb-list">
	<?=$this->file_include('bloc/service/ipbx/asterisk/pbx_services/handynumbers/form',array('type' => 'emergency','count' => $egency_nb));?>
	</div>
</div>

<div id="sb-part-last" class="b-nodisplay">
	<div class="sb-list">
	<?=$this->file_include('bloc/service/ipbx/asterisk/pbx_services/handynumbers/form',array('type' => 'special','count' => $special_nb));?>
	</div>
</div>

<?=$form->submit(array('name' => 'submit','id' => 'it-submit','value' => $this->bbf('fm_bt-save')));?>
</form>
<?php
	endif;
?>

	</div>
	<div class="sb-foot xspan"><span class="span-left">&nbsp;</span><span class="span-center">&nbsp;</span><span class="span-right">&nbsp;</span></div>
</div>

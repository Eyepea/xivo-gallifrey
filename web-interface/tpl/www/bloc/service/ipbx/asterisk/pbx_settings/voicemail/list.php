<?php
	$url = &$this->get_module('url');
	$form = &$this->get_module('form');
	$dhtml = &$this->get_module('dhtml');

	$pager = $this->get_var('pager');
	$act = $this->get_var('act');

	$page = $url->pager($pager['pages'],
			    $pager['page'],
			    $pager['prev'],
			    $pager['next'],
			    'service/ipbx/pbx_settings/voicemail',
			    array('act' => $act));
?>
<div class="b-list">
<?php
	if($page !== ''):
		echo '<div class="b-page">',$page,'</div>';
	endif;
?>
<form action="#" name="fm-voicemail-list" method="post" accept-charset="utf-8">
<?=$form->hidden(array('name' => XIVO_SESS_NAME,'value' => XIVO_SESS_ID));?>
<?=$form->hidden(array('name' => 'act','value' => $act));?>
<?=$form->hidden(array('name' => 'page','value' => $pager['page']));?>
<table cellspacing="0" cellpadding="0" border="0">
	<tr class="sb-top">
		<th class="th-left xspan"><span class="span-left">&nbsp;</span></th>
		<th class="th-center"><?=$this->bbf('col_fullname');?></th>
		<th class="th-center"><?=$this->bbf('col_mailbox');?></th>
		<th class="th-center"><?=$this->bbf('col_email');?></th>
		<th class="th-center" id="col-action" colspan="2"><?=$this->bbf('col_action');?></th>
		<th class="th-right xspan"><span class="span-right">&nbsp;</span></th>
	</tr>
<?php

	if(($list = $this->get_var('list')) === false || ($nb = count($list)) === 0):
?>
	<tr class="sb-content">
		<td colspan="7" class="td-single"><?=$this->bbf('no_voicemail');?></td>
	</tr>
<?php
	else:
		for($i = 0;$i < $nb;$i++):

			$ref = &$list[$i];

			if($ref['commented'] === true):
				$icon = 'disable';
			else:
				$icon = 'enable';
			endif;

			if(xivo_haslen($ref['email']) === false):
				$ref['email'] = '-';
			endif;
?>
	<tr onmouseover="this.tmp = this.className; this.className = 'sb-content l-infos-over';" onmouseout="this.className = this.tmp;" class="sb-content l-infos-<?=(($i % 2) + 1)?>on2">
		<td class="td-left"><?=$form->checkbox(array('name' => 'voicemails[]','value' => $ref['uniqueid'],'label' => false,'id' => 'it-voicemails-'.$i,'checked' => false,'field' => false));?></td>
		<td class="txt-left"><label for="it-voicemails-<?=$i?>" id="lb-voicemails-<?=$i?>"><?=$url->img_html('img/site/flag/'.$icon.'.gif',null,'class="icons-list"');?><?=$ref['fullname']?></label></td>
		<td><?=$ref['mailbox']?></td>
		<td><?=$ref['email']?></td>
		<td class="td-right" colspan="3">
		<?=$url->href_html($url->img_html('img/site/button/edit.gif',$this->bbf('opt_modify'),'border="0"'),'service/ipbx/pbx_settings/voicemail',array('act' => 'edit','id' => $ref['uniqueid']),null,$this->bbf('opt_modify'));?>
		<?=$url->href_html($url->img_html('img/site/button/delete.gif',$this->bbf('opt_delete'),'border="0"'),'service/ipbx/pbx_settings/voicemail',array('act' => 'delete','id' => $ref['uniqueid'],'page' => $pager['page']),'onclick="return(confirm(\''.$dhtml->escape($this->bbf('opt_delete_confirm')).'\'));"',$this->bbf('opt_delete'));?>
		</td>
	</tr>
<?php
		endfor;
	endif;
?>
	<tr class="sb-foot">
		<td class="td-left xspan b-nosize"><span class="span-left b-nosize">&nbsp;</span></td>
		<td class="td-center" colspan="5"><span class="b-nosize">&nbsp;</span></td>
		<td class="td-right xspan b-nosize"><span class="span-right b-nosize">&nbsp;</span></td>
	</tr>
</table>
</form>
<?php
	if($page !== ''):
		echo '<div class="b-page">',$page,'</div>';
	endif;
?>
</div>

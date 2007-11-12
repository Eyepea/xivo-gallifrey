<?php
	$url = &$this->get_module('url');
	$form = &$this->get_module('form');

	$pager = $this->get_var('pager');
	$list = $this->get_var('list');
	$act = $this->get_var('act');

	$page = $url->pager($pager['pages'],$pager['page'],$pager['prev'],$pager['next'],'service/ipbx/trunk_management/iax',array('act' => $act));
?>
<div class="b-list">
<?php
	if($page !== ''):
		echo '<div class="b-page">',$page,'</div>';
	endif;
?>
<form action="#" name="fm-peers-list" method="post" accept-charset="utf-8">
<?=$form->hidden(array('name' => XIVO_SESS_NAME,'value' => XIVO_SESS_ID));?>
<?=$form->hidden(array('name' => 'act','value' => $act));?>
<?=$form->hidden(array('name' => 'page','value' => $pager['page']));?>
<table cellspacing="0" cellpadding="0" border="0">
	<tr class="sb-top">
		<th class="th-left xspan"><span class="span-left">&nbsp;</span></th>
		<th class="th-center"><?=$this->bbf('col_name');?></th>
		<th class="th-center"><?=$this->bbf('col_host');?></th>
		<th class="th-center"><?=$this->bbf('col_connection-type');?></th>
		<th class="th-center"><?=$this->bbf('col_call-limit');?></th>
		<th class="th-center" id="col-action" colspan="2"><?=$this->bbf('col_action');?></th>
		<th class="th-right xspan"><span class="span-right">&nbsp;</span></th>
	</tr>
<?php

	if($list === false || ($nb = count($list)) === 0):
?>
	<tr class="sb-content">
		<td colspan="8" class="td-single"><?=$this->bbf('no_trunk');?></td>
	</tr>
<?php
	else:
		for($i = $pager['beg'],$j = 0;$i < $pager['end'] && $i < $pager['total'];$i++,$j++):

			$ref = &$list[$i];

			if($ref['commented'] === true):
				$icon = 'disable';
			else:
				$icon = 'enable';
			endif;

			$mod = $j % 2 === 0 ? 1 : 2;

			if($ref['type'] === 'user'):
				$connectype = 'in';
			elseif($ref['type'] === 'peer'):
				$connectype = 'out';
			else:
				$connectype = 'inout';
			endif;

			$calllimit = xivo_uint($ref['call-limit']);
?>
	<tr onmouseover="this.tmp = this.className; this.className = 'sb-content l-infos-over';" onmouseout="this.className = this.tmp;" class="sb-content l-infos-<?=$mod?>on2">
		<td class="td-left"><?=$form->checkbox(array('name' => 'peers[]','value' => $ref['id'],'label' => false,'id' => 'it-peers-'.$i,'checked' => false,'field' => false));?></td>
		<td class="txt-left"><label for="it-peers-<?=$i?>" id="lb-peers-<?=$i?>"><?=$url->img_html('img/site/flag/'.$icon.'.gif',null,'class="icons-list"');?><?=$ref['name']?></label></td>
		<td><?=($ref['host'] === 'dynamic' ? $this->bbf('trunk_host-unknown') : $ref['host'])?></td>
		<td><?=$this->bbf('trunk_connect-'.$connectype.'bound');?></td>
		<td><?=($calllimit === 0 ? $this->bbf('trunk_call-unlimited') : $calllimit)?></td>
		<td class="td-right" colspan="3">
		<?=$url->href_html($url->img_html('img/site/button/edit.gif',$this->bbf('opt_modify'),'border="0"'),'service/ipbx/trunk_management/iax',array('act' => 'edit','id' => $ref['id']),null,$this->bbf('opt_modify'));?>
		<?=$url->href_html($url->img_html('img/site/button/delete.gif',$this->bbf('opt_delete'),'border="0"'),'service/ipbx/trunk_management/iax',array('act' => 'delete','id' => $ref['id'],'page' => $pager['page']),'onclick="return(confirm(\''.xivo_stript($this->bbf('opt_delete_confirm')).'\') ? true : false);"',$this->bbf('opt_delete'));?>
		</td>
	</tr>
<?php
		endfor;
	endif;
?>
	<tr class="sb-foot">
		<td class="td-left xspan b-nosize"><span class="span-left b-nosize">&nbsp;</span></td>
		<td class="td-center" colspan="6"><span class="b-nosize">&nbsp;</span></td>
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

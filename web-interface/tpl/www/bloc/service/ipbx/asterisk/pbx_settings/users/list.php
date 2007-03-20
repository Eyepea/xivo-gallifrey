<?php
	$url = &$this->get_module('url');
	$form = &$this->get_module('form');

	$pager = $this->vars('pager');
	$list = $this->vars('list');
	$act = $this->vars('act');
	$ract = $this->vars('ract');

	$param = array('ract' => $ract);
	$search = '';

	if($ract === 'search' && ($search = $this->vars('search')) !== ''):
		$act = 'search';
		$param['search'] = $search;
	endif;

	$page = $url->pager($pager['pages'],$pager['page'],$pager['prev'],$pager['next'],'service/ipbx/pbx_settings/users',array('act' => $act,$param));
?>
<div class="b-list">
<?php
	if($page !== ''):
		echo '<div class="b-page">',$page,'</div>';
	endif;
?>
<form action="#" name="fm-users-list" method="post" accept-charset="utf-8">
<?=$form->hidden(array('name' => XIVO_SESS_NAME,'value' => XIVO_SESS_ID));?>
<?=$form->hidden(array('name' => 'act','value' => $act));?>
<?=$form->hidden(array('name' => 'ract','value' => $ract));?>
<?=$form->hidden(array('name' => 'page','value' => $pager['page']));?>
<?=($search !== '' ? $form->hidden(array('name' => 'search','value' => $search)) : '');?>
<table cellspacing="0" cellpadding="0" border="0">
	<tr class="sb-top">
		<th class="th-left xspan"><span class="span-left">&nbsp;</span></th>
		<th class="th-center"><?=$this->bbf('col_callerid');?></th>
		<th class="th-center"><?=$this->bbf('col_protocol');?></th>
		<th class="th-center"><?=$this->bbf('col_username');?></th>
		<th class="th-center"><?=$this->bbf('col_phone');?></th>
		<th class="th-center"><?=$this->bbf('col_provisioning');?></th>
		<th class="th-center" id="col-action" colspan="2"><?=$this->bbf('col_action');?></th>
		<th class="th-right xspan"><span class="span-right">&nbsp;</span></th>
	</tr>
<?php

	if($list === false || ($nb = count($list)) === 0):
?>
	<tr class="sb-content">
		<td colspan="9" class="td-single"><?=$this->bbf('no_user')?></td>
	</tr>
<?php
	else:
		for($i = $pager['beg']; $i < $pager['end'] && $i < $pager['total'];$i++):

			$ref = &$list[$i];

			if($ref['protocol']['initialized'] === false):
				$icon = 'unavailable';
			elseif($ref['protocol']['commented'] === true):
				$icon = 'disable';
			else:
				$icon = 'enable';
			endif;

			$mod = $i % 2 === 0 ? 1 : 2;
?>
	<tr onmouseover="this.tmp = this.className; this.className = 'sb-content l-infos-over';" onmouseout="this.className = this.tmp;" class="sb-content l-infos-<?=$mod?>on2">
		<td class="td-left"><?=$form->checkbox(array('name' => 'users['.$ref['ufeatures']['protocol'].'][]','value' => $ref['protocol']['id'],'label' => false,'id' => 'it-users-'.$i,'checked' => false,'field' => false));?></td>
		<td class="txt-left"><label for="it-users-<?=$i?>" id="lb-users-<?=$i?>"><?=$url->img_html('img/site/flag/'.$icon.'.gif',null,'class="icons-list"');?><?=$ref['protocol']['callerid']?></label></td>
		<td><?=$ref['ufeatures']['protocol']?></td>
		<td><?=$ref['protocol']['name']?></td>
		<td><?=(xivo_empty($ref['ufeatures']['number']) === false ? $ref['ufeatures']['number'] : '-')?></td>
		<td><?=$ref['ufeatures']['provisioningid']?></td>
		<td class="td-right" colspan="3">
		<?=$url->href_html($url->img_html('img/site/button/edit.gif',$this->bbf('opt_modify'),'border="0"'),'service/ipbx/pbx_settings/users',array('act' => 'edit','id' => $ref['ufeatures']['id'],'page' => $pager['page']),null,$this->bbf('opt_modify'));?>
		<?=$url->href_html($url->img_html('img/site/button/delete.gif',$this->bbf('opt_delete'),'border="0"'),'service/ipbx/pbx_settings/users',array('act' => 'delete','id' => $ref['ufeatures']['id'],'page' => $pager['page'],$param),'onclick="return(confirm(\''.xivo_stript($this->bbf('opt_delete_confirm')).'\') ? true : false);"',$this->bbf('opt_delete'));?>
		</td>
	</tr>
<?php
		endfor;
	endif;
?>
	<tr class="sb-foot">
		<td class="td-left xspan b-nosize"><span class="span-left b-nosize">&nbsp;</span></td>
		<td class="td-center" colspan="7"><span class="b-nosize">&nbsp;</span></td>
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

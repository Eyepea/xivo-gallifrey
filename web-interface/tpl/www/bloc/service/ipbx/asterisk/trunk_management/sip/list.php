<?php
	$url = &$this->get_module('url');
	$form = &$this->get_module('form');
	$dhtml = &$this->get_module('dhtml');

	$pager = $this->get_var('pager');
	$list = $this->get_var('list');
	$act = $this->get_var('act');

	$page = $url->pager($pager['pages'],
			    $pager['page'],
			    $pager['prev'],
			    $pager['next'],
			    'service/ipbx/trunk_management/sip',
			    array('act' => $act));
?>
<div class="b-list">
<?php
	if($page !== ''):
		echo '<div class="b-page">',$page,'</div>';
	endif;
?>
<form action="#" name="fm-trunk-list" method="post" accept-charset="utf-8">
<?=$form->hidden(array('name' => XIVO_SESS_NAME,'value' => XIVO_SESS_ID));?>
<?=$form->hidden(array('name' => 'act','value' => $act));?>
<?=$form->hidden(array('name' => 'page','value' => $pager['page']));?>
<table cellspacing="0" cellpadding="0" border="0">
	<tr class="sb-top">
		<th class="th-left xspan"><span class="span-left">&nbsp;</span></th>
		<th class="th-center"><?=$this->bbf('col_name');?></th>
		<th class="th-center"><?=$this->bbf('col_host');?></th>
		<th class="th-center"><?=$this->bbf('col_type');?></th>
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
		for($i = 0;$i < $nb;$i++):

			$ref = &$list[$i];

			if($ref['protocol']['commented'] === true):
				$icon = 'disable';
			else:
				$icon = 'enable';
			endif;

			$calllimit = xivo_uint($ref['protocol']['call-limit']);
?>
	<tr onmouseover="this.tmp = this.className; this.className = 'sb-content l-infos-over';" onmouseout="this.className = this.tmp;" class="sb-content l-infos-<?=(($i % 2) + 1)?>on2">
		<td class="td-left"><?=$form->checkbox(array('name' => 'trunks[]','value' => $ref['trunkfeatures']['id'],'label' => false,'id' => 'it-trunks-'.$i,'checked' => false,'field' => false));?></td>
		<td class="txt-left"><label for="it-trunks-<?=$i?>" id="lb-trunks-<?=$i?>"><?=$url->img_html('img/site/flag/'.$icon.'.gif',null,'class="icons-list"');?><?=$ref['protocol']['name']?></label></td>
		<td><?=($ref['protocol']['host'] === 'dynamic' ? $this->bbf('protocol_host-unknown') : $ref['protocol']['host'])?></td>
		<td><?=$this->bbf('protocol_type-'.$ref['protocol']['type']);?></td>
		<td><?=($calllimit === 0 ? $this->bbf('protocol_call-unlimited') : $calllimit)?></td>
		<td class="td-right" colspan="3">
		<?=$url->href_html($url->img_html('img/site/button/edit.gif',$this->bbf('opt_modify'),'border="0"'),'service/ipbx/trunk_management/sip',array('act' => 'edit','id' => $ref['trunkfeatures']['id']),null,$this->bbf('opt_modify'));?>
		<?=$url->href_html($url->img_html('img/site/button/delete.gif',$this->bbf('opt_delete'),'border="0"'),'service/ipbx/trunk_management/sip',array('act' => 'delete','id' => $ref['trunkfeatures']['id'],'page' => $pager['page']),'onclick="return(confirm(\''.$dhtml->escape($this->bbf('opt_delete_confirm')).'\'));"',$this->bbf('opt_delete'));?>
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

<?php
	$url = &$this->get_module('url');
?>
<div class="b-list">
<table cellspacing="0" cellpadding="0" border="0">
	<tr class="sb-top">
		<th class="th-left xspan"><span class="span-left">&nbsp;</span></th>
		<th class="th-center"><?=$this->bbf('col_name');?></th>
		<th class="th-center"><?=$this->bbf('col_number');?></th>
		<th class="th-center"><?=$this->bbf('col_number-users');?></th>
		<th class="th-center" id="col-action" colspan="2"><?=$this->bbf('col_action');?></th>
		<th class="th-right xspan"><span class="span-right">&nbsp;</span></th>
	</tr>
<?php
	$list = $this->vars('list');

	if($list === false || ($nb = count($list)) === 0):
?>
	<tr class="sb-content">
		<td colspan="7" class="td-single"><?=$this->bbf('no_group')?></td>
	</tr>
<?php
	else:
		for($i = 0; $i < $nb;$i++):

			$ref = &$list[$i];

			$mod = $i % 2 === 0 ? 1 : 2;
?>
	<tr class="sb-content l-infos-<?=$mod?>on2">
		<td class="td-left txt-left" colspan="2"><?=$ref['gfeatures']['name']?></td>
		<td><?=($ref['gfeatures']['number'] !== '' ? $ref['gfeatures']['number'] : '-')?></td>
		<td><?=$ref['nb_qmember']?></td>
		<td class="td-right" colspan="3">
		<?=$url->href_html($url->img_html('img/site/button/edit.gif',$this->bbf('opt_modify'),'border="0"'),'service/ipbx/pbx_settings/groups',array('act' => 'edit','id' => $ref['gfeatures']['id']),null,$this->bbf('opt_modify'));?>

<?php
	if($ref['nb_qmember'] === 0):
		echo $url->href_html($url->img_html('img/site/button/delete.gif',$this->bbf('opt_delete'),'border="0"'),'service/ipbx/pbx_settings/groups',array('act' => 'delete','id' => $ref['gfeatures']['id']),'onclick="return(confirm(\''.xivo_stript($this->bbf('opt_delete_confirm')).'\') ? true : false);"',$this->bbf('opt_delete'));
	endif;
?>
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
</div>

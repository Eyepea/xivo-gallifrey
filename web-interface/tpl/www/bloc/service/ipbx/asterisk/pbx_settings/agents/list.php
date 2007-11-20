<?php
	$url = &$this->get_module('url');
	$form = &$this->get_module('form');
	$dhtml = &$this->get_module('dhtml');

	$pager = $this->get_var('pager');
	$list = $this->get_var('list_grps');
	$act = $this->get_var('act');

	$page = $url->pager($pager['pages'],$pager['page'],$pager['prev'],$pager['next'],'service/ipbx/pbx_settings/agents',array('act' => $act));
?>
<div id="sr-agentgroup" class="b-list">
<?php
	if($page !== ''):
		echo '<div class="b-page">',$page,'</div>';
	endif;
?>
<table cellspacing="0" cellpadding="0" border="0">
	<tr class="sb-top">
		<th class="th-left xspan"><span class="span-left">&nbsp;</span></th>
		<th class="th-center"><?=$this->bbf('col_group');?></th>
		<th class="th-center"><?=$this->bbf('col_number-agents');?></th>
		<th class="th-center" id="col-action"><?=$this->bbf('col_action');?></th>
		<th class="th-right xspan"><span class="span-right">&nbsp;</span></th>
	</tr>
<?php

	if($list === false || ($nb = count($list)) === 0):
?>
	<tr class="sb-content">
		<td colspan="5" class="td-single"><?=$this->bbf('no_group');?></td>
	</tr>
<?php
	else:
		for($i = $pager['beg'],$j = 0;$i < $pager['end'] && $i < $pager['total'];$i++,$j++):

			$ref = &$list[$i];

			$mod = $j % 2 === 0 ? 1 : 2;
?>
	<tr onmouseover="this.tmp = this.className; this.className = 'sb-content l-infos-over';" onmouseout="this.className = this.tmp;" class="sb-content l-infos-<?=$mod?>on2">
		<td class="td-left txt-left curpointer" colspan="2" onclick="location.href = this.firstChild;"><?=$url->href_html($ref['agroup']['name'],'service/ipbx/pbx_settings/agents',array('act' => 'listagent','group' => $ref['agroup']['id']));?></td>
		<td><?=$ref['nb_agent']?></td>
		<td class="td-right" colspan="2">
		<?=$url->href_html($url->img_html('img/site/button/edit.gif',$this->bbf('opt_modify'),'border="0"'),'service/ipbx/pbx_settings/agents',array('act' => 'edit','group' => $ref['agroup']['id']),null,$this->bbf('opt_modify'));?>

<?php
		if($ref['agroup']['deletable'] === true):
			echo $url->href_html($url->img_html('img/site/button/delete.gif',$this->bbf('opt_delete'),'border="0"'),'service/ipbx/pbx_settings/agents',array('act' => 'delete','group' => $ref['agroup']['id'],'page' => $pager['page']),'onclick="return(confirm(\''.$dhtml->escape($this->bbf('opt_delete_confirm')).'\'));"',$this->bbf('opt_delete'));
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
		<td class="td-center" colspan="3"><span class="b-nosize">&nbsp;</span></td>
		<td class="td-right xspan b-nosize"><span class="span-right b-nosize">&nbsp;</span></td>
	</tr>
</table>
<?php
	if($page !== ''):
		echo '<div class="b-page">',$page,'</div>';
	endif;
?>
</div>

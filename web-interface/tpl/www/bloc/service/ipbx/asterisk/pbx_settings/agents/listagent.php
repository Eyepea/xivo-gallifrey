<?php
	$url = &$this->get_module('url');
	$form = &$this->get_module('form');

	$pager = $this->vars('pager');
	$list = $this->vars('list');
	$act = $this->vars('act');
	$group = $this->vars('group');

	$page = $url->pager($pager['pages'],
			    $pager['page'],
			    $pager['prev'],
			    $pager['next'],
			    'service/ipbx/pbx_settings/agents',
			    array('act' => $act,'group' => $group));
?>
<div id="sr-agent" class="b-list">
<?php
	if($page !== ''):
		echo '<div class="b-page">',$page,'</div>';
	endif;
?>
<form action="#" name="fm-agents-list" method="post" accept-charset="utf-8">
<?=$form->hidden(array('name' => XIVO_SESS_NAME,'value' => XIVO_SESS_ID));?>
<?=$form->hidden(array('name' => 'act','value' => $act));?>
<?=$form->hidden(array('name' => 'page','value' => $pager['page']));?>
<?=$form->hidden(array('name' => 'group','value' => $group));?>
<table cellspacing="0" cellpadding="0" border="0">
	<tr class="sb-top">
		<th class="th-left xspan"><span class="span-left">&nbsp;</span></th>
		<th class="th-center"><?=$this->bbf('col_fullname');?></th>
		<th class="th-center"><?=$this->bbf('col_number');?></th>
		<th class="th-center"><?=$this->bbf('col_passwd');?></th>
		<th class="th-center" id="col-action"><?=$this->bbf('col_action');?></th>
		<th class="th-right xspan"><span class="span-right">&nbsp;</span></th>
	</tr>
<?php

	if($list === false || ($nb = count($list)) === 0):
?>
	<tr class="sb-content">
		<td colspan="6" class="td-single"><?=$this->bbf('no_agent');?></td>
	</tr>
<?php
	else:
		for($i = $pager['beg'],$j = 0;$i < $pager['end'] && $i < $pager['total'];$i++,$j++):

			$ref = &$list[$i];

			if(xivo_empty($ref['afeatures']['firstname']) === true && xivo_empty($ref['afeatures']['lastname']) === true):
				$fullname = '-';
			else:
				$fullname = trim($ref['afeatures']['firstname'].' '.$ref['afeatures']['lastname']);
			endif;

			if($ref['agent']['passwd'] === '')
				$ref['agent']['passwd'] = '-';

			$mod = $j % 2 === 0 ? 1 : 2;
?>
	<tr onmouseover="this.tmp = this.className; this.className = 'sb-content l-infos-over';" onmouseout="this.className = this.tmp;" class="sb-content l-infos-<?=$mod?>on2">
		<td class="td-left"><?=$form->checkbox(array('name' => 'agents[]','value' => $ref['afeatures']['id'],'label' => false,'id' => 'it-agents-'.$i,'checked' => false,'field' => false));?></td>
		<td class="txt-left"><label for="it-agents-<?=$i?>" id="lb-agents-<?=$i?>"><?=$fullname?></label></td>
		<td><?=$ref['agent']['number']?></td>
		<td><?=$ref['agent']['passwd']?></td>
		<td class="td-right" colspan="2">
		<?=$url->href_html($url->img_html('img/site/button/edit.gif',$this->bbf('opt_modify'),'border="0"'),'service/ipbx/pbx_settings/agents',array('act' => 'editagent','group' => $ref['agroup']['id'],'id' => $ref['afeatures']['id']),null,$this->bbf('opt_modify'));?>
		<?=$url->href_html($url->img_html('img/site/button/delete.gif',$this->bbf('opt_delete'),'border="0"'),'service/ipbx/pbx_settings/agents',array('act' => 'deleteagent','group' => $ref['agroup']['id'],'id' => $ref['afeatures']['id'],'page' => $pager['page']),'onclick="return(confirm(\''.xivo_stript($this->bbf('opt_delete_confirm')).'\') ? true : false);"',$this->bbf('opt_delete'));?>
		</td>
	</tr>
<?php
		endfor;
	endif;
?>
	<tr class="sb-foot">
		<td class="td-left xspan b-nosize"><span class="span-left b-nosize">&nbsp;</span></td>
		<td class="td-center" colspan="4"><span class="b-nosize">&nbsp;</span></td>
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

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
			    'service/ipbx/system_management/context',
			    array('act' => $act));
?>
<div class="b-list">
<?php
	if($page !== ''):
		echo '<div class="b-page">',$page,'</div>';
	endif;
?>
<form action="#" name="fm-context-list" method="post" accept-charset="utf-8">
<?=$form->hidden(array('name' => XIVO_SESS_NAME,'value' => XIVO_SESS_ID));?>
<?=$form->hidden(array('name' => 'act','value' => $act));?>
<?=$form->hidden(array('name' => 'page','value' => $pager['page']));?>
<table cellspacing="0" cellpadding="0" border="0">
	<tr class="sb-top">
		<th class="th-left xspan"><span class="span-left">&nbsp;</span></th>
		<th class="th-center"><?=$this->bbf('col_name');?></th>
		<th class="th-center"><?=$this->bbf('col_displayname');?></th>
		<th class="th-center"><?=$this->bbf('col_entity');?></th>
		<th class="th-center" id="col-action" colspan="2"><?=$this->bbf('col_action');?></th>
		<th class="th-right xspan"><span class="span-right">&nbsp;</span></th>
	</tr>
<?php

	if(($list = $this->get_var('list')) === false || ($nb = count($list)) === 0):
?>
	<tr class="sb-content">
		<td colspan="7" class="td-single"><?=$this->bbf('no_context');?></td>
	</tr>
<?php
	else:
		for($i = 0;$i < $nb;$i++):

			$ref = &$list[$i];

			if($ref['context']['commented'] === true):
				$icon = 'disable';
			else:
				$icon = 'enable';
			endif;
			if($ref['entity'] === false):
				$entity = '-';
			else:
				$entity = $ref['entity']['identity'];
			endif;
?>
	<tr onmouseover="this.tmp = this.className; this.className = 'sb-content l-infos-over';" onmouseout="this.className = this.tmp;" class="sb-content l-infos-<?=(($i % 2) + 1)?>on2">
		<td class="td-left"><?=$form->checkbox(array('name' => 'contexts[]','value' => $ref['context']['name'],'label' => false,'id' => 'it-contexts-'.$i,'checked' => false,'field' => false));?></td>
		<td class="txt-left"><label for="it-contexts-<?=$i?>" id="lb-contexts-<?=$i?>"><?=$url->img_html('img/site/flag/'.$icon.'.gif',null,'class="icons-list"');?><?=$ref['context']['name']?></label></td>
		<td><?=$ref['context']['displayname']?></td>
		<td><?=$entity?></td>
		<td class="td-right" colspan="3">
		<?=$url->href_html($url->img_html('img/site/button/edit.gif',$this->bbf('opt_modify'),'border="0"'),'service/ipbx/system_management/context',array('act' => 'edit','id' => $ref['context']['name']),null,$this->bbf('opt_modify'));?>

<?php
	if($ref['deletable'] === true):
		echo $url->href_html($url->img_html('img/site/button/delete.gif',$this->bbf('opt_delete'),'border="0"'),'service/ipbx/system_management/context',array('act' => 'delete','id' => $ref['context']['name'],'page' => $pager['page']),'onclick="return(confirm(\''.$dhtml->escape($this->bbf('opt_delete_confirm')).'\'));"',$this->bbf('opt_delete'));
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
</form>
<?php
	if($page !== ''):
		echo '<div class="b-page">',$page,'</div>';
	endif;
?>
</div>

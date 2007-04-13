<?php
	$url = &$this->get_module('url');
	$form = &$this->get_module('form');

	$pager = $this->vars('pager');
	$list = $this->vars('list');
	$act = $this->vars('act');

	$page = $url->pager($pager['pages'],$pager['page'],$pager['prev'],$pager['next'],'service/ipbx/call_management/did',array('act' => $act));
?>
<div class="b-list">
<?php
	if($page !== ''):
		echo '<div class="b-page">',$page,'</div>';
	endif;
?>
<form action="#" name="fm-did-list" method="post" accept-charset="utf-8">
<?=$form->hidden(array('name' => XIVO_SESS_NAME,'value' => XIVO_SESS_ID));?>
<?=$form->hidden(array('name' => 'act','value' => $act));?>
<?=$form->hidden(array('name' => 'page','value' => $pager['page']));?>
<table cellspacing="0" cellpadding="0" border="0">
	<tr class="sb-top">
		<th class="th-left xspan"><span class="span-left">&nbsp;</span></th>
		<th class="th-center"><?=$this->bbf('col_did');?></th>
		<th class="th-center"><?=$this->bbf('col_type');?></th>
		<th class="th-center"><?=$this->bbf('col_fullname');?></th>
		<th class="th-center"><?=$this->bbf('col_number');?></th>
		<th class="th-center" id="col-action" colspan="2"><?=$this->bbf('col_action');?></th>
		<th class="th-right xspan"><span class="span-right">&nbsp;</span></th>
	</tr>
<?php

	if($list === false || ($nb = count($list)) === 0):
?>
	<tr class="sb-content">
		<td colspan="8" class="td-single"><?=$this->bbf('no_did')?></td>
	</tr>
<?php
	else:
		for($i = $pager['beg'],$j = 0;$i < $pager['end'] && $i < $pager['total'];$i++,$j++):

			$ref = &$list[$i];

			$type = $this->bbf('did_type-'.$ref['dfeatures']['type']); 

			if($ref['dfeatures']['disable'] === true):
				$icon = 'unavailable';
				$type = '-';
			elseif($ref['did']['commented'] === true):
				$icon = 'disable';
			else:
				$icon = 'enable';
			endif;

			$fullname = $number = '-';

			if($ref['tyfeatures'] !== false && $ref['dfeatures']['disable'] === false):
				if($ref['dfeatures']['type'] !== 'user'):
					$fullname = $ref['tyfeatures']['name'];
				elseif(xivo_empty($ref['tyfeatures']['firstname']) === false || xivo_empty($ref['tyfeatures']['lastname']) === false):
					$fullname = trim($ref['tyfeatures']['firstname'].' '.$ref['tyfeatures']['lastname']);
				endif;

				$number = $ref['tyfeatures']['number'];
			endif;

			$mod = $j % 2 === 0 ? 1 : 2;
?>
	<tr onmouseover="this.tmp = this.className; this.className = 'sb-content l-infos-over';" onmouseout="this.className = this.tmp;" class="sb-content l-infos-<?=$mod?>on2">
		<td class="td-left"><?=$form->checkbox(array('name' => 'dids[]','value' => $ref['dfeatures']['id'],'label' => false,'id' => 'it-dids-'.$i,'checked' => false,'field' => false));?></td>
		<td class="txt-left"><label for="it-dids-<?=$i?>" id="lb-dids-<?=$i?>"><?=$url->img_html('img/site/flag/'.$icon.'.gif',null,'class="icons-list"');?><?=$ref['did']['exten']?></label></td>
		<td><?=$type?></td>
		<td><?=$fullname?></td>
		<td><?=$number?></td>
		<td class="td-right" colspan="3">
		<?=$url->href_html($url->img_html('img/site/button/edit.gif',$this->bbf('opt_modify'),'border="0"'),'service/ipbx/call_management/did',array('act' => 'edit','id' => $ref['dfeatures']['id']),null,$this->bbf('opt_modify'));?>
		<?=$url->href_html($url->img_html('img/site/button/delete.gif',$this->bbf('opt_delete'),'border="0"'),'service/ipbx/call_management/did',array('act' => 'delete','id' => $ref['dfeatures']['id'],'page' => $pager['page']),'onclick="return(confirm(\''.xivo_stript($this->bbf('opt_delete_confirm')).'\') ? true : false);"',$this->bbf('opt_delete'));?>
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

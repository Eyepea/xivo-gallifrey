<?php
	$url = &$this->get_module('url');
	$form = &$this->get_module('form');

	$pager = $this->get_var('pager');
	$list = $this->get_var('list');
	$act = $this->get_var('act');

	$param = array();

	if(($search = (string) $this->get_var('search')) !== ''):
		$param['search'] = $search;
	endif;

	$page = $url->pager($pager['pages'],
			    $pager['page'],
			    $pager['prev'],
			    $pager['next'],
			    'service/ipbx/call_management/incall',
			    array('act' => $act,$param));
?>
<div class="b-list">
<?php
	if($page !== ''):
		echo '<div class="b-page">',$page,'</div>';
	endif;
?>
<form action="#" name="fm-incall-list" method="post" accept-charset="utf-8">
<?=$form->hidden(array('name' => XIVO_SESS_NAME,'value' => XIVO_SESS_ID));?>
<?=$form->hidden(array('name' => 'act','value' => $act));?>
<?=$form->hidden(array('name' => 'page','value' => $pager['page']));?>
<?=$form->hidden(array('name' => 'search','value' => ''));?>
<table cellspacing="0" cellpadding="0" border="0">
	<tr class="sb-top">
		<th class="th-left xspan"><span class="span-left">&nbsp;</span></th>
		<th class="th-center"><?=$this->bbf('col_did');?></th>
		<th class="th-center"><?=$this->bbf('col_destination');?></th>
		<th class="th-center"><?=$this->bbf('col_identity');?></th>
		<th class="th-center" id="col-action" colspan="2"><?=$this->bbf('col_action');?></th>
		<th class="th-right xspan"><span class="span-right">&nbsp;</span></th>
	</tr>
<?php

	if($list === false || ($nb = count($list)) === 0):
?>
	<tr class="sb-content">
		<td colspan="7" class="td-single"><?=$this->bbf('no_incall');?></td>
	</tr>
<?php
	else:
		for($i = 0;$i < $nb;$i++):

			$ref = &$list[$i];

			$type = $this->bbf('incall_type-'.$ref['incall']['type']); 

			if($ref['incall']['linked'] === false):
				$icon = 'unavailable';
				$type = '-';
			elseif($ref['incall']['commented'] === true):
				$icon = 'disable';
			else:
				$icon = 'enable';
			endif;

			$identity = '-';

			if($ref['type'] !== false && $ref['incall']['linked'] === true):
				if($ref['incall']['type'] === 'schedule'):
					$identity = $ref['type']['name'];
				elseif($ref['incall']['type'] === 'application'):
					$identity = $this->bbf('incall_type-application-'.$ref['incall']['typeval'],$ref['incall']['applicationval']);
				elseif(isset($ref['type']['identity']) === true):
					$identity = $ref['type']['identity'];
				endif;
			endif;

			$mod = $i % 2 === 0 ? 1 : 2;
?>
	<tr onmouseover="this.tmp = this.className; this.className = 'sb-content l-infos-over';" onmouseout="this.className = this.tmp;" class="sb-content l-infos-<?=$mod?>on2">
		<td class="td-left"><?=$form->checkbox(array('name' => 'incalls[]','value' => $ref['incall']['id'],'label' => false,'id' => 'it-incalls-'.$i,'checked' => false,'field' => false));?></td>
		<td class="txt-left"><label for="it-incalls-<?=$i?>" id="lb-incalls-<?=$i?>"><?=$url->img_html('img/site/flag/'.$icon.'.gif',null,'class="icons-list"');?><?=$ref['incall']['exten']?></label></td>
		<td><?=$type?></td>
		<td><?=$identity?></td>
		<td class="td-right" colspan="3">
		<?=$url->href_html($url->img_html('img/site/button/edit.gif',$this->bbf('opt_modify'),'border="0"'),'service/ipbx/call_management/incall',array('act' => 'edit','id' => $ref['incall']['id']),null,$this->bbf('opt_modify'));?>
		<?=$url->href_html($url->img_html('img/site/button/delete.gif',$this->bbf('opt_delete'),'border="0"'),'service/ipbx/call_management/incall',array('act' => 'delete','id' => $ref['incall']['id'],'page' => $pager['page'],$param),'onclick="return(confirm(\''.xivo_stript($this->bbf('opt_delete_confirm')).'\') ? true : false);"',$this->bbf('opt_delete'));?>
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

<?php
	$url = &$this->get_module('url');
?>
<div class="b-list">
<table cellspacing="0" cellpadding="0" border="0">
	<tr class="sb-top">
		<th class="th-left xspan"><span class="span-left">&nbsp;</span></th>
		<th class="th-center"><?=$this->bbf('col_login');?></th>
		<th class="th-center"><?=$this->bbf('col_password');?></th>
		<th class="th-center"><?=$this->bbf('col_type');?></th>
		<th class="th-center"><?=$this->bbf('col_dcreate');?></th>
		<th class="th-center"><?=$this->bbf('col_valid');?></th>
		<th class="th-center" colspan="2"><?=$this->bbf('col_action');?></th>
		<th class="th-right xspan"><span class="span-right">&nbsp;</span></th>
	</tr>
<?php
	$list = $this->vars('list');

	if($list === false || ($arr = xivo_get_aks($list)) === false):
?>
	<tr class="sb-content">
		<td colspan="9" class="td-single"><?=$this->bbf('no_user');?></td>
	</tr>
<?php
	else:
		for($i = 0; $i < $arr['cnt'];$i++):
			$k = $arr['keys'][$i];
			$v = &$list[$k];
			$mod = $i % 2 === 0 ? 1 : 2;
?>
	<tr class="sb-content l-infos-<?=$mod?>on2">
		<td class="td-left" colspan="2"><?=$v['login']?></td>
		<td><?=$v['passwd']?></td>
		<td><?=$v['meta']?></td>
		<td><?=strftime($this->bbf('date_format_yymmdd'),$v['dcreate']);?></td>
		<td><?=$this->bbf('valid_'.($v['valid'] === true ? 'true' : 'false'));?></td>
		<td class="td-right" colspan="3">
		<?=$url->href_html($url->img_html('img/site/button/edit.gif',$this->bbf('opt_modify'),'border="0"'),'xivo/configuration',array('cat' => 'edit','id' => $v['id']),null,$this->bbf('opt_modify'));?>
<?php
		if(xivo_user::chk_authorize('admin',$v['meta']) === true):
			echo '&nbsp;',$url->href_html($url->img_html('img/site/button/key.gif',$this->bbf('opt_acl'),'border="0"'),'xivo/configuration',array('cat' => 'acl','id' => $v['id']),null,$this->bbf('opt_acl'));
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
		<td class="td-center" colspan="7"><span class="b-nosize">&nbsp;</span></td>
		<td class="td-right xspan b-nosize"><span class="span-right b-nosize">&nbsp;</span></td>
	</tr>
</table>
</div>

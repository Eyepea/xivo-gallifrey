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
			    'service/ipbx/pbx_settings/meetme',
			    array('act' => $act));
?>
<div class="b-list">
<?php
	if($page !== ''):
		echo '<div class="b-page">',$page,'</div>';
	endif;
?>
<form action="#" name="fm-meetme-list" method="post" accept-charset="utf-8">
<?=$form->hidden(array('name' => XIVO_SESS_NAME,'value' => XIVO_SESS_ID));?>
<?=$form->hidden(array('name' => 'act','value' => $act));?>
<?=$form->hidden(array('name' => 'page','value' => $pager['page']));?>
<table cellspacing="0" cellpadding="0" border="0">
	<tr class="sb-top">
		<th class="th-left xspan"><span class="span-left">&nbsp;</span></th>
		<th class="th-center"><?=$this->bbf('col_name');?></th>
		<th class="th-center"><?=$this->bbf('col_number');?></th>
		<th class="th-center"><?=$this->bbf('col_pin');?></th>
		<th class="th-center"><?=$this->bbf('col_admin-pin');?></th>
		<th class="th-center" id="col-action" colspan="2"><?=$this->bbf('col_action');?></th>
		<th class="th-right xspan"><span class="span-right">&nbsp;</span></th>
	</tr>
<?php
	if(($list = $this->get_var('list')) === false || ($nb = count($list)) === 0):
?>
	<tr class="sb-content">
		<td colspan="8" class="td-single"><?=$this->bbf('no_meetme');?></td>
	</tr>
<?php
	else:
		for($i = 0;$i < $nb;$i++):

			$ref = &$list[$i];

			if($ref['meetmeroom']['commented'] === true):
				$icon = 'disable';
			else:
				$icon = 'enable';
			endif;
?>
	<tr onmouseover="this.tmp = this.className; this.className = 'sb-content l-infos-over';"
	    onmouseout="this.className = this.tmp;"
	    class="sb-content l-infos-<?=(($i % 2) + 1)?>on2">
		<td class="td-left">
			<?=$form->checkbox(array('name'		=> 'meetme[]',
						 'value'	=> $ref['meetmeroom']['id'],
						 'label'	=> false,
						 'id'		=> 'it-meetme-'.$i,
						 'checked'	=> false,
						 'field'	=> false));?>
		</td>
		<td class="txt-left">
			<label for="it-meetme-<?=$i?>" id="lb-meetme-<?=$i?>">
<?php
				echo	$url->img_html('img/site/flag/'.$icon.'.gif',null,'class="icons-list"'),
					xivo_trunc($ref['mfeatures']['name'],25,'...',false);
?>
			</label>
		</td>
		<td><?=$ref['meetmeroom']['number']?></td>
		<td><?=(xivo_haslen($ref['meetmeroom']['pin']) === true ? $ref['meetmeroom']['pin'] : '-')?></td>
		<td><?=(xivo_haslen($ref['meetmeroom']['admin-pin']) === true ? $ref['meetmeroom']['admin-pin'] : '-')?></td>
		<td class="td-right" colspan="3">
<?php
		echo	$url->href_html($url->img_html('img/site/button/edit.gif',
						       $this->bbf('opt_modify'),
						       'border="0"'),
					'service/ipbx/pbx_settings/meetme',
					array('act'	=> 'edit',
					      'id'	=> $ref['meetmeroom']['id']),
					null,
					$this->bbf('opt_modify')),"\n",
			$url->href_html($url->img_html('img/site/button/delete.gif',
						       $this->bbf('opt_delete'),
						       'border="0"'),
					'service/ipbx/pbx_settings/meetme',
					array('act'	=> 'delete',
					      'id'	=> $ref['meetmeroom']['id'],
					      'page'	=> $pager['page']),
					'onclick="return(confirm(\''.$dhtml->escape($this->bbf('opt_delete_confirm')).'\'));"',
					$this->bbf('opt_delete'));
?>
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

<?php
	$url = &$this->get_module('url');
	$form = &$this->get_module('form');
	$dhtml = &$this->get_module('dhtml');

	$pager = $this->get_var('pager');
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
		<th class="th-center"><?=$this->bbf('col_context');?></th>
		<th class="th-center"><?=$this->bbf('col_destination');?></th>
		<th class="th-center"><?=$this->bbf('col_identity');?></th>
		<th class="th-center" id="col-action" colspan="2"><?=$this->bbf('col_action');?></th>
		<th class="th-right xspan"><span class="span-right">&nbsp;</span></th>
	</tr>
<?php

	if(($list = $this->get_var('list')) === false || ($nb = count($list)) === 0):
?>
	<tr class="sb-content">
		<td colspan="8" class="td-single"><?=$this->bbf('no_incall');?></td>
	</tr>
<?php
	else:
		for($i = 0;$i < $nb;$i++):

			$ref = &$list[$i];

			$destination = $this->bbf('incall_destination-'.$ref['destination']);

			if($ref['linked'] === false):
				$icon = 'unavailable';
				$destination = '-';
			elseif($ref['commented'] === true):
				$icon = 'disable';
			else:
				$icon = 'enable';
			endif;

			if($ref['destination'] === 'endcall'):
				$destination = $this->bbf('incall_destination-endcall-'.$ref['destidentity']);
				$destidentity = '-';
			elseif($ref['destination'] === 'application'):
				$destination = $this->bbf('incall_destination-application-'.$ref['destidentity']);
				if($ref['destidentity'] === 'faxtomail'):
					$destidentity = $ref['actionarg1'];
				else:
					$destidentity = '-';
				endif;
			else:
				$destidentity = $ref['destidentity'];
			endif;
?>
	<tr onmouseover="this.tmp = this.className; this.className = 'sb-content l-infos-over';"
	    onmouseout="this.className = this.tmp;"
	    class="sb-content l-infos-<?=(($i % 2) + 1)?>on2">
		<td class="td-left">
			<?=$form->checkbox(array('name'		=> 'incalls[]',
						 'value'	=> $ref['id'],
						 'label'	=> false,
						 'id'		=> 'it-incalls-'.$i,
						 'checked'	=> false,
						 'field'	=> false));?>
		</td>
		<td class="txt-left">
			<label for="it-incalls-<?=$i?>" id="lb-incalls-<?=$i?>">
<?php
				echo	$url->img_html('img/site/flag/'.$icon.'.gif',null,'class="icons-list"'),
					$ref['exten'];
?>
			</label>
		</td>
		<td><?=$ref['context']?></td>
		<td><?=$destination?></td>
		<td><?=$destidentity?></td>
		<td class="td-right" colspan="3">
<?php
			echo	$url->href_html($url->img_html('img/site/button/edit.gif',
							       $this->bbf('opt_modify'),
							       'border="0"'),
						'service/ipbx/call_management/incall',
						array('act'	=> 'edit',
						      'id'	=> $ref['id']),
						null,
						$this->bbf('opt_modify')),"\n",
				$url->href_html($url->img_html('img/site/button/delete.gif',
							       $this->bbf('opt_delete'),
							       'border="0"'),
						'service/ipbx/call_management/incall',
						array('act'	=> 'delete',
						      'id'	=> $ref['id'],
						      'page'	=> $pager['page'],
						      $param),
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

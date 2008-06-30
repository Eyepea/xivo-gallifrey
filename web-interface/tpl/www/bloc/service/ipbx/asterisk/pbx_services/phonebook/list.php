<?php
	$url = &$this->get_module('url');
	$form = &$this->get_module('form');
	$dhtml = &$this->get_module('dhtml');

	$pager = $this->get_var('pager');
	$act = $this->get_var('act');

	$param = array();

	if(($search = (string) $this->get_var('search')) !== ''):
		$param['search'] = $search;
	else:
		$param = null;	
	endif;

	$page = $url->pager($pager['pages'],
			    $pager['page'],
			    $pager['prev'],
			    $pager['next'],
			    'service/ipbx/pbx_services/phonebook',
			    array('act' => $act,$param));
?>
<div class="b-list">
<?php
	if($page !== ''):
		echo	'<div class="b-total">',
			$this->bbf('number_phonebook-result',
				   '<b>'.$this->get_var('total').'</b>'),
			'</div><div class="b-page">',
			$page,
			'</div><div class="clearboth"></div>';
	endif;
?>
<form action="#" name="fm-phonebook-list" method="post" accept-charset="utf-8">
<?=$form->hidden(array('name' => XIVO_SESS_NAME,'value' => XIVO_SESS_ID));?>
<?=$form->hidden(array('name' => 'act','value' => $act));?>
<?=$form->hidden(array('name' => 'page','value' => $pager['page']));?>
<?=$form->hidden(array('name' => 'search','value' => ''));?>
<table cellspacing="0" cellpadding="0" border="0">
	<tr class="sb-top">
		<th class="th-left xspan"><span class="span-left">&nbsp;</span></th>
		<th class="th-center"><?=$this->bbf('col_displayname');?></th>
		<th class="th-center"><?=$this->bbf('col_society');?></th>
		<th class="th-center"><?=$this->bbf('col_tel-office');?></th>
		<th class="th-center"><?=$this->bbf('col_tel-mobile');?></th>
		<th class="th-center"><?=$this->bbf('col_email');?></th>
		<th class="th-center" id="col-action" colspan="2"><?=$this->bbf('col_action');?></th>
		<th class="th-right xspan"><span class="span-right">&nbsp;</span></th>
	</tr>
<?php

	if(($list = $this->get_var('list')) === false || ($nb = count($list)) === 0):
?>
	<tr class="sb-content">
		<td colspan="9" class="td-single"><?=$this->bbf('no_phonebook');?></td>
	</tr>
<?php
	else:
		for($i = 0;$i < $nb;$i++):

			$ref = &$list[$i];

			if(is_array($ref['phonebooknumber']) === false):
				$ref['phonebooknumber'] = array();
			endif;

			if(isset($ref['phonebooknumber']['office']) === false
			|| xivo_haslen($ref['phonebooknumber']['office'],'number') === false):
				$ref['phonebooknumber']['office'] = array();
				$ref['phonebooknumber']['office']['number'] = '-';
			endif;

			if(isset($ref['phonebooknumber']['mobile']) === false
			|| xivo_haslen($ref['phonebooknumber']['mobile'],'number') === false):
				$ref['phonebooknumber']['mobile'] = array();
				$ref['phonebooknumber']['mobile']['number'] = '-';
			endif;
?>
	<tr onmouseover="this.tmp = this.className; this.className = 'sb-content l-infos-over';"
	    onmouseout="this.className = this.tmp;"
	    class="sb-content l-infos-<?=(($i % 2) + 1)?>on2">
		<td class="td-left">
			<?=$form->checkbox(array('name'		=> 'phonebook[]',
						 'value'	=> $ref['phonebook']['id'],
						 'label'	=> false,
						 'id'		=> 'it-phonebook-'.$i,
						 'checked'	=> false,
						 'field'	=> false));?>
		</td>
		<td class="txt-left">
			<label for="it-phonebook-<?=$i?>" id="lb-phonebook-<?=$i?>">
				<?=xivo_htmlen(xivo_trunc($ref['phonebook']['displayname'],30,'...',false));?>
			</label>
		</td>
		<td><?=(xivo_haslen($ref['phonebook']['society']) === true ? $ref['phonebook']['society'] : '-')?></td>
		<td><?=$ref['phonebooknumber']['office']['number']?></td>
		<td><?=$ref['phonebooknumber']['mobile']['number']?></td>
		<td><?=(xivo_haslen($ref['phonebook']['email']) === true ? $ref['phonebook']['email'] : '-')?></td>
		<td class="td-right" colspan="3">
<?php
			echo	$url->href_html($url->img_html('img/site/button/edit.gif',
							       $this->bbf('opt_modify'),
							       'border="0"'),
						'service/ipbx/pbx_services/phonebook',
						array('act'	=> 'edit',
						      'id'	=> $ref['phonebook']['id']),
						null,
						$this->bbf('opt_modify')),"\n",
				$url->href_html($url->img_html('img/site/button/delete.gif',
							       $this->bbf('opt_delete'),
							       'border="0"'),
						'service/ipbx/pbx_services/phonebook',
						array('act'	=> 'delete',
						      'id'	=> $ref['phonebook']['id'],
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
		<td class="td-center" colspan="7"><span class="b-nosize">&nbsp;</span></td>
		<td class="td-right xspan b-nosize"><span class="span-right b-nosize">&nbsp;</span></td>
	</tr>
</table>
</form>
<?php
	if($page !== ''):
		echo	'<div class="b-total">',
			$this->bbf('number_phonebook-result',
				   '<b>'.$this->get_var('total').'</b>'),
			'</div><div class="b-page">',
			$page,
			'</div><div class="clearboth"></div>';
	endif;
?>
</div>

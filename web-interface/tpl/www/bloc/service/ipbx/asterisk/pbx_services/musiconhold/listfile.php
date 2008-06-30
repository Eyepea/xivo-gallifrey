<?php
	$url = &$this->get_module('url');
	$form = &$this->get_module('form');
	$dhtml = &$this->get_module('dhtml');

	$pager = $this->get_var('pager');
	$act = $this->get_var('act');
	$cat = $this->get_var('cat');

	$page = $url->pager($pager['pages'],
			    $pager['page'],
			    $pager['prev'],
			    $pager['next'],
			    'service/ipbx/pbx_services/musiconhold',
			    array('act' => $act,'cat' => $cat));
?>
<div class="b-list">
<?php
	if($page !== ''):
		echo '<div class="b-page">',$page,'</div>';
	endif;
?>
<table cellspacing="0" cellpadding="0" border="0">
	<tr class="sb-top">
		<th class="th-left xspan"><span class="span-left">&nbsp;</span></th>
		<th class="th-center"><?=$this->bbf('col_file');?></th>
		<th class="th-center" id="col-action"><?=$this->bbf('col_action');?></th>
		<th class="th-right xspan"><span class="span-right">&nbsp;</span></th>
	</tr>
<?php
	if(($list = $this->get_var('list')) === false || ($nb = count($list)) === 0):
?>
	<tr class="sb-content">
		<td colspan="4" class="td-single"><?=$this->bbf('no_file');?></td>
	</tr>
<?php
	else:
		for($i = $pager['beg'],$j = 0; $i < $pager['end'] && $i < $pager['total'];$i++,$j++):

			$ref = &$list[$i];
?>
	<tr onmouseover="this.tmp = this.className; this.className = 'sb-content l-infos-over';"
	    onmouseout="this.className = this.tmp;"
	    class="sb-content l-infos-<?=(($j % 2) + 1)?>on2">
		<td class="td-left txt-left curpointer"
		    colspan="2"
		    onclick="location.href = xivo_firstchild(this);">
		    	<?=$url->href_html($ref,
					   'service/ipbx/pbx_services/musiconhold',
					   array('act'	=> 'download',
					         'cat'	=> $cat,
						 'id'	=> $ref,
						 'page'	=> $pager['page']));?>
		</td>
		<td class="td-right" colspan="2">
<?php
			echo	$url->href_html($url->img_html('img/site/button/edit.gif',
							       $this->bbf('opt_modify'),
							       'border="0"'),
						'service/ipbx/pbx_services/musiconhold',
						array('act'	=> 'editfile',
						      'cat'	=> $cat,
						      'id'	=> $ref),
						null,
						$this->bbf('opt_modify')),"\n",
				$url->href_html($url->img_html('img/site/button/delete.gif',
							       $this->bbf('opt_delete'),
							       'border="0"'),
						'service/ipbx/pbx_services/musiconhold',
						array('act'	=> 'deletefile',
						      'cat'	=> $cat,
						      'id'	=> $ref,
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
		<td class="td-center" colspan="2"><span class="b-nosize">&nbsp;</span></td>
		<td class="td-right xspan b-nosize"><span class="span-right b-nosize">&nbsp;</span></td>
	</tr>
</table>
<?php
	if($page !== ''):
		echo '<div class="b-page">',$page,'</div>';
	endif;
?>
</div>

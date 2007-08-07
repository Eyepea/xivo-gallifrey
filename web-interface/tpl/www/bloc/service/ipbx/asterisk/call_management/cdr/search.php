<?php
	$form = &$this->get_module('form');
	$url = &$this->get_module('url');

	$element = $this->vars('element');
	$pager = $this->vars('pager');

	$result = $this->vars('result');
	$info = $this->vars('info');

	if(xivo_empty($info['amaflags']) === true):
		$amaflags = null;
	else:
		$amaflags = xivo_uint($info['amaflags']);
	endif;
	
	$page = $exportcsv = '';

	if($result === false):
		$js_result = 'xivo_smenu[\'last\'] = true;';
	else:
		$js_result = 'xivo_smenu[\'tab\'] = \'smenu-tab-2\';'."\n".
			     'xivo_smenu[\'part\'] = \'sb-part-result\';';

		if($info !== null):
			$page_query = $info;
			$page_query['search'] = 1;
			$page_query['act'] = 'search';
			$page = $url->pager($pager['pages'],
					    $pager['page'],
					    $pager['prev'],
					    $pager['next'],
					    'service/ipbx/call_management/cdr',
					    $page_query);
		endif;


		$exportcsv_query = $info;
		$exportcsv_query['search'] = 1;
		$exportcsv_query['act'] = 'exportcsv';
	endif;

	$dhtml = &$this->get_module('dhtml');
	$dhtml->write_js($js_result);
?>
<div id="sr-cdr" class="b-infos b-form">
	<h3 class="sb-top xspan"><span class="span-left">&nbsp;</span><span class="span-center"><?=$this->bbf('title_content_name');?></span><span class="span-right">&nbsp;</span></h3>

<div class="sb-smenu">
	<ul>
<?php
	if($result === false):
?>
		<li id="smenu-tab-1" class="moo-last" onclick="xivo_smenu_click(this,'moc','sb-part-first',1);" onmouseout="xivo_smenu_out(this,'moo',1);" onmouseover="xivo_smenu_over(this,'mov',1);">
			<div><span class="span-center"><a href="#" onclick="xivo_smenu_click(this,'moc','sb-part-first'); return(false);"><?=$this->bbf('smenu_search');?></a></span></div><span class="span-right">&nbsp;</span>
		</li>
<?php
	else:
?>
		<li id="smenu-tab-1" class="moo" onclick="xivo_smenu_click(this,'moc','sb-part-first');" onmouseout="xivo_smenu_out(this,'moo');" onmouseover="xivo_smenu_over(this,'mov');">
			<div><span class="span-center"><a href="#" onclick="xivo_smenu_click(this,'moc','sb-part-first'); return(false);"><?=$this->bbf('smenu_search');?></a></span></div><span class="span-right">&nbsp;</span>
		</li>
		<li id="smenu-tab-2" class="moo" onclick="xivo_smenu_click(this,'moc','sb-part-result');" onmouseout="xivo_smenu_out(this,'moo');" onmouseover="xivo_smenu_over(this,'mov');">
			<div><span class="span-center"><a href="#" onclick="xivo_smenu_click(this,'moc','sb-part-result'); return(false);"><?=$this->bbf('smenu_result');?></a></span></div><span class="span-right">&nbsp;</span>
		</li>
		<li id="smenu-tab-3" class="moo-last" onclick="xivo_smenu_click(this,'moc','sb-part-result',1); location.href = this.childNodes[1].firstChild.firstChild.href;" onmouseout="xivo_smenu_out(this,'moo',1);" onmouseover="xivo_smenu_over(this,'mov',1);">
			<div id="toto"><span class="span-center"><?=$url->href_html($this->bbf('smenu_exportcsv'),'service/ipbx/call_management/cdr',$exportcsv_query,'onclick="xivo_smenu_click(this,\'moc\',\'sb-part-result\');"');?></span></div><span class="span-right">&nbsp;</span>
		</li>
<?php
	endif;
?>
	</ul>
</div>

	<div class="sb-content">

<div id="sb-part-first"<?=($result !== false ? ' class="b-nodisplay"' : '')?>>

<form action="#" method="post" accept-charset="utf-8">

<?=$form->hidden(array('name' => XIVO_SESS_NAME,'value' => XIVO_SESS_ID));?>
<?=$form->hidden(array('name' => 'fm_send','value' => '1'));?>
<?=$form->hidden(array('name' => 'act','value' => 'search'));?>

<div class="fm-field fm-desc-inline">
<?=$form->text(array('desc' => $this->bbf('fm_dbeg'),'field' => false,'name' => 'dbeg','labelid' => 'dbeg','default' => $element['dbeg']['default'],'value' => $info['dbeg']),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->text(array('desc' => $this->bbf('fm_dend'),'field' => false,'name' => 'dend','labelid' => 'dend','value' => $info['dend']),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>
</div>

<?=$form->select(array('desc' => $this->bbf('fm_channel'),'name' => 'channel','labelid' => 'channel','empty' => true,'bbf' => array('mixvalue','fm_channel-opt'),'default' => $element['channel']['default'],'value' => $info['channel']),$element['channel']['value'],'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->select(array('desc' => $this->bbf('fm_disposition'),'name' => 'disposition','labelid' => 'disposition','empty' => true,'key' => false,'bbf' => array('concatkey','fm_disposition-opt-'),'default' => $element['disposition']['default'],'value' => $info['disposition']),$element['disposition']['value'],'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->select(array('desc' => $this->bbf('fm_amaflags'),'name' => 'amaflags','labelid' => 'amaflags','empty' => true,'bbf' => array('mixvalue','fm_amaflags-opt-'),'default' => $element['amaflags']['default'],'value' => $amaflags),$element['amaflags']['value'],'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<div class="fm-field fm-multifield">
<?=$form->text(array('desc' => $this->bbf('fm_src'),'field' => false,'name' => 'src','labelid' => 'src','size' => 15,'default' => $element['src']['default'],'value' => $info['src']),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>
<?=$form->select(array('field' => false,'name' => 'srcformat','labelid' => 'srcformat','key' => false,'bbf' => array('concatkey','fm_srcformat-opt-'),'default' => $element['srcformat']['default'],'value' => $info['srcformat']),$element['srcformat']['value'],'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>
</div>

<div class="fm-field fm-multifield">
<?=$form->text(array('desc' => $this->bbf('fm_dst'),'field' => false,'name' => 'dst','labelid' => 'dst','size' => 15,'default' => $element['dst']['default'],'value' => $info['dst']),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>
<?=$form->select(array('field' => false,'name' => 'dstformat','labelid' => 'dstformat','key' => false,'bbf' => array('concatkey','fm_dstformat-opt-'),'default' => $element['dstformat']['default'],'value' => $info['dstformat']),$element['dstformat']['value'],'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>
</div>

<div class="fm-field fm-multifield">
<?=$form->text(array('desc' => $this->bbf('fm_clid'),'field' => false,'name' => 'clid','labelid' => 'clid','size' => 15,'notag' => false,'default' => $element['clid']['default'],'value' => $info['clid']),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>
<?=$form->select(array('field' => false,'name' => 'clidformat','labelid' => 'clidformat','key' => false,'bbf' => array('concatkey','fm_clidformat-opt-'),'default' => $element['clidformat']['default'],'value' => $info['clidformat']),$element['clidformat']['value'],'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>
</div>

<div class="fm-field fm-multifield">
<?=$form->text(array('desc' => $this->bbf('fm_accountcode'),'field' => false,'name' => 'accountcode','labelid' => 'accountcode','size' => 15,'default' => $element['accountcode']['default'],'value' => $info['accountcode']),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>
<?=$form->select(array('field' => false,'name' => 'accountcodeformat','labelid' => 'accountcodeformat','key' => false,'bbf' => array('concatkey','fm_accountcodeformat-opt-'),'default' => $element['accountcodeformat']['default'],'value' => $info['accountcodeformat']),$element['accountcodeformat']['value'],'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>
</div>

<div class="fm-field fm-multifield">
<?=$form->text(array('desc' => $this->bbf('fm_userfield'),'field' => false,'name' => 'userfield','labelid' => 'userfield','size' => 15,'default' => $element['userfield']['default'],'value' => $info['userfield']),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>
<?=$form->select(array('field' => false,'name' => 'userfieldformat','labelid' => 'userfieldformat','key' => false,'bbf' => array('concatkey','fm_userfieldformat-opt-'),'default' => $element['userfieldformat']['default'],'value' => $info['userfieldformat']),$element['userfieldformat']['value'],'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>
</div>

<div class="fm-field fm-desc-inline">
<div class="fm-multifield">
<?=$form->text(array('desc' => $this->bbf('fm_dubeg'),'field' => false,'name' => 'dubeg','labelid' => 'dubeg','default' => $element['dubeg']['default'],'value' => $info['dubeg']),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>
<?=$form->select(array('field' => false,'name' => 'dubegunit','id' => 'dubegunit','label' => false,'key' => false,'bbf' => array('concatkey','fm_dubegunit-opt-'),'default' => $element['dubegunit']['default'],'value' => $info['dubegunit']),$element['dubegunit']['value'],'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>
</div>

<div class="fm-multifield">
<?=$form->text(array('desc' => $this->bbf('fm_duend'),'field' => false,'name' => 'duend','labelid' => 'duend','value' => $info['duend']),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>
<?=$form->select(array('field' => false,'name' => 'duendunit','id' => 'duendunit','label' => false,'key' => false,'bbf' => array('concatkey','fm_duendunit-opt-'),'default' => $element['duendunit']['default'],'value' => $info['duendunit']),$element['duendunit']['value'],'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>
</div>
</div>

<?=$form->submit(array('name' => 'submit','id' => 'it-submit','value' => $this->bbf('fm_bt-search')));?>

</form>

</div>

<?php
	if($result !== false):
?>
<div id="sb-part-result">
<div class="sb-list">
<?php
	if($page !== ''):
		echo '<div class="b-total">',$this->bbf('number_cdr-result','<b>'.$this->vars('total').'</b>'),
		'</div><div class="b-page">',$page,'</div><div class="clearboth"></div>';
	endif;
?>
	<table cellspacing="0" cellpadding="0" border="0">
		<tr class="sb-top">
			<th class="th-left"><?=$this->bbf('col_calldate');?></th>
			<th class="th-center"><?=$this->bbf('col_src');?></th>
			<th class="th-center"><?=$this->bbf('col_dst');?></th>
			<th class="th-right"><?=$this->bbf('col_duration');?></th>
		</tr>
<?php
	if($result === null || ($nb = count($result)) === 0):
?>
		<tr>
			<td colspan="4" class="td-single"><?=$this->bbf('no_cdr-result');?></td>
		</tr>
<?
	else:
		for($i = 0;$i < $nb;$i++):
			$ref = &$result[$i];
			$mod = $i % 2 === 0 ? 1 : 2;

			if(xivo_empty($ref['src']) === true):
				$src = '-';
			else:
				$src = xivo_htmlen(xivo_trunc($ref['src'],15,'...'));
			endif;

			if(xivo_empty($ref['dst']) === true):
				$dst = '-';
			else:
				$dst = xivo_htmlen(xivo_trunc($ref['dst'],15,'...'));
			endif;

			$duration = xivo_calc_duration(0,0,$ref['duration'],true);

			$cnt_duration = count($duration);

			if($cnt_duration === 4):
				$bbf_duration = 'entry_duration-dayhourminsec';
			elseif($cnt_duration === 3):
				$bbf_duration = 'entry_duration-hourminsec';
			elseif($cnt_duration === 2):
				$bbf_duration = 'entry_duration-minsec';
			else:
				$bbf_duration = 'entry_duration-sec';
			endif;

			$billsec = xivo_calc_duration(0,0,$ref['billsec'],true);

			$cnt_billsec = count($billsec);

			if($cnt_billsec === 4):
				$bbf_billsec = 'entry_billsec-dayhourminsec';
			elseif($cnt_billsec === 3):
				$bbf_billsec = 'entry_billsec-hourminsec';
			elseif($cnt_billsec === 2):
				$bbf_billsec = 'entry_billsec-minsec';
			else:
				$bbf_billsec = 'entry_billsec-sec';
			endif;

			if($ref['channel'] === XIVO_SRE_IPBX_AST_CHAN_UNKNOWN)
				$ref['channel'] = $this->bbf('entry_channel-unknown');
?>
	<tr class="sb-content l-infos-<?=$mod?>on2 curpointer" onmouseover="this.tmp = this.className; this.className = 'sb-content l-infos-over curpointer';" onmouseout="this.className = this.tmp;" onclick="this.entryline = xivo_eid('cdr-infos-<?=$i?>').style.display; xivo_eid('cdr-infos-<?=$i?>').style.display = this.entryline == '' || this.entryline == 'none' ?  'table-row' : 'none';">
		<td class="td-left txt-left"><a href="#" onclick="return(false);"><?=strftime($this->bbf('date_format_yymmddhhiiss'),$ref['callunixtime']);?></a></td>
		<td><?=$src?></td>
		<td><?=$dst?></td>
		<td class="td-right"><?=$this->bbf($bbf_duration,$duration);?></td>
	</tr>
	<tr id="cdr-infos-<?=$i?>" class="sb-content l-infos-<?=$mod?>on2 b-nodisplay cdr-infos">
		<td colspan="4" class="td-single">
			<dl>
			<?php
				if(xivo_empty($ref['channel']) === false):
					echo '<dt>',$this->bbf('entry_channel'),'</dt>';
					echo '<dd title="',xivo_htmlen($ref['channel']),'">',xivo_htmlen(xivo_trunc($ref['channel'],30,'...',false)),'<br /></dd>';
				endif;
				if(xivo_empty($ref['disposition']) === false):
					echo '<dt>',$this->bbf('entry_disposition'),'</dt>';
					echo '<dd>',$this->bbf('entry_disposition-'.$ref['disposition']),'<br /></dd>';
				endif;
				if(xivo_empty($ref['amaflagsmeta']) === false):
					echo '<dt>',$this->bbf('entry_amaflagsmeta'),'</dt>';
					echo '<dd>',$this->bbf('entry_amaflagsmeta-'.$ref['amaflagsmeta']),'<br /></dd>';
				endif;
				if(xivo_empty($ref['clid']) === false):
					echo '<dt>',$this->bbf('entry_clid'),'</dt>';
					echo '<dd title="',xivo_htmlen($ref['clid']),'">',xivo_htmlen(xivo_trunc($ref['clid'],30,'...',false)),'<br /></dd>';
				endif;
				if(xivo_empty($ref['accountcode']) === false):
					echo '<dt>',$this->bbf('entry_accountcode'),'</dt>';
					echo '<dd title="',xivo_htmlen($ref['accountcode']),'">',xivo_htmlen(xivo_trunc($ref['accountcode'],20,'...',false)),'<br /></dd>';
				endif;
				if(xivo_empty($ref['userfield']) === false):
					echo '<dt>',$this->bbf('entry_userfield'),'</dt>';
					echo '<dd title="',xivo_htmlen($ref['userfield']),'">',xivo_htmlen(xivo_trunc($ref['userfield'],20,'...',false)),'<br /></dd>';
				endif;
			?>
			</dl>
			<dl>
			<?php
				if(xivo_empty($ref['dcontext']) === false):
					echo '<dt>',$this->bbf('entry_dcontext'),'</dt>';
					echo '<dd title="',xivo_htmlen($ref['dcontext']),'">',xivo_htmlen(xivo_trunc($ref['dcontext'],30,'...',false)),'<br /></dd>';
				endif;
				if(xivo_empty($ref['dstchannel']) === false):
					echo '<dt>',$this->bbf('entry_dstchannel'),'</dt>';
					echo '<dd title="',xivo_htmlen($ref['dstchannel']),'">',xivo_htmlen(xivo_trunc($ref['dstchannel'],20,'...',false)),'<br /></dd>';
				endif;

				echo '<dt>',$this->bbf('entry_billsec'),'</dt>';
				echo '<dd>',$this->bbf($bbf_billsec,$billsec),'<br /></dd>';

				if(xivo_empty($ref['lastapp']) === false):
					echo '<dt>',$this->bbf('entry_lastapp'),'</dt>';
					echo '<dd title="',xivo_htmlen($ref['lastapp']),'">',xivo_htmlen(xivo_trunc($ref['lastapp'],20,'...',false)),'<br /></dd>';
				endif;
				if(xivo_empty($ref['lastdata']) === false):
					echo '<dt>',$this->bbf('entry_lastdata'),'</dt>';
					echo '<dd title="',xivo_htmlen($ref['lastdata']),'">',xivo_htmlen(xivo_trunc($ref['lastdata'],25,'...',false)),'<br /></dd>';
				endif;
				if(xivo_empty($ref['uniqueid']) === false):
					echo '<dt>',$this->bbf('entry_uniqueid'),'</dt>';
					echo '<dd title="',xivo_htmlen($ref['uniqueid']),'">',xivo_htmlen(xivo_trunc($ref['uniqueid'],20,'...',false)),'<br /></dd>';
				endif;
			?>
			</dl>
		</td>
	</tr>
<?php
		endfor;
	endif;
?>
	</table>
<?php
	if($page !== ''):
		echo '<div class="b-total">',$this->bbf('number_cdr-result','<b>'.$this->vars('total').'</b>'),
		'</div><div class="b-page">',$page,'</div><div class="clearboth"></div>';
	endif;
?>
</div>
</div>
<?php
	endif;
?>
	</div>
	<div class="sb-foot xspan"><span class="span-left">&nbsp;</span><span class="span-center">&nbsp;</span><span class="span-right">&nbsp;</span></div>
</div>

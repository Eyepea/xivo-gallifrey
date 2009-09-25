<?php

#
# XiVO Web-Interface
# Copyright (C) 2006-2009  Proformatique <technique@proformatique.com>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#

$form = &$this->get_module('form');
$url = &$this->get_module('url');

$element = $this->get_var('element');
$pager = $this->get_var('pager');

$result = $this->get_var('result');
$info = $this->get_var('info');
$context_list = $this->get_var('context_list');

if(($dcontext_custom = (string) $this->get_var('dcontext-custom')) !== ''):
	$dcontext = 'custom';
else:
	$dcontext = (string) $info['dcontext'];
endif;

if(dwho_has_len($info['amaflags']) === false):
	$amaflags = null;
else:
	$amaflags = dwho_uint($info['amaflags']);
endif;

$page = $exportcsv = '';

$js_result = array();

if($result === false):
	$js_result[] = 'xivo_smenu[\'last\'] = true;';
else:
	$js_result[] = 'xivo_smenu[\'tab\'] = \'smenu-tab-2\';';
	$js_result[] = 'xivo_smenu[\'part\'] = \'sb-part-result\';';

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
	<h3 class="sb-top xspan">
		<span class="span-left">&nbsp;</span>
		<span class="span-center"><?=$this->bbf('title_content_name');?></span>
		<span class="span-right">&nbsp;</span>
	</h3>

<div class="sb-smenu">
	<ul>
<?php
	if($result === false):
?>
		<li id="smenu-tab-1"
		    class="moo-last"
		    onclick="xivo_smenu_click(this,'moc','sb-part-first',1);"
		    onmouseout="xivo_smenu_out(this,'moo',1);"
		    onmouseover="xivo_smenu_over(this,'mov',1);">
			<div class="tab">
				<span class="span-center"><a href="#" onclick="return(false);"><?=$this->bbf('smenu_search');?></a></span>
			</div>
			<span class="span-right">&nbsp;</span>
		</li>
<?php
	else:
?>
		<li id="smenu-tab-1"
		    class="moo"
		    onclick="xivo_smenu_click(this,'moc','sb-part-first');"
		    onmouseout="xivo_smenu_out(this,'moo');"
		    onmouseover="xivo_smenu_over(this,'mov');">
			<div class="tab">
				<span class="span-center"><a href="#" onclick="return(false);"><?=$this->bbf('smenu_search');?></a></span>
			</div>
			<span class="span-right">&nbsp;</span>
		</li>
		<li id="smenu-tab-2"
		    class="moo"
		    onclick="xivo_smenu_click(this,'moc','sb-part-result');"
		    onmouseout="xivo_smenu_out(this,'moo');"
		    onmouseover="xivo_smenu_over(this,'mov');">
			<div class="tab">
				<span class="span-center"><a href="#" onclick="return(false);"><?=$this->bbf('smenu_result');?></a></span>
			</div>
			<span class="span-right">&nbsp;</span>
		</li>
		<li id="smenu-tab-3"
		    class="moo-last"
		    onclick="xivo_smenu_click(this,'moc','sb-part-result',1);
			     location.href = dwho.dom.node.firstchild(
						dwho.dom.node.firstchild(
							dwho.dom.node.firstchild(this)));"
		    onmouseout="xivo_smenu_out(this,'moo',1);" onmouseover="xivo_smenu_over(this,'mov',1);">
			<div class="tab">
				<span class="span-center"><?=$url->href_html($this->bbf('smenu_exportcsv'),
									     'service/ipbx/call_management/cdr',
									     $exportcsv_query,
									     'onclick="return(false);"');?></span>
			</div>
			<span class="span-right">&nbsp;</span>
		</li>
<?php
	endif;
?>
	</ul>
</div>

<div class="sb-content">

<div id="sb-part-first"<?=($result !== false ? ' class="b-nodisplay"' : '')?>>
<form action="#" method="post" accept-charset="utf-8">
<?php
	echo	$form->hidden(array('name'	=> DWHO_SESS_NAME,
				    'value'	=> DWHO_SESS_ID)),

		$form->hidden(array('name'	=> 'fm_send',
				    'value'	=> 1)),

		$form->hidden(array('name'	=> 'act',
				    'value'	=> 'search'));
?>

<div class="fm-field fm-desc-inline">
	<div class="fm-multifield">
<?php
	echo	$form->text(array('desc'	=> $this->bbf('fm_dbeg'),
				  'field'	=> false,
				  'name'	=> 'dbeg',
				  'labelid'	=> 'dbeg',
				  'default'	=> dwho_i18n::strftime_l('%Y-%m-%d',null),
				  'value'	=> $info['dbeg']));
?>
<a href="#"
   onclick="dwho_eid('cal-dend').style.display = 'none';
	    xivo_calendar_display('cal-dbeg','it-dbeg');"
   onmouseover="xivo_calendar_body();"
   onmouseout="xivo_calendar_body('cal-dbeg','it-dbeg');"
   title="<?=$this->bbf('bt_showcalendar');?>"><?=$url->img_html('img/site/button/row-down.gif',
								 $this->bbf('bt_showcalendar'),
								 'id="bt-showcalbeg"
								  border="0"
								  style="vertical-align: bottom;padding-left: 2px;"');?>
</a>
	</div>
	<div id="cal-dbeg"
	     class="b-nodisplay"
	     onmouseover="xivo_calendar_body();"
	     onmouseout="xivo_calendar_body('cal-dbeg','it-dbeg');">
	</div>
	<div class="fm-multifield">
<?php
	echo	$form->text(array('desc'	=> $this->bbf('fm_dend'),
				  'field'	=> false,
				  'name'	=> 'dend',
				  'labelid'	=> 'dend',
				  'value'	=> $info['dend']));
?>
<a href="#"
   onclick="dwho_eid('cal-dbeg').style.display = 'none';
	    xivo_calendar_display('cal-dend','it-dend');"
   onmouseover="xivo_calendar_body();"
   onmouseout="xivo_calendar_body('cal-dend','it-dend');"
   title="<?=$this->bbf('bt_showcalendar');?>"><?=$url->img_html('img/site/button/row-down.gif',
								 $this->bbf('bt_showcalendar'),
								 'id="bt-showcalend"
								  border="0"
								  style="vertical-align: bottom;padding-left: 2px;"');?>
</a>
	</div>
	<div id="cal-dend"
	     class="b-nodisplay"
	     onmouseover="xivo_calendar_body();"
	     onmouseout="xivo_calendar_body('cal-dend','it-dend');">
	</div>
</div>

<?php
	echo	$form->select(array('desc'	=> $this->bbf('fm_channel'),
				    'name'	=> 'channel',
				    'labelid'	=> 'channel',
				    'empty'	=> true,
				    'bbf'	=> 'fm_channel-opt',
				    'bbf_opt'	=> array('argmode' => 'paramvalue'),
				    'default'	=> $element['channel']['default'],
				    'value'	=> $info['channel']),
			      $element['channel']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_disposition'),
				    'name'	=> 'disposition',
				    'labelid'	=> 'disposition',
				    'empty'	=> true,
				    'key'	=> false,
				    'bbf'	=> 'fm_disposition-opt',
				    'bbf_opt'	=> array('argmode' => 'paramkey'),
				    'default'	=> $element['disposition']['default'],
				    'value'	=> $info['disposition']),
			      $element['disposition']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_amaflags'),
				    'name'	=> 'amaflags',
				    'labelid'	=> 'amaflags',
				    'empty'	=> true,
				    'bbf'	=> 'ast_amaflag_name_info',
				    'bbf_opt'	=> array('argmode' => 'paramvalue'),
				    'default'	=> $element['amaflags']['default'],
				    'value'	=> $amaflags),
			      $element['amaflags']['value']);

if($context_list !== false):
	echo	$form->select(array('desc'	=> $this->bbf('fm_dcontext'),
				    'name'	=> 'dcontext',
				    'labelid'	=> 'dcontext',
				    'empty'	=> true,
				    'bbf'	=> 'fm_dcontext-opt',
				    'bbf_opt'	=> array('argmode' => 'paramvalue'),
				    'key'	=> 'identity',
				    'altkey'	=> 'name',
				    'value'	=> $dcontext),

			      $context_list,
			      'onchange="xivo_chg_attrib(\'fm_dcontext\',
							 \'fd-dcontext-custom\',
							 Number(this.value === \'custom\'));"'),

		$form->text(array('desc'	=> '&nbsp;',
				  'name'	=> 'dcontext-custom',
				  'labelid'	=> 'dcontext-custom',
				  'size'	=> 15,
				  'value'	=> $dcontext_custom));
else:
	echo	$form->text(array('desc'	=> $this->bbf('fm_dcontext'),
				  'name'	=> 'dcontext',
				  'labelid'	=> 'dcontext',
				  'default'	=> $element['dcontext']['default'],
				  'value'	=> $info['dcontext']));
endif;
?>

<div class="fm-field fm-multifield">
<?php
	echo	$form->text(array('desc'	=> $this->bbf('fm_src'),
				  'field'	=> false,
				  'name'	=> 'src',
				  'labelid'	=> 'src',
				  'size'	=> 15,
				  'default'	=> $element['src']['default'],
				  'value'	=> $info['src'])),

		$form->select(array('field'	=> false,
				    'name'	=> 'srcformat',
				    'labelid'	=> 'srcformat',
				    'key'	=> false,
				    'bbf'	=> 'fm_search-format',
				    'bbf_opt'	=> array('argmode' => 'paramkey'),
				    'default'	=> $element['srcformat']['default'],
				    'value'	=> $info['srcformat']),
			      $element['srcformat']['value']);
?>
</div>

<div class="fm-field fm-multifield">
<?php
	echo	$form->text(array('desc'	=> $this->bbf('fm_dst'),
				  'field'	=> false,
				  'name'	=> 'dst',
				  'labelid'	=> 'dst',
				  'size'	=> 15,
				  'default'	=> $element['dst']['default'],
				  'value'	=> $info['dst'])),

		$form->select(array('field'	=> false,
				    'name'	=> 'dstformat',
				    'labelid'	=> 'dstformat',
				    'key'	=> false,
				    'bbf'	=> 'fm_search-format',
				    'bbf_opt'	=> array('argmode' => 'paramkey'),
				    'default'	=> $element['dstformat']['default'],
				    'value'	=> $info['dstformat']),
			      $element['dstformat']['value']);
?>
</div>

<div class="fm-field fm-multifield">
<?php
	echo	$form->text(array('desc'	=> $this->bbf('fm_clid'),
				  'field'	=> false,
				  'name'	=> 'clid',
				  'labelid'	=> 'clid',
				  'size'	=> 15,
				  'notag'	=> false,
				  'default'	=> $element['clid']['default'],
				  'value'	=> $info['clid'])),

		$form->select(array('field'	=> false,
				    'name'	=> 'clidformat',
				    'labelid'	=> 'clidformat',
				    'key'	=> false,
				    'bbf'	=> 'fm_search-format',
				    'bbf_opt'	=> array('argmode' => 'paramkey'),
				    'default'	=> $element['clidformat']['default'],
				    'value'	=> $info['clidformat']),
			      $element['clidformat']['value']);
?>
</div>

<div class="fm-field fm-multifield">
<?php
	echo	$form->text(array('desc'	=> $this->bbf('fm_accountcode'),
				  'field'	=> false,
				  'name'	=> 'accountcode',
				  'labelid'	=> 'accountcode',
				  'size'	=> 15,
				  'default'	=> $element['accountcode']['default'],
				  'value'	=> $info['accountcode'])),

		$form->select(array('field'	=> false,
				    'name'	=> 'accountcodeformat',
				    'labelid'	=> 'accountcodeformat',
				    'key'	=> false,
				    'bbf'	=> 'fm_search-format',
				    'bbf_opt'	=> array('argmode' => 'paramkey'),
				    'default'	=> $element['accountcodeformat']['default'],
				    'value'	=> $info['accountcodeformat']),
			      $element['accountcodeformat']['value']);
?>
</div>

<div class="fm-field fm-multifield">
<?php
	echo	$form->text(array('desc'	=> $this->bbf('fm_userfield'),
				    'field'	=> false,
				    'name'	=> 'userfield',
				    'labelid'	=> 'userfield',
				    'size'	=> 15,
				    'default'	=> $element['userfield']['default'],
				    'value'	=> $info['userfield'])),

		$form->select(array('field'	=> false,
				    'name'	=> 'userfieldformat',
				    'labelid'	=> 'userfieldformat',
				    'key'	=> false,
				    'bbf'	=> 'fm_search-format',
				    'bbf_opt'	=> array('argmode' => 'paramkey'),
				    'default'	=> $element['userfieldformat']['default'],
				    'value'	=> $info['userfieldformat']),
			      $element['userfieldformat']['value']);
?>
</div>

<div class="fm-field fm-desc-inline">
	<div class="fm-multifield">
<?php
	echo	$form->text(array('desc'	=> $this->bbf('fm_dubeg'),
				  'field'	=> false,
				  'name'	=> 'dubeg',
				  'labelid'	=> 'dubeg',
				  'default'	=> $element['dubeg']['default'],
				  'value'	=> $info['dubeg'])),

		$form->select(array('field'	=> false,
				    'name'	=> 'dubegunit',
				    'id'	=> 'dubegunit',
				    'label'	=> false,
				    'key'	=> false,
				    'bbf'	=> 'fm_dubegunit-opt',
				    'bbf_opt'	=> array('argmode' => 'paramkey'),
				    'default'	=> $element['dubegunit']['default'],
				    'value'	=> $info['dubegunit']),
			      $element['dubegunit']['value']);
?>
	</div>

	<div class="fm-multifield">
<?php
	echo	$form->text(array('desc'	=> $this->bbf('fm_duend'),
				  'field'	=> false,
				  'name'	=> 'duend',
				  'labelid'	=> 'duend',
				  'value'	=> $info['duend'])),

		$form->select(array('field'	=> false,
				    'name'	=> 'duendunit',
				    'id'	=> 'duendunit',
				    'label'	=> false,
				    'key'	=> false,
				    'bbf'	=> 'fm_duendunit-opt',
				    'bbf_opt'	=> array('argmode' => 'paramkey'),
				    'default'	=> $element['duendunit']['default'],
				    'value'	=> $info['duendunit']),
			      $element['duendunit']['value']);
?>
	</div>
</div>

<?php
	echo	$form->submit(array('name'	=> 'submit',
				    'id'	=> 'it-submit',
				    'value'	=> $this->bbf('fm_bt-search')));
?>

</form>
</div>

<?php
	if($result !== false):
?>
<div id="sb-part-result">
<div class="sb-list">
<?php
	if($page !== ''):
		echo	'<div class="b-total">',
			$this->bbf('number_cdr-result','<b>'.$this->get_var('total').'</b>'),
			'</div>',
			'<div class="b-page">',
			$page,
			'</div>',
			'<div class="clearboth"></div>';
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
			$mod = ($i % 2) + 1;

			if(dwho_has_len($ref['src']) === false):
				$src = '-';
			else:
				$src = dwho_htmlen(dwho_trunc($ref['src'],15,'...'));
			endif;

			if(dwho_has_len($ref['dst']) === false):
				$dst = '-';
			else:
				$dst = dwho_htmlen(dwho_trunc($ref['dst'],15,'...'));
			endif;

			$duration = dwho_calc_duration(null,null,$ref['duration'],true);

			if(($cnt_duration = count($duration)) === 4):
				$bbf_duration = 'entry_duration-dayhourminsec';
			elseif($cnt_duration === 3):
				$bbf_duration = 'entry_duration-hourminsec';
			elseif($cnt_duration === 2):
				$bbf_duration = 'entry_duration-minsec';
			else:
				$bbf_duration = 'entry_duration-sec';
			endif;

			$billsec = dwho_calc_duration(null,null,$ref['billsec'],true);

			if(($cnt_billsec = count($billsec)) === 4):
				$bbf_billsec = 'entry_billsec-dayhourminsec';
			elseif($cnt_billsec === 3):
				$bbf_billsec = 'entry_billsec-hourminsec';
			elseif($cnt_billsec === 2):
				$bbf_billsec = 'entry_billsec-minsec';
			else:
				$bbf_billsec = 'entry_billsec-sec';
			endif;

			if($ref['channel'] === XIVO_SRE_IPBX_AST_CHAN_UNKNOWN)
				$ref['channel'] = $this->bbf('entry_channel','unknown');
?>
	<tr class="sb-content l-infos-<?=$mod?>on2 curpointer"
	    onmouseover="this.tmp = this.className;
			 this.className = 'sb-content l-infos-over curpointer';"
	    onmouseout="this.className = this.tmp;"
	    onclick="this.entryline = dwho_eid('cdr-infos-<?=$i?>').style.display;
		     dwho_eid('cdr-infos-<?=$i?>').style.display = this.entryline === '' || this.entryline === 'none'
								   ? 'table-row'
								   : 'none';">
		<td class="td-left">
			<a href="#" onclick="return(false);"><?=dwho_i18n::strftime_l(
								$this->bbf('date_format_yymmddhhiiss'),
								null,
								strtotime($ref['calldate']));?></a>
		</td>
		<td><?=$src?></td>
		<td><?=$dst?></td>
		<td class="td-right"><?=$this->bbf($bbf_duration,$duration);?></td>
	</tr>
	<tr id="cdr-infos-<?=$i?>" class="sb-content l-infos-<?=$mod?>on2 b-nodisplay cdr-infos">
		<td colspan="4" class="td-single">
		<dl>
		<?php
			if(dwho_has_len($ref['channel']) === true):
				echo	'<dt>',$this->bbf('entry_channel'),'</dt>',
					'<dd title="',dwho_htmlen($ref['channel']),'">',
					dwho_htmlen(dwho_trunc($ref['channel'],30,'...',false)),
					'<br /></dd>';
			endif;

			if(dwho_has_len($ref['disposition']) === true):
				echo	'<dt>',$this->bbf('entry_disposition'),'</dt>',
					'<dd>',$this->bbf('entry_disposition',$ref['disposition']),'<br /></dd>';
			endif;

			if(dwho_has_len($ref['amaflagsmeta']) === true):
				echo	'<dt>',$this->bbf('entry_amaflagsmeta'),'</dt>',
					'<dd>',$this->bbf('ast_amaflag_name_info',$ref['amaflagsmeta']),'<br /></dd>';
			endif;

			if(dwho_has_len($ref['clid']) === true):
				echo	'<dt>',$this->bbf('entry_clid'),'</dt>',
					'<dd title="',dwho_htmlen($ref['clid']),'">',
					dwho_htmlen(dwho_trunc($ref['clid'],30,'...',false)),
					'<br /></dd>';
			endif;

			if(dwho_has_len($ref['accountcode']) === true):
				echo	'<dt>',$this->bbf('entry_accountcode'),'</dt>',
					'<dd title="',dwho_htmlen($ref['accountcode']),'">',
					dwho_htmlen(dwho_trunc($ref['accountcode'],20,'...',false)),
					'<br /></dd>';
			endif;

			if(dwho_has_len($ref['userfield']) === true):
				echo	'<dt>',$this->bbf('entry_userfield'),'</dt>',
					'<dd title="',dwho_htmlen($ref['userfield']),'">',
					dwho_htmlen(dwho_trunc($ref['userfield'],20,'...',false)),
					'<br /></dd>';
			endif;
		?>
		</dl>
		<dl>
		<?php
			if(dwho_has_len($ref['dcontext']) === true):
				echo	'<dt>',$this->bbf('entry_dcontext'),'</dt>',
					'<dd title="',dwho_htmlen($ref['dcontext']),'">',
					dwho_htmlen(dwho_trunc($ref['dcontext'],30,'...',false)),
					'<br /></dd>';
			endif;

			if(dwho_has_len($ref['dstchannel']) === true):
				echo	'<dt>',$this->bbf('entry_dstchannel'),'</dt>',
					'<dd title="',dwho_htmlen($ref['dstchannel']),'">',
					dwho_htmlen(dwho_trunc($ref['dstchannel'],20,'...',false)),
					'<br /></dd>';
			endif;

			echo	'<dt>',$this->bbf('entry_billsec'),'</dt>',
				'<dd>',$this->bbf($bbf_billsec,$billsec),'<br /></dd>';

			if(dwho_has_len($ref['lastapp']) === true):
				echo	'<dt>',$this->bbf('entry_lastapp'),'</dt>',
					'<dd title="',dwho_htmlen($ref['lastapp']),'">',
					dwho_htmlen(dwho_trunc($ref['lastapp'],20,'...',false)),
					'<br /></dd>';
			endif;

			if(dwho_has_len($ref['lastdata']) === true):
				echo	'<dt>',$this->bbf('entry_lastdata'),'</dt>',
					'<dd title="',dwho_htmlen($ref['lastdata']),'">',
					dwho_htmlen(dwho_trunc($ref['lastdata'],25,'...',false)),
					'<br /></dd>';
			endif;

			if(dwho_has_len($ref['uniqueid']) === true):
				echo	'<dt>',$this->bbf('entry_uniqueid'),'</dt>',
					'<dd title="',dwho_htmlen($ref['uniqueid']),'">',
					dwho_htmlen(dwho_trunc($ref['uniqueid'],20,'...',false)),
					'<br /></dd>';
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
		echo	'<div class="b-total">',
			$this->bbf('number_cdr-result','<b>'.$this->get_var('total').'</b>'),
			'</div>',
			'<div class="b-page">',$page,'</div>',
			'<div class="clearboth"></div>';
	endif;
?>
</div>
</div>
<?php
	endif;
?>
	</div>
	<div class="sb-foot xspan">
		<span class="span-left">&nbsp;</span>
		<span class="span-center">&nbsp;</span>
		<span class="span-right">&nbsp;</span>
	</div>
</div>

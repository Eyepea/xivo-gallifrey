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
$dhtml = &$this->get_module('dhtml');

$element = $this->get_var('element');
$context_list = $this->get_var('context_list');
$err = $this->get_var('error');

if(($fm_save = $this->get_var('fm_save')) === true):
	$dhtml->write_js('xivo_form_result(true,\''.$dhtml->escape($this->bbf('fm_success-save')).'\');');
elseif($fm_save === false):
	$dhtml->write_js('xivo_form_result(false,\''.$dhtml->escape($this->bbf('fm_error-save')).'\');');
endif;

$format = $this->get_var('voicemail','format','var_val');

$attachformat = is_array($format) === true && empty($format) === false;

$zonemessages = $this->get_var('zonemessages');

if(is_array($zonemessages) === true && ($zmsg_nb = count($zonemessages)) > 0):
	$dhtml->write_js('dwho.dom.set_table_list(\'timezone\','.$zmsg_nb.');');
else:
	$zmsg_nb = 0;
endif;

?>
<div class="b-infos b-form">
<h3 class="sb-top xspan">
	<span class="span-left">&nbsp;</span>
	<span class="span-center"><?=$this->bbf('title_content_name');?></span>
	<span class="span-right">&nbsp;</span>
</h3>
<div class="sb-smenu">
	<ul>
		<li id="smenu-tab-1"
		    class="moo"
		    onclick="xivo_smenu_click(this,'moc','sb-part-first');"
		    onmouseout="xivo_smenu_out(this,'moo');"
		    onmouseover="xivo_smenu_over(this,'mov');">
			<div class="tab">
				<span class="span-center">
					<a href="#" onclick="return(false);"><?=$this->bbf('smenu_general');?></a>
				</span>
			</div>
			<span class="span-right">&nbsp;</span>
		</li>
		<li id="smenu-tab-2"
		    class="moo"
		    onclick="xivo_smenu_click(this,'moc','sb-part-voicemenu');"
		    onmouseout="xivo_smenu_out(this,'moo');"
		    onmouseover="xivo_smenu_over(this,'mov');">
			<div class="tab">
				<span class="span-center">
					<a href="#" onclick="return(false);"><?=$this->bbf('smenu_voicemenu');?></a>
				</span>
			</div>
			<span class="span-right">&nbsp;</span>
		</li>
		<li id="smenu-tab-3"
		    class="moo"
		    onclick="xivo_smenu_click(this,'moc','sb-part-email');"
		    onmouseout="xivo_smenu_out(this,'moo');"
		    onmouseover="xivo_smenu_over(this,'mov');">
			<div class="tab">
				<span class="span-center">
					<a href="#" onclick="return(false);"><?=$this->bbf('smenu_email');?></a>
				</span>
			</div>
			<span class="span-right">&nbsp;</span>
		</li>
		<li id="smenu-tab-4"
		    class="moo"
		    onclick="xivo_smenu_click(this,'moc','sb-part-pager');"
		    onmouseout="xivo_smenu_out(this,'moo');"
		    onmouseover="xivo_smenu_over(this,'mov');">
			<div class="tab">
				<span class="span-center">
					<a href="#" onclick="return(false);"><?=$this->bbf('smenu_pager');?></a>
				</span>
			</div>
			<span class="span-right">&nbsp;</span>
		</li>
		<li id="smenu-tab-5"
		    class="moo"
		    onclick="xivo_smenu_click(this,'moc','sb-part-timezone');"
		    onmouseout="xivo_smenu_out(this,'moo');"
		    onmouseover="xivo_smenu_over(this,'mov');">
			<div class="tab">
				<span class="span-center">
					<a href="#" onclick="return(false);"><?=$this->bbf('smenu_timezones');?></a>
				</span>
			</div>
			<span class="span-right">&nbsp;</span>
		</li>
		<li id="smenu-tab-6"
		    class="moo"
		    onclick="xivo_smenu_click(this,'moc','sb-part-adsi');"
		    onmouseout="xivo_smenu_out(this,'moo');"
		    onmouseover="xivo_smenu_over(this,'mov');">
			<div class="tab">
				<span class="span-center">
					<a href="#" onclick="return(false);"><?=$this->bbf('smenu_adsi');?></a>
				</span>
			</div>
			<span class="span-right">&nbsp;</span>
		</li>
		<li id="smenu-tab-7"
		    class="moo-last"
		    onclick="xivo_smenu_click(this,'moc','sb-part-last',1);"
		    onmouseout="xivo_smenu_out(this,'moo',1);"
		    onmouseover="xivo_smenu_over(this,'mov',1);">
			<div class="tab">
				<span class="span-center">
					<a href="#" onclick="return(false);"><?=$this->bbf('smenu_advanced');?></a>
				</span>
			</div>
			<span class="span-right">&nbsp;</span>
		</li>
	</ul>
</div>

<div class="sb-content">
<form action="#" method="post" accept-charset="utf-8" onsubmit="dwho.form.select('it-voicemail-format');">

<?php
	echo	$form->hidden(array('name'	=> DWHO_SESS_NAME,
				    'value'	=> DWHO_SESS_ID)),
		$form->hidden(array('name'	=> 'fm_send',
				    'value'	=> 1));
?>

<div id="sb-part-first">

<?php

	echo	$form->select(array('desc'	=> $this->bbf('fm_voicemail-maxmsg'),
				    'name'	=> 'voicemail[maxmsg]',
				    'labelid'	=> 'voicemail-maxmsg',
				    'key'	=> false,
				    'selected'	=> $this->get_var('voicemail','maxmsg','var_val'),
				    'default'	=> $element['voicemail']['maxmsg']['default']),
			      $element['voicemail']['maxmsg']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_voicemail-silencethreshold'),
				    'name'	=> 'voicemail[silencethreshold]',
				    'labelid'	=> 'voicemail-silencethreshold',
				    'key'	=> false,
				    'selected'	=> $this->get_var('voicemail','silencethreshold','var_val'),
				    'default'	=> $element['voicemail']['silencethreshold']['default']),
			      $element['voicemail']['silencethreshold']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_voicemail-minmessage'),
				    'name'	=> 'voicemail[minmessage]',
				    'labelid'	=> 'voicemail-minmessage',
				    'key'	=> false,
				    'bbf'	=> 'fm_voicemail-message-opt',
				    'bbfopt'	=> array('argmode'	=> 'paramvalue',
							 'time'		=> array(
									'from'		=> 'second',
									'format'	=> '%M%s')),
				    'selected'	=> $this->get_var('voicemail','minmessage','var_val'),
				    'default'	=> $element['voicemail']['minmessage']['default']),
			      $element['voicemail']['minmessage']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_voicemail-maxmessage'),
				    'name'	=> 'voicemail[maxmessage]',
				    'labelid'	=> 'voicemail-maxmessage',
				    'key'	=> false,
				    'bbf'	=> 'fm_voicemail-message-opt',
				    'bbfopt'	=> array('argmode'	=> 'paramvalue',
							 'time'		=> array(
									'from'		=> 'second',
									'format'	=> '%M%s')),
				    'selected'	=> $this->get_var('voicemail','maxmessage','var_val'),
				    'default'	=> $element['voicemail']['maxmessage']['default']),
			      $element['voicemail']['maxmessage']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_voicemail-maxsilence'),
				    'name'	=> 'voicemail[maxsilence]',
				    'labelid'	=> 'voicemail-maxsilence',
				    'key'	=> false,
				    'bbf'	=> 'fm_voicemail-maxsilence-opt',
				    'bbfopt'	=> array('argmode' => 'paramvalue'),
				    'selected'	=> $this->get_var('voicemail','maxsilence','var_val'),
				    'default'	=> $element['voicemail']['maxsilence']['default']),
			      $element['voicemail']['maxsilence']['value']),

		$form->checkbox(array('desc'	=> $this->bbf('fm_voicemail-review'),
				      'name'	=> 'voicemail[review]',
				      'labelid'	=> 'voicemail-review',
				      'checked'	=> $this->get_var('voicemail','review','var_val'),
				      'default'	=> $element['voicemail']['review']['default'])),

		$form->checkbox(array('desc'	=> $this->bbf('fm_voicemail-operator'),
				      'name'	=> 'voicemail[operator]',
				      'labelid'	=> 'voicemail-operator',
				      'checked'	=> $this->get_var('voicemail','operator','var_val'),
				      'default'	=> $element['voicemail']['operator']['default']));

?>

<div id="formatlist" class="fm-paragraph fm-multilist">
	<p>
		<label id="lb-formatlist" for="it-voicemail-formatlist">
			<?=$this->bbf('fm_voicemail-format');?>
		</label>
	</p>
	<div class="slt-outlist">
		<?=$form->select(array('name'		=> 'voicemail[formatlist]',
				       'label'		=> false,
				       'id'		=> 'it-voicemail-formatlist',
				       'multiple'	=> true,
				       'size'		=> 5,
				       'paragraph'	=> false,
				       'key'		=> false,
				       'bbf'		=> 'ast_format_name_info',
				       'bbfopt'		=> array('argmode' => 'paramvalue')),
				 $element['voicemail']['format']['value']);?>
	</div>
	<div class="inout-list">
		<a href="#"
		   onclick="xivo_voicemail_format('in'); return(dwho.dom.free_focus());"
		   title="<?=$this->bbf('bt_informat');?>">
			<?=$url->img_html('img/site/button/row-left.gif',
					  $this->bbf('bt_informat'),
					  'class="bt-inlist" id="bt-informat" border="0"');?></a><br />
		<a href="#"
		   onclick="xivo_voicemail_format('out'); return(dwho.dom.free_focus());"
		   title="<?=$this->bbf('bt_outformat');?>">
			<?=$url->img_html('img/site/button/row-right.gif',
					  $this->bbf('bt_outformat'),
					  'class="bt-outlist" id="bt-outformat" border="0"');?></a>
	</div>
	<div class="slt-inlist">
		<?=$form->select(array('name'		=> 'voicemail[format][]',
				       'label'		=> false,
				       'id'		=> 'it-voicemail-format',
				       'multiple'	=> true,
				       'size'		=> 5,
				       'paragraph'	=> false,
				       'key'		=> false,
				       'bbf'		=> 'ast_format_name_info',
				       'bbfopt'		=> array('argmode' => 'paramvalue')),
				 $format);?>
	</div>
</div>
<div class="clearboth"></div>
</div>

<div id="sb-part-voicemenu" class="b-nodisplay">
<?php
	echo	$form->select(array('desc'	=> $this->bbf('fm_voicemail-maxlogins'),
				    'name'	=> 'voicemail[maxlogins]',
				    'labelid'	=> 'voicemail-maxlogins',
				    'key'	=> false,
				    'selected'	=> $this->get_var('voicemail','maxlogins','var_val'),
				    'default'	=> $element['voicemail']['maxlogins']['default']),
			      $element['voicemail']['maxlogins']['value']),

		$form->checkbox(array('desc'	=> $this->bbf('fm_voicemail-envelope'),
				      'name'	=> 'voicemail[envelope]',
				      'labelid'	=> 'voicemail-envelope',
				      'checked'	=> $this->get_var('voicemail','envelope','var_val'),
				      'default'	=> $element['voicemail']['envelope']['default'])),

		$form->checkbox(array('desc'	=> $this->bbf('fm_voicemail-saycid'),
				      'name'	=> 'voicemail[saycid]',
				      'labelid'	=> 'voicemail-saycid',
				      'checked'	=> $this->get_var('voicemail','saycid','var_val'),
				      'default'	=> $element['voicemail']['saycid']['default']));

if($context_list !== false):
	echo	$form->select(array('desc'	=> $this->bbf('fm_voicemail-cidinternalcontexts'),
				    'name'	=> 'voicemail[cidinternalcontexts]',
				    'labelid'	=> 'voicemail-cidinternalcontexts',
				    'empty'	=> true,
				    'key'	=> 'identity',
				    'altkey'	=> 'name',
				    'default'	=> $element['voicemail']['cidinternalcontexts']['default'],
				    'selected'	=> $this->get_var('voicemail','cidinternalcontexts','var_val')),
			      $context_list);
endif;

	echo	$form->checkbox(array('desc'	=> $this->bbf('fm_voicemail-sayduration'),
				      'name'	=> 'voicemail[sayduration]',
				      'labelid'	=> 'voicemail-sayduration',
				      'checked'	=> $this->get_var('voicemail','sayduration','var_val'),
				      'default'	=> $element['voicemail']['sayduration']['default'])),

		$form->select(array('desc'	=> $this->bbf('fm_voicemail-saydurationm'),
				    'name'	=> 'voicemail[saydurationm]',
				    'labelid'	=> 'voicemail-saydurationm',
				    'key'	=> false,
				    'bbf'	=> 'fm_voicemail-saydurationm-opt',
				    'bbfopt'	=> array('argmode' => 'paramvalue'),
				    'selected'	=> $this->get_var('voicemail','saydurationm','var_val'),
				    'default'	=> $element['voicemail']['saydurationm']['default']),
			      $element['voicemail']['saydurationm']['value']),

		$form->checkbox(array('desc'	=> $this->bbf('fm_voicemail-forcename'),
				      'name'	=> 'voicemail[forcename]',
				      'labelid'	=> 'voicemail-forcename',
				      'checked'	=> $this->get_var('voicemail','forcename','var_val'),
				      'default'	=> $element['voicemail']['forcename']['default'])),

		$form->checkbox(array('desc'	=> $this->bbf('fm_voicemail-forcegreetings'),
				      'name'	=> 'voicemail[forcegreetings]',
				      'labelid'	=> 'voicemail-forcegreetings',
				      'checked'	=> $this->get_var('voicemail','forcegreetings','var_val'),
				      'default'	=> $element['voicemail']['forcegreetings']['default'])),

		$form->checkbox(array('desc'	=> $this->bbf('fm_voicemail-tempgreetwarn'),
				      'name'	=> 'voicemail[tempgreetwarn]',
				      'labelid'	=> 'voicemail-tempgreetwarn',
				      'checked'	=> $this->get_var('voicemail','tempgreetwarn','var_val'),
				      'default'	=> $element['voicemail']['tempgreetwarn']['default'])),

		$form->select(array('desc'	=> $this->bbf('fm_voicemail-maxgreet'),
				    'name'	=> 'voicemail[maxgreet]',
				    'labelid'	=> 'voicemail-maxgreet',
				    'key'	=> false,
				    'bbf'	=> 'fm_voicemail-maxgreet-opt',
				    'bbfopt'	=> array('argmode'	=> 'paramvalue',
							 'time'		=> array(
									'from'		=> 'second',
									'format'	=> '%M%s')),
				    'selected'	=> $this->get_var('voicemail','maxgreet','var_val'),
				    'default'	=> $element['voicemail']['maxgreet']['default']),
			      $element['voicemail']['maxgreet']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_voicemail-skipms'),
				    'name'	=> 'voicemail[skipms]',
				    'labelid'	=> 'voicemail-skipms',
				    'key'	=> false,
				    'bbf'	=> 'fm_voicemail-skipms-opt',
				    'bbfopt'	=> array('argmode'	=> 'paramvalue',
							 'time'		=> array(
									'from'		=> 'millisecond',
									'format'	=> '%M%s')),
				    'selected'	=> $this->get_var('voicemail','skipms','var_val'),
				    'default'	=> $element['voicemail']['skipms']['default']),
			      $element['voicemail']['skipms']['value']),

		$form->checkbox(array('desc'	=> $this->bbf('fm_voicemail-sendvoicemail'),
				      'name'	=> 'voicemail[sendvoicemail]',
				      'labelid'	=> 'voicemail-sendvoicemail',
				      'checked'	=> $this->get_var('voicemail','sendvoicemail','var_val'),
				      'default'	=> $element['voicemail']['sendvoicemail']['default'])),

		$form->checkbox(array('desc'	=> $this->bbf('fm_voicemail-usedirectory'),
				      'name'	=> 'voicemail[usedirectory]',
				      'labelid'	=> 'voicemail-usedirectory',
				      'checked'	=> $this->get_var('voicemail','usedirectory','var_val'),
				      'default'	=> $element['voicemail']['usedirectory']['default'])),

		$form->checkbox(array('desc'	=> $this->bbf('fm_voicemail-nextaftercmd'),
				      'name'	=> 'voicemail[nextaftercmd]',
				      'labelid'	=> 'voicemail-nextaftercmd',
				      'checked'	=> $this->get_var('voicemail','nextaftercmd','var_val'),
				      'default'	=> $element['voicemail']['nextaftercmd']['default']));

if($context_list !== false):
	echo	$form->select(array('desc'	=> $this->bbf('fm_voicemail-dialout'),
				    'name'	=> 'voicemail[dialout]',
				    'labelid'	=> 'voicemail-dialout',
				    'empty'	=> true,
				    'key'	=> 'identity',
				    'altkey'	=> 'name',
				    'default'	=> $element['voicemail']['dialout']['default'],
				    'selected'	=> $this->get_var('voicemail','dialout','var_val')),
			      $context_list),

		$form->select(array('desc'	=> $this->bbf('fm_voicemail-callback'),
				    'name'	=> 'voicemail[callback]',
				    'labelid'	=> 'voicemail-callback',
				    'empty'	=> true,
				    'key'	=> 'identity',
				    'altkey'	=> 'name',
				    'default'	=> $element['voicemail']['callback']['default'],
				    'selected'	=> $this->get_var('voicemail','callback','var_val')),
			      $context_list),

		$form->select(array('desc'	=> $this->bbf('fm_voicemail-exitcontext'),
				    'name'	=> 'voicemail[exitcontext]',
				    'labelid'	=> 'voicemail-exitcontext',
				    'empty'	=> true,
				    'key'	=> 'identity',
				    'altkey'	=> 'name',
				    'default'	=> $element['voicemail']['exitcontext']['default'],
				    'selected'	=> $this->get_var('voicemail','exitcontext','var_val')),
			      $context_list);
endif;

?>
</div>

<div id="sb-part-email" class="b-nodisplay">
<?php
	echo	$form->checkbox(array('desc'	=> $this->bbf('fm_voicemail-attach'),
				      'name'	=> 'voicemail[attach]',
				      'labelid'	=> 'voicemail-attach',
				      'checked'	=> $this->get_var('voicemail','attach','var_val'),
				      'default'	=> $element['voicemail']['attach']['default'])),

		$form->select(array('desc'	=> $this->bbf('fm_voicemail-attachformat'),
				    'name'	=> 'voicemail[attachformat]',
				    'labelid'	=> 'voicemail-attachformat',
				    'key'	=> false,
				    'bbf'	=> 'ast_format_name_info',
				    'bbfopt'	=> array('argmode' => 'paramvalue')),
			      $format,
			      ($attachformat === false ? 'class="it-disabled" disabled="disabled"' : '')),

		$form->select(array('desc'	=> $this->bbf('fm_voicemail-volgain'),
				    'name'	=> 'voicemail[volgain]',
				    'labelid'	=> 'voicemail-volgain',
				    'key'	=> false,
				    'selected'	=> $this->get_var('voicemail','volgain','var_val'),
				    'default'	=> $element['voicemail']['volgain']['default']),
			      $element['voicemail']['volgain']['value']),

		$form->text(array('desc'	=> $this->bbf('fm_voicemail-mailcmd'),
				  'name'	=> 'voicemail[mailcmd]',
				  'labelid'	=> 'voicemail-mailcmd',
				  'size'	=> 15,
				  'value'	=> $this->get_var('voicemail','mailcmd','var_val'),
				  'default'	=> $element['voicemail']['mailcmd']['default']),
			    'class="it-readonly" readonly="readonly"'),

		$form->text(array('desc'	=> $this->bbf('fm_voicemail-charset'),
				  'name'	=> 'voicemail[charset]',
				  'labelid'	=> 'voicemail-charset',
				  'size'	=> 15,
				  'value'	=> $this->get_var('voicemail','charset','var_val'),
				  'default'	=> $element['voicemail']['charset']['default']),
			    'class="it-readonly" readonly="readonly"'),

		$form->text(array('desc'	=> $this->bbf('fm_voicemail-serveremail'),
				  'name'	=> 'voicemail[serveremail]',
				  'labelid'	=> 'voicemail-serveremail',
				  'size'	=> 15,
				  'value'	=> $this->get_var('voicemail','serveremail','var_val'),
				  'default'	=> $element['voicemail']['serveremail']['default'])),

		$form->text(array('desc'	=> $this->bbf('fm_voicemail-fromstring'),
				  'name'	=> 'voicemail[fromstring]',
				  'labelid'	=> 'voicemail-fromstring',
				  'size'	=> 15,
				  'value'	=> $this->get_var('voicemail','fromstring','var_val'),
				  'default'	=> $element['voicemail']['fromstring']['default'])),

		$form->text(array('desc'	=> $this->bbf('fm_voicemail-emaildateformat'),
				  'name'	=> 'voicemail[emaildateformat]',
				  'labelid'	=> 'voicemail-emaildateformat',
				  'size'	=> 15,
				  'value'	=> $this->get_var('voicemail','emaildateformat','var_val'),
				  'default'	=> $element['voicemail']['emaildateformat']['default'])),

		$form->text(array('desc'	=> $this->bbf('fm_voicemail-emaildatelocale'),
				  'name'	=> 'voicemail[emaildatelocale]',
				  'labelid'	=> 'voicemail-emaildatelocale',
				  'size'	=> 15,
				  'value'	=> $this->get_var('voicemail','emaildatelocale','var_val'),
				  'default'	=> $element['voicemail']['emaildatelocale']['default'])),

		$form->checkbox(array('desc'	=> $this->bbf('fm_voicemail-pbxskip'),
				      'name'	=> 'voicemail[pbxskip]',
				      'labelid'	=> 'voicemail-pbxskip',
				      'checked'	=> $this->get_var('voicemail','pbxskip','var_val'),
				      'default'	=> $element['voicemail']['pbxskip']['default'])),

		$form->text(array('desc'	=> $this->bbf('fm_voicemail-emailsubject'),
				  'name'	=> 'voicemail[emailsubject]',
				  'labelid'	=> 'voicemail-emailsubject',
				  'size'	=> 15,
				  'value'	=> $this->get_var('voicemail','emailsubject','var_val'),
				  'default'	=> $element['voicemail']['emailsubject']['default']));
?>
	<div class="fm-paragraph fm-description">
		<p>
			<label id="lb-emailbody" for="it-emailbody"><?=$this->bbf('fm_voicemail-emailbody');?></label>
		</p>
		<?=$form->textarea(array('paragraph'	=> false,
					 'name'		=> 'voicemail[emailbody]',
					 'label'	=> false,
					 'id'		=> 'it-voicemail-emailbody',
					 'cols'		=> 60,
					 'rows'		=> 10),
				   $this->get_var('voicemail','emailbody','var_val'));?>
	</div>
</div>

<div id="sb-part-pager" class="b-nodisplay">
<?php
	echo	$form->text(array('desc'	=> $this->bbf('fm_voicemail-pagerfromstring'),
				  'name'	=> 'voicemail[pagerfromstring]',
				  'labelid'	=> 'voicemail-pagerfromstring',
				  'size'	=> 15,
				  'value'	=> $this->get_var('voicemail','pagerfromstring','var_val'),
				  'default'	=> $element['voicemail']['pagerfromstring']['default'])),

		$form->text(array('desc'	=> $this->bbf('fm_voicemail-pagersubject'),
				  'name'	=> 'voicemail[pagersubject]',
				  'labelid'	=> 'voicemail-pagersubject',
				  'size'	=> 15,
				  'value'	=> $this->get_var('voicemail','pagersubject','var_val'),
				  'default'	=> $element['voicemail']['pagersubject']['default']));
?>
	<div class="fm-paragraph fm-description">
		<p>
			<label id="lb-pagerbody" for="it-pagerbody"><?=$this->bbf('fm_voicemail-pagerbody');?></label>
		</p>
		<?=$form->textarea(array('paragraph'	=> false,
					 'name'		=> 'voicemail[pagerbody]',
					 'label'	=> false,
					 'id'		=> 'it-voicemail-pagerbody',
					 'cols'		=> 60,
					 'rows'		=> 10),
				   $this->get_var('voicemail','pagerbody','var_val'));?>
	</div>
</div>

<div id="sb-part-timezone" class="b-nodisplay">
<div class="sb-list">
	<table cellspacing="0" cellpadding="0" border="0">
		<thead>
		<tr class="sb-top">
			<th class="th-left"><?=$this->bbf('col_timezone-name');?></th>
			<th class="th-center"><?=$this->bbf('col_timezone-timezone');?></th>
			<th class="th-center"><?=$this->bbf('col_timezone-options');?></th>
			<th class="th-right">
				<?=$url->href_html($url->img_html('img/site/button/mini/orange/bo-add.gif',
								  $this->bbf('col_timezone-add'),
								  'border="0"'),
						   '#',
						   null,
						   'onclick="dwho.dom.make_table_list(\'timezone\',this); return(dwho.dom.free_focus());"',
						   $this->bbf('col_timezone-add'));?>
			</th>
		</tr>
		</thead>
		<tbody id="timezone">
<?php

if($zmsg_nb > 0):
	reset($zonemessages);

	foreach($zonemessages as $key => $val):

		if(isset($err['zonemessages'][$key]) === true):
			$errdisplay = ' l-infos-error';
		else:
			$errdisplay = '';
		endif;

?>
		<tr class="fm-paragraph<?=$errdisplay?>">
			<td class="td-left">
				<?=$form->text(array('paragraph'	=> false,
						     'name'		=> 'zonemessages[name][]',
						     'id'		=> false,
						     'label'		=> false,
						     'value'		=> $val['name'],
						     'default'		=> $element['zonemessages']['name']['default']));?>
			</td>
			<td>
				<?=$form->select(array('paragraph'	=> false,
						       'name'		=> 'zonemessages[timezone][]',
						       'key'		=> true,
						       'id'		=> false,
						       'label'		=> false,
						       'selected'	=> $val['timezone'],
						       'default'	=> $element['zonemessages']['timezone']['default']),
						 $this->get_var('timezone_list'));?>
			</td>
			<td>
				<?=$form->text(array('paragraph'	=> false,
						     'name'		=> 'zonemessages[msg_format][]',
						     'id'		=> false,
						     'label'		=> false,
						     'size'		=> 25,
						     'value'		=> $val['msg_format'],
						     'default'		=> $element['zonemessages']['msg_format']['default']));?>
			</td>
			<td class="td-right">
				<?=$url->href_html($url->img_html('img/site/button/mini/blue/delete.gif',
								  $this->bbf('opt_delete'),
								  'border="0"'),
						   '#',
						   null,
						   'onclick="dwho.dom.make_table_list(\'timezone\',this,1); return(dwho.dom.free_focus());"',
						   $this->bbf('opt_delete'));?>
			</td>
		</tr>
<?php
		endforeach;
	endif;
?>
		</tbody>
		<tfoot>
		<tr id="no-timezone"<?=($zmsg_nb > 0 ? ' class="b-nodisplay"' : '')?>>
			<td colspan="4" class="td-single"><?=$this->bbf('no_timezone');?></td>
		</tr>
		</tfoot>
	</table>
	<table class="b-nodisplay" cellspacing="0" cellpadding="0" border="0">
		<tbody id="ex-timezone">
		<tr class="fm-paragraph">
			<td class="td-left">
				<?=$form->text(array('paragraph'	=> false,
						     'name'		=> 'zonemessages[name][]',
						     'id'		=> false,
						     'label'		=> false,
						     'disabled'		=> true,
						     'default'		=> $element['zonemessages']['name']['default']));?>
				</td>
			<td>
				<?=$form->select(array('paragraph'	=> false,
						       'name'		=> 'zonemessages[timezone][]',
						       'key'		=> true,
						       'id'		=> false,
						       'label'		=> false,
						       'disabled'	=> true,
						       'default'	=> $element['zonemessages']['timezone']['default']),
						 $this->get_var('timezone_list'));?>
			</td>
			<td>
				<?=$form->text(array('paragraph'	=> false,
						     'name'		=> 'zonemessages[msg_format][]',
						     'id'		=> false,
						     'label'		=> false,
						     'disabled'		=> true,
						     'size'		=> 25,
						     'default'		=> $element['zonemessages']['msg_format']['default']));?>
			</td>
			<td class="td-right">
				<?=$url->href_html($url->img_html('img/site/button/mini/blue/delete.gif',
								  $this->bbf('opt_delete'),
								  'border="0"'),
						   '#',
						   null,
						   'onclick="dwho.dom.make_table_list(\'timezone\',this,1); return(dwho.dom.free_focus());"',
						   $this->bbf('opt_delete'));?>
			</td>
		</tr>
		</tbody>
	</table>
</div>
</div>

<div id="sb-part-adsi" class="b-nodisplay">
<?php
	echo	$form->text(array('desc'	=> $this->bbf('fm_voicemail-adsifdn'),
				  'name'	=> 'voicemail[adsifdn]',
				  'labelid'	=> 'voicemail-adsifdn',
				  'value'	=> $this->get_var('voicemail','adsifdn','var_val'),
				  'default'	=> $element['voicemail']['adsifdn']['default'])),

		$form->text(array('desc'	=> $this->bbf('fm_voicemail-adsisec'),
				  'name'	=> 'voicemail[adsisec]',
				  'labelid'	=> 'voicemail-adsisec',
				  'value'	=> $this->get_var('voicemail','adsisec','var_val'),
				  'default'	=> $element['voicemail']['adsisec']['default'])),

		$form->text(array('desc'	=> $this->bbf('fm_voicemail-adsiver'),
				  'name'	=> 'voicemail[adsiver]',
				  'labelid'	=> 'voicemail-adsiver',
				  'size'	=> 5,
				  'value'	=> $this->get_var('voicemail','adsiver','var_val'),
				  'default'	=> $element['voicemail']['adsiver']['default']));
?>
</div>

<div id="sb-part-last" class="b-nodisplay">
<?php
	echo	$form->checkbox(array('desc'	=> $this->bbf('fm_voicemail-searchcontexts'),
				      'name'	=> 'voicemail[searchcontexts]',
				      'labelid'	=> 'voicemail-searchcontexts',
				      'checked'	=> $this->get_var('voicemail','searchcontexts','var_val'),
				      'default'	=> $element['voicemail']['searchcontexts']['default'])),

		$form->text(array('desc'	=> $this->bbf('fm_voicemail-externpass'),
				  'name'	=> 'voicemail[externpass]',
				  'labelid'	=> 'voicemail-externpass',
				  'size'	=> 15,
				  'value'	=> $this->get_var('voicemail','externpass','var_val'),
				  'default'	=> $element['voicemail']['externpass']['default']),
				  'class="it-readonly" readonly="readonly"'),

		$form->text(array('desc'	=> $this->bbf('fm_voicemail-externnotify'),
				  'name'	=> 'voicemail[externnotify]',
				  'labelid'	=> 'voicemail-externnotify',
				  'size'	=> 15,
				  'value'	=> $this->get_var('voicemail','externnotify','var_val'),
				  'default'	=> $element['voicemail']['externnotify']['default'])),

		$form->text(array('desc'	=> $this->bbf('fm_smdiport'),
				  'name'	=> 'smdiport',
				  'labelid'	=> 'smdiport',
				  'value'	=> $this->get_var('info','smdiport','var_val'),
				  'default'	=> $element['voicemail']['smdiport']['default'])),

		$form->text(array('desc'	=> $this->bbf('fm_voicemail-odbcstorage'),
				  'name'	=> 'voicemail[odbcstorage]',
				  'labelid'	=> 'voicemail-odbcstorage',
				  'size'	=> 15,
				  'value'	=> $this->get_var('voicemail','odbcstorage','var_val'),
				  'default'	=> $element['voicemail']['odbcstorage']['default'])),

		$form->text(array('desc'	=> $this->bbf('fm_voicemail-odbctable'),
				  'name'	=> 'voicemail[odbctable]',
				  'labelid'	=> 'voicemail-odbctable',
				  'size'	=> 15,
				  'value'	=> $this->get_var('voicemail','odbctable','var_val'),
				  'default'	=> $element['voicemail']['odbctable']['default']));
?>
</div>
	<?=$form->submit(array('name'	=> 'submit',
			       'id'	=> 'it-submit',
			       'value'	=> $this->bbf('fm_bt-save')));?>
</form>
	</div>
	<div class="sb-foot xspan">
		<span class="span-left">&nbsp;</span>
		<span class="span-center">&nbsp;</span>
		<span class="span-right">&nbsp;</span>
	</div>
</div>

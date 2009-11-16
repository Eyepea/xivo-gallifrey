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

$url = &$this->get_module('url');
$form = &$this->get_module('form');
$dhtml = &$this->get_module('dhtml');

$info = $this->get_var('info');
$smenu = $this->get_var('fm_smenu');

$handynumbers_js = array();

if(($fm_save = $this->get_var('fm_save')) === true):
	$handynumbers_js[] = 'xivo_form_result(true,\''.$dhtml->escape($this->bbf('fm_success-save')).'\');';
elseif($fm_save === false):
	$handynumbers_js[] = 'xivo_form_result(false,\''.$dhtml->escape($this->bbf('fm_error-save')).'\');';
endif;

if(($smenu_tab = $this->get_var('dwsm_form_tab')) !== ''
&& ($smenu_part = $this->get_var('dwsm_form_part')) !== ''):
	$handynumbers_js[] = 'dwho_submenu.set_options({
				\'onload_tab\':		"'.$dhtml->escape($smenu_tab,ENT_NOQUOTES).'",
				\'onload_part\':	"'.$dhtml->escape($smenu_part,ENT_NOQUOTES).'"});';

	if($smenu_part === 'sb-part-last'):
		$handynumbers_js[] = 'dwho_submenu.set_option(\'onload_last\',true);';
	endif;
endif;

if(empty($info['emergency']) === false):
	$egency_nb = count($info['emergency']);
	$handynumbers_js[] = 'dwho.dom.set_table_list(\'emergency\','.$egency_nb.');';
else:
	$egency_nb = 0;
endif;


if(empty($info['special']) === false):
	$special_nb = count($info['special']);
	$handynumbers_js[] = 'dwho.dom.set_table_list(\'special\','.$special_nb.');';
else:
	$special_nb = 0;
endif;

$dhtml->write_js($handynumbers_js);

?>
<div class="b-infos b-form">
<h3 class="sb-top xspan">
	<span class="span-left">&nbsp;</span>
	<span class="span-center"><?=$this->bbf('title_content_name');?></span>
	<span class="span-right">&nbsp;</span>
</h3>

<div class="sb-smenu">
	<ul>
		<li id="dwsm-tab-1"
		    class="dwsm-blur"
		    onclick="dwho_submenu.select(this,'sb-part-first');"
		    onmouseout="dwho_submenu.blur(this);"
		    onmouseover="dwho_submenu.focus(this);">
			<div class="tab">
				<span class="span-center"><a href="#" onclick="return(false);"><?=$this->bbf('smenu_emergency');?></a></span>
			</div>
			<span class="span-right">&nbsp;</span>
		</li>
		<li id="dwsm-tab-2"
		    class="dwsm-blur-last"
		    onclick="dwho_submenu.select(this,'sb-part-last',1);"
		    onmouseout="dwho_submenu.blur(this,1);"
		    onmouseover="dwho_submenu.focus(this,1);">
			<div class="tab">
				<span class="span-center"><a href="#" onclick="return(false);"><?=$this->bbf('smenu_special');?></a></span>
			</div>
			<span class="span-right">&nbsp;</span>
		</li>
	</ul>
</div>

<div class="sb-content">
<?php

if($this->get_var('trunkslist') === false):
	echo	'<div class="txt-center">',
		$url->href_html($this->bbf('create_trunk'),
				'service/ipbx/trunk_management/sip',
				'act=add'),
		'</div>';
else:

?>
<form action="#" method="post" accept-charset="utf-8" onsubmit="dwho_submenu.submit_form(this);">
<?php
	echo	$form->hidden(array('name'	=> DWHO_SESS_NAME,
				    'value'	=> DWHO_SESS_ID)),

		$form->hidden(array('name'	=> 'fm_send',
				    'value'	=> 1)),

		$form->hidden(array('name'	=> 'dwsm-form-tab',
				    'value'	=> 'dwsm-tab-1')),

		$form->hidden(array('name'	=> 'dwsm-form-part',
				    'value'	=> 'sb-part-first'));
?>
	<div id="sb-part-first" class="b-nodisplay">
		<div class="sb-list">
<?php
	$this->file_include('bloc/service/ipbx/asterisk/pbx_services/handynumbers/form',
			    array('type'	=> 'emergency',
				  'count'	=> $egency_nb));
?>
		</div>
	</div>

	<div id="sb-part-last" class="b-nodisplay">
		<div class="sb-list">
<?php
	$this->file_include('bloc/service/ipbx/asterisk/pbx_services/handynumbers/form',
			    array('type'	=> 'special',
				  'count'	=> $special_nb));
?>
		</div>
	</div>
	<?=$form->submit(array('name'	=> 'submit',
			       'id'	=> 'it-submit',
			       'value'	=> $this->bbf('fm_bt-save')));?>
</form>
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

<?php

#
# XiVO Web-Interface
# Copyright (C) 2010  Proformatique <technique@proformatique.com>
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
$element = $this->get_var('element');
$error = $this->get_var('error');

if(($fm_save = $this->get_var('fm_save')) === true):
	$dhtml->write_js('xivo_form_result(true,\''.$dhtml->escape($this->bbf('fm_success-save')).'\');');
elseif($fm_save === false):
	$dhtml->write_js('xivo_form_result(false,\''.$dhtml->escape($this->bbf('fm_error-save')).'\');');
endif;

$error_js = array();
$error_nb = count($error['ctimain']);

for($i = 0;$i < $error_nb;$i++):
	$error_js[] = 'dwho.form.error[\'it-ctimain-'.$error['ctimain'][$i].'\'] = true;';
endfor;

if(isset($error_js[0]) === true)
	$dhtml->write_js($error_js);

?>
<div class="b-infos b-form">
<h3 class="sb-top xspan">
	<span class="span-left">&nbsp;</span>
	<span class="span-center"><?=$this->bbf('title_content_name');?></span>
	<span class="span-right">&nbsp;</span>
</h3>

<div class="sb-content">
<form action="#" method="post" accept-charset="utf-8" onsubmit="dwho.form.select('it-xivoserver');">

<?php
	echo	$form->hidden(array('name'	=> DWHO_SESS_NAME,
				    'value'	=> DWHO_SESS_ID)),
		$form->hidden(array('name'	=> 'fm_send',
				    'value'	=> 1));
?>

<div id="sb-part-first">
<?php
	echo	$form->select(array('desc'	=> $this->bbf('fm_cti_commandset'),
				    'name'	=> 'cti[commandset]',
				    'labelid'	=> 'cti_commandset',
				    'key'	=> false,
				    'default'	=> $element['ctimain']['commandset']['default'],
				    'help'	=> $this->bbf('hlp_fm_cti_commandset'),
				    'selected'	=> $this->get_var('ctimain','commandset','var_val')),
			      $element['ctimain']['commandset']['value']);
				  
?>
<fieldset id="cti-servers">
	<legend><?=$this->bbf('cti-servers');?></legend>
	<div class="sb-list">
	<table cellspacing="0" cellpadding="0" border="0">
		<tr class="sb-top">
<?php
	echo	
		"<th width=\"50%\">", $this->bbf('fm_cti_list_ip'), "</th>",
		"<th class=\"th-right\">".$this->bbf('fm_cti_list_port')."</th>",
		"</tr>",

		'<tr><td>',
			$form->text(array('desc'	=> $this->bbf('fm_cti_fagi_ip'),
					'name'	=> 'cti[fagi_ip]',
					'labelid'	=> 'cti_fagi_ip',
					'value'		=> $info['ctimain']['fagi_ip'],
					'default'	=> $element['ctimain']['fagi_ip']['default'] //,
					/* 'help'		=> $this->bbf('hlp_fm_cti_fagi_ip') */ )),
		'</td><td class="td-right">',
			$form->text(array(#'desc'	=> $this->bbf('fm_cti_fagi_port'),
					'name'		=> 'cti[fagi_port]',
					'labelid'	=> 'cti_fagi_port',
					'value'		=> $info['ctimain']['fagi_port'],
					'default'	=> $element['ctimain']['fagi_port']['default'],
					'help'		=> $this->bbf('hlp_fm_cti_fagi_port'))),
		'</td></tr>',

		'<tr class="sb-content"><td>',
			$form->text(array('desc'	=> $this->bbf('fm_cti_cti_ip'),
					'name'		=> 'cti[cti_ip]',
					'labelid'	=> 'cti_cti_ip',
					'value'		=> $info['ctimain']['cti_ip'],
					'default'	=> $element['ctimain']['cti_ip']['default'] //,
					/* 'help'		=> $this->bbf('hlp_fm_cti_cti_ip') */ )),
		'</td><td class="td-right">',
			$form->text(array(#'desc'	=> $this->bbf('fm_cti_cti_port'),
					'name'		=> 'cti[cti_port]',
					'labelid'	=> 'cti_cti_port',
					'value'		=> $info['ctimain']['cti_port'],
					'default'	=> $element['ctimain']['cti_port']['default'],
					'help'		=> $this->bbf('hlp_fm_cti_cti_port'))),
		'</td></tr class="sb-content">',

		'<tr class="sb-content"><td>',
			$form->text(array('desc'	=> $this->bbf('fm_cti_webi_ip'),
					'name'		=> 'cti[webi_ip]',
					'labelid'	=> 'cti_webi_ip',
					'value'		=> $info['ctimain']['webi_ip'],
					'default'	=> $element['ctimain']['webi_ip']['default'] //,
					/* 'help'		=> $this->bbf('hlp_fm_cti_webi_ip') */ )),
		'</td><td class="td-right">',
			$form->text(array(#'desc'	=> $this->bbf('fm_cti_webi_port'),
					'name'		=> 'cti[webi_port]',
					'labelid'	=> 'cti_webi_port',
					'value'		=> $info['ctimain']['webi_port'],
					'default'	=> $element['ctimain']['webi_port']['default'],
					'help'		=> $this->bbf('hlp_fm_cti_webi_port'))),
		'</td></tr class="sb-content">',

		'<tr class="sb-content"><td>',
			$form->text(array('desc'	=> $this->bbf('fm_cti_info_ip'),
					'name'		=> 'cti[info_ip]',
					'labelid'	=> 'cti_info_ip',
					'value'		=> $info['ctimain']['info_ip'],
					'default'	=> $element['ctimain']['info_ip']['default'] //,
					/* 'help'		=> $this->bbf('hlp_fm_cti_info_ip') */ )),

		'</td><td class="td-right">',
			$form->text(array(#'desc'	=> $this->bbf('fm_cti_info_port'),
					'name'		=> 'cti[info_port]',
					'labelid'	=> 'cti_info_port',
					'value'		=> $info['ctimain']['info_port'],
					'default'	=> $element['ctimain']['info_port']['default'],
					'help'		=> $this->bbf('hlp_fm_cti_info_port'))),
		'</td></tr class="sb-content">',

		'<tr class="sb-content"><td>',
			$form->text(array('desc'	=> $this->bbf('fm_cti_announce_ip'),
					'name'		=> 'cti[announce_ip]',
					'labelid'	=> 'cti_announce_ip',
					'value'		=> $info['ctimain']['announce_ip'],
					'default'	=> $element['ctimain']['announce_ip']['default'] //,
					/* 'help'		=> $this->bbf('hlp_fm_cti_announce_ip') */ )),
		'</td><td class="td-right">',
			$form->text(array(#'desc'	=> $this->bbf('fm_cti_annouce_port'),
					'name'		=> 'cti[announce_port]',
					'labelid'	=> 'cti_announce_port',
					'value'		=> $info['ctimain']['announce_port'],
					'default'	=> $element['ctimain']['announce_port']['default'],
					'help'		=> $this->bbf('hlp_fm_cti_announce_port'))),
		'</td></tr>';
?>
</div></table></fieldset>
<fieldset id="cti-intervals">
	<legend><?=$this->bbf('cti-intervals');?></legend>
<?php

	echo	$form->text(array('desc'	=> $this->bbf('fm_cti_updates_period'),
					'name'		=> 'cti[updates_period]',
					'labelid'	=> 'cti_updates_period',
					'value'		=> $info['ctimain']['updates_period'],
					'default'	=> $element['ctimain']['updates_period']['default'],
					'help'		=> $this->bbf('hlp_fm_cti_updates_period'))),

			$form->text(array('desc'	=> $this->bbf('fm_cti_socket_timeout'),
					'name'		=> 'cti[socket_timeout]',
					'labelid'	=> 'cti_socket_timeout',
					'value'		=> $info['ctimain']['socket_timeout'],
					'default'	=> $element['ctimain']['socket_timeout']['default'],
					'help'		=> $this->bbf('hlp_fm_cti_socket_timeout'))),

			$form->text(array('desc'	=> $this->bbf('fm_cti_login_timeout'),
					'name'		=> 'cti[login_timeout]',
					'labelid'	=> 'cti_login_timeout',
					'value'		=> $info['ctimain']['login_timeout'],
					'default'	=> $element['ctimain']['login_timeout']['default'],
					'help'		=> $this->bbf('hlp_fm_cti_login_timeout')));
?>
</fieldset>
<?php
			
	echo	$form->checkbox(array('desc' => $this->bbf('fm_cti_parting_astid_context'),
							'name' => 'cti[parting_astid_context]',
							'labelid' => 'cti_parting_astid_context',
							'checked' => 
								(strpos('context', $info['ctimain']['parting_astid_context']) === false)
								? false : true)),
			
			$form->checkbox(array('desc' => $this->bbf('fm_cti_parting_astid_ipbx'),
							'name' => 'cti[parting_astid_ipbx]',
							'labelid' => 'cti_parting_astid_ipbx',
							'checked' =>  
								(strpos('astid', $info['ctimain']['parting_astid_context']) === false)
								? false : true));


?>
<br />
</div>
<fieldset id="cti-xivo_servers">
	<legend><?=$this->bbf('cti-xivo_servers');?></legend>
		<div id="xivoserverlist" class="fm-paragraph fm-multilist">
			<div class="slt-outlist">
<?php
		echo	$form->select(array('name'	=> 'xivoserverlist',
					    'label'	=> false,
					    'id'	=> 'it-xivoserverlist',
					    'key'	=> 'identity',
					    'altkey'	=> 'id',
					    'help'	=> $this->bbf('hlp_xivoserverlist'),
					    'multiple'	=> true,
					    'size'	=> 5,
					    'paragraph'	=> false),
				      $info['xivoserver']['list']);
?>
			</div>
			<div class="inout-list">
				<a href="#"
				   onclick="dwho.form.move_selected('it-xivoserverlist','it-xivoserver');
					    return(dwho.dom.free_focus());"
				   title="<?=$this->bbf('bt_inxivoserver');?>">
					<?=$url->img_html('img/site/button/arrow-left.gif',
							  $this->bbf('bt_inxivoserver'),
							  'class="bt-inlist" id="bt-inxivoserver" border="0"');?></a><br />

				<a href="#"
				   onclick="dwho.form.move_selected('it-xivoserver','it-xivoserverlist');
					    return(dwho.dom.free_focus());"
				   title="<?=$this->bbf('bt_outxivoserver');?>">
					<?=$url->img_html('img/site/button/arrow-right.gif',
							  $this->bbf('bt_outxivoserver'),
							  'class="bt-outlist" id="bt-outxivoserver" border="0"');?></a>
			</div>
			<div class="slt-inlist">
<?php
		echo	$form->select(array('name'	=> 'xivoserver[]',
					    'label'	=> false,
					    'id'	=> 'it-xivoserver',
					    'key'	=> 'identity',
					    'altkey'	=> 'id',
					    'help'	=> $this->bbf('hlp_xivoserver'),
					    'multiple'	=> true,
					    'size'	=> 5,
					    'paragraph'	=> false),
				      $info['xivoserver']['slt']);
?>
			</div>
		</div>
</fieldset>
		<div class="clearboth"></div>

<?php
	echo	$form->submit(array('name'	=> 'submit',
				    'id'	=> 'it-submit',
				    'value'	=> $this->bbf('fm_bt-save')));
?>
</form>

	</div>
	<div class="sb-foot xspan">
		<span class="span-left">&nbsp;</span>
		<span class="span-center">&nbsp;</span>
		<span class="span-right">&nbsp;</span>
	</div>
</div>

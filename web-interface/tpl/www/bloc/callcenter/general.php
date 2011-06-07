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

$form    = &$this->get_module('form');
$url     = &$this->get_module('url');
$dhtml   = &$this->get_module('dhtml');

$error   = $this->get_var('error');
$element = $this->get_var('element');
$info    = $this->get_var('info');

if(($fm_save = $this->get_var('fm_save')) === true):
	$dhtml->write_js('xivo_form_result(true,\''.$dhtml->escape($this->bbf('fm_success-save')).'\');');
elseif($fm_save === false):
	$dhtml->write_js('xivo_form_result(false,\''.$dhtml->escape($this->bbf('fm_error-save')).'\');');
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
		<li id="dwsm-tab-1"
		    class="dwsm-blur"
		    onclick="dwho_submenu.select(this,'sb-part-first');"
		    onmouseout="dwho_submenu.blur(this);"
		    onmouseover="dwho_submenu.focus(this);">
			<div class="tab">
				<span class="span-center">
					<a href="#" onclick="return(false);"><?=$this->bbf('smenu_general');?></a>
				</span>
			</div>
			<span class="span-right">&nbsp;</span>
		</li>
		<li id="dwsm-tab-2"
		    class="dwsm-blur"
		    onclick="dwho_submenu.select(this,'sb-part-purges');"
		    onmouseout="dwho_submenu.blur(this);"
		    onmouseover="dwho_submenu.focus(this);">
			<div class="tab">
				<span class="span-center">
					<a href="#" onclick="return(false);"><?=$this->bbf('smenu_purges');?></a>
				</span>
			</div>
			<span class="span-right">&nbsp;</span>
		</li>
		<li id="dwsm-tab-3"
		    class="dwsm-blur-last"
		    onclick="dwho_submenu.select(this,'sb-part-variables',1);"
		    onmouseout="dwho_submenu.blur(this,1);"
		    onmouseover="dwho_submenu.focus(this,1);">
			<div class="tab">
				<span class="span-center">
					<a href="#" onclick="return(false);"><?=$this->bbf('smenu_variables');?></a>
				</span>
			</div>
			<span class="span-right">&nbsp;</span>
		</li>
	</ul>
</div>

<div class="sb-content">
<form action="#" method="post" accept-charset="utf-8" onsubmit="dwho.form.select('it-localnet'); dwho.form.select('it-codec');">

<?php
	echo	$form->hidden(array('name'	=> DWHO_SESS_NAME,
				    'value'	=> DWHO_SESS_ID)),
		$form->hidden(array('name'	=> 'fm_send',
				    'value'	=> 1));
?>

<div id="sb-part-first">

<?php
	echo	$form->text(array('desc'	=> $this->bbf('fm_records_path'),
					  'name'	  	=> 'records_path',
					  'labelid'		=> 'records_path',
					  'help'		  => $this->bbf('hlp_fm_records_path'),
					  'required'	=> false,
					  'value'		  => $this->get_var('info','records_path'),
					  'default'		=> $element['records_path']['default'],
					  'error'		  => $this->bbf_args('error',
						   $this->get_var('error', 'records_path')) )),

		$form->text(array('desc'	=> $this->bbf('fm_records_announce'),
					  'name'	   	=> 'records_announce',
					  'labelid'		=> 'records_announce',
					  'size'		  => 15,
					  'help'		  => $this->bbf('hlp_fm_records_announce'),
					  'required'	=> false,
					  'value'		  => $this->get_var('info','records_announce'),
					  'default'		=> $element['records_announce']['default'],
					  'error'		  => $this->bbf_args('error',
						   $this->get_var('error', 'records_announce')) ));
?>
</div>

<div id="sb-part-purges" class="b-nodisplay">
	<fieldset id="fld-syst-tagged">
		<legend><?=$this->bbf('fld-purge-syst-tagged');?></legend>

<?php
	echo	$form->text(array('desc'	=> $this->bbf('fm_delay'),
					  'name'	  	=> 'purge_syst_tagged_delay',
					  'labelid'		=> 'purge_syst_tagged_delay',
					  'help'		  => $this->bbf('hlp_fm_purge_syst_tagged_delay'),
					  'required'	=> false,
					  'value'		  => $this->get_var('info','purge_syst_tagged_delay'),
					  'default'		=> $element['purge_syst_tagged_delay']['default'],
					  'error'		  => $this->bbf_args('error',
						   $this->get_var('error', 'purge_syst_tagged_delay')) )),

		$form->text(array('desc'	=> $this->bbf('fm_at'),
					  'name'	   	=> 'purge_syst_tagged_at',
					  'labelid'		=> 'purge_syst_tagged_at',
					  'size'		  => 15,
					  'help'		  => $this->bbf('hlp_fm_purge_syst_tagged_at'),
					  'required'	=> false,
					  'value'		  => $this->get_var('info','purge_syst_tagged_at'),
					  'default'		=> $element['purge_syst_tagged_at']['default'],
					  'error'		  => $this->bbf_args('error',
							$this->get_var('error', 'purge_syst_tagged_at')) ));
?>
	</fieldset>

	<fieldset id="fld-syst-untagged">
		<legend><?=$this->bbf('fld-purge-syst-untagged');?></legend>

<?php
	echo	$form->text(array('desc'	=> $this->bbf('fm_delay'),
					  'name'	  	=> 'purge_syst_untagged_delay',
					  'labelid'		=> 'purge_syst_untagged_delay',
					  'help'		  => $this->bbf('hlp_fm_purge_syst_untagged_delay'),
					  'required'	=> false,
					  'value'		  => $this->get_var('info','purge_syst_untagged_delay'),
					  'default'		=> $element['purge_syst_untagged_delay']['default'],
					  'error'		  => $this->bbf_args('error',
						   $this->get_var('error', 'purge_syst_untagged_delay')) )),

		$form->text(array('desc'	=> $this->bbf('fm_at'),
					  'name'	   	=> 'purge_syst_untagged_at',
					  'labelid'		=> 'purge_syst_untagged_at',
					  'size'		  => 15,
					  'help'		  => $this->bbf('hlp_fm_purge_syst_untagged_at'),
					  'required'	=> false,
					  'value'		  => $this->get_var('info','purge_syst_untagged_at'),
					  'default'		=> $element['purge_syst_untagged_at']['default'],
					  'error'		  => $this->bbf_args('error',
							$this->get_var('error', 'purge_syst_untagged_at')) ));
?>
	</fieldset>
	<fieldset id="fld-punct">
		<legend><?=$this->bbf('fld-purge-punct');?></legend>

<?php
	echo	$form->text(array('desc'	=> $this->bbf('fm_delay'),
					  'name'	  	=> 'purge_punct_delay',
					  'labelid'		=> 'purge_punct_delay',
					  'help'		  => $this->bbf('hlp_fm_purge_punct_delay'),
					  'required'	=> false,
					  'value'		  => $this->get_var('info','purge_punct_delay'),
					  'default'		=> $element['purge_punct_delay']['default'],
					  'error'		  => $this->bbf_args('error',
						   $this->get_var('error', 'purge_punct_delay')) )),

		$form->text(array('desc'	=> $this->bbf('fm_at'),
					  'name'	   	=> 'purge_punct_at',
					  'labelid'		=> 'purge_punct_at',
					  'size'		  => 15,
					  'help'		  => $this->bbf('hlp_fm_purge_punct_at'),
					  'required'	=> false,
					  'value'		  => $this->get_var('info','purge_punct_at'),
					  'default'		=> $element['purge_punct_at']['default'],
					  'error'		  => $this->bbf_args('error',
							$this->get_var('error', 'purge_punct_at')) ));
?>
	</fieldset>
<script type="text/javascript">
	/* <![CDATA[ */
		$(document).ready(function() {
				  $('#it-purge_syst_tagged_at').timepicker();
				  $('#it-purge_syst_untagged_at').timepicker();
				  $('#it-purge_punct_at').timepicker();
			});
	/* ]]> */
</script>
</div>

<div id="sb-part-variables" class="b-nodisplay">

<fieldset>
	<legend><?= $this->bbf('svi_choices_legend');?></legend>
<div id="sb-list-addons">
<?php
	// SVICHOICES

	$type = 'disp2';
	$count = count($info['svichoices']);
	$keys  = array_keys($info['svichoices']);

	$errdisplay = '';
?>
	<p>&nbsp;</p>
	<div class="sb-list">
		<table cellspacing="0" cellpadding="0" border="0">
			<thead>
			<tr class="sb-top">

				<th class="th-left"><?=$this->bbf('column_svichoices_name');?></th>
				<th class="th-left"><?=$this->bbf('column_svichoices_astvar');?></th>
				<th class="th-right th-rule">
					<?=$url->href_html($url->img_html('img/site/button/mini/orange/bo-add.gif',
									  $this->bbf('col_add'),
									  'border="0"'),
							   '#',
							   null,
							   'onclick="dwho.dom.make_table_list(\'disp2\',this); return(dwho.dom.free_focus());"',
							   $this->bbf('col_add'));?>
				</th>
			</tr>
			</thead>
			<tbody id="disp2">
		<?php
		if($count > 0):
			for($i = 0;$i < $count;$i++):

		?>
			<tr class="fm-paragraph<?=$errdisplay?>">
				<td class="td-center">
<?php
	echo $form->text(array(
		'name'	   	=> 'svichoices_astvar[]',
		'id'        => "it-svichoices-astvar[$i]",
		'size'      => 20,
		'value'		  => $info['svichoices'][$keys[$i]]));
?>
				</td>
				<td class="td-center">
<?php
	echo $form->text(array(
		'name'	   	=> 'svichoices_name[]',
		'id'        => "it-svichoices-name[$i]",
		'size'      => 20,
		'value'		  => $keys[$i]));
 ?>
				</td>
				<td class="td-right">
					<?=$url->href_html($url->img_html('img/site/button/mini/blue/delete.gif',
									  $this->bbf('opt_'.$type.'-delete'),
									  'border="0"'),
							   '#',
							   null,
							   'onclick="dwho.dom.make_table_list(\''.$type.'\',this,1); return(dwho.dom.free_focus());"',
							   $this->bbf('opt_'.$type.'-delete'));?>
				</td>
			</tr>

		<?php
			endfor;
		endif;
		?>
			</tbody>
			<tfoot>
			<tr id="no-<?=$type?>"<?=($count > 0 ? ' class="b-nodisplay"' : '')?>>
				<td colspan="5" class="td-single"><?=$this->bbf('no_'.$type);?></td>
			</tr>
			</tfoot>
		</table>
		<table class="b-nodisplay" cellspacing="0" cellpadding="0" border="0">
			<tbody id="ex-<?=$type?>">
			<tr class="fm-paragraph">
				<td class="td-center">
<?php
	echo $form->text(array(
		'name'	   	=> 'svichoices_astvar[]',
		'id'        => "it-svichoices-astvar[]",
		'size'      => 20,
		'value'		  => ""));
?>
				</td>
				<td class="td-center">
<?php
	echo $form->text(array(
		'name'	   	=> 'svichoices_name[]',
		'id'        => "it-svichoices-name[]",
		'size'      => 20,
		'value'		  => ""));
?>
				</td>

				<td class="td-right">
					<?=$url->href_html($url->img_html('img/site/button/mini/blue/delete.gif',
									  $this->bbf('opt_'.$type.'-delete'),
									  'border="0"'),
							   '#',
							   null,
							   'onclick="dwho.dom.make_table_list(\''.$type.'\',this,1); return(dwho.dom.free_focus());"',
							   $this->bbf('opt_'.$type.'-delete'));?>
				</td>
			</tr>
			</tbody>
		</table>
	</div>
</div>
</fieldset>

<fieldset>
	<legend><?= $this->bbf('svi_entries_legend');?></legend>
<div id="sb-list-addons">
<?php
	// SVIENTRIES

	$type = 'disp3';
	$count = count($info['svientries']);
	$keys  = array_keys($info['svientries']);
	$errdisplay = '';
?>
	<p>&nbsp;</p>
	<div class="sb-list">
		<table cellspacing="0" cellpadding="0" border="0">
			<thead>
			<tr class="sb-top">

				<th class="th-left"><?=$this->bbf('column_svientries_name');?></th>
				<th class="th-center"><?=$this->bbf('column_svientries_astvar');?></th>
				<th class="th-right th-rule">
					<?=$url->href_html($url->img_html('img/site/button/mini/orange/bo-add.gif',
									  $this->bbf('col_add'),
									  'border="0"'),
							   '#',
							   null,
							   'onclick="dwho.dom.make_table_list(\'disp3\',this); return(dwho.dom.free_focus());"',
							   $this->bbf('col_add'));?>
				</th>
			</tr>
			</thead>
			<tbody id="disp3">
		<?php
		if($count > 0):
			for($i = 0;$i < $count;$i++):

		?>
			<tr class="fm-paragraph<?=$errdisplay?>">
				<td class="td-center">
<?php
	echo $form->text(array(
		'name'	   	=> 'svientries_astvar[]',
		'id'        => "it-svientries-astvar[$i]",
		'size'      => 20,
		'value'		  => $info['svientries'][$keys[$i]]));
?>
				</td>
				<td class="td-center">
<?php
	echo $form->text(array(
		'name'	   	=> 'svientries_name[]',
		'id'        => "it-svientries-name[$i]",
		'size'      => 20,
		'value'		  => $keys[$i]));
?>
				</td>
				<td class="td-right">
					<?=$url->href_html($url->img_html('img/site/button/mini/blue/delete.gif',
									  $this->bbf('opt_'.$type.'-delete'),
									  'border="0"'),
							   '#',
							   null,
							   'onclick="dwho.dom.make_table_list(\''.$type.'\',this,1); return(dwho.dom.free_focus());"',
							   $this->bbf('opt_'.$type.'-delete'));?>
				</td>
			</tr>

		<?php
			endfor;
		endif;
		?>
			</tbody>
			<tfoot>
			<tr id="no-<?=$type?>"<?=($count > 0 ? ' class="b-nodisplay"' : '')?>>
				<td colspan="5" class="td-single"><?=$this->bbf('no_'.$type);?></td>
			</tr>
			</tfoot>
		</table>
		<table class="b-nodisplay" cellspacing="0" cellpadding="0" border="0">
			<tbody id="ex-<?=$type?>">
			<tr class="fm-paragraph">
				<td class="td-center">
<?php
	echo $form->text(array(
		'name'	   	=> 'svientries_astvar[]',
		'id'        => "it-svientries-astvar[]",
		'size'      => 20,
		'value'		  => ""));
?>
				</td>
				<td class="td-center">
<?php
	echo $form->text(array(
		'name'	   	=> 'svientries_name[]',
		'id'        => "it-svientries-name[]",
		'size'      => 20,
		'value'		  => ""));
?>
				</td>

				<td class="td-right">
					<?=$url->href_html($url->img_html('img/site/button/mini/blue/delete.gif',
									  $this->bbf('opt_'.$type.'-delete'),
									  'border="0"'),
							   '#',
							   null,
							   'onclick="dwho.dom.make_table_list(\''.$type.'\',this,1); return(dwho.dom.free_focus());"',
							   $this->bbf('opt_'.$type.'-delete'));?>
				</td>
			</tr>
			</tbody>
		</table>
	</div>
</div>
</fieldset>

<fieldset>
	<legend><?= $this->bbf('svi_variables_legend');?></legend>
<div id="sb-list-addons">
<?php
	// SVIVARIABLES

	$type = 'disp4';
	$count = count($info['svivariables']);
	$keys  = array_keys($info['svivariables']);
	$errdisplay = '';
?>
	<p>&nbsp;</p>
	<div class="sb-list">
		<table cellspacing="0" cellpadding="0" border="0">
			<thead>
			<tr class="sb-top">

				<th class="th-left"><?=$this->bbf('column_svivariables_name');?></th>
				<th class="th-left"><?=$this->bbf('column_svivariables_astvar');?></th>
				<th class="th-right th-rule">
					<?=$url->href_html($url->img_html('img/site/button/mini/orange/bo-add.gif',
									  $this->bbf('col_add'),
									  'border="0"'),
							   '#',
							   null,
							   'onclick="dwho.dom.make_table_list(\'disp4\',this); return(dwho.dom.free_focus());"',
							   $this->bbf('col_add'));?>
				</th>
			</tr>
			</thead>
			<tbody id="disp4">
		<?php
		if($count > 0):
			for($i = 0;$i < $count;$i++):

		?>
			<tr class="fm-paragraph<?=$errdisplay?>">
				<td class="td-center">
<?php
	echo $form->text(array(
		'name'	   	=> 'svivariables_astvar[]',
		'id'        => "it-svivariables-astvar[$i]",
		'size'      => 20,
		'value'		  => $info['svivariables'][$keys[$i]]));
?>
				</td>
				<td class="td-center">
<?php
	echo $form->text(array(
		'name'	   	=> 'svivariables_name[]',
		'size'      => 20,
		'id'        => "it-svivariables-name[$i]",
		'value'		  => $keys[$i]));
?>
				</td>
				<td class="td-right">
					<?=$url->href_html($url->img_html('img/site/button/mini/blue/delete.gif',
									  $this->bbf('opt_'.$type.'-delete'),
									  'border="0"'),
							   '#',
							   null,
							   'onclick="dwho.dom.make_table_list(\''.$type.'\',this,1); return(dwho.dom.free_focus());"',
							   $this->bbf('opt_'.$type.'-delete'));?>
				</td>
			</tr>

		<?php
			endfor;
		endif;
		?>
			</tbody>
			<tfoot>
			<tr id="no-<?=$type?>"<?=($count > 0 ? ' class="b-nodisplay"' : '')?>>
				<td colspan="5" class="td-single"><?=$this->bbf('no_'.$type);?></td>
			</tr>
			</tfoot>
		</table>
		<table class="b-nodisplay" cellspacing="0" cellpadding="0" border="0">
			<tbody id="ex-<?=$type?>">
			<tr class="fm-paragraph">
				<td class="td-center">
<?php
	echo $form->text(array(
		'name'	   	=> 'svivariables_astvar[]',
		'id'        => "it-svivariables-astvar[]",
		'size'      => 20,
		'value'		  => ""));
?>
				</td>
				<td class="td-center">
<?php
	echo $form->text(array(
		'name'	   	=> 'svivariables_name[]',
		'id'        => "it-svivariables-name[]",
		'size'      => 20,
		'value'		  => ""));
?>
				</td>

				<td class="td-right">
					<?=$url->href_html($url->img_html('img/site/button/mini/blue/delete.gif',
									  $this->bbf('opt_'.$type.'-delete'),
									  'border="0"'),
							   '#',
							   null,
							   'onclick="dwho.dom.make_table_list(\''.$type.'\',this,1); return(dwho.dom.free_focus());"',
							   $this->bbf('opt_'.$type.'-delete'));?>
				</td>
			</tr>
			</tbody>
		</table>
	</div>
</div>
</fieldset>

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

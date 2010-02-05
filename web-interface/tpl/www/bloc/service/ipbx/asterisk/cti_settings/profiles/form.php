<?php

#
# XiVO Web-Interface
# Copyright (C) 2006-2010  Proformatique <technique@proformatique.com>
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
$info = $this->get_var('info');
$preslist = $this->get_var('preslist');

$preferencesavail = $this->get_var('preferencesavail');
$xletsavail = $this->get_var('xletsavail');
$xletslocavail = $this->get_var('xletslocavail');

$yesno = array($this->bbf('no'), $this->bbf('yes'));

#$profiles = $this->get_var('profiles');
if($this->get_var('fm_save') === false):
	$dhtml = &$this->get_module('dhtml');
	$dhtml->write_js('xivo_form_result(false,\''.$dhtml->escape($this->bbf('fm_error-save')).'\');');
endif;
?>

<div id="sb-part-first">
<?php
	echo	$form->text(array('desc'	=> $this->bbf('fm_profiles_name'),
				  'name'	=> 'profiles[name]',
				  'labelid'	=> 'profiles-name',
				  'size'	=> 15,
				  'default'	=> $element['ctiprofiles']['name']['default'],
				  'value'	=> $info['ctiprofiles']['name']));

	echo	$form->text(array('desc'	=> $this->bbf('fm_profiles_appliname'),
				  'name'	=> 'profiles[appliname]',
				  'labelid'	=> 'profiles-appliname',
				  'size'	=> 15,
				  'comment' => $this->bbf('cmt_profiles_appliname'),
				  'default'	=> $element['ctiprofiles']['appliname']['default'],
				  'value'	=> $info['ctiprofiles']['appliname']));

	echo	$form->text(array('desc'	=> $this->bbf('fm_profiles_maxgui'),
				  'name'	=> 'profiles[maxgui]',
				  'labelid'	=> 'profiles-maxgui',
				  'size'	=> 5,
				  'comment' => $this->bbf('cmt_profiles_maxgui'),
				  'default'	=> $element['ctiprofiles']['maxgui']['default'],
				  'value'	=> $info['ctiprofiles']['maxgui']));

	echo $form->select(array('desc'		=> $this->bbf('fm_profiles_presence'),
				   'name'		=> 'presence',
				   'id'		=> false,
				   'label'		=> false,
				   'key'		=> false,
				   'selected'	=> $info['ctiprofiles']['presence']
			 ),
			 $preslist);
?>
	<div class="fm-paragraph fm-description">
		<fieldset id="cti-profiles_services">
			<legend><?=$this->bbf('cti-profiles-services');?></legend>
			<div id="profiles_services" class="fm-paragraph fm-multilist">
				<div class="slt-outlist">
<?php
				echo    $form->select(array('name'  => 'serviceslist',
							'label' => false,
							'id'    => 'it-serviceslist',
							'key'   => 'name',
							'altkey'    => 'id',
							'multiple'  => true,
							'size'  => 5,
							'paragraph' => false),
							$info['services']['list']);
?>
				</div>
				<div class="inout-list">
					<a href="#"
					onclick="dwho.form.move_selected('it-serviceslist','it-services');
					return(dwho.dom.free_focus());"
					title="<?=$this->bbf('bt_inaccess_profiles');?>">
					<?=$url->img_html('img/site/button/arrow-left.gif',
							$this->bbf('bt_inaccess_profiles'),
							'class="bt-inlist" id="bt-inaccess_profiles" border="0"');?></a><br />

					<a href="#"
					onclick="dwho.form.move_selected('it-services','it-serviceslist');
					return(dwho.dom.free_focus());"
					title="<?=$this->bbf('bt_outaccess_profiles');?>">
					<?=$url->img_html('img/site/button/arrow-right.gif',
							$this->bbf('bt_outaccess_profiles'),
							'class="bt-outlist" id="bt-outaccess_profiles" border="0"');?></a>
				</div>
				<div class="slt-inlist">
<?php
				echo    $form->select(array('name'  => 'services[]',
						'label' => false,
						'id'    => 'it-services',
						'key'	=> 'name',
						'altkey'    => 'id',
						'multiple'  => true,
						'size'  => 5,
						'paragraph' => false),
					$info['services']['slt']);
?>
				</div>
			</div>
		</fieldset>
		<div class="clearboth"></div>
	</div>
	<div class="fm-paragraph fm-description">
		<fieldset id="cti-profiles_funcs">
			<legend><?=$this->bbf('cti-profiles_funcs');?></legend>
			<div id="profiles_funcs" class="fm-paragraph fm-multilist">
				<div class="slt-outlist">
<?php
				echo    $form->select(array('name'  => 'funcslist',
							'label' => false,
							'id'    => 'it-funcslist',
							'key'   => 'name',
							'altkey'    => 'id',
							'multiple'  => true,
							'size'  => 5,
							'paragraph' => false),
							$info['funcs']['list']);
?>
				</div>
				<div class="inout-list">
					<a href="#"
					onclick="dwho.form.move_selected('it-funcslist','it-funcs');
					return(dwho.dom.free_focus());"
					title="<?=$this->bbf('bt_inaccess_profiles');?>">
					<?=$url->img_html('img/site/button/arrow-left.gif',
							$this->bbf('bt_inaccess_profiles'),
							'class="bt-inlist" id="bt-inaccess_profiles" border="0"');?></a><br />

					<a href="#"
					onclick="dwho.form.move_selected('it-funcs','it-funcslist');
					return(dwho.dom.free_focus());"
					title="<?=$this->bbf('bt_outaccess_profiles');?>">
					<?=$url->img_html('img/site/button/arrow-right.gif',
							$this->bbf('bt_outaccess_profiles'),
							'class="bt-outlist" id="bt-outaccess_profiles" border="0"');?></a>
				</div>
				<div class="slt-inlist">
<?php
				echo    $form->select(array('name'  => 'funcs[]',
						'label' => false,
						'id'    => 'it-funcs',
						'key'	=> 'name',
						'altkey'    => 'id',
						'multiple'  => true,
						'size'  => 5,
						'paragraph' => false),
					$info['funcs']['slt']);
?>
				</div>
			</div>
		</fieldset>
		<div class="clearboth"></div>
	</div>
<?php
	$type = 'preferences';
	$count = count($info['preferences']['slt']);
?>
</div>
<div id="sb-part-prefs" class="b-nodisplay">
	<div class="sb-list">
		<table cellspacing="0" cellpadding="0" border="0">
			<thead>
			<tr class="sb-top">
				<th class="th-left"><?=$this->bbf('col_'.$type.'-name');?></th>
				<th class="th-center"><?=$this->bbf('col_'.$type.'-args');?></th>
				<th class="th-right">
					<?=$url->href_html($url->img_html('img/site/button/mini/orange/bo-add.gif',
									  $this->bbf('col_'.$type.'-add'),
									  'border="0"'),
							   '#',
							   null,
							   'onclick="dwho.dom.make_table_list(\''.$type.'\',this); return(dwho.dom.free_focus());"',
							   $this->bbf('col_'.$type.'-add'));?>
				</th>
			</tr>
			</thead>
			<tbody id="preferences">
		<?php
		if($count > 0):
			for($i = 0;$i < $count;$i++):
					$errdisplay = '';
					$pattern = '/^(.*)\((.*)\)/';
					$match = array();
					preg_match($pattern, $info['preferences']['slt'][$i], $match);
		?>
			<tr class="fm-paragraph<?=$errdisplay?>">
				<td class="td-left txt-center">
	<?php
					echo $form->select(array('paragraph'	=> false,
								   'name'		=> 'preferenceslist[]',
								   'id'		=> false,
								   'label'		=> false,
								   'key'		=> false,
								   'selected'	=> $match[1],
								   'invalid'	=> true,
							 ),
							 $info['preferences']['avail']);?>
				</td>
				<td>
					<?=$form->text(array('paragraph'	=> false,
								 'name'		=> 'preferencesargs[]',
								 'id'		=> false,
								 'label'		=> false,
								 'size'		=> 15,
								 'value'		=> $match[2],
								 'default'		=> $match[2]));?>
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
				<td colspan="3" class="td-single"><?=$this->bbf('no_'.$type);?></td>
			</tr>
			</tfoot>
		</table>
		<table class="b-nodisplay" cellspacing="0" cellpadding="0" border="0">
			<tbody id="ex-<?=$type?>">
			<tr class="fm-paragraph">
				<td class="td-left txt-center">
	<?php
					echo $form->select(array('paragraph'	=> false,
								   'name'		=> 'preferenceslist[]',
								   'id'		=> false,
								   'label'		=> false,
								   'key'		=> false,
								   'invalid'	=> true
							 ),
							 $info['preferences']['avail']);?>
				</td>
				<td>
					<?=$form->text(array('paragraph'	=> false,
								 'name'		=> 'preferencesargs[]',
								 'id'		=> false,
								 'label'		=> false,
								 'size'		=> 15,
								 'disabled'	=> true,
								 'default'		=> ''));?>
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
<div id="sb-part-last" class="b-nodisplay">
<?php
	$type = 'xlets';
	$count = count($info['xlets']['slt']);
?>
	<p>&nbsp;</p>
	<div class="sb-list">
		<table cellspacing="0" cellpadding="0" border="0">
			<thead>
			<tr class="sb-top">
				<th class="th-left"><?=$this->bbf('col_'.$type.'-name');?></th>
				<th class="th-center"><?=$this->bbf('col_'.$type.'-args');?></th>
				<th class="th-center"><?=$this->bbf('col_'.$type.'-f');?></th>
				<th class="th-center"><?=$this->bbf('col_'.$type.'-c');?></th>
				<th class="th-center"><?=$this->bbf('col_'.$type.'-m');?></th>
				<th class="th-center"><?=$this->bbf('col_'.$type.'-s');?></th>
				<th class="th-right">
					<?=$url->href_html($url->img_html('img/site/button/mini/orange/bo-add.gif',
									  $this->bbf('col_'.$type.'-add'),
									  'border="0"'),
							   '#',
							   null,
							   'onclick="dwho.dom.make_table_list(\''.$type.'\',this); return(dwho.dom.free_focus());"',
							   $this->bbf('col_'.$type.'-add'));?>
				</th>
			</tr>
			</thead>
			<tbody id="xlets">
		<?php
		if($count > 0):
			for($i = 0;$i < $count;$i++):

		?>
			<tr class="fm-paragraph<?=$errdisplay?>">
				<td class="td-left txt-center">
	<?php
					echo $form->select(array('paragraph'	=> false,
								   'name'		=> 'xletslist[]',
								   'id'		=> false,
								   'label'		=> false,
								   'key'		=> false,
								   'selected'	=> $info['xlets']['slt'][$i][0]
							 ),
							 $info['xlets']['list']['xlets']);?>
				</td>
				<td>
					<?=$form->select(array('paragraph'	=> false,
								 'name'		=> 'xletsloc[]',
								 'id'		=> false,
								 'label'		=> false,
								 'size'		=> 15,
								 'key'		=> false,
								 'selected'		=> $info['xlets']['slt'][$i][1]
							),
							$xletslocavail);?>
				</td>
				<td>
					<?=$form->select(array('paragraph'	=> false,
								   'name'		=> 'xletsf[]',
								   'id'		=> false,
								   'label'		=> false,
								   'selected'	=> strpos($info['xlets']['slt'][$i][2], 'f') === false ? 0 : 1
							 ),
							 $yesno);?>
				</td>
				<td>
					<?=$form->select(array('paragraph'	=> false,
								   'name'		=> 'xletsc[]',
								   'id'		=> false,
								   'label'		=> false,
								   'selected'	=> strpos($info['xlets']['slt'][$i][2], 'c') === false ? 0 : 1
							 ),
							 $yesno);?>
				</td>
				<td>
					<?=$form->select(array('paragraph'	=> false,
								   'name'		=> 'xletsm[]',
								   'id'		=> false,
								   'label'		=> false,
								   'selected'	=> strpos($info['xlets']['slt'][$i][2], 'm') === false ? 0 : 1
							 ),
							 $yesno);?>
				</td>
				<td>
					<?=$form->select(array('paragraph'	=> false,
								   'name'		=> 'xletss[]',
								   'id'		=> false,
								   'label'		=> false,
								   'selected'	=> strpos($info['xlets']['slt'][$i][2], 's') === false ? 0 : 1
							 ),
							 $yesno);?>
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
				<td colspan="7" class="td-single"><?=$this->bbf('no_'.$type);?></td>
			</tr>
			</tfoot>
		</table>
		<table class="b-nodisplay" cellspacing="0" cellpadding="0" border="0">
			<tbody id="ex-<?=$type?>">
			<tr class="fm-paragraph">
				<td class="td-left txt-center">
	<?php
					echo $form->select(array('paragraph'	=> false,
								   'name'		=> 'xletslist[]',
								   'id'			=> false,
								   'label'		=> false,
								   'invalid'	=> true,
								   'key'		=> false
							 ),
							 $info['xlets']['list']['xlets']);?>
				</td>
				<td>
					<?=$form->select(array('paragraph'	=> false,
								 'name'		=> 'xletsloc[]',
								 'id'		=> false,
								 'label'	=> false,
								 'invalid'	=> true,
								 'key'		=> false,
								 'size'		=> 15
							),
							$xletslocavail);?>
				</td>
				<td>
					<?=$form->select(array('paragraph'	=> false,
								   'name'		=> 'xletsf[]',
								   'id'			=> true,
								   'label'		=> false,
								   'default'	=> 1
							 ),
							 $yesno);?>
				</td>
				<td>
					<?=$form->select(array('paragraph'	=> false,
								   'name'		=> 'xletsc[]',
								   'id'			=> false,
								   'label'		=> false,
								   'default'	=> 1
							 ),
							 $yesno);?>
				</td>
				<td>
					<?=$form->select(array('paragraph'	=> false,
								   'name'		=> 'xletsm[]',
								   'id'			=> false,
								   'label'		=> false,
								   'default'	=> 1
							 ),
							 $yesno);?>
				</td>
				<td>
					<?=$form->select(array('paragraph'	=> false,
								   'name'		=> 'xletss[]',
								   'id'			=> false,
								   'label'		=> false,
								   'default'	=> 1
							 ),
							 $yesno);?>
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


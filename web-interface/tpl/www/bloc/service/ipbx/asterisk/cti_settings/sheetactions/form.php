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
$profileclientlist = $this->get_var('profileclientlist');
$contextavail = $this->get_var('contextavail');
$screens = $this->get_var('screens');
$systrays = $this->get_var('systrays');
$informations = $this->get_var('informations');


if($this->get_var('fm_save') === false):
	$dhtml = &$this->get_module('dhtml');
	$dhtml->write_js('xivo_form_result(false,\''.$dhtml->escape($this->bbf('fm_error-save')).'\');');
endif;

?>

<div id="sb-part-first">
<?php
	echo	$form->text(array('desc'	=> $this->bbf('fm_sheetactions_name'),
				  'name'	=> 'sheetactions[name]',
				  'labelid'	=> 'sheetactions-name',
				  'size'	=> 15,
				  'default'	=> $element['sheetactions']['name']['default'],
				  'value'	=> $info['sheetactions']['name']));

	echo	$form->text(array('desc'	=> $this->bbf('fm_sheetactions_whom'),
				  'name'	=> 'sheetactions[whom]',
				  'labelid'	=> 'sheetactions-whom',
				  'size'	=> 15,
				  'default'	=> $element['sheetactions']['whom']['default'],
				  'value'	=> $info['sheetactions']['whom']));

	echo    $form->checkbox(array('desc' => $this->bbf('fm_sheetactions_focus'),
						'name' => 'sheetactions[focus]',
						'labelid' => 'sheetactions_focus',
						'checked' => $info['sheetactions']['focus']));

?>
<!-- ///////////////////////////////// CONTEXTS ///////////////////////////// -->
    <div class="fm-paragraph fm-description">
        <fieldset id="cti-sheetactions_context">
            <legend><?=$this->bbf('cti-sheetactions-context');?></legend>
            <div id="sheetactions_context" class="fm-paragraph fm-multilist">
                <div class="slt-outlist">
<?php
                echo    $form->select(array('name'  => 'contextlist',
                            'label' => false,
                            'id'    => 'it-contextlist',
                            'key'   => 'name',
                            'altkey'    => 'id',
                            'multiple'  => true,
                            'size'  => 5,
                            'paragraph' => false),
                            $info['context']['list']);
?>
                </div>
                <div class="inout-list">
                    <a href="#"
                    onclick="dwho.form.move_selected('it-contextlist','it-context');
                    return(dwho.dom.free_focus());"
                    title="<?=$this->bbf('bt_inaccess_sheetactions');?>">
                    <?=$url->img_html('img/site/button/arrow-left.gif',
                            $this->bbf('bt_inaccess_sheetactions'),
                            'class="bt-inlist" id="bt-inaccess_sheetactions" border="0"');?></a><br />

                    <a href="#"
                    onclick="dwho.form.move_selected('it-context','it-contextlist');
                    return(dwho.dom.free_focus());"
                    title="<?=$this->bbf('bt_outaccess_sheetactions');?>">
                    <?=$url->img_html('img/site/button/arrow-right.gif',
                            $this->bbf('bt_outaccess_sheetactions'),
                            'class="bt-outlist" id="bt-outaccess_sheetactions" border="0"');?></a>
                </div>
                <div class="slt-inlist">
<?php
                echo    $form->select(array('name'  => 'context[]',
                        'label' => false,
                        'id'    => 'it-context',
                        'key'   => 'name',
                        'altkey'    => 'id',
                        'multiple'  => true,
                        'size'  => 5,
                        'paragraph' => false),
                    $info['context']['slt']);
?>
                </div>
            </div>
        </fieldset>
        <div class="clearboth"></div>
	</div>

<!-- ///////////////////////////////// PROFILS ///////////////////////////// -->
    <div class="fm-paragraph fm-description">
        <fieldset id="cti-sheetactions_capaids">
            <legend><?=$this->bbf('cti-sheetactions-capaids');?></legend>
            <div id="sheetactions_capaids" class="fm-paragraph fm-multilist">
                <div class="slt-outlist">
<?php
                echo    $form->select(array('name'  => 'capaidslist',
                            'label' => false,
                            'id'    => 'it-capaidslist',
                            'key'   => 'name',
                            'altkey'    => 'id',
                            'multiple'  => true,
                            'size'  => 5,
                            'paragraph' => false),
                            $info['capaids']['list']);
?>
                </div>
                <div class="inout-list">
                    <a href="#"
                    onclick="dwho.form.move_selected('it-capaidslist','it-capaids');
                    return(dwho.dom.free_focus());"
                    title="<?=$this->bbf('bt_inaccess_sheetactions');?>">
                    <?=$url->img_html('img/site/button/arrow-left.gif',
                            $this->bbf('bt_inaccess_sheetactions'),
                            'class="bt-inlist" id="bt-inaccess_sheetactions" border="0"');?></a><br />

                    <a href="#"
                    onclick="dwho.form.move_selected('it-capaids','it-capaidslist');
                    return(dwho.dom.free_focus());"
                    title="<?=$this->bbf('bt_outaccess_sheetactions');?>">
                    <?=$url->img_html('img/site/button/arrow-right.gif',
                            $this->bbf('bt_outaccess_sheetactions'),
                            'class="bt-outlist" id="bt-outaccess_sheetactions" border="0"');?></a>
                </div>
                <div class="slt-inlist">
<?php
                echo    $form->select(array('name'  => 'capaids[]',
                        'label' => false,
                        'id'    => 'it-capaids',
                        'key'   => 'name',
                        'altkey'    => 'id',
                        'multiple'  => true,
                        'size'  => 5,
                        'paragraph' => false),
                    $info['capaids']['slt']);
?>
                </div>
            </div>
        </fieldset>
        <div class="clearboth"></div>
	</div>
	<br />
	<div class="fm-paragraph fm-description">
		<p>
			<label id="lb-description" for="it-description"><?=$this->bbf('fm_description');?></label>
		</p>
		<?=$form->textarea(array('paragraph'    => false,
					 'label'    => false,
					 'name'     => 'sheetactions[description]',
					 'id'       => 'it-description',
					 'cols'     => 60,
					 'rows'     => 5,
					 'default'  => $element['sheetactions']['description']['default']),
				   $info['sheetactions']['description']);?>
	</div>
</div>
<div id="sb-part-screens" class="b-nodisplay">
<!-- ///////////////////////////////// SCREENS ///////////////////////////// -->
<?php
	$type = 'screens';
	$count = count($screens);
	$errdisplay = '';
	echo	$form->text(array('desc'	=> $this->bbf('fm_sheetactions_qtui'),
				  'name'	=> 'sheetactions[sheet_qtui]',
				  'labelid'	=> 'sheetactions-qtui',
				  'size'	=> 30,
				  'default'	=> $element['sheetactions']['sheet_qtui']['default'],
				  'value'	=> $info['sheetactions']['sheet_qtui']));

?>
	<p>&nbsp;</p>
	<div class="sb-list">
		<table cellspacing="0" cellpadding="0" border="0">
			<thead>
			<tr class="sb-top">

				<th class="th-left"><?=$this->bbf('col_1');?></th>
				<th class="th-center"><?=$this->bbf('col_2');?></th>
				<th class="th-center"><?=$this->bbf('col_3');?></th>
				<th class="th-center"><?=$this->bbf('col_4');?></th>
				<th class="th-right">
					<?=$url->href_html($url->img_html('img/site/button/mini/orange/bo-add.gif',
									  $this->bbf('col_add'),
									  'border="0"'),
							   '#',
							   null,
							   'onclick="dwho.dom.make_table_list(\''.$type.'\',this); return(dwho.dom.free_focus());"',
							   $this->bbf('col_add'));?>
				</th>
			</tr>
			</thead>
			<tbody id="<?=$type?>">
		<?php
		if($count > 0):
			foreach($screens as $v)
			{

		?>
			<tr class="fm-paragraph<?=$errdisplay?>">
				<td class="td-left txt-center">
	<?php
					echo $form->text(array('paragraph'	=> false,
								   'name'		=> 'screencol1[]',
								   'id'		=> false,
								   'label'		=> false,
								   'size'		=> 15,
								   'key'		=> false,
								   'value'		=> $v[0],
								   'default'	=> ''));
	 ?>
				</td>
				<td>
	<?php
					echo $form->text(array('paragraph'	=> false,
								   'name'		=> 'screencol2[]',
								   'id'		=> false,
								   'label'		=> false,
								   'size'		=> 15,
								   'key'		=> false,
								   'value'		=> $v[1],
								   'default'	=> ''));
	 ?>
				</td>
				<td>
	<?php
					echo $form->text(array('paragraph'	=> false,
								   'name'		=> 'screencol3[]',
								   'id'		=> false,
								   'label'		=> false,
								   'size'		=> 15,
								   'key'		=> false,
								   'value'		=> $v[2],
								   'default'	=> ''));
	 ?>
				</td>
				<td>
	<?php
					echo $form->text(array('paragraph'	=> false,
								   'name'		=> 'screencol4[]',
								   'id'		=> false,
								   'label'		=> false,
								   'size'		=> 15,
								   'key'		=> false,
								   'value'		=> $v[3],
								   'default'	=> ''));
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
			}
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
					echo $form->text(array('paragraph'	=> false,
								   'name'		=> 'screencol1[]',
								   'id'		=> false,
								   'label'		=> false,
								   'size'		=> 15,
								   'key'		=> false,
								   'default'	=> ''));
	 ?>
				</td>
				<td>
	<?php
					echo $form->text(array('paragraph'	=> false,
								   'name'		=> 'screencol2[]',
								   'id'		=> false,
								   'label'		=> false,
								   'size'		=> 15,
								   'key'		=> false,
								   'default'	=> ''));
	 ?>
				</td>
				<td>
	<?php
					echo $form->text(array('paragraph'	=> false,
								   'name'		=> 'screencol3[]',
								   'id'		=> false,
								   'label'		=> false,
								   'size'		=> 15,
								   'key'		=> false,
								   'default'	=> ''));
	 ?>
				</td>
				<td>
	<?php
					echo $form->text(array('paragraph'	=> false,
								   'name'		=> 'screencol4[]',
								   'id'		=> false,
								   'label'		=> false,
								   'size'		=> 15,
								   'key'		=> false,
								   'default'	=> ''));
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
<div id="sb-part-systrays" class="b-nodisplay">

<!-- ///////////////////////////////// SYSTRAYS ///////////////////////////// -->
<?php
	$type = 'systrays';
	$count = count($systrays);
	$errdisplay = '';
?>
	<p>&nbsp;</p>
	<div class="sb-list">
		<table cellspacing="0" cellpadding="0" border="0">
			<thead>
			<tr class="sb-top">

				<th class="th-left"><?=$this->bbf('col_1');?></th>
				<th class="th-center"><?=$this->bbf('col_2');?></th>
				<th class="th-center"><?=$this->bbf('col_3');?></th>
				<th class="th-center"><?=$this->bbf('col_4');?></th>
				<th class="th-right">
					<?=$url->href_html($url->img_html('img/site/button/mini/orange/bo-add.gif',
									  $this->bbf('col_add'),
									  'border="0"'),
							   '#',
							   null,
							   'onclick="dwho.dom.make_table_list(\''.$type.'\',this); return(dwho.dom.free_focus());"',
							   $this->bbf('col_add'));?>
				</th>
			</tr>
			</thead>
			<tbody id="<?=$type?>">
		<?php
		if($count > 0):
			foreach($systrays as $v)
			{

		?>
			<tr class="fm-paragraph<?=$errdisplay?>">
				<td class="td-left txt-center">
	<?php
					echo $form->text(array('paragraph'	=> false,
								   'name'		=> 'systrayscol1[]',
								   'id'		=> false,
								   'label'		=> false,
								   'size'		=> 15,
								   'key'		=> false,
								   'value'		=> $v[0],
								   'default'	=> ''));
	 ?>
				</td>
				<td>
	<?php
					echo $form->text(array('paragraph'	=> false,
								   'name'		=> 'systrayscol2[]',
								   'id'		=> false,
								   'label'		=> false,
								   'size'		=> 15,
								   'key'		=> false,
								   'value'		=> $v[1],
								   'default'	=> ''));
	 ?>
				</td>
				<td>
	<?php
					echo $form->text(array('paragraph'	=> false,
								   'name'		=> 'systrayscol3[]',
								   'id'		=> false,
								   'label'		=> false,
								   'size'		=> 15,
								   'key'		=> false,
								   'value'		=> $v[2],
								   'default'	=> ''));
	 ?>
				</td>
				<td>
	<?php
					echo $form->text(array('paragraph'	=> false,
								   'name'		=> 'systrayscol4[]',
								   'id'		=> false,
								   'label'		=> false,
								   'size'		=> 15,
								   'key'		=> false,
								   'value'		=> $v[3],
								   'default'	=> ''));
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
			}
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
					echo $form->text(array('paragraph'	=> false,
								   'name'		=> 'systrayscol1[]',
								   'id'		=> false,
								   'label'		=> false,
								   'size'		=> 15,
								   'key'		=> false,
								   'default'	=> ''));
	 ?>
				</td>
				<td>
	<?php
					echo $form->text(array('paragraph'	=> false,
								   'name'		=> 'systrayscol2[]',
								   'id'		=> false,
								   'label'		=> false,
								   'size'		=> 15,
								   'key'		=> false,
								   'default'	=> ''));
	 ?>
				</td>
				<td>
	<?php
					echo $form->text(array('paragraph'	=> false,
								   'name'		=> 'systrayscol3[]',
								   'id'		=> false,
								   'label'		=> false,
								   'size'		=> 15,
								   'key'		=> false,
								   'default'	=> ''));
	 ?>
				</td>
				<td>
	<?php
					echo $form->text(array('paragraph'	=> false,
								   'name'		=> 'systrayscol4[]',
								   'id'		=> false,
								   'label'		=> false,
								   'size'		=> 15,
								   'key'		=> false,
								   'default'	=> ''));
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
<div id="sb-part-last" class="b-nodisplay">

<!-- ///////////////////////////////// INFORMATIONS ///////////////////////////// -->
<?php
	$type = 'informations';
	$count = count($informations);
	$errdisplay = '';
?>
	<p>&nbsp;</p>
	<div class="sb-list">
		<table cellspacing="0" cellpadding="0" border="0">
			<thead>
			<tr class="sb-top">

				<th class="th-left"><?=$this->bbf('col_1');?></th>
				<th class="th-center"><?=$this->bbf('col_2');?></th>
				<th class="th-center"><?=$this->bbf('col_3');?></th>
				<th class="th-center"><?=$this->bbf('col_4');?></th>
				<th class="th-right">
					<?=$url->href_html($url->img_html('img/site/button/mini/orange/bo-add.gif',
									  $this->bbf('col_add'),
									  'border="0"'),
							   '#',
							   null,
							   'onclick="dwho.dom.make_table_list(\''.$type.'\',this); return(dwho.dom.free_focus());"',
							   $this->bbf('col_add'));?>
				</th>
			</tr>
			</thead>
			<tbody id="<?=$type?>">
		<?php
		if($count > 0):
			foreach($informations as $v)
			{

		?>
			<tr class="fm-paragraph<?=$errdisplay?>">
				<td class="td-left txt-center">
	<?php
					echo $form->text(array('paragraph'	=> false,
								   'name'		=> 'infoscol1[]',
								   'id'		=> false,
								   'label'		=> false,
								   'size'		=> 15,
								   'key'		=> false,
								   'value'		=> $v[0],
								   'default'	=> ''));
	 ?>
				</td>
				<td>
	<?php
					echo $form->text(array('paragraph'	=> false,
								   'name'		=> 'infoscol2[]',
								   'id'		=> false,
								   'label'		=> false,
								   'size'		=> 15,
								   'key'		=> false,
								   'value'		=> $v[1],
								   'default'	=> ''));
	 ?>
				</td>
				<td>
	<?php
					echo $form->text(array('paragraph'	=> false,
								   'name'		=> 'infoscol3[]',
								   'id'		=> false,
								   'label'		=> false,
								   'size'		=> 15,
								   'key'		=> false,
								   'value'		=> $v[2],
								   'default'	=> ''));
	 ?>
				</td>
				<td>
	<?php
					echo $form->text(array('paragraph'	=> false,
								   'name'		=> 'infoscol4[]',
								   'id'		=> false,
								   'label'		=> false,
								   'size'		=> 15,
								   'key'		=> false,
								   'value'		=> $v[3],
								   'default'	=> ''));
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
			}
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
					echo $form->text(array('paragraph'	=> false,
								   'name'		=> 'infoscol1[]',
								   'id'		=> false,
								   'label'		=> false,
								   'size'		=> 15,
								   'key'		=> false,
								   'default'	=> ''));
	 ?>
				</td>
				<td>
	<?php
					echo $form->text(array('paragraph'	=> false,
								   'name'		=> 'infoscol2[]',
								   'id'		=> false,
								   'label'		=> false,
								   'size'		=> 15,
								   'key'		=> false,
								   'default'	=> ''));
	 ?>
				</td>
				<td>
	<?php
					echo $form->text(array('paragraph'	=> false,
								   'name'		=> 'infoscol3[]',
								   'id'		=> false,
								   'label'		=> false,
								   'size'		=> 15,
								   'key'		=> false,
								   'default'	=> ''));
	 ?>
				</td>
				<td>
	<?php
					echo $form->text(array('paragraph'	=> false,
								   'name'		=> 'infoscol4[]',
								   'id'		=> false,
								   'label'		=> false,
								   'size'		=> 15,
								   'key'		=> false,
								   'default'	=> ''));
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


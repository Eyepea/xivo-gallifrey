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

$info = $this->get_var('info');
$element = $this->get_var('element');

if($this->get_var('fm_save') === false):
	$dhtml->write_js('xivo_form_result(false,\''.$dhtml->escape($this->bbf('fm_error-save')).'\');');
endif;

?>

<div id="sb-part-first">
<?php
	echo	$form->text(array('desc'	=> $this->bbf('fm_ldapfilter_name'),
				  'name'	=> 'ldapfilter[name]',
				  'labelid'	=> 'ldapfilter-name',
				  'size'	=> 15,
				  'default'	=> $element['ldapfilter']['name']['default'],
				  'value'	=> $info['ldapfilter']['name']));

	if(($ldapservers = $this->get_var('ldapservers')) !== false):
		echo	$form->select(array('desc'	=> $this->bbf('fm_ldapfilter_ldapserverid'),
					    'name'	=> 'ldapfilter[ldapserverid]',
					    'labelid'	=> 'ldapfilter-ldapserverid',
					    'invalid'	=> ($this->get_var('act') === 'edit'),
					    'key'	=> 'identity',
					    'altkey'	=> 'id',
					    'default'	=> $element['ldapfilter']['ldapserverid']['default'],
					    'selected'	=> $info['ldapfilter']['ldapserverid']),
				      $ldapservers);
	else:
		echo	'<div class="txt-center">',
			$url->href_html($this->bbf('create_ldapserver'),
					'xivo/configuration/manage/ldapserver',
					'act=add'),
			'</div>';
	endif;

	echo	$form->text(array('desc'	=> $this->bbf('fm_ldapfilter_user'),
				  'name'	=> 'ldapfilter[user]',
				  'labelid'	=> 'ldapfilter-user',
				  'size'	=> 15,
				  'default'	=> $element['ldapfilter']['user']['default'],
				  'value'	=> $info['ldapfilter']['user'])),

		$form->text(array('desc'	=> $this->bbf('fm_ldapfilter_passwd'),
				  'name'	=> 'ldapfilter[passwd]',
				  'labelid'	=> 'ldapfilter-passwd',
				  'size'	=> 15,
				  'default'	=> $element['ldapfilter']['passwd']['default'],
				  'value'	=> $info['ldapfilter']['passwd'])),

		$form->text(array('desc'	=> $this->bbf('fm_ldapfilter_basedn'),
				  'name'	=> 'ldapfilter[basedn]',
				  'labelid'	=> 'ldapfilter-basedn',
				  'size'	=> 30,
				  'default'	=> $element['ldapfilter']['basedn']['default'],
				  'value'	=> $info['ldapfilter']['basedn'])),

		$form->text(array('desc'	=> $this->bbf('fm_ldapfilter_filter'),
				  'name'	=> 'ldapfilter[filter]',
				  'labelid'	=> 'ldapfilter-filter',
				  'size'	=> 30,
				  'notag'	=> false,
				  'default'	=> $element['ldapfilter']['filter']['default'],
				  'value'	=> $info['ldapfilter']['filter'])),

		$form->select(array('desc'	=> $this->bbf('fm_ldapfilter_additionaltype'),
				    'name'	=> 'ldapfilter[additionaltype]',
				    'labelid'	=> 'ldapfilter-additionaltype',
				    'key'	=> false,
				    'bbf'	=> 'fm_ldapfilter_additionaltype-opt',
				    'bbfopt'	=> array('argmode' => 'paramvalue'),
				    'default'	=> $element['ldapfilter']['additionaltype']['default'],
				    'selected'	=> $info['ldapfilter']['additionaltype']),
			      $element['ldapfilter']['additionaltype']['value'],
			      'onchange="xivo_chg_additionaltype(this.value);"'),

		$form->text(array('desc'	=> '&nbsp;',
				  'name'	=> 'ldapfilter[additionaltext]',
				  'labelid'	=> 'ldapfilter-additionaltext',
				  'size'	=> 15,
				  'default'	=> $element['ldapfilter']['additionaltext']['default'],
				  'value'	=> $info['ldapfilter']['additionaltext']));
?>
	<div class="fm-paragraph fm-description">
		<p>
			<label id="lb-ldapfilter-description" for="it-ldapfilter-description"><?=$this->bbf('fm_ldapfilter_description');?></label>
		</p>
		<?=$form->textarea(array('paragraph'	=> false,
					 'label'	=> false,
					 'name'		=> 'ldapfilter[description]',
					 'id'		=> 'it-ldapfilter-description',
					 'cols'		=> 60,
					 'rows'		=> 5,
					 'default'	=> $element['ldapfilter']['description']['default']),
				   $info['ldapfilter']['description']);?>
	</div>
</div>

<div id="sb-part-last" class="b-nodisplay">

<fieldset id="fld-ldapfilter-attrdisplayname">
	<legend><?=$this->bbf('fld-ldapfilter-attrdisplayname');?></legend>

	<div class="fm-paragraph fm-multilist">
		<div class="slt-list">
			<div class="bt-adddelete">
				<a href="#"
				   onclick="xivo_fm_select_add_attrldap('it-ldapfilter-attrdisplayname',
									prompt('<?=$dhtml->escape($this->bbf('add_ldapfilter-attrdisplayname'));?>'));
					    return(dwho.dom.free_focus());"
				   title="<?=$this->bbf('bt_ldapfilter-attrdisplayname-add');?>">
					<?=$url->img_html('img/site/button/mini/blue/add.gif',
							  $this->bbf('bt_ldapfilter-attrdisplayname-add'),
							  'class="bt-addlist"
							   id="bt-ldapfilter-attrdisplayname-add"
							   border="0"');?></a><br />
					<a href="#"
					   onclick="dwho.form.select_delete_entry('it-ldapfilter-attrdisplayname');
						    return(dwho.dom.free_focus());"
					   title="<?=$this->bbf('bt_delete_ldapfilter-attrdisplayname');?>">
					<?=$url->img_html('img/site/button/mini/orange/delete.gif',
							  $this->bbf('bt_delete_ldapfilter-attrdisplayname'),
							  'class="bt-deletelist"
							   id="bt-ldapfilter-attrdisplayname-delete"
							   border="0"');?></a>
			</div>
<?php
		echo	$form->select(array('name'	=> 'ldapfilter[attrdisplayname][]',
					    'label'	=> false,
					    'id'	=> 'it-ldapfilter-attrdisplayname',
					    'key'	=> false,
					    'multiple'	=> true,
					    'size'	=> 5,
					    'paragraph'	=> false),
				      $info['ldapfilter']['attrdisplayname']);
?>
		</div>

		<div class="bt-updown">
			<a href="#"
			   onclick="dwho.form.order_selected('it-ldapfilter-attrdisplayname',1);
				    return(dwho.dom.free_focus());"
			   title="<?=$this->bbf('bt_up_ldapfilter-attrdisplayname');?>">
				<?=$url->img_html('img/site/button/row-up.gif',
						  $this->bbf('bt_up_ldapfilter-attrdisplayname'),
						  'class="bt-uplist"
						   id="bt-ldapfilter-attrdisplayname-up"
						   border="0"');?></a><br />
			<a href="#"
			   onclick="dwho.form.order_selected('it-ldapfilter-attrdisplayname',-1);
				    return(dwho.dom.free_focus());"
			   title="<?=$this->bbf('bt_down_ldapfilter-attrdisplayname');?>">
				<?=$url->img_html('img/site/button/row-down.gif',
						  $this->bbf('bt_down_ldapfilter-attrdisplayname'),
						  'class="bt-downlist"
						   id="bt-ldapfilter-attrdisplayname-down"
						   border="0"');?></a>
		</div>
	</div>
	<div class="clearboth"></div>
</fieldset>

<fieldset id="fld-ldapfilter-attrphonenumber">
	<legend><?=$this->bbf('fld-ldapfilter-attrphonenumber');?></legend>

	<div class="fm-paragraph fm-multilist">
		<div class="slt-list">
			<div class="bt-adddelete">
				<a href="#"
				   onclick="xivo_fm_select_add_attrldap('it-ldapfilter-attrphonenumber',
									prompt('<?=$dhtml->escape($this->bbf('add_ldapfilter-attrphonenumber'));?>'));
					    return(dwho.dom.free_focus());"
				   title="<?=$this->bbf('bt_ldapfilter-attrphonenumber-add');?>">
					<?=$url->img_html('img/site/button/mini/blue/add.gif',
							  $this->bbf('bt_ldapfilter-attrphonenumber-add'),
							  'class="bt-addlist"
							   id="bt-ldapfilter-attrphonenumber-add"
							   border="0"');?></a><br />
				<a href="#"
				   onclick="dwho.form.select_delete_entry('it-ldapfilter-attrphonenumber');
					    return(dwho.dom.free_focus());"
				   title="<?=$this->bbf('bt_delete_ldapfilter-attrphonenumber');?>">
					<?=$url->img_html('img/site/button/mini/orange/delete.gif',
							  $this->bbf('bt_delete_ldapfilter-attrphonenumber'),
							  'class="bt-deletelist"
							   id="bt-ldapfilter-attrphonenumber-delete"
							   border="0"');?></a>
			</div>
<?php
		echo	$form->select(array('name'	=> 'ldapfilter[attrphonenumber][]',
					    'label'	=> false,
					    'id'	=> 'it-ldapfilter-attrphonenumber',
					    'key'	=> false,
					    'multiple'	=> true,
					    'size'	=> 5,
					    'paragraph'	=> false),
				      $info['ldapfilter']['attrphonenumber']);
?>
		</div>

		<div class="bt-updown">
			<a href="#"
			   onclick="dwho.form.order_selected('it-ldapfilter-attrphonenumber',1);
				    return(dwho.dom.free_focus());"
			   title="<?=$this->bbf('bt_up_ldapfilter-attrphonenumber');?>">
				<?=$url->img_html('img/site/button/row-up.gif',
						  $this->bbf('bt_up_ldapfilter-attrphonenumber'),
						  'class="bt-uplist"
						   id="bt-ldapfilter-attrphonenumber-up"
						   border="0"');?></a><br />
			<a href="#"
			   onclick="dwho.form.order_selected('it-ldapfilter-attrphonenumber',-1);
				    return(dwho.dom.free_focus());"
			   title="<?=$this->bbf('bt_down_ldapfilter-attrphonenumber');?>">
				<?=$url->img_html('img/site/button/row-down.gif',
						  $this->bbf('bt_down_ldapfilter-attrphonenumber'),
						  'class="bt-downlist"
						   id="bt-ldapfilter-attrphonenumber-down"
						   border="0"');?></a>
		</div>
	</div>
	<div class="clearboth"></div>
</fieldset>

</div>

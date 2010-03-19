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

$info = $this->get_var('info');
$element = $this->get_var('element');

if($this->get_var('fm_save') === false):
    $dhtml = &$this->get_module('dhtml');
    $dhtml->write_js('xivo_form_result(false,\''.$dhtml->escape($this->bbf('fm_error-save')).'\');');
endif;

?>

<?php
	echo	$form->text(array('desc'	=> $this->bbf('fm_name'),
				  'name'	=> 'name',
				  'labelid'	=> 'name',
				  'size'	=> 15,
				  'default'	=> $element['name']['default'],
				  'value' => $info['name'],
		          'error'	=> $this->bbf_args('name',
					   $this->get_var('error', 'name')) )),

		$form->select(array('desc'	=> $this->bbf('fm_type'),
					'name'	=> 'type',
					'labelid'	=> 'type',
					'default'	=> $element['type']['default'],
					'selected'	=> $info['type']),
				  array('Sqlite','MySQL','File','Webservices','Internal'),
				  'onchange="javascript:hidediv();"'),

		$form->text(array('desc'	=> $this->bbf('fm_uri'),
				  'name'	=> 'uri',
				  'labelid'	=> 'uri',
				  'default'	=> $element['uri']['default'],
				  'value'	=> $info['uri'],
		          'error'	=> $this->bbf_args('uri',
					   $this->get_var('error', 'uri')) ));
?>
<div id="sgbdr" class="b-nodisplay">
<?php
		echo $form->text(array('desc'	=> $this->bbf('fm_host'),
				  'name'	=> 'host',
				  'labelid'	=> 'host',
				  'default'	=> $element['host']['default'],
				  'value'	=> $info['host'],
		          'error'	=> $this->bbf_args('host',
					   $this->get_var('error', 'host')) )),

		$form->text(array('desc'	=> $this->bbf('fm_port'),
				  'name'	=> 'port',
				  'labelid'	=> 'port',
				  'default'	=> $element['port']['default'],
				  'value'	=> $info['port'],
		          'error'	=> $this->bbf_args('port',
					   $this->get_var('error', 'port')) )),

		$form->text(array('desc'	=> $this->bbf('fm_dbname'),
				  'name'	=> 'dbname',
				  'labelid'	=> 'dbname',
				  'default'	=> $element['dbname']['default'],
				  'value'	=> $info['dbname'],
		          'error'	=> $this->bbf_args('dbname',
					   $this->get_var('error', 'dbname')) )),

		$form->text(array('desc'	=> $this->bbf('fm_tablename'),
				  'name'	=> 'tablename',
				  'labelid'	=> 'tablename',
				  'default'	=> $element['tablename']['default'],
				  'value'	=> $info['tablename'],
		          'error'	=> $this->bbf_args('tablename',
					   $this->get_var('error', 'tablename')) )),

		$form->text(array('desc'	=> $this->bbf('fm_user'),
				  'name'	=> 'user',
				  'labelid'	=> 'user',
				  'default'	=> $element['user']['default'],
				  'value'	=> $info['user'],
		          'error'	=> $this->bbf_args('user',
					   $this->get_var('error', 'user')) )),

		$form->text(array('desc'	=> $this->bbf('fm_password'),
				  'name'	=> 'password',
				  'labelid'	=> 'password',
				  'default'	=> $element['password']['default'],
				  'value'	=> $info['password'],
		          'error'	=> $this->bbf_args('password',
					   $this->get_var('error', 'password')) ));
	?>
	</div>
	<div class="fm-paragraph fm-description">
		<p>
			<label id="lb-description" for="it-description"><?=$this->bbf('fm_description');?></label>
		</p>
		<?=$form->textarea(array('paragraph'	=> false,
					 'label'	=> false,
					 'name'		=> 'description',
					 'id'		=> 'it-description',
					 'cols'		=> 60,
					 'rows'		=> 5,
					 'default'	=> $element['description']['default'],
		          'error'	=> $this->bbf_args('description',
					   $this->get_var('error', 'description')) ),
				   $info['description']);?>
	</div>

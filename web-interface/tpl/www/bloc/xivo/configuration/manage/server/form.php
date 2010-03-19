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

echo	$form->text(array('desc'	=> $this->bbf('fm_name'),
			  'name'	=> 'name',
			  'labelid'	=> 'name',
			  'size'	=> 15,
			  'default'	=> $element['name']['default'],
			  'value'	=> $info['name'],
	          'error'	=> $this->bbf_args('name',
				   $this->get_var('error', 'name')))),

	$form->text(array('desc'	=> $this->bbf('fm_host'),
			  'name'	=> 'host',
			  'labelid'	=> 'host',
			  'size'	=> 15,
			  'default'	=> $element['host']['default'],
			  'value'	=> $info['host'],
	          'error'	=> $this->bbf_args('host',
				   $this->get_var('error', 'host')))),

	$form->text(array('desc'	=> $this->bbf('fm_port'),
			  'name'	=> 'port',
			  'labelid'	=> 'port',
			  'default'	=> $element['port']['default'],
			  'value'	=> $info['port'],
	          'error'	=> $this->bbf_args('port',
				   $this->get_var('error', 'port')))),

	$form->checkbox(array('desc'	=> $this->bbf('fm_ssl'),
			      'name'	=> 'ssl',
			      'labelid'	=> 'ssl',
			      'default'	=> $element['ssl']['default'],
			      'checked'	=> $info['ssl'],
	          'error'	=> $this->bbf_args('ssl',
				   $this->get_var('error', 'ssl')))),

	$form->text(array('desc'	=> $this->bbf('fm_webi_addr'),
			  'name'	=> 'webi',
			  'labelid'	=> 'webi',
			  'size'	=> 15,
			  'default'	=> $element['webi']['default'],
			  'value'	=> $info['webi'],
	          'error'	=> $this->bbf_args('webi',
				   $this->get_var('error', 'webi')))),

	$form->text(array('desc'	=> $this->bbf('fm_ami_port'),
			  'name'	=> 'ami_port',
			  'labelid'	=> 'ami_port',
			  'size'	=> 15,
			  'default'	=> $element['ami_port']['default'],
			  'value'	=> $info['ami_port'],
	          'error'	=> $this->bbf_args('ami_port',
				   $this->get_var('error', 'ami_port')))),

	$form->text(array('desc'	=> $this->bbf('fm_ami_login'),
			  'name'	=> 'ami_login',
			  'labelid'	=> 'ami_login',
			  'size'	=> 15,
			  'default'	=> $element['ami_login']['default'],
			  'value'	=> $info['ami_login'],
	          'error'	=> $this->bbf_args('ami_login',
				   $this->get_var('error', 'ami_login')))),

	$form->text(array('desc'	=> $this->bbf('fm_ami_pass'),
			  'name'	=> 'ami_pass',
			  'labelid'	=> 'ami_pass',
			  'size'	=> 15,
			  'default'	=> $element['ami_pass']['default'],
			  'value'	=> $info['ami_pass'],
	          'error'	=> $this->bbf_args('ami_pass',
				   $this->get_var('error', 'ami_pass'))));

?>
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
				 'default'	=> $element['description']['default']),
			   $info['description']);?>
</div>

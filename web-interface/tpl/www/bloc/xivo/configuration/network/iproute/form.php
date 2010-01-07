<?php

#
# XiVO Web-Interface
# Copyright (C) 2009-2010  Proformatique <technique@proformatique.com>
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
			  'default'	=> $element['iproute']['name']['default'],
			  'value'	=> $info['name']));

	if(($interfaces = $this->get_var('interfaces')) !== false):
		echo	$form->select(array('desc'	=> $this->bbf('fm_iface'),
					    'name'	=> 'iface',
					    'labelid'	=> 'iface',
					    'key'	=> 'identity',
					    'altkey'	=> 'name',
					    'default'	=> $element['iproute']['iface']['default'],
					    'selected'	=> $info['iface']),
				      $interfaces);
	else:
		echo	'<div class="txt-center">',
			$url->href_html($this->bbf('create_interface'),'xivo/configuration/network/interface','act=list'),
			'</div>';
	endif;

echo	$form->text(array('desc'	=> $this->bbf('fm_destination'),
			  'name'	=> 'destination',
			  'labelid'	=> 'destination',
			  'size'	=> 15,
			  'default'	=> $element['iproute']['destination']['default'],
			  'value'	=> $info['destination'])),

	$form->text(array('desc'	=> $this->bbf('fm_netmask'),
			  'name'	=> 'netmask',
			  'labelid'	=> 'netmask',
			  'size'	=> 15,
			  'value'	=> $info['netmask'])),

	$form->text(array('desc'	=> $this->bbf('fm_gateway'),
			  'name'	=> 'gateway',
			  'labelid'	=> 'gateway',
			  'size'	=> 15,
			  'default'	=> $element['iproute']['gateway']['default'],
			  'value'	=> $info['gateway']));
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
				 'default'	=> $element['iproute']['description']['default']),
			   $info['description']);?>
</div>

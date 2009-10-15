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

?>
<div id="fd-ipbxapplication-playback" class="b-nodisplay">
<?php

$form = &$this->get_module('form');
$url = &$this->get_module('url');

$apparg_playback = $this->get_var('apparg_playback');
$sound_list = $this->get_var('destination_list','sounds');

if(empty($sound_list) === false):
	echo $form->select(array('desc'		=> $this->bbf('fm_ipbxapplication_playback-filename'),
				 'name'		=> 'ipbxapplication[playback][filename]',
				 'labelid'	=> 'ipbxapplication-playback-filename',
				 'key'		=> 'name',
				 'altkey'	=> 'filename',
				 'default'	=> $apparg_playback['filename']['default']),
			   $sound_list);
else:
	echo	'<div class="txt-center">',
		$url->href_html($this->bbf('add_playback-sound'),
				'service/ipbx/pbx_services/sounds',
				array('act' => 'list','dir' => 'playback')),
		'</div>';
endif;

echo	$form->checkbox(array('desc'	=> $this->bbf('fm_ipbxapplication_playback-skip'),
			      'name'	=> 'ipbxapplication[playback][skip]',
			      'labelid'	=> 'ipbxapplication-playback-skip',
			      'default'	=> $apparg_playback['skip']['default'])),

	$form->checkbox(array('desc'	=> $this->bbf('fm_ipbxapplication_playback-noanswer'),
			      'name'	=> 'ipbxapplication[playback][noanswer]',
			      'labelid'	=> 'ipbxapplication-playback-noanswer',
			      'default'	=> $apparg_playback['noanswer']['default'])),

	$form->checkbox(array('desc'	=> $this->bbf('fm_ipbxapplication_playback-j'),
			      'name'	=> 'ipbxapplication[playback][j]',
			      'labelid'	=> 'ipbxapplication-playback-j',
			      'default'	=> $apparg_playback['j']['default'])),

	$form->button(array('name'	=> 'add-ipbxapplication-playback',
			    'id'	=> 'it-add-ipbxapplication-playback',
			    'value'	=> $this->bbf('fm_bt-add')),
		   'onclick="xivo_ast_application_playback();"');

?>
</div>

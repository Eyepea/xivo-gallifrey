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

?>
<div class="b-infos b-form">
	<h3 class="sb-top xspan">
		<span class="span-left">&nbsp;</span>
		<span class="span-center"><?=$this->bbf('title_content_name');?></span>
		<span class="span-right">&nbsp;</span>
	</h3>
	<div class="sb-content">
		<form action="#" method="post" accept-charset="utf-8">
<?php
	echo	$form->hidden(array('name'	=> DWHO_SESS_NAME,
				    'value'	=> DWHO_SESS_ID)),

		$form->hidden(array('name'	=> 'fm_send',
				    'value'	=> 1)),

		$form->hidden(array('name'	=> 'act',
				    'value'	=> 'edit')),

		$form->hidden(array('name'	=> 'id',
				    'value'	=> $info['name']));
?>
		<p class="fm-paragraph txt-center width-auto"><?=$this->bbf('fm_filename'),' ',$info['name']?></p>
		<div class="fm-paragraph txt-center width-auto">
			<p><label id="lb-content" for="it-content"><?=$this->bbf('fm_content');?></label></p>
				<?=$form->textarea(array('paragraph'	=> false,
							 'name'		=> 'content',
							 'label'	=> false,
							 'notag'	=> false,
							 'id'		=> 'it-content',
							 'cols'		=> 90,
							 'rows'		=> 30),
						   $info['content']);?>
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

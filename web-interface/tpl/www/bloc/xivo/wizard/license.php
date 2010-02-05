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

?>
<div class="fm-paragraph fm-description">
	<p>
		<label id="lb-license" for="it-license"><?=$this->bbf('fm_license');?></label>
	</p>
<?php
echo	$form->textarea(array('paragraph'	=> false,
			      'label'		=> false,
			      'notag'		=> false,
			      'name'		=> 'license',
			      'id'		=> 'it-license',
			      'cols'		=> 70,
			      'rows'		=> 25,
			      'readonly'	=> true),
			$this->get_var('license'));
?>
</div>
<div class="fm-desc-inline fm-paragraph">
<?php
echo	$form->checkbox(array('paragraph'	=> false,
			      'desc'		=> $this->bbf('fm_license-agree'),
			      'name'		=> 'license-agree',
			      'labelid'		=> 'license-agree',
			      'checked'		=> false,
			      'error'		=> $this->bbf_args('error_fm_license',$this->get_var('error','license'))));
?>
</div>

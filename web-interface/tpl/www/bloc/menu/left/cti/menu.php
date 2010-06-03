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

$url   = &$this->get_module('url');
$dhtml = &$this->get_module('dhtml');

?>
<dl>
	<dt>
		<span class="span-left">&nbsp;</span>
		<span class="span-center"><?=$this->bbf('mn_left_ti_ctisettings');?></span>
		<span class="span-right">&nbsp;</span>
	</dt>
	<dd>
<?php
	if(xivo_user::chk_acl_section('service/cti') === true):
		echo 	'<dl>';

		if(xivo_user::chk_acl('cti', 'general', 'service') === true):
			echo	'<dd id="mn-cti--general">',
				$url->href_html($this->bbf('mn_left_ctisettings-general'),
					'cti/general'),
				'</dd>';
		endif;
		if(xivo_user::chk_acl('cti', 'presences', 'service') === true):
			echo	'<dd id="mn-cti--presences">',
				$url->href_html($this->bbf('mn_left_ctisettings-presences'),
					'cti/presences'),
				'</dd>';
		endif;
		if(xivo_user::chk_acl('cti','phonehints', 'service') === true):
			echo	'<dd id="mn-cti--phonehints">',
				$url->href_html($this->bbf('mn_left_ctisettings-phonehints'),
					'cti/phonehints'),
				'</dd>';
		endif;
		if(xivo_user::chk_acl('cti','profiles', 'service') === true):
			echo	'<dd id="mn-cti--profiles">',
				$url->href_html($this->bbf('mn_left_ctisettings-profiles'),
					'cti/profiles'),
				'</dd>';
		endif;
		if(xivo_user::chk_acl('cti','directories', 'service') === true):
			echo	'<dd id="mn-cti--directories">',
				$url->href_html($this->bbf('mn_left_ctisettings-directories'),
					'cti/directories'),
				'</dd>';
		endif;
		if(xivo_user::chk_acl('cti','reversedirectories', 'service') === true):
			echo	'<dd id="mn-cti--reversedirectories">',
				$url->href_html($this->bbf('mn_left_ctisettings-reversedirectories'),
					'cti/reversedirectories'),
				'</dd>';
		endif;
		if(xivo_user::chk_acl('cti','displays', 'service') === true):
			echo	'<dd id="mn-cti--displays">',
				$url->href_html($this->bbf('mn_left_ctisettings-displays'),
					'cti/displays'),
				'</dd>';
		endif;
		if(xivo_user::chk_acl('cti','contexts', 'service') === true):
			echo	'<dd id="mn-cti--contexts">',
				$url->href_html($this->bbf('mn_left_ctisettings-contexts'),
					'cti/contexts'),
				'</dd>';
		endif;
		if(xivo_user::chk_acl('cti','sheetactions', 'service') === true):
			echo	'<dd id="mn-cti--sheetactions">',
				$url->href_html($this->bbf('mn_left_ctisettings-sheetactions'),
					'cti/sheetactions'),
				'</dd>';
		endif;
		if(xivo_user::chk_acl('cti','sheetevents', 'service') === true):
			echo	'<dd id="mn-cti--sheetevents">',
				$url->href_html($this->bbf('mn_left_ctisettings-sheetevents'),
					'cti/sheetevents'),
				'</dd>';
		endif;
	endif;
	echo	'</dl>';

?>
	</dd>
	<dd class="b-nosize">
		<span class="span-left">&nbsp;</span>
		<span class="span-center">&nbsp;</span>
		<span class="span-right">&nbsp;</span>
	</dd>
</dl>

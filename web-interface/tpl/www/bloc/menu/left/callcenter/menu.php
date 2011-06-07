<?php

#
# XiVO Web-Interface
# Copyright (C) 2010  Proformatique <technique@proformatique.com>
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
		<span class="span-center"><?=$this->bbf('mn_left_ti_callcenter');?></span>
		<span class="span-right">&nbsp;</span>
	</dt>
	<dd>
<?php
	if(xivo_user::chk_acl_section('service/cti') === true):
		echo '<dl>';
		echo '<dt>',$this->bbf('mn_left_ti_callcenter-campaigns'),'</dt>';

		echo	'<dd id="mn-general">',
			$url->href_html($this->bbf('mn_left_callcenter-general'),
				'callcenter/general'),
			'</dd>';
		echo	'<dd id="mn-campaigns">',
			$url->href_html($this->bbf('mn_left_callcenter-campaigns'),
				'callcenter/campaigns'),
			'</dd>';
		echo	'<dd id="mn-tags">',
			$url->href_html($this->bbf('mn_left_callcenter-tags'),
				'callcenter/tags'),
			'</dd>';
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

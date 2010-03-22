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

$url = &$this->get_module('url');

?>
<dl>
	<dt>
		<span class="span-left">&nbsp;</span>
		<span class="span-center"><?=$this->bbf('mn_left_name');?></span>
		<span class="span-right">&nbsp;</span>
	</dt>
	<dd>
		<dl>
			<dt><?=$this->bbf('mn_left_ti_localhost');?></dt>
			<dd id="mn-apache">
				<?=$url->href_html($this->bbf('mn_left_munin-apache'),
						   'stats/munin/apache');?>
			</dd>
			<dd id="mn-asterisk">
				<?=$url->href_html($this->bbf('mn_left_munin-asterisk'),
						   'stats/munin/asterisk');?>
			</dd>
			<dd id="mn-disk">
				<?=$url->href_html($this->bbf('mn_left_munin-disk'),
						   'stats/munin/disk');?>
			</dd>
			<dd id="mn-time">
				<?=$url->href_html($this->bbf('mn_left_munin-time'),
						   'stats/munin/time');?>
			</dd>
			<dd id="mn-postfix">
				<?=$url->href_html($this->bbf('mn_left_munin-postfix'),
						   'stats/munin/postfix');?>
			</dd>
			<dd id="mn-processes">
				<?=$url->href_html($this->bbf('mn_left_munin-processes'),
						   'stats/munin/processes');?>
			</dd>
			<dd id="mn-network">
				<?=$url->href_html($this->bbf('mn_left_munin-network'),
						   'stats/munin/network');?>
			</dd>
			<dd id="mn-system">
				<?=$url->href_html($this->bbf('mn_left_munin-system'),
						   'stats/munin/system');?>
			</dd>
			<dd id="mn-other">
				<?=$url->href_html($this->bbf('mn_left_munin-other'),
						   'stats/munin/other');?>
			</dd>
		</dl>
	</dd>
	<dd class="b-nosize">
		<span class="span-left">&nbsp;</span>
		<span class="span-center">&nbsp;</span>
		<span class="span-right">&nbsp;</span>
	</dd>
</dl>

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

$url  = &$this->get_module('url');
$tree = &$this->get_var('tree');

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
			<?php if(array_key_exists('apache', $tree)) { ?>
			<dd id="mn-apache">
				<?=$url->href_html($this->bbf('mn_left_munin-apache'),
						   'graphs/munin/apache');?>
			</dd>
			<?php 
				} 
				
				if(array_key_exists('asterisk', $tree)) {
			?>
			<dd id="mn-asterisk">
				<?=$url->href_html($this->bbf('mn_left_munin-asterisk'),
						   'graphs/munin/asterisk');?>
			</dd>
			<?php 
				} 
				
				if(array_key_exists('disk', $tree)) {
			?>
			<dd id="mn-disk">
				<?=$url->href_html($this->bbf('mn_left_munin-disk'),
						   'graphs/munin/disk');?>
			</dd>
			<?php 
				} 
				
				if(array_key_exists('time', $tree)) {
			?>
			<dd id="mn-time">
				<?=$url->href_html($this->bbf('mn_left_munin-time'),
						   'graphs/munin/time');?>
			</dd>
			<?php 
				} 
				
				if(array_key_exists('postfix', $tree)) {
			?>
			<dd id="mn-postfix">
				<?=$url->href_html($this->bbf('mn_left_munin-postfix'),
						   'graphs/munin/postfix');?>
			</dd>
			<?php 
				} 
				
				if(array_key_exists('processes', $tree)) {
			?>
			<dd id="mn-processes">
				<?=$url->href_html($this->bbf('mn_left_munin-processes'),
						   'graphs/munin/processes');?>
			</dd>
			<?php 
				} 
				
				if(array_key_exists('network', $tree)) {
			?>
			<dd id="mn-network">
				<?=$url->href_html($this->bbf('mn_left_munin-network'),
						   'graphs/munin/network');?>
			</dd>
			<?php 
				} 
				
				if(array_key_exists('system', $tree)) {
			?>
			<dd id="mn-system">
				<?=$url->href_html($this->bbf('mn_left_munin-system'),
						   'graphs/munin/system');?>
			</dd>
			<?php 
				} 
				
				if(array_key_exists('other', $tree)) {
			?>
			<dd id="mn-other">
				<?=$url->href_html($this->bbf('mn_left_munin-other'),
						   'graphs/munin/other');?>
			</dd>
			<?php } ?>
		</dl>
	</dd>
	<dd class="b-nosize">
		<span class="span-left">&nbsp;</span>
		<span class="span-center">&nbsp;</span>
		<span class="span-right">&nbsp;</span>
	</dd>
</dl>

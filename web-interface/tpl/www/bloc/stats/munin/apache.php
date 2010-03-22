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

$url     = &$this->get_module('url');
$basedir = $this->get_var('basedir');

?>

<div id="sr-users" class="b-infos b-form">
	<h3 class="sb-top xspan">
		<span class="span-left">&nbsp;</span>
		<span class="span-center"><?=$this->bbf('title_content_name');?></span>
		<span class="span-right">&nbsp;</span>
	</h3>
	<div class="sb-content">
    	<fieldset>
		<legend><?=$this->bbf('apache_accesses');?></legend>
		<div class="sb-list">

<?php
    echo $url->img_html("$basedir/XIVO.localdomain-apache_accesses-day.png", 'daily graph');
    echo $url->img_html("$basedir/XIVO.localdomain-apache_accesses-week.png", 'weekly graph');
    echo $url->img_html("$basedir/XIVO.localdomain-apache_accesses-month.png", 'monthly graph');
    echo $url->img_html("$basedir/XIVO.localdomain-apache_accesses-year.png", 'yearly graph');
?>
        </div>
        </fieldset>
        
    	<fieldset>
		<legend><?=$this->bbf('apache_processes');?></legend>
		<div class="sb-list">

<?php
    echo $url->img_html("$basedir/XIVO.localdomain-apache_processes-day.png", 'daily graph');
    echo $url->img_html("$basedir/XIVO.localdomain-apache_processes-week.png", 'weekly graph');
    echo $url->img_html("$basedir/XIVO.localdomain-apache_processes-month.png", 'monthly graph');
    echo $url->img_html("$basedir/XIVO.localdomain-apache_processes-year.png", 'yearly graph');
?>
        </div>
        </fieldset>
    </div>
	<div class="sb-foot xspan">
		<span class="span-left">&nbsp;</span>
		<span class="span-center">&nbsp;</span>
		<span class="span-right">&nbsp;</span>
	</div>
</div>

<!--
      <td><img src="XIVO.localdomain-apache_accesses-day.png" alt="daily graph" width="495"  height="271"/></td>
      <td><img src="XIVO.localdomain-apache_accesses-week.png" alt="weekly graph" width="495"  height="271"/></td>
    </tr>
    <tr>
      <td><img src="XIVO.localdomain-apache_accesses-month.png" alt="monthly graph" width="495"  height="271"/></td>
      <td><img src="XIVO.localdomain-apache_accesses-year.png" alt="yearly graph" width="495"  height="271"/></td>
      
-->

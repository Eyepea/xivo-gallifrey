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

require_once(DWHO_PATH_ROOT.DIRECTORY_SEPARATOR.'munin.inc');

$basedir        = "img/graphs/munin/$domain/";
$graph_tree     =  dwho_munin_graphstree($basedir, $domain);
$module_tree    = $graph_tree[$module];
sort($module_tree);

$_TPL->set_var('basedir', $basedir);
$_TPL->set_var('domain' , $domain);
$_TPL->set_var('graphs' , $module_tree);
$_TPL->set_var('freqs'  , $freqs);

$_TPL->set_var('tree'   , $graph_tree);

$menu = &$_TPL->get_module('menu');
$menu->set_top('top/user/'.$_USR->get_info('meta'));
$menu->set_left('left/graphs/graphs');

$dhtml = &$_TPL->get_module('dhtml');
$dhtml->set_css('css/graphs/graphs.css');

$_TPL->set_bloc('main',"graphs/munin/$module");
$_TPL->set_struct('graphs/index');
$_TPL->display('index');

?>


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

$array = array();

$array['element'] = array();

$array['element']['commented']['value'] = array(0,1);
$array['element']['commented']['default'] = 0;
$array['element']['commented']['set'] = true;

$array['element']['cat_metric'] = array();
$array['element']['cat_metric']['default'] = 0;

$array['element']['var_metric'] = array();
$array['element']['var_metric']['default'] = 0;

$array['element']['filename'] = array();
$array['element']['filename']['default'] = '';

$array['element']['category'] = array();
$array['element']['category']['default'] = '';

$array['element']['var_name'] = array();
$array['element']['var_name']['default'] = '';

$array['element']['var_val'] = array();
$array['element']['var_val']['default'] = '';
$array['element']['var_val']['null'] = true;


$array['filter'] = array();
$array['filter']['commented'] = array('bool' => true);
$array['filter']['cat_metric'] = array('set' => false,'cast' => 'uint');
$array['filter']['var_metric'] = array('set' => false,'cast' => 'uint');
$array['filter']['filename'] = array('regexp' => '/^[a-zA-Z0-9_-]+\.(?:conf|ael)$/','maxlen' => 128);
$array['filter']['category'] = array('set' => false,'chk' => 2,'cast' => 'sgraph','maxlen' => 128);
$array['filter']['var_name'] = array('set' => false,'chk' => 2,'cast' => 'sgraph','maxlen' => 128);
$array['filter']['var_val'] = array('set' => false,'chk' => 2);

?>

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
$array['element']['blindxfer'] = array();
$array['element']['blindxfer']['default'] = '*1';

$array['element']['atxfer'] = array();
$array['element']['atxfer']['default'] = '*2';

$array['element']['automon'] = array();
$array['element']['automon']['default'] = '*3';

$array['element']['disconnect'] = array();
$array['element']['disconnect']['default'] = '*0';


$array['filter'] = array();
$array['filter']['blindxfer'] = array('regexp' => '/^[0-9\*#]+$/','minlen' => 1,'maxlen' => 40);
$array['filter']['atxfer'] = array('regexp' => '/^[0-9\*#]+$/','minlen' => 1,'maxlen' => 40);
$array['filter']['automon'] = array('regexp' => '/^[0-9\*#]+$/','minlen' => 1,'maxlen' => 40);
$array['filter']['disconnect'] = array('regexp' => '/^[0-9\*#]+$/','minlen' => 1,'maxlen' => 40);

?>

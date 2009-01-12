<?php

#
# XiVO Web-Interface
# Copyright (C) 2006, 2007, 2008  Proformatique <technique@proformatique.com>
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

$array['element']['number'] = array();
$array['element']['number']['default'] = '';

$array['element']['pin'] = array();
$array['element']['pin']['default'] = '';

$array['element']['admin-pin'] = array();
$array['element']['admin-pin']['default'] = '';

$array['filter'] = array();
$array['filter']['number'] = array('regexp' => '/^[0-9\*#]+$/','minlen' => 1,'maxlen' => 40);
$array['filter']['pin'] = array('set' => false,'chk' => 2,'regexp' => '/^[0-9\*#]+$/','maxlen' => 40);
$array['filter']['admin-pin'] = array('set' => false,'chk' => 2,'regexp' => '/^[0-9\*#]+$/','maxlen' => 40);

?>

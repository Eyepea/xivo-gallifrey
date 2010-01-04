<?php

#
# XiVO Web-Interface
# Copyright (C) 2006-2009  Proformatique <technique@proformatique.com>
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

$array['element']['confno'] = array();
$array['element']['confno']['default'] = '';

$array['element']['pin'] = array();
$array['element']['pin']['default'] = '';

$array['element']['pinadmin'] = array();
$array['element']['pinadmin']['default'] = '';

$array['filter'] = array();
$array['filter']['confno'] = array('regexp' => '/^[a-zA-Z0-9_\.-]+$/','minlen' => 1,'maxlen' => 80);
$array['filter']['pin'] = array('set' => false,'chk' => 2,'regexp' => '/^[0-9\*#]+$/','maxlen' => 40);
$array['filter']['pinadmin'] = array('set' => false,'chk' => 2,'regexp' => '/^[0-9\*#]+$/','maxlen' => 40);

?>

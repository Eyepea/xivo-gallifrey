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

$array['element']['name'] = array();
$array['element']['name']['default'] = '';

$array['element']['timezone'] = array();
$array['element']['timezone']['default'] = 'Europe/Paris';

$array['element']['msg_format'] = array();
$array['element']['msg_format']['default'] = '';

$array['filter'] = array();
$array['filter']['name'] = array('minlen' => 1,'maxlen' => 80,'regexp' => '/^[a-z0-9_-]+$/i');
$array['filter']['timezone'] = array('minlen' => 9,'maxlen' => 30,'regexp' => '/^[A-Z][a-z]{3,9}\/[a-zA-Z\/_-]{4,22}$/');
$array['filter']['msg_format'] = array('minlen' => 1,'maxlen' => 512,'regexp' => '/^[0-9a-z\'\/ _-]+$/i');

?>

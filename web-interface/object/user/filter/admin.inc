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

$array['admin'] = array();
$array['admin']['add'] = array();
$array['admin']['add']['login'] = array('minlen' => 4,'maxlen' => 64,'regexp' => '/^[a-zA-Z0-9-\._~\!\$&\'\(\)\*\+,;=%]+$/');
$array['admin']['add']['passwd'] = array('minlen' => 4,'maxlen' => 64,'regexp' => '/^[a-zA-Z0-9-\._~\!\$&\'\(\)\*\+,;=:%]+$/');
$array['admin']['add']['valid'] = array('bool' => true);
$array['admin']['add']['meta'] = array('key' => array('admin'));

$array['admin']['edit'] = array();
$array['admin']['edit']['login'] = array('set' => false,'chk' => 0);
$array['admin']['edit']['passwd'] = array('minlen' => 4,'maxlen' => 64,'regexp' => '/^[a-zA-Z0-9-\._~\!\$&\'\(\)\*\+,;=:%]+$/');
$array['admin']['edit']['valid'] = array('bool' => true);
$array['admin']['edit']['meta'] = array('key' => array('admin'));

?>

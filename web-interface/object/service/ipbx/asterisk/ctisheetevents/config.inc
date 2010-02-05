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

$array = array();
$array['element'] = array();

$array['element']['agentlinked'] = array();
$array['element']['agentlinked']['default'] = '';

$array['element']['agentunlinked'] = array();
$array['element']['agentunlinked']['default'] = '';

$array['element']['faxreceived'] = array();
$array['element']['faxreceived']['default'] = '';

$array['element']['incomingqueue'] = array();
$array['element']['incomingqueue']['default'] = '';

$array['element']['incominggroup'] = array();
$array['element']['incominggroup']['default'] = '';

$array['element']['incomingdid'] = array();
$array['element']['incomingdid']['default'] = '';

$array['element']['dial'] = array();
$array['element']['dial']['default'] = '';

$array['element']['link'] = array();
$array['element']['link']['default'] = '';

$array['element']['unlink'] = array();
$array['element']['unlink']['default'] = '';

$array['element']['custom'] = array();
$array['element']['custom']['default'] = '';


dwho::load_class('dwho_network');

$array['filter'] = array();
$array['filter']['agentlinked'] 	= array('set' => false, 'chk' => 2, 'regexp' => '/^[a-z0-9_\-\,]+$/i');
$array['filter']['agentunlinked'] 	= array('set' => false, 'chk' => 2, 'regexp' => '/^[a-z0-9_\-\,]+$/i');
$array['filter']['faxreceived'] 	= array('set' => false, 'chk' => 2, 'regexp' => '/^[a-z0-9_\-\,]+$/i');
$array['filter']['incomingqueue'] 	= array('set' => false, 'chk' => 2, 'regexp' => '/^[a-z0-9_\-\,]+$/i');
$array['filter']['incominggroup'] 	= array('set' => false, 'chk' => 2, 'regexp' => '/^[a-z0-9_\-\,]+$/i');
$array['filter']['incomingdid'] 	= array('set' => false, 'chk' => 2, 'regexp' => '/^[a-z0-9_\-\,]+$/i');
$array['filter']['dial'] 	= array('set' => false, 'chk' => 2, 'regexp' => '/^[a-z0-9_\-\,]+$/i');
$array['filter']['link'] 	= array('set' => false, 'chk' => 2, 'regexp' => '/^[a-z0-9_\-\,]+$/i');
$array['filter']['unlink'] 	= array('set' => false, 'chk' => 2, 'regexp' => '/^[a-z0-9_\-\,]+$/i');
$array['filter']['custom'] 	= array('set' => false, 'chk' => 2, 'regexp' => '/^.*$/i');
?>

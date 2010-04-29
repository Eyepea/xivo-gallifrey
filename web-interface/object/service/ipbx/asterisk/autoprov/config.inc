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

$array['autoprov'] = array();

$array['autoprov']['prov']['url'] = '/prov';
$array['autoprov']['prov']['method'] = 'POST';
$array['autoprov']['prov']['protocol'] = 'tcp';
$array['autoprov']['prov']['sndtimeo'] = array('sec' => 10,'usec' => 0);
$array['autoprov']['prov']['rcvtimeo'] = array('sec' => 10,'usec' => 0);

$array['autoprov']['list']['url'] = '/list';
$array['autoprov']['list']['method'] = 'GET';
$array['autoprov']['list']['protocol'] = 'tcp';
$array['autoprov']['list']['sndtimeo'] = array('sec' => 10,'usec' => 0);
$array['autoprov']['list']['rcvtimeo'] = array('sec' => 10,'usec' => 0);

$array['element'] = array();

$array['element']['modact'] = array();
$array['element']['modact']['value'] = array('guest','prov');
$array['element']['modact']['default'] = '';
$array['element']['modact']['unset'] = true;

$array['element']['from'] = array();
$array['element']['from']['default'] = 'webi';
$array['element']['from']['forcedef'] = true;

$array['element']['macaddr'] = array();
$array['element']['macaddr']['default'] = '';

$array['element']['proto'] = array();
$array['element']['proto']['value'] = array('sip');

$array['element']['vendor'] = array();
$array['element']['vendor']['default'] = '';

$array['element']['model'] = array();
$array['element']['model']['default'] = '';

$array['element']['iduserfeatures'] = array();
$array['element']['iduserfeatures']['default'] = '';

$array['filter'] = array();
$array['filter']['modact'] = array('key' => array('guest','prov'));
$array['filter']['from'] = array('set' => false);
$array['filter']['macaddr'] = array('minlen' => 11,'maxlen' => 17,'regexp' => '/^(?:[A-F0-9]{2}:){5}(?:[A-F0-9]{2})$/');
$array['filter']['vendor'] = array('maxlen' => 16,'regexp' => '/^[a-z0-9\-_]+$/');
$array['filter']['model'] = array('maxlen' => 16,'regexp' => '/^[a-z0-9\-_]+$/');
$array['filter']['proto'] = array('key' => array('sip', 'sccp'));
$array['filter']['iduserfeatures'] = array('cast' => 'ulongint');

?>

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

$array['element']['title'] = array();
$array['element']['title']['value'] = array('mr','mrs','ms');
$array['element']['title']['default'] = 'mr';

$array['element']['firstname'] = array();
$array['element']['firstname']['default'] = '';

$array['element']['lastname'] = array();
$array['element']['lastname']['default'] = '';

$array['element']['displayname'] = array();
$array['element']['displayname']['default'] = '';

$array['element']['society'] = array();
$array['element']['society']['default'] = '';

$array['element']['email'] = array();
$array['element']['email']['default'] = '';

$array['element']['url'] = array();
$array['element']['url']['default'] = '';

$array['element']['image'] = array();
$array['element']['image']['default'] = '';
$array['element']['image']['null'] = true;

$array['element']['description'] = array();
$array['element']['description']['default'] = '';


dwho::load_class('dwho_network');

$array['filter'] = array();
$array['filter']['title'] = array('key' => array('mr','mrs','ms'));
$array['filter']['firstname'] = array('set' => false,'chk' => 2,'cast' => 'sgraph','maxlen' => 128);
$array['filter']['lastname'] = array('set' => false,'chk' => 2,'cast' => 'sgraph','maxlen' => 128);
$array['filter']['displayname'] = array('cast' => 'sgraph','minlen' => 3,'maxlen' => 64);
$array['filter']['society'] = array('set' => false,'chk' => 2,'cast' => 'sgraph','maxlen' => 128);
$array['filter']['email'] = array('set' => false,'chk' => 2,'maxlen' => 255,'callback' => 'dwho_network::valid_email');
$array['filter']['url'] = array('set' => false,'chk' => 2,'cast' => 'sgraph','maxlen' => 255);
$array['filter']['description'] = array('set' => false,'chk' => 2,'maxlen' => 256000,'cast' => 'sgraph_crlf_tab','eol' => true);

?>

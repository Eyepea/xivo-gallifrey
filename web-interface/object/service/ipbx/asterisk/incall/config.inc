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

$array['element']['exten'] = array();
$array['element']['exten']['default'] = '';

$array['element']['context'] = array();
$array['element']['context']['default'] = '';

$array['element']['preprocess_subroutine'] = array();
$array['element']['preprocess_subroutine']['default'] = '';
$array['element']['preprocess_subroutine']['null'] = true;

$array['element']['description'] = array();
$array['element']['description']['default'] = '';

$array['element']['faxdetectenable'] = array();
$array['element']['faxdetectenable']['value'] = array(0,1);
$array['element']['faxdetectenable']['default'] = 0;
$array['element']['faxdetectenable']['set'] = true;

$array['element']['faxdetecttimeout'] = array();
$array['element']['faxdetecttimeout']['value'] = range(0,30);
$array['element']['faxdetecttimeout']['default'] = 4;
$array['element']['faxdetecttimeout']['setdef'] = true;

$array['element']['faxdetectemail'] = array();
$array['element']['faxdetectemail']['default'] = '';

$array['element']['commented'] = array();
$array['element']['commented']['value'] = array(0,1);
$array['element']['commented']['default'] = 0;
$array['element']['commented']['set'] = true;


dwho::load_class('dwho_network');

$array['filter'] = array();
$array['filter']['exten'] = array('maxlen' => 40,'regexp' => '/^_?[0-9NXZ\*#\-\[\]]+[\.\!]?$/');
$array['filter']['context'] = array('callback' => 'xivo_service_asterisk::chk_context');
$array['filter']['preprocess_subroutine'] = array('set' => false,'chk' => 2,'callback' => 'xivo_service_asterisk::chk_context');
$array['filter']['description'] = array('set' => false,'chk' => 2,'maxlen' => 1000,'cast' => 'sgraph_crlf_tab','eol' => true);
$array['filter']['faxdetectenable'] = array('bool' => true);
$array['filter']['faxdetecttimeout'] = array('set' => false,'chk' => 2,'cast' => 'uint','between' => array(0,30));
$array['filter']['faxdetectemail'] = array('set' => false,'chk' => 2,'maxlen' => 255,'callback' => 'dwho_network::valid_email');
$array['filter']['commented'] = array('bool' => true);

?>

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

$array['element']['name'] = array();
$array['element']['name']['default'] = '';

$array['element']['ldapserverid'] = array();
$array['element']['ldapserverid']['default'] = '';

$array['element']['user'] = array();
$array['element']['user']['default'] = '';
$array['element']['user']['null'] = true;

$array['element']['passwd'] = array();
$array['element']['passwd']['default'] = '';
$array['element']['passwd']['null'] = true;

$array['element']['basedn'] = array();
$array['element']['basedn']['default'] = '';

$array['element']['filter'] = array();
$array['element']['filter']['default'] = '';

$array['element']['attrdisplayname'] = array();
$array['element']['attrdisplayname']['default'] = '';

$array['element']['attrphonenumber'] = array();
$array['element']['attrphonenumber']['default'] = '';

$array['element']['additionaltype'] = array();
$array['element']['additionaltype']['value'] = array('office','home','mobile','fax','other','custom');
$array['element']['additionaltype']['default'] = 'office';

$array['element']['additionaltext'] = array();
$array['element']['additionaltext']['default'] = '';

$array['element']['commented'] = array();
$array['element']['commented']['value'] = array(0,1);
$array['element']['commented']['default'] = 0;
$array['element']['commented']['set'] = true;

$array['element']['description'] = array();
$array['element']['description']['default'] = '';


$array['filter'] = array();
$array['filter']['name'] = array('maxlen' => 128,'regexp' => '/^[a-z0-9_-]+$/i');
$array['filter']['ldapserverid'] = array('cast' => 'ulongint');
$array['filter']['user'] = array('set' => false,'chk' => 2,'maxlen' => 255,'cast' => 'graph','notag' => false);
$array['filter']['passwd'] = array('set' => false,'chk' => 2,'maxlen' => 255,'cast' => 'graph','notag' => false);
$array['filter']['basedn'] = array('minlen' => 1,'maxlen' => 255,'callback' => 'ldap_explode_dn','callbackarg' => 1);
$array['filter']['filter'] = array('set' => false,'chk' => 2,'maxlen' => 255,'cast' => 'sgraph','notag' => false);
$array['filter']['attrdisplayname'] = array('minlen' => 1,'maxlen' => 255,'regexp' => '/^(?:(?:[a-zA-Z0-9-]+|[0-9]+(?:\.[0-9]+)*)[,]?)+$/');
$array['filter']['attrphonenumber'] = array('minlen' => 1,'maxlen' => 255,'regexp' => '/^(?:(?:[a-zA-Z0-9-]+|[0-9]+(?:\.[0-9]+)*)[,]?)+$/');
$array['filter']['additionaltype'] = array('key' => array('office','home','mobile','fax','other','custom'));
$array['filter']['additionaltext'] = array('set' => false,'chk' => 2,'maxlen' => 16,'cast' => 'sgraph');
$array['filter']['commented'] = array('bool' => true);
$array['filter']['description'] = array('set' => false,'chk' => 2,'maxlen' => 1000,'cast' => 'sgraph_crlf_tab','eol' => true);

?>

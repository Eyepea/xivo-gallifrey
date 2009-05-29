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

$array['type'] = array();
$array['type']['extension'] = array();
$array['type']['extension']['generalfeatures'] = array();
$array['type']['extension']['generalfeatures']['parkext'] = true;
$array['type']['extension']['generalfeatures']['pickupexten'] = true;
$array['type']['extension']['featuremap'] = array();
$array['type']['extension']['featuremap']['blindxfer'] = true;
$array['type']['extension']['featuremap']['atxfer'] = true;
$array['type']['extension']['featuremap']['automon'] = true;
$array['type']['extension']['featuremap']['disconnect'] = true;
$array['type']['extension']['extenfeatures'] = array();
$array['type']['extension']['extenfeatures']['vmusermsg'] = true;
$array['type']['extension']['extenfeatures']['fwdundoall'] = true;
$array['type']['extension']['extenfeatures']['fwdundorna'] = true;
$array['type']['extension']['extenfeatures']['fwdundobusy'] = true;
$array['type']['extension']['extenfeatures']['fwdundounc'] = true;
$array['type']['extension']['extenfeatures']['recsnd'] = true;
$array['type']['extension']['extenfeatures']['phonestatus'] = true;
$array['type']['extension']['extenfeatures']['enablevm'] = true;
$array['type']['extension']['extenfeatures']['enablednd'] = true;
$array['type']['extension']['extenfeatures']['incallrec'] = true;
$array['type']['extension']['extenfeatures']['incallfilter'] = true;
$array['type']['extension']['extenfeatures']['calllistening'] = true;
$array['type']['extension']['extenfeatures']['vmuserpurge'] = true;
$array['type']['extension']['extenfeatures']['directoryaccess'] = true;

$array['element'] = array();

$array['element']['iduserfeatures'] = array();
$array['element']['iduserfeatures']['default'] = 0;

$array['element']['fknum'] = array();
$array['element']['fknum']['value'] = range(1,100);
$array['element']['fknum']['default'] = 6;

$array['element']['exten'] = array();
$array['element']['exten']['default'] = '';
$array['element']['exten']['null'] = true;

$array['element']['type'] = array();
$array['element']['type']['value'] = array('user','group','queue','meetme','extension','bosssecretary','custom');
$array['element']['type']['default'] = 'user';
$array['element']['type']['unset'] = true;

$array['element']['typeextenumbers'] = array();
$array['element']['typeextenumbers']['value'] = array('extenfeatures','featuremap','generalfeatures');
$array['element']['typeextenumbers']['default'] = '';
$array['element']['typeextenumbers']['null'] = true;

$array['element']['typevalextenumbers'] = array();
$array['element']['typevalextenumbers']['default'] = '';
$array['element']['typevalextenumbers']['null'] = true;

$array['element']['typeextenumbersright'] = array();
$array['element']['typeextenumbersright']['value'] = array('user','group','queue','meetme');
$array['element']['typeextenumbersright']['default'] = '';
$array['element']['typeextenumbersright']['null'] = true;

$array['element']['typevalextenumbersright'] = array();
$array['element']['typevalextenumbersright']['default'] = '';
$array['element']['typevalextenumbersright']['null'] = true;

$array['element']['supervision'] = array();
$array['element']['supervision']['value'] = array(0,1);
$array['element']['supervision']['default'] = 0;
$array['element']['supervision']['set'] = true;

$array['filter'] = array();
$array['filter']['iduserfeatures'] = array('cast' => 'ulongint');
$array['filter']['fknum'] = array('cast' => 'uint','between' => array(1,100));
$array['filter']['exten'] = array('set' => false,'chk' => 2,'maxlen' => 40,'regexp' => '/^[0-9\*#]+$/');
$array['filter']['typeextenumbers'] = array('set' => false,'chk' => 2,'key' => array('extenfeatures','featuremap','generalfeatures'));
$array['filter']['typevalextenumbers'] = array('set' => false,'chk' => 2,'maxlen' => 255,'cast' => 'sgraph');
$array['filter']['typeextenumbersright'] = array('set' => false,'chk' => 2,'key' => array('user','group','queue','meetme'));
$array['filter']['typevalextenumbersright'] = array('set' => false,'chk' => 2,'maxlen' => 255,'cast' => 'sgraph');
$array['filter']['supervision'] = array('bool' => true);

?>

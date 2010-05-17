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

$array['element']['name'] = array();
$array['element']['name']['default'] = '';

$array['element']['pin'] = array();
$array['element']['pin']['default'] = '';

$array['element']['label'] = array();
$array['element']['label']['default'] = '';

$array['element']['description'] = array();
$array['element']['description']['default'] = '';

$array['element']['context'] = array();
$array['element']['context']['default'] = '';

$array['element']['incominglimit'] = array();
$array['element']['incominglimit']['default'] = '';

$array['element']['transfer'] = array();
$array['element']['transfer']['default'] = '';
$array['element']['transfer']['value'] = array('on', 'off');

$array['element']['mailbox'] = array();
$array['element']['mailbox']['default'] = '';

$array['element']['vmnum'] = array();
$array['element']['vmnum']['default'] = '';

$array['element']['meetmenum'] = array();
$array['element']['meetmenum']['default'] = '';

$array['element']['cid_name'] = array();
$array['element']['cid_name']['default'] = '';

$array['element']['cid_num'] = array();
$array['element']['cid_num']['default'] = '';

$array['element']['trnsfvm'] = array();
$array['element']['trnsfvm']['default'] = '';

$array['element']['secondary_dialtone_digits'] = array();
$array['element']['secondary_dialtone_digits']['default'] = '';
$array['element']['secondary_dialtone_digits']['value'] = array(1, 2, 3, 4, 5, 6, 7, 8, 9);

$array['element']['secondary_dialtone_tone'] = array();
$array['element']['secondary_dialtone_tone']['default'] = '';

$array['element']['musicclass'] = array();
$array['element']['musicclass']['default'] = '';

$array['element']['language'] = array();
$array['element']['language']['default'] = '';

$array['element']['accountcode'] = array();
$array['element']['accountcode']['default'] = '';

$array['element']['audio_tos'] = array();
$array['element']['audio_tos']['default'] = '';

$array['element']['audio_cos'] = array();
$array['element']['audio_cos']['default'] = '';

$array['element']['video_tos'] = array();
$array['element']['video_tos']['default'] = '';

$array['element']['video_cos'] = array();
$array['element']['video_cos']['default'] = '';

$array['element']['echocancel'] = array();
$array['element']['echocancel']['default'] = '';
$array['element']['echocancel']['value'] = array('on', 'off');

$array['element']['silencesuppression'] = array();
$array['element']['silencesuppression']['default'] = '';
$array['element']['silencesuppression']['value'] = array('on', 'off');

$array['element']['callgroup'] = array();
$array['element']['callgroup']['default'] = '';

$array['element']['pickupgroup'] = array();
$array['element']['pickupgroup']['default'] = '';

$array['element']['amaflags'] = array();
$array['element']['amaflags']['default'] = '';
$array['element']['amaflags']['value'] = array('default', 'omit', 'billing', 'documentation');

$array['element']['adhocnumber'] = array();
$array['element']['adhocnumber']['default'] = '';

$array['element']['setvar'] = array();
$array['element']['setvar']['default'] = '';


//
$array['filter'] = array();
$array['filter']['name']                         = array('set' => false, 'chk'	=> 2);
$array['filter']['pin']                          = array('set' => false, 'chk'	=> 2);
$array['filter']['label']                        = array('set' => false, 'chk'	=> 2);
$array['filter']['description']                  = array('set' => false, 'chk'	=> 2);
$array['filter']['context']                      = array('set' => false, 'chk'	=> 2);
$array['filter']['incominglimit']                = array('set' => false, 'chk'	=> 2);
$array['filter']['transfer']                     = array('set' => false, 'chk'	=> 2);
$array['filter']['mailbox']                      = array('set' => false, 'chk'	=> 2);
$array['filter']['vmnum']                        = array('set' => false, 'chk'	=> 2);
$array['filter']['meetmenum']                    = array('set' => false, 'chk'	=> 2);
$array['filter']['cid_name']                     = array('set' => false, 'chk'	=> 2);
$array['filter']['cid_num']                      = array('set' => false, 'chk'	=> 2);
$array['filter']['trnsfvm']                      = array('set' => false, 'chk'	=> 2);
$array['filter']['secondary_dialtone_digits']    = array('set' => false, 'chk'	=> 2);
$array['filter']['secondary_dialtone_tone']      = array('set' => false, 'chk'	=> 2);
$array['filter']['musicclass']                   = array('set' => false, 'chk'	=> 2);
$array['filter']['language']                     = array('set' => false, 'chk'	=> 2);
$array['filter']['accountcode']                  = array('set' => false, 'chk'	=> 2);
$array['filter']['audio_tos']                    = array('set' => false, 'chk'	=> 2);
$array['filter']['audio_cos']                    = array('set' => false, 'chk'	=> 2);
$array['filter']['video_tos']                    = array('set' => false, 'chk'	=> 2);
$array['filter']['video_cos']                    = array('set' => false, 'chk'	=> 2);
$array['filter']['echocancel']                   = array('set' => false, 'chk'	=> 2);
$array['filter']['silencesuppression']           = array('set' => false, 'chk'	=> 2);
$array['filter']['callgroup']                    = array('set' => false, 'chk'	=> 2);
$array['filter']['pickupgroup']                  = array('set' => false, 'chk'	=> 2);
$array['filter']['amaflags']                     = array('set' => false, 'chk'	=> 2);
$array['filter']['adhocnumber']                  = array('set' => false, 'chk'	=> 2);
$array['filter']['setvar']                       = array('set' => false, 'chk'	=> 2);


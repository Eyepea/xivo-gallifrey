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

$array['element']['apache2'] = array();
$array['element']['apache2']['default'] = false;

$array['element']['asterisk'] = array();
$array['element']['asterisk']['default'] = false;

$array['element']['dhcp'] = array();
$array['element']['dhcp']['default'] = false;

$array['element']['monit'] = array();
$array['element']['monit']['default'] = false;

$array['element']['mysql'] = array();
$array['element']['mysql']['default'] = false;

$array['element']['ntp'] = array();
$array['element']['ntp']['default'] = false;

$array['element']['rsync'] = array();
$array['element']['rsync']['default'] = false;

$array['element']['smokeping'] = array();
$array['element']['smokeping']['default'] = false;

$array['element']['mailto'] = array();
$array['element']['mailto']['default'] = false;

$array['element']['alert_emails'] = array();
$array['element']['alert_emails']['default'] = '';

$array['element']['serial'] = array();
$array['element']['serial']['default'] = '';

$array['element']['authkeys'] = array();
$array['element']['authkeys']['default'] = '';

$array['element']['com_mode'] = array();
$array['element']['com_mode']['default'] = '';

$array['element']['user'] = array();
$array['element']['user']['default'] = '';

$array['element']['password'] = array();
$array['element']['password']['default'] = '';

$array['element']['dest_user'] = array();
$array['element']['dest_user']['default'] = '';

$array['element']['dest_password'] = array();
$array['element']['dest_password']['default'] = '';


### filters ###
$array['filter'] = array();
$array['filter']['com_mode']	        = array('chk'   => 1);

?>

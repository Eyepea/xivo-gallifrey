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

$array['ini'] = array();

$array['ini']['error'] = array();
$array['ini']['error']['level'] = 2047;
$array['ini']['error']['report_type'] = 1;
$array['ini']['error']['report_mode'] = 1;
$array['ini']['error']['report_func'] = 1;
$array['ini']['error']['email'] = 'xivo-webinterface-error@proformatique.com';
$array['ini']['error']['file'] = '/var/log/pf-xivo-web-interface/error.log';

$array['ini']['locale'] = array();
$array['ini']['locale']['language'] = 'fr';
$array['ini']['locale']['territory'] = 'FR';
$array['ini']['locale']['charset'] = 'UTF-8';

$array['ini']['session'] = array();
$array['ini']['session']['name'] = '_eid';
$array['ini']['session']['time'] = 7200;
$array['ini']['session']['path'] = '/';
$array['ini']['session']['save_path'] = '/var/lib/pf-xivo-web-interface/session';

$array['ini']['service'] = array();
$array['ini']['service']['ipbx'] = true;
$array['ini']['service']['monitoring'] = true;

?>

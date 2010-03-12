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

# XIVO_MYDOMAIN
$array['element']['mydomain'] = array();
$array['element']['mydomain']['default'] = '';

# XIVO_SMTP_ORIGIN
$array['element']['origin'] = array();
$array['element']['origin']['default'] = '';

# XIVO_SMTP_RELAYHOST
$array['element']['relayhost'] = array();
$array['element']['relayhost']['default'] = '';

# XIVO_SMTP_FALLBACK_RELAYHOST
$array['element']['fallback_relayhost'] = array();
$array['element']['fallback_relayhost']['default'] = '';

# XIVO_CANONICAL
$array['element']['canonical'] = array();
$array['element']['canonical']['default'] = '';

### filters ###
$array['filter'] = array();
$array['filter']['mydomain']	        = array('chk' => 2, 'maxlen' => 255);
$array['filter']['origin'] 		        = array('chk' => 2, 'maxlen' => 255);
$array['filter']['relayhost'] 		    = array('chk' => 2, 'maxlen' => 255);
$array['filter']['fallback_relayhost'] 	= array('chk' => 2, 'maxlen' => 255);
$array['filter']['canonical'] 		    = array('chk' => 2, 'maxlen' => 255);

?>

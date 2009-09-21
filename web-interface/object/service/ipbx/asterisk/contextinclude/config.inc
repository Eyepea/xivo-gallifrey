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

$array['element']['context'] = array();
$array['element']['context']['default'] = '';

$array['element']['include'] = array();
$array['element']['include']['default'] = '';

$array['filter'] = array();
$array['filter']['context'] = array('callback'		=> 'xivo_service_asterisk::chk_context',
				    'callbackarg'	=> array(array(
								XIVO_SRE_IPBX_AST_CONTEXT_AUTHENTICATION,
								XIVO_SRE_IPBX_AST_CONTEXT_ZONEMESSAGES)));

$array['filter']['include'] = array('callback'		=> 'xivo_service_asterisk::chk_context',
				    'callbackarg'	=> array(array(
								XIVO_SRE_IPBX_AST_CONTEXT_AUTHENTICATION,
								XIVO_SRE_IPBX_AST_CONTEXT_ZONEMESSAGES)));

?>

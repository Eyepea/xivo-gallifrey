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

$array['element'] = array();

$array['element']['protocol'] = array();
$array['element']['protocol']['value'] = array(
					XIVO_SRE_IPBX_AST_PROTO_SIP,
					XIVO_SRE_IPBX_AST_PROTO_IAX,
					XIVO_SRE_IPBX_AST_PROTO_CUSTOM);

$array['element']['protocolid'] = array();
$array['element']['protocolid']['default'] = 0;

$array['element']['registerid'] = array();
$array['element']['registerid']['default'] = 0;

$array['element']['registercommented'] = array();
$array['element']['registercommented']['value'] = array(0,1);
$array['element']['registercommented']['default'] = 0;
$array['element']['registercommented']['set'] = true;

$array['element']['description'] = array();
$array['element']['description']['default'] = '';

$array['filter'] = array();
$array['filter']['protocol'] = array('key' => array(
						XIVO_SRE_IPBX_AST_PROTO_SIP,
						XIVO_SRE_IPBX_AST_PROTO_IAX,
						XIVO_SRE_IPBX_AST_PROTO_CUSTOM));
$array['filter']['protocolid'] = array('cast' => 'ulongint');
$array['filter']['registerid'] = array('set' => false,'chk' => 2,'cast' => 'ulongint');
$array['filter']['registercommented'] = array('bool' => true);
$array['filter']['description'] = array('set' => false,'chk' => 2,'maxlen' => 1000,'cast' => 'sgraph_crlf_tab','eol' => true);

?>

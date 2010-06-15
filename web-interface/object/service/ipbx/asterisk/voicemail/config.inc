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

?>
<?php

$array = array();

$array['element'] = array();

$array['element']['context'] = array();
$array['element']['context']['default'] = '';

$array['element']['mailbox'] = array();
$array['element']['mailbox']['default'] = '';

$array['element']['password'] = array();
$array['element']['password']['default'] = '';

$array['element']['fullname'] = array();
$array['element']['fullname']['default'] = '';

$array['element']['email'] = array();
$array['element']['email']['default'] = '';
$array['element']['email']['null'] = true;

$array['element']['pager'] = array();
$array['element']['pager']['default'] = '';
$array['element']['pager']['null'] = true;

$array['element']['dialout'] = array();
$array['element']['dialout']['default'] = '';
$array['element']['dialout']['null'] = true;

$array['element']['callback'] = array();
$array['element']['callback']['default'] = '';
$array['element']['callback']['null'] = true;

$array['element']['exitcontext'] = array();
$array['element']['exitcontext']['default'] = '';
$array['element']['exitcontext']['null'] = true;

$array['element']['tz'] = array();
$array['element']['tz']['default'] = '';
$array['element']['tz']['null'] = true;

$array['element']['language'] = array();
$array['element']['language']['value'] = array('en_US', 'es_ES', 'fr_FR', 'fr_CA');
$array['element']['language']['default'] = '';
$array['element']['language']['null'] = true;

$array['element']['attach'] = array();
$array['element']['attach']['value'] = array(1,0);
$array['element']['attach']['default'] = '';
$array['element']['attach']['null'] = true;

$array['element']['saycid'] = array();
$array['element']['saycid']['value'] = array(1,0);
$array['element']['saycid']['default'] = '';
$array['element']['saycid']['null'] = true;

$array['element']['review'] = array();
$array['element']['review']['value'] = array(1,0);
$array['element']['review']['default'] = '';
$array['element']['review']['null'] = true;

$array['element']['operator'] = array();
$array['element']['operator']['value'] = array(1,0);
$array['element']['operator']['default'] = '';
$array['element']['operator']['null'] = true;

$array['element']['envelope'] = array();
$array['element']['envelope']['value'] = array(1,0);
$array['element']['envelope']['default'] = '';
$array['element']['envelope']['null'] = true;

$array['element']['sayduration'] = array();
$array['element']['sayduration']['value'] = array(1,0);
$array['element']['sayduration']['default'] = '';
$array['element']['sayduration']['null'] = true;

$array['element']['saydurationm'] = array();
$array['element']['saydurationm']['value'] = array(1,2,3,4,5,10,15,20,25,30,35,40,45,50,55,60);
$array['element']['saydurationm']['default'] = '';
$array['element']['saydurationm']['null'] = true;

$array['element']['sendvoicemail'] = array();
$array['element']['sendvoicemail']['value'] = array(1,0);
$array['element']['sendvoicemail']['default'] = '';
$array['element']['sendvoicemail']['null'] = true;

$array['element']['deletevoicemail'] = array();
$array['element']['deletevoicemail']['value'] = array(0,1);
$array['element']['deletevoicemail']['default'] = 0;
$array['element']['deletevoicemail']['set'] = true;

$array['element']['forcename'] = array();
$array['element']['forcename']['value'] = array(1,0);
$array['element']['forcename']['default'] = '';
$array['element']['forcename']['null'] = true;

$array['element']['forcegreetings'] = array();
$array['element']['forcegreetings']['value'] = array(1,0);
$array['element']['forcegreetings']['default'] = '';
$array['element']['forcegreetings']['null'] = true;

$array['element']['hidefromdir'] = array();
$array['element']['hidefromdir']['value'] = array('no','yes');
$array['element']['hidefromdir']['default'] = 'no';
$array['element']['hidefromdir']['set'] = true;

$array['element']['maxmsg'] = array();
$array['element']['maxmsg']['value'] = array(1,10,15,20,25,50,75,100,
					     125,150,175,200,300,400,500,600,700,800,900,
					     1000,2000,3000,4000,5000,6000,7000,8000,9000,9999);
$array['element']['maxmsg']['default'] = '';
$array['element']['maxmsg']['null'] = true;

$array['element']['commented'] = array();
$array['element']['commented']['value'] = array(0,1);
$array['element']['commented']['default'] = 0;
$array['element']['commented']['set'] = true;

$array['element']['option'] = array();
$array['element']['option']['value'] = array('none','add','search');
$array['element']['option']['default'] = 'none';
$array['element']['option']['unset'] = true;


dwho::load_class('dwho_network');

$array['filter'] = array();
$array['filter']['context'] = array('callback' => 'xivo_service_asterisk::chk_context',
				    'callbackarg' => array(array(XIVO_SRE_IPBX_AST_CONTEXT_ZONEMESSAGES)));
$array['filter']['mailbox'] = array('minlen' => 1,'maxlen' => 40,'regexp' => '/^[0-9#\*]+$/');
$array['filter']['password'] = array('regexp' => '/^[0-9#\*]+$/','maxlen' => 80);
$array['filter']['fullname'] = array('cast' => 'sgraph','maxlen' => 80);
$array['filter']['email'] = array('set' => false,'chk' => 2,'maxlen' => 80,'callback' => 'dwho_network::valid_email');
$array['filter']['pager'] = array('set' => false,'chk' => 2,'regexp' => '/^[0-9#\*]+$/','maxlen' => 80);
$array['filter']['dialout'] = array('set' => false,
				    'chk' => 2,
				    'callback' => 'xivo_service_asterisk::chk_context',
				    'callbackarg' => array(array(XIVO_SRE_IPBX_AST_CONTEXT_ZONEMESSAGES)));
$array['filter']['callback'] = array('set' => false,
				     'chk' => 2,
				     'callback' => 'xivo_service_asterisk::chk_context',
				     'callbackarg' => array(array(XIVO_SRE_IPBX_AST_CONTEXT_ZONEMESSAGES)));
$array['filter']['exitcontext'] = array('set' => false,
				     'chk' => 2,
				     'callback' => 'xivo_service_asterisk::chk_context',
				     'callbackarg' => array(array(XIVO_SRE_IPBX_AST_CONTEXT_ZONEMESSAGES)));
$array['filter']['tz'] = array('set' => false,'chk' => 2,'minlen' => 1,'maxlen' => 80,'regexp' => '/^[a-z0-9_-]+$/i');
$array['filter']['language'] = array('set' => false,'chk' => 2,'minlen' => 5,'maxlen' => 5,'key' => array('en_US', 'es_ES', 'fr_FR', 'fr_CA'));
$array['filter']['attach'] = array('set' => false,'chk' => 2,'bool' => true);
$array['filter']['saycid'] = array('set' => false,'chk' => 2,'bool' => true);
$array['filter']['review'] = array('set' => false,'chk' => 2,'bool' => true);
$array['filter']['operator'] = array('set' => false,'chk' => 2,'bool' => true);
$array['filter']['envelope'] = array('set' => false,'chk' => 2,'bool' => true);
$array['filter']['sayduration'] = array('set' => false,'chk' => 2,'bool' => true);
$array['filter']['saydurationm'] = array('set' => false,'chk' => 2,'key' => array(1,2,3,4,5,10,15,20,25,30,35,40,45,50,55,60));
$array['filter']['sendvoicemail'] = array('set' => false,'chk' => 2,'bool' => true);
$array['filter']['deletevoicemail'] = array('bool' => true);
$array['filter']['forcename'] = array('set' => false,'chk' => 2,'bool' => true);
$array['filter']['forcegreetings'] = array('set' => false,'chk' => 2,'bool' => true);
$array['filter']['hidefromdir'] = array('bool' => true);
$array['filter']['maxmsg'] = array('set' => false,'chk' => 2,
				   'key' => array(1,10,15,20,25,50,75,100,
						  125,150,175,200,300,400,500,600,700,800,900,
						  1000,2000,3000,4000,5000,6000,7000,8000,9000,9999));
$array['filter']['commented'] = array('bool' => true);

?>

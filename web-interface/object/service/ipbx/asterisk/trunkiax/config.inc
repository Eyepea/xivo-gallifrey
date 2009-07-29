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

$array['element']['type'] = array();
$array['element']['type']['value'] = array('user','peer','friend');
$array['element']['type']['default'] = 'peer';
$array['element']['type']['setdef'] = true;

$array['element']['username'] = array();
$array['element']['username']['default'] = '';

$array['element']['secret'] = array();
$array['element']['secret']['default'] = '';

$array['element']['dbsecret'] = array();
$array['element']['dbsecret']['unset'] = true;

$array['element']['context'] = array();
$array['element']['context']['default'] = '';
$array['element']['context']['null'] = true;

$array['element']['language'] = array();
$array['element']['language']['value'] = array('en','fr');
$array['element']['language']['default'] = '';
$array['element']['language']['null'] = true;

$array['element']['accountcode'] = array();
$array['element']['accountcode']['default'] = '';
$array['element']['accountcode']['null'] = true;

$array['element']['amaflags'] = array();
$array['element']['amaflags']['value'] = xivo_service_asterisk::get_amaflags_list();
$array['element']['amaflags']['default'] = '';
$array['element']['amaflags']['null'] = true;

$array['element']['mailbox'] = array();
$array['element']['mailbox']['unset'] = true;

$array['element']['callerid'] = array();
$array['element']['callerid']['default'] = '';
$array['element']['callerid']['null'] = true;

$array['element']['fullname'] = array();
$array['element']['fullname']['unset'] = true;

$array['element']['cid_number'] = array();
$array['element']['cid_number']['unset'] = true;

$array['element']['trunk'] = array();
$array['element']['trunk']['value'] = array(0,1);
$array['element']['trunk']['default'] = 0;
$array['element']['trunk']['set'] = true;

$array['element']['auth'] = array();
$array['element']['auth']['value'] = array('plaintext'		=> 'plaintext',
					   'md5'		=> 'md5',
					   'rsa'		=> 'rsa',
					   'plaintext,md5'	=> 'plaintext-md5',
					   'plaintext,rsa'	=> 'plaintext-rsa',
					   'md5,rsa'		=> 'md5-rsa',
					   'plaintext,md5,rsa'	=> 'plaintext-md5-rsa');
$array['element']['auth']['default'] = 'plaintext,md5';
$array['element']['auth']['setdef'] = true;

$array['element']['encryption'] = array();
$array['element']['encryption']['value'] = array('no','yes','aes128');
$array['element']['encryption']['default'] = '';
$array['element']['encryption']['null'] = true;

$array['element']['maxauthreq'] = array();
$array['element']['maxauthreq']['value'] = range(0,20);
$array['element']['maxauthreq']['default'] = '';
$array['element']['maxauthreq']['null'] = true;

$array['element']['inkeys'] = array();
$array['element']['inkeys']['default'] = '';
$array['element']['inkeys']['null'] = true;

$array['element']['outkey'] = array();
$array['element']['outkey']['default'] = '';
$array['element']['outkey']['null'] = true;

$array['element']['adsi'] = array();
$array['element']['adsi']['value'] = array(1,0);
$array['element']['adsi']['default'] = '';
$array['element']['adsi']['null'] = true;

$array['element']['transfer'] = array();
$array['element']['transfer']['value'] = array('no','yes','mediaonly');
$array['element']['transfer']['default'] = '';
$array['element']['transfer']['null'] = true;

$array['element']['codecpriority'] = array();
$array['element']['codecpriority']['value'] = array('disabled','host','caller','reqonly');
$array['element']['codecpriority']['default'] = '';
$array['element']['codecpriority']['null'] = true;

$array['element']['jitterbuffer'] = array();
$array['element']['jitterbuffer']['value'] = array(1,0);
$array['element']['jitterbuffer']['default'] = '';
$array['element']['jitterbuffer']['null'] = true;

$array['element']['forcejitterbuffer'] = array();
$array['element']['forcejitterbuffer']['value'] = array(1,0);
$array['element']['forcejitterbuffer']['default'] = '';
$array['element']['forcejitterbuffer']['null'] = true;

$array['element']['sendani'] = array();
$array['element']['sendani']['value'] = array(0,1);
$array['element']['sendani']['default'] = 0;
$array['element']['sendani']['set'] = true;

$array['element']['qualify'] = array();
$array['element']['qualify']['value'] = array('no','yes',500,1000,1500,2000,2500,3000,3500,4000);
$array['element']['qualify']['default'] = 'no';
$array['element']['qualify']['setdef'] = true;

$array['element']['qualifysmoothing'] = array();
$array['element']['qualifysmoothing']['value'] = array(0,1);
$array['element']['qualifysmoothing']['default'] = 0;
$array['element']['qualifysmoothing']['set'] = true;

$array['element']['qualifyfreqok'] = array();
$array['element']['qualifyfreqok']['value'] = array(500,1000,2000,3000,4000,5000,
						    10000,15000,20000,30000,35000,
						    40000,45000,50000,55000,
						    60000,65000,70000,75000,
						    80000,85000,90000,95000,
						    100000,105000,110000,
						    115000,120000);
$array['element']['qualifyfreqok']['default'] = 60000;
$array['element']['qualifyfreqok']['setdef'] = true;

$array['element']['qualifyfreqnotok'] = array();
$array['element']['qualifyfreqnotok']['value'] = array(500,1000,2000,3000,4000,5000,
						       10000,15000,20000,30000,35000,
						       40000,45000,50000,55000,
						       60000,65000,70000,75000,
						       80000,85000,90000,95000,
						       100000,105000,110000,
						       115000,120000);
$array['element']['qualifyfreqnotok']['default'] = 10000;
$array['element']['qualifyfreqnotok']['setdef'] = true;

$array['element']['timezone'] = array();
$array['element']['timezone']['default'] = '';
$array['element']['timezone']['null'] = true;

$array['element']['disallow'] = array();
$array['element']['disallow']['value'] = array('all');
$array['element']['disallow']['default'] = 'all';
$array['element']['disallow']['null'] = true;

$array['element']['allow'] = array();
$array['element']['allow']['value'] = xivo_service_asterisk::get_codecs_list();
$array['element']['allow']['default'] = '';
$array['element']['allow']['null'] = true;

$array['element']['mohinterpret'] = array();
$array['element']['mohinterpret']['unset'] = true;

$array['element']['mohsuggest'] = array();
$array['element']['mohsuggest']['unset'] = true;

$array['element']['deny'] = array();
$array['element']['deny']['default'] = '';
$array['element']['deny']['null'] = true;

$array['element']['permit'] = array();
$array['element']['permit']['default'] = '';
$array['element']['permit']['null'] = true;

$array['element']['defaultip'] = array();
$array['element']['defaultip']['unset'] = true;

$array['element']['sourceaddress'] = array();
$array['element']['sourceaddress']['unset'] = true;

$array['element']['setvar'] = array();
$array['element']['setvar']['default'] = '';
$array['element']['setvar']['forcedef'] = true;

$array['element']['host'] = array();
$array['element']['host']['value'] = array('dynamic');
$array['element']['host']['default'] = 'dynamic';
$array['element']['host']['setdef'] = true;

$array['element']['host-static'] = array();
$array['element']['host-static']['default'] = '';
$array['element']['host-static']['unset'] = true;

$array['element']['host-type'] = array();
$array['element']['host-type']['value'] = array('dynamic','static');
$array['element']['host-type']['unset'] = true;

$array['element']['port'] = array();
$array['element']['port']['default'] = 4569;
$array['element']['port']['null'] = true;

$array['element']['mask'] = array();
$array['element']['mask']['default'] = '';
$array['element']['mask']['null'] = true;

$array['element']['regexten'] = array();
$array['element']['regexten']['default'] = '';
$array['element']['regexten']['null'] = true;

$array['element']['peercontext'] = array();
$array['element']['peercontext']['unset'] = true;

$array['element']['ipaddr'] = array();
$array['element']['ipaddr']['unset'] = true;

$array['element']['regseconds'] = array();
$array['element']['regseconds']['unset'] = true;

$array['element']['protocol'] = array();
$array['element']['protocol']['default'] = XIVO_SRE_IPBX_AST_PROTO_IAX;
$array['element']['protocol']['forcedef'] = true;

$array['element']['category'] = array();
$array['element']['category']['default'] = 'trunk';
$array['element']['category']['forcedef'] = true;

$array['filter'] = array();
$array['filter']['name'] = array('minlen' => 1,'maxlen' => 40,'regexp' => '/^[a-z0-9_-]+$/i');
$array['filter']['type'] = array('set' => false,'chk' => 2,'key' => array('user','peer','friend'));
$array['filter']['username'] = array('set' => false,'chk' => 2,'maxlen' => 80,'cast' => 'sgraph');
$array['filter']['secret'] = array('set'	=> false,
				   'chk'	=> 2,
				   'minlen'	=> 1,
				   'maxlen'	=> 80,
				   'regexp'	=> '/^[a-zA-Z0-9-\._~\!\$&\'\(\)\*\+,;=:%]+$/');
$array['filter']['dbsecret'] = array('set'	=> false,
				     'chk'	=> 2,
				     'minlen'	=> 3,
				     'maxlen'	=> 255,
				     'regexp'	=> '/^[a-zA-Z0-9-\._~\!\$&\'\(\)\*\+,;=%]+\/[a-zA-Z0-9-\._~\!\$&\'\(\)\*\+,;=:%]+$/');
$array['filter']['context'] = array('set' => false,
				    'chk' => 2,
				    'callback' => 'xivo_service_asterisk::chk_context',
				    'callbackarg' => array(array(XIVO_SRE_IPBX_AST_CONTEXT_ZONEMESSAGES)));
$array['filter']['language'] = array('set' => false,'chk' => 2,'minlen' => 2,'maxlen' => 2,'key' => array('en','fr'));
$array['filter']['accountcode'] = array('set' => false,'chk' => 2,'regexp' => '/^[a-z0-9_-]+$/i','maxlen' => 20);
$array['filter']['amaflags'] = array('set'	=> false,
				     'chk'	=> 2,
				     'key'	=> xivo_service_asterisk::get_amaflags_list());
$array['filter']['mailbox'] = array('set' => false,'chk' => 0);
$array['filter']['callerid'] = array('set'	=> false,
				     'chk'	=> 2,
				     'maxlen'	=> 80,
				     'regexp'	=> '/^'.XIVO_SRE_IPBX_AST_REGEXP_CALLERID.'$/u',
				     'notag'	=> false);
$array['filter']['fullname'] = array('set' => false,'chk' => 0);
$array['filter']['cid_number'] = array('set' => false,'chk' => 0);
$array['filter']['trunk'] = array('bool' => true);
$array['filter']['auth'] = array('set'	=> false,
				 'chk'	=> 2,
				 'key'	=> array('plaintext',
						 'md5',
						 'rsa',
						 'plaintext,md5',
						 'plaintext,rsa',
						 'md5,rsa',
						 'plaintext,md5,rsa'));
$array['filter']['encryption'] = array('set' => false,'chk' => 2,'key' => array('no','yes','aes128'));
$array['filter']['maxauthreq'] = array('set' => false,'chk' => 2,'cast' => 'uint','between' => array(0,20));
$array['filter']['inkeys'] = array('set' => false,'chk' => 2,'maxlen' => 80,'cast' => 'sgraph');
$array['filter']['outkey'] = array('set' => false,'chk' => 2,'maxlen' => 80,'cast' => 'sgraph');
$array['filter']['adsi'] = array('set' => false,'chk' => 2,'bool' => true);
$array['filter']['transfer'] = array('set' => false,'chk' => 2,'key' => array('no','yes','mediaonly'));
$array['filter']['codecpriority'] = array('set' => false,'chk' => 2,'key' => array('disabled','host','caller','reqonly'));
$array['filter']['jitterbuffer'] = array('set' => false,'chk' => 2,'bool' => true);
$array['filter']['forcejitterbuffer'] = array('set' => false,'chk' => 2,'bool' => true);
$array['filter']['sendani'] = array('bool' => true);
$array['filter']['qualify'] = array('set' => false,'chk' => 2,'key' => array('no','yes',500,1000,1500,2000,2500,3000,3500,4000));
$array['filter']['qualifysmoothing'] = array('bool' => true);
$array['filter']['qualifyfreqok'] = array('set'	=> false,
					  'chk'	=> 2,
					  'key'	=> array(500,1000,2000,3000,4000,5000,
							 10000,15000,20000,30000,35000,
							 40000,45000,50000,55000,60000,
							 65000,70000,75000,80000,85000,
							 90000,95000,100000,105000,110000,
							 115000,120000));
$array['filter']['qualifyfreqnotok'] = array('set'	=> false,
					     'chk'	=> 2,
					     'key'	=> array(500,1000,2000,3000,4000,5000,
								 10000,15000,20000,30000,35000,
								 40000,45000,50000,55000,60000,
								 65000,70000,75000,80000,85000,
								 90000,95000,100000,105000,110000,
								 115000,120000));
$array['filter']['timezone'] = array('set'	=> false,
				     'chk'	=> 2,
				     'minlen'	=> 9,
				     'maxlen'	=> 30,
				     'regexp'	=> '/^[A-Z][a-z]{3,9}\/[a-zA-Z\/_-]{4,22}$/');
$array['filter']['disallow'] = array('set' => false,'chk' => 2,'key' => array('all'));
$array['filter']['allow'] = array('set' => false,'chk' => 2,'uvalue' => xivo_service_asterisk::get_codecs_list());
$array['filter']['mohinterpret'] = array('set' => false,'chk' => 0);
$array['filter']['mohsuggest'] = array('set' => false,'chk' => 0);
$array['filter']['deny'] = array('set'			=> false,
				 'chk'			=> 2,
				 'callback'		=> 'xivo_filter::chk_ipv4_subnet',
				 'callbackarg'	=> array(null,XIVO_IPV4_NETMASK_DOTDEC));
$array['filter']['permit'] = array('set'		=> false,
				   'chk'		=> 2,
				   'callback'		=> 'xivo_filter::chk_ipv4_subnet',
				   'callbackarg'	=> array(null,XIVO_IPV4_NETMASK_DOTDEC));
$array['filter']['defaultip'] = array('set' => false,'chk' => 0);
$array['filter']['sourceaddress'] = array('set' => false,'chk' => 0);
$array['filter']['setvar'] = array('set' => false,'chk' => 0);
$array['filter']['host'] = array('set'		=> false,
				 'chk'		=> 2,
				 'key'		=> array('dynamic','static'),
				 'strict'	=> false,
				 'callback'	=> 'xivo_filter::chk_host_ipv4_strict');
$array['filter']['port'] = array('set' => false,'chk' => 2,'cast' => 'uint','between' => array(1,65535));
$array['filter']['mask'] = array('set' => false,'chk' => 2,'callback' => 'xivo_filter::chk_ipv4_netmask_dotdec');
$array['filter']['regexten'] = array('set' => false,'chk' => 0);
$array['filter']['peercontext'] = array('set' => false,'chk' => 0);
$array['filter']['ipaddr'] = array('set' => false,'chk' => 0);
$array['filter']['regseconds'] = array('set' => false,'chk' => 0);
$array['filter']['protocol'] = array('set' => false);
$array['filter']['category'] = array('set' => false);

?>

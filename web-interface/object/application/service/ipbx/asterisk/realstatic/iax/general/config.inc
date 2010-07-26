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

$array = array();
$array['element'] = array();

$array['element']['bindport'] = array();
$array['element']['bindport']['default'] = 4569;
$array['element']['bindport']['setdef'] = true;

$array['element']['bindaddr'] = array();
$array['element']['bindaddr']['default'] = '0.0.0.0';
$array['element']['bindaddr']['setdef'] = true;

$array['element']['iaxthreadcount'] = array();
$array['element']['iaxthreadcount']['value'] = range(1,256);
$array['element']['iaxthreadcount']['default'] = 10;

$array['element']['iaxmaxthreadcount'] = array();
$array['element']['iaxmaxthreadcount']['value'] = range(0,256);
$array['element']['iaxmaxthreadcount']['default'] = 100;

$array['element']['iaxcompat'] = array();
$array['element']['iaxcompat']['value'] = array('no','yes');
$array['element']['iaxcompat']['default'] = 'no';
$array['element']['iaxcompat']['set'] = true;

$array['element']['authdebug'] = array();
$array['element']['authdebug']['value'] = array('no','yes');
$array['element']['authdebug']['default'] = 'yes';
$array['element']['authdebug']['set'] = true;

$array['element']['delayreject'] = array();
$array['element']['delayreject']['value'] = array('no','yes');
$array['element']['delayreject']['default'] = 'no';
$array['element']['delayreject']['set'] = true;

$array['element']['trunkfreq'] = array();
$array['element']['trunkfreq']['value']['value'] = array(10,15,20,25,30,35,40,45,50,55,60,65,70,75,80,85,90,95,100);
$array['element']['trunkfreq']['default'] = 20;

$array['element']['trunktimestamps'] = array();
$array['element']['trunktimestamps']['value'] = array('no','yes');
$array['element']['trunktimestamps']['default'] = 'yes';
$array['element']['trunktimestamps']['set'] = true;

$array['element']['regcontext'] = array();
$array['element']['regcontext']['default'] = '';
$array['element']['regcontext']['null'] = true;

$array['element']['minregexpire'] = array();
$array['element']['minregexpire']['value'] = array(0,10,20,30,40,50,60,70,80,90,100,110,120);
$array['element']['minregexpire']['default'] = 60;

$array['element']['maxregexpire'] = array();
$array['element']['maxregexpire']['value'] = array(0,10,20,30,40,50,60,70,80,90,100,110,120);
$array['element']['maxregexpire']['default'] = 60;

$array['element']['bandwidth'] = array();
$array['element']['bandwidth']['value'] = array('low','medium','high');
$array['element']['bandwidth']['default'] = 'high';

$array['element']['tos'] = array();
$array['element']['tos']['value'] = array('CS0','CS1','CS2','CS3','CS4','CS5','CS6','CS7',
					  'AF11','AF12','AF13','AF21','AF22','AF23',
					  'AF31', 'AF32','AF33','AF41','AF42','AF43','EF');
$array['element']['tos']['default'] = '';
$array['element']['tos']['null'] = true;

$array['element']['jitterbuffer'] = array();
$array['element']['jitterbuffer']['value'] = array('no','yes');
$array['element']['jitterbuffer']['default'] = 'no';
$array['element']['jitterbuffer']['set'] = true;

$array['element']['forcejitterbuffer'] = array();
$array['element']['forcejitterbuffer']['value'] = array('no','yes');
$array['element']['forcejitterbuffer']['default'] = 'no';
$array['element']['forcejitterbuffer']['set'] = true;

$array['element']['maxjitterbuffer'] = array();
$array['element']['maxjitterbuffer']['value'] = array(0,250,500,750,1000,1250,1500,1750,2000);
$array['element']['maxjitterbuffer']['default'] = 1000;

$array['element']['maxjitterinterps'] = array();
$array['element']['maxjitterinterps']['value'] = range(0,20);
$array['element']['maxjitterinterps']['default'] = 10;

$array['element']['resyncthreshold'] = array();
$array['element']['resyncthreshold']['value'] = array(-1,0,250,500,750,1000,1250,1500,1750,2000);
$array['element']['resyncthreshold']['default'] = 1000;

$array['element']['accountcode'] = array();
$array['element']['accountcode']['default'] = '';
$array['element']['accountcode']['null'] = true;

$array['element']['amaflags'] = array();
$array['element']['amaflags']['value'] = xivo_service_asterisk::get_amaflags_list();
$array['element']['amaflags']['default'] = XIVO_SRE_IPBX_AST_CDR_AMAFLAGS_META_DEFAULT;

$array['element']['adsi'] = array();
$array['element']['adsi']['value'] = array('no','yes');
$array['element']['adsi']['default'] = 'no';
$array['element']['adsi']['set'] = true;

$array['element']['transfer'] = array();
$array['element']['transfer']['value'] = array('no','yes','mediaonly');
$array['element']['transfer']['default'] = 'yes';

$array['element']['language'] = array();
$array['element']['language']['value'] = array('de_DE', 'en_US', 'es_ES', 'fr_FR', 'fr_CA');
$array['element']['language']['default'] = 'fr_FR';

$array['element']['mohinterpret'] = array();
$array['element']['mohinterpret']['default'] = XIVO_SRE_IPBX_AST_MOH_DEFAULT;
$array['element']['mohinterpret']['setdef'] = true;

$array['element']['mohsuggest'] = array();
$array['element']['mohsuggest']['default'] = '';
$array['element']['mohsuggest']['null'] = true;

$array['element']['encryption'] = array();
$array['element']['encryption']['value'] = array('no','yes','aes128');
$array['element']['encryption']['default'] = 'no';

$array['element']['maxauthreq'] = array();
$array['element']['maxauthreq']['value'] = range(0,20);
$array['element']['maxauthreq']['default'] = 3;

$array['element']['codecpriority'] = array();
$array['element']['codecpriority']['value'] = array('disabled','host','caller','reqonly');
$array['element']['codecpriority']['default'] = 'host';

$array['element']['disallow'] = array();
$array['element']['disallow']['value'] = array('all');
$array['element']['disallow']['default'] = 'all';
$array['element']['disallow']['unset'] = true;

$array['element']['allow'] = array();
$array['element']['allow']['value'] = xivo_service_asterisk::get_codecs_list();
$array['element']['allow']['default'] = '';
$array['element']['allow']['null'] = true;

$array['element']['rtcachefriends'] = array();
$array['element']['rtcachefriends']['value'] = array('no','yes');
$array['element']['rtcachefriends']['default'] = 'yes';
$array['element']['rtcachefriends']['set'] = true;

$array['element']['rtupdate'] = array();
$array['element']['rtupdate']['value'] = array('no','yes');
$array['element']['rtupdate']['default'] = 'yes';
$array['element']['rtupdate']['set'] = true;

$array['element']['rtignoreregexpire'] = array();
$array['element']['rtignoreregexpire']['value'] = array('no','yes');
$array['element']['rtignoreregexpire']['default'] = 'no';
$array['element']['rtignoreregexpire']['set'] = true;

$array['element']['rtautoclear'] = array();
$array['element']['rtautoclear']['value'] = array('no','yes',1,10,20,30,40,50,60,70,80,90,
						  100,110,120,130,140,150,160,170,180,190,
						  200,210,220,230,240);
$array['element']['rtautoclear']['default'] = 'no';

$array['element']['pingtime'] = array();
$array['element']['pingtime']['value'] = array(0,5,10,15,20,25,30,35,40,45,50,55,60);
$array['element']['pingtime']['default'] = 20;

$array['element']['lagrqtime'] = array();
$array['element']['lagrqtime']['value'] = array(0,5,10,15,20,25,30,35,40,45,50,55,60);
$array['element']['lagrqtime']['default'] = 10;

$array['element']['nochecksums'] = array();
$array['element']['nochecksums']['value'] = array('no','yes');
$array['element']['nochecksums']['default'] = 'no';
$array['element']['nochecksums']['set'] = true;

$array['element']['autokill'] = array();
$array['element']['autokill']['value'] = array('no','yes',500,1000,1500,2000,2500,3000,3500,4000);
$array['element']['autokill']['default'] = 'yes';

$array['element']['calltokenoptional'] = array();
$array['element']['calltokenoptional']['default'] = '';
$array['element']['calltokenoptional']['null'] = true;


dwho::load_class('dwho_network');

$array['filter'] = array();
$array['filter']['bindport'] = array('set' => false,'chk' => 2,'cast' => 'uint','between' => array(1,65535));
$array['filter']['bindaddr'] = array('set' => false,'chk' => 2,'callback' => 'dwho_network::valid_host',
				     'callbackarg'	=> array((DWHO_NETWORK_HOST_IPV4_DOTDEC |DWHO_NETWORK_HOST_HOSTNAME_ALL)),
				     'maxlen' => 64);
$array['filter']['iaxthreadcount'] = array('cast' => 'uint','between' => array(1,256));
$array['filter']['iaxmaxthreadcount'] = array('cast' => 'uint','between' => array(0,256));
$array['filter']['iaxcompat'] = array('bool' => true);
$array['filter']['authdebug'] = array('bool' => true);
$array['filter']['delayreject'] = array('bool' => true);
$array['filter']['trunkfreq'] = array('cast' => 'uint','between' => array(10,100,5));
$array['filter']['trunktimestamps'] = array('bool' => true);
$array['filter']['regcontext'] = array('set' => false,'chk' => 2,'callback' => 'xivo_service_asterisk::chk_context');
$array['filter']['minregexpire'] = array('cast' => 'uint','between' => array(0,120,10));
$array['filter']['maxregexpire'] = array('cast' => 'uint','between' => array(0,120,10));
$array['filter']['bandwidth'] = array('key' => array('low','medium','high'));
$array['filter']['tos'] = array('set' => false,'chk' => 2,'regexp' => '/^(CS[0-7]|AF[1-4][1-3]|EF)$/');
$array['filter']['jitterbuffer'] = array('bool' => true);
$array['filter']['forcejitterbuffer'] = array('bool' => true);
$array['filter']['maxjitterbuffer'] = array('cast' => 'uint','between' => array(0,2000,250));
$array['filter']['maxjitterinterps'] = array('cast' => 'uint','between' => array(0,20));
$array['filter']['resyncthreshold'] = array('key' => array(-1,0,250,500,750,1000,1250,1500,1750,2000));
$array['filter']['accountcode'] = array('set' => false,'chk' => 2,'maxlen' => 20,'regexp' => '/^[a-z0-9_-]+$/i');
$array['filter']['amaflags'] = array('key' => xivo_service_asterisk::get_amaflags_list());
$array['filter']['adsi'] = array('bool' => true);
$array['filter']['transfer'] = array('key' => array('no','yes','mediaonly'));
$array['filter']['language'] = array('key' => array('de_DE', 'en_US', 'es_ES', 'fr_FR', 'fr_CA'));
$array['filter']['mohinterpret'] = array('set' => false,'chk' => 2,'maxlen' => 20);
$array['filter']['mohsuggest'] = array('set' => false,'chk' => 2,'maxlen' => 20);
$array['filter']['encryption'] = array('key' => array('no','yes','aes128'));
$array['filter']['maxauthreq'] = array('cast' => 'uint','between' => array(0,20));
$array['filter']['codecpriority'] = array('key' => array('disabled','host','caller','reqonly'));
$array['filter']['disallow'] = array('set' => false,'chk' => 2,'key' => array('all'));
$array['filter']['allow'] = array('set' => false,'chk' => 2,'uvalue' => xivo_service_asterisk::get_codecs_list());
$array['filter']['rtcachefriends'] = array('bool' => true);
$array['filter']['rtupdate'] = array('bool' => true);
$array['filter']['rtignoreregexpire'] = array('bool' => true);
$array['filter']['rtautoclear'] = array('key' => array('yes','no',1,10,20,30,40,50,60,70,80,90,
						       100,110,120,130,140,150,160,170,180,
						       190,200,210,220,230,240));
$array['filter']['pingtime'] = array('cast' => 'uint','between' => array(0,60,5));
$array['filter']['lagrqtime'] = array('cast' => 'uint','between' => array(0,60,5));
$array['filter']['nochecksums'] = array('bool' => true);
$array['filter']['autokill'] = array('key' => array('no','yes',500,1000,1500,2000,2500,3000,3500,4000));
$array['filter']['calltokenoptional'] = array('set' => false,'chk' => 2);

?>

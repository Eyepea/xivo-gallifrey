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

$array['element']['maxmsg'] = array();
$array['element']['maxmsg']['value'] = array(1,10,15,20,25,50,75,100,
					     125,150,175,200,300,400,500,600,700,800,900,
					     1000,2000,3000,4000,5000,6000,7000,8000,9000,9999);
$array['element']['maxmsg']['default'] = 100;

$array['element']['silencethreshold'] = array();
$array['element']['silencethreshold']['value'] = array(0,2,4,8,16,32,64,128,256,512);
$array['element']['silencethreshold']['default'] = 256;

$array['element']['minmessage'] = array();
$array['element']['minmessage']['value'] = array(0,1,2,3,4,5,6,7,8,9,10,20,30,40,50,60,70,80,90,
						 100,110,120,130,140,150,160,170,180,190,
						 200,210,220,230,240,250,260,270,280,290,
						 300,310,320,330,340,350,360);
$array['element']['minmessage']['default'] = 2;

$array['element']['maxmessage'] = array();
$array['element']['maxmessage']['value'] = array(0,1,2,3,4,5,6,7,8,9,10,20,30,40,50,60,70,80,90,
						 100,110,120,130,140,150,160,170,180,190,
						 200,210,220,230,240,250,260,270,280,290,
						 300,310,320,330,340,350,360);
$array['element']['maxmessage']['default'] = 0;

$array['element']['maxsilence'] = array();
$array['element']['maxsilence']['value'] = range(1,30);
$array['element']['maxsilence']['default'] = 15;

$array['element']['review'] = array();
$array['element']['review']['value'] = array('no','yes');
$array['element']['review']['default'] = 'no';
$array['element']['review']['set'] = true;

$array['element']['operator'] = array();
$array['element']['operator']['value'] = array('no','yes');
$array['element']['operator']['default'] = 'yes';
$array['element']['operator']['set'] = true;

$array['element']['format'] = array();
$array['element']['format']['value'] = array(
					XIVO_SRE_IPBX_AST_FORMAT_AUDIO_WAV,
					XIVO_SRE_IPBX_AST_FORMAT_AUDIO_WAV49,
					XIVO_SRE_IPBX_AST_FORMAT_AUDIO_GSM,
					XIVO_SRE_IPBX_AST_FORMAT_AUDIO_OGG,
					XIVO_SRE_IPBX_AST_FORMAT_AUDIO_ULAW,
					XIVO_SRE_IPBX_AST_FORMAT_AUDIO_ALAW,
					XIVO_SRE_IPBX_AST_FORMAT_AUDIO_AU,
					XIVO_SRE_IPBX_AST_FORMAT_AUDIO_VOX,
					XIVO_SRE_IPBX_AST_FORMAT_AUDIO_SLIN);
$array['element']['format']['default'] = '';
$array['element']['format']['null'] = true;

$array['element']['maxlogins'] = array();
$array['element']['maxlogins']['value'] = range(1,20);
$array['element']['maxlogins']['default'] = 3;

$array['element']['envelope'] = array();
$array['element']['envelope']['value'] = array('no','yes');
$array['element']['envelope']['default'] = 'yes';
$array['element']['envelope']['set'] = true;

$array['element']['saycid'] = array();
$array['element']['saycid']['value'] = array('no','yes');
$array['element']['saycid']['default'] = 'no';
$array['element']['saycid']['set'] = true;

$array['element']['cidinternalcontexts'] = array();
$array['element']['cidinternalcontexts']['default'] = '';
$array['element']['cidinternalcontexts']['null'] = true;

$array['element']['sayduration'] = array();
$array['element']['sayduration']['value'] = array('no','yes');
$array['element']['sayduration']['default'] = 'yes';
$array['element']['sayduration']['set'] = true;

$array['element']['saydurationm'] = array();
$array['element']['saydurationm']['value'] = array(1,2,3,4,5,10,15,20,25,30,35,40,45,50,55,60);
$array['element']['saydurationm']['default'] = 2;

$array['element']['forcename'] = array();
$array['element']['forcename']['value'] = array('no','yes');
$array['element']['forcename']['default'] = 'no';
$array['element']['forcename']['set'] = true;

$array['element']['forcegreetings'] = array();
$array['element']['forcegreetings']['value'] = array('no','yes');
$array['element']['forcegreetings']['default'] = 'no';
$array['element']['forcegreetings']['set'] = true;

$array['element']['tempgreetwarn'] = array();
$array['element']['tempgreetwarn']['value'] = array('no','yes');
$array['element']['tempgreetwarn']['default'] = 'yes';
$array['element']['tempgreetwarn']['set'] = true;

$array['element']['maxgreet'] = array();
$array['element']['maxgreet']['value'] = array(0,1,2,3,4,5,6,7,8,9,10,20,30,40,50,60,70,80,90,
					       100,110,120,130,140,150,160,170,180,190,
					       200,210,220,230,240,250,260,270,280,290,
					       300,310,320,330,340,350,360);
$array['element']['maxgreet']['default'] = 0;

$array['element']['skipms'] = array();
$array['element']['skipms']['value'] = array(1000,2000,3000,4000,5000,6000,7000,8000,9000,
					     10000,15000,20000,25000,30000,35000,
					     40000,45000,50000,55000,60000);
$array['element']['skipms']['default'] = 3000;

$array['element']['sendvoicemail'] = array();
$array['element']['sendvoicemail']['value'] = array('no','yes');
$array['element']['sendvoicemail']['default'] = 'no';
$array['element']['sendvoicemail']['set'] = true;

$array['element']['usedirectory'] = array();
$array['element']['usedirectory']['value'] = array('no','yes');
$array['element']['usedirectory']['default'] = 'yes';
$array['element']['usedirectory']['set'] = true;

$array['element']['nextaftercmd'] = array();
$array['element']['nextaftercmd']['value'] = array('no','yes');
$array['element']['nextaftercmd']['default'] = 'yes';
$array['element']['nextaftercmd']['set'] = true;

$array['element']['dialout'] = array();
$array['element']['dialout']['default'] = '';
$array['element']['dialout']['null'] = true;

$array['element']['callback'] = array();
$array['element']['callback']['default'] = '';
$array['element']['callback']['null'] = true;

$array['element']['exitcontext'] = array();
$array['element']['exitcontext']['default'] = '';
$array['element']['exitcontext']['null'] = true;

$array['element']['attach'] = array();
$array['element']['attach']['value'] = array('no','yes');
$array['element']['attach']['default'] = 'yes';
$array['element']['attach']['set'] = true;

$array['element']['volgain'] = array();
$array['element']['volgain']['value'] = array(-5,'-4.5',-4,'-3.5',-3,'-2.5',-2,-1.5,1,'-0.5',
					      0,'0.5',1,'1.5',2,'2.5',3,'3.5',4,'4.5',5);
$array['element']['volgain']['default'] = 0;

$array['element']['mailcmd'] = array();
$array['element']['mailcmd']['default'] = '/usr/sbin/sendmail -t';
$array['element']['mailcmd']['forcedef'] = true;

$array['element']['serveremail'] = array();
$array['element']['serveremail']['default'] = 'xivo';
$array['element']['serveremail']['setdef'] = true;

$array['element']['charset'] = array();
$array['element']['charset']['default'] = 'UTF-8';
$array['element']['charset']['forcedef'] = true;

$array['element']['fromstring'] = array();
$array['element']['fromstring']['default'] = 'XIVO PBX';
$array['element']['fromstring']['setdef'] = true;

$array['element']['emaildateformat'] = array();
$array['element']['emaildateformat']['default'] = '%A, %B %d, %Y at %r';
$array['element']['emaildateformat']['setdef'] = true;

$array['element']['emaildatelocale'] = array();
$array['element']['emaildatelocale']['default'] = 'en_US.UTF-8';
$array['element']['emaildatelocale']['null'] = true;

$array['element']['pbxskip'] = array();
$array['element']['pbxskip']['value'] = array('no','yes');
$array['element']['pbxskip']['default'] = 'no';
$array['element']['pbxskip']['set'] = true;

$array['element']['emailsubject'] = array();
$array['element']['emailsubject']['default'] = '';
$array['element']['emailsubject']['null'] = true;

$array['element']['emailbody'] = array();
$array['element']['emailbody']['default'] = '';
$array['element']['emailbody']['null'] = true;

$array['element']['pagerfromstring'] = array();
$array['element']['pagerfromstring']['default'] = 'XIVO PBX';
$array['element']['pagerfromstring']['setdef'] = true;

$array['element']['pagersubject'] = array();
$array['element']['pagersubject']['default'] = '';
$array['element']['pagersubject']['null'] = true;

$array['element']['pagerbody'] = array();
$array['element']['pagerbody']['default'] = '';
$array['element']['pagerbody']['null'] = true;

$array['element']['adsifdn'] = array();
$array['element']['adsifdn']['default'] = '0000000F';
$array['element']['adsifdn']['setdef'] = true;

$array['element']['adsisec'] = array();
$array['element']['adsisec']['default'] = '9BDBF7AC';
$array['element']['adsisec']['setdef'] = true;

$array['element']['adsiver'] = array();
$array['element']['adsiver']['default'] = 1;
$array['element']['adsiver']['setdef'] = true;

$array['element']['searchcontexts'] = array();
$array['element']['searchcontexts']['value'] = array('no','yes');
$array['element']['searchcontexts']['default'] = 'no';
$array['element']['searchcontexts']['set'] = true;

$array['element']['externpass'] = array();
$array['element']['externpass']['default'] = '/usr/share/asterisk/bin/change-pass-vm';
$array['element']['externpass']['forcedef'] = true;

$array['element']['externnotify'] = array();
$array['element']['externnotify']['default'] = '';
$array['element']['externnotify']['null'] = true;

$array['element']['smdiport'] = array();
$array['element']['smdiport']['default'] = '';
$array['element']['smdiport']['null'] = true;

$array['element']['odbcstorage'] = array();
$array['element']['odbcstorage']['default'] = '';
$array['element']['odbcstorage']['null'] = true;

$array['element']['odbctable'] = array();
$array['element']['odbctable']['default'] = '';
$array['element']['odbctable']['null'] = true;

$array['filter'] = array();
$array['filter']['maxmsg'] = array('key' => array(1,10,15,20,25,50,75,100,
						  125,150,175,200,300,400,500,600,700,800,900,
						  1000,2000,3000,4000,5000,6000,7000,8000,9000,9999));
$array['filter']['silencethreshold'] = array('key' => array(0,2,4,8,16,32,64,128,256,512));
$array['filter']['minmessage'] = array('key' => array(0,1,2,3,4,5,6,7,8,9,10,20,30,40,50,60,70,80,90,
						      100,110,120,130,140,150,160,170,180,190,
						      200,210,220,230,240,250,260,270,280,290,
						      300,310,320,330,340,350,360));
$array['filter']['maxmessage'] = array('key' => array(0,1,2,3,4,5,6,7,8,9,10,20,30,40,50,60,70,80,90,
						      100,110,120,130,140,150,160,170,180,190,
						      200,210,220,230,240,250,260,270,280,290,
						      300,310,320,330,340,350,360));
$array['filter']['maxsilence'] = array('cast' => 'uint','between' => array(1,30));
$array['filter']['review'] = array('bool' => true);
$array['filter']['operator'] = array('bool' => true);
$array['filter']['format'] = array('set' => false,'chk' => 2,'uvalue' => array(
					XIVO_SRE_IPBX_AST_FORMAT_AUDIO_WAV,
					XIVO_SRE_IPBX_AST_FORMAT_AUDIO_WAV49,
					XIVO_SRE_IPBX_AST_FORMAT_AUDIO_GSM,
					XIVO_SRE_IPBX_AST_FORMAT_AUDIO_OGG,
					XIVO_SRE_IPBX_AST_FORMAT_AUDIO_ULAW,
					XIVO_SRE_IPBX_AST_FORMAT_AUDIO_ALAW,
					XIVO_SRE_IPBX_AST_FORMAT_AUDIO_AU,
					XIVO_SRE_IPBX_AST_FORMAT_AUDIO_VOX,
					XIVO_SRE_IPBX_AST_FORMAT_AUDIO_SLIN));
$array['filter']['maxlogins'] = array('cast' => 'uint','between' => array(1,20));
$array['filter']['envelope'] = array('bool' => true);
$array['filter']['saycid'] = array('bool' => true);
$array['filter']['cidinternalcontexts'] = array('set' => false,'chk' => 2,'callback' => 'xivo_service_asterisk::chk_context');
$array['filter']['sayduration'] = array('bool' => true);
$array['filter']['saydurationm'] = array('key' => array(1,2,3,4,5,10,15,20,25,30,35,40,45,50,55,60));
$array['filter']['forcename'] = array('bool' => true);
$array['filter']['forcegreetings'] = array('bool' => true);
$array['filter']['tempgreetwarn'] = array('bool' => true);
$array['filter']['maxgreet'] = array('key' => array(0,1,2,3,4,5,6,7,8,9,10,20,30,40,50,60,70,80,90,
						    100,110,120,130,140,150,160,170,180,190,
						    200,210,220,230,240,250,260,270,280,290,
						    300,310,320,330,340,350,360));
$array['filter']['skipms'] = array('key' => array(1000,2000,3000,4000,5000,6000,7000,8000,9000,
						  10000,15000,20000,25000,30000,35000,40000,
						  45000,50000,55000,60000));
$array['filter']['sendvoicemail'] = array('bool' => true);
$array['filter']['usedirectory'] = array('bool' => true);
$array['filter']['nextaftercmd'] = array('bool' => true);
$array['filter']['dialout'] = array('set' => false,'chk' => 2,'callback' => 'xivo_service_asterisk::chk_context');
$array['filter']['callback'] = array('set' => false,'chk' => 2,'callback' => 'xivo_service_asterisk::chk_context');
$array['filter']['exitcontext'] = array('set' => false,'chk' => 2,'callback' => 'xivo_service_asterisk::chk_context');
$array['filter']['attach'] = array('bool' => true);
$array['filter']['volgain'] = array('cast' => 'float','between' => array(-5,5));
//$array['filter']['mailcmd'] = array('set' => false,'chk' => 2,'cast' => 'sgraph','maxlen' => 160);
$array['filter']['mailcmd'] = array('set' => false);
$array['filter']['serveremail'] = array('set' => false,'chk' => 2,'regexp' => '/^[a-z0-9@\-_\.]+$/i','maxlen' => 80);
$array['filter']['charset'] = array('set' => false);
$array['filter']['fromstring'] = array('set' => false,'chk' => 2,'regexp' => '/^[a-z0-9\-_\. ]+$/i','maxlen' => 100);
$array['filter']['emaildateformat'] = array('set' => false,'chk' => 2,'cast' => 'sgraph','maxlen' => 32);
$array['filter']['emaildatelocale'] = array('set' => false,'chk' => 2,'regexp' => '/^[a-z0-9@\-_\.]+$/i','maxlen' => 32);
$array['filter']['pbxskip'] = array('bool' => true);
$array['filter']['emailsubject'] = array('set' => false,'chk' => 2,'maxlen' => 255,'cast' => 'sgraph');
$array['filter']['emailbody'] = array('set' => false,'chk' => 2,'maxlen' => 1000,'cast' => 'sgraph_crlf_tab','eol' => true);
$array['filter']['pagerfromstring'] = array('set' => false,'chk' => 2,'regexp' => '/^[a-z0-9\-_\. ]+$/i','maxlen' => 100);
$array['filter']['pagersubject'] = array('set' => false,'chk' => 2,'maxlen' => 255,'cast' => 'sgraph');
$array['filter']['pagerbody'] = array('set' => false,'chk' => 2,'maxlen' => 512,'cast' => 'sgraph_crlf_tab','eol' => true);
$array['filter']['adsifdn'] = array('set' => false,'chk' => 2,'cast' => 'xdigit','maxlen' => 8);
$array['filter']['adsisec'] = array('set' => false,'chk' => 2,'cast' => 'xdigit','maxlen' => 8);
$array['filter']['adsiver'] = array('set' => false,'chk' => 2,'cast' => 'int','between' => array(-1,255));
$array['filter']['searchcontexts'] = array('bool' => true);
//$array['filter']['externpass'] = array('set' => false,'chk' => 2,'cast' => 'sgraph','maxlen' => 128);
$array['filter']['externpass'] = array('set' => false);
$array['filter']['externnotify'] = array('set' => false,'chk' => 2,'cast' => 'sgraph','maxlen' => 160);
$array['filter']['smdiport'] = array('set' => false,'chk' => 2,'cast' => 'sgraph','maxlen' => 255);
$array['filter']['odbcstorage'] = array('set' => false,'chk' => 2,'cast' => 'sgraph','maxlen' => 80);
$array['filter']['odbctable'] = array('set' => false,'chk' => 2,'cast' => 'sgraph','maxlen' => 80);

?>

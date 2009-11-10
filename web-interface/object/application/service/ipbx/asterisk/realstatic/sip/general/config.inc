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

$array['element']['bindport'] = array();
$array['element']['bindport']['default'] = 5060;
$array['element']['bindport']['setdef'] = true;

$array['element']['bindaddr'] = array();
$array['element']['bindaddr']['default'] = '0.0.0.0';
$array['element']['bindaddr']['setdef'] = true;

$array['element']['videosupport'] = array();
$array['element']['videosupport']['value'] = array('no','yes');
$array['element']['videosupport']['default'] = 'no';
$array['element']['videosupport']['set'] = true;

$array['element']['autocreatepeer'] = array();
$array['element']['autocreatepeer']['value'] = array('no','yes');
$array['element']['autocreatepeer']['default'] = 'no';
$array['element']['autocreatepeer']['set'] = true;

$array['element']['allowguest'] = array();
$array['element']['allowguest']['value'] = array('no','yes');
$array['element']['allowguest']['default'] = 'no';
$array['element']['allowguest']['set'] = true;

$array['element']['allowsubscribe'] = array();
$array['element']['allowsubscribe']['value'] = array('no','yes');
$array['element']['allowsubscribe']['default'] = 'yes';
$array['element']['allowsubscribe']['set'] = true;

$array['element']['allowoverlap'] = array();
$array['element']['allowoverlap']['value'] = array('no','yes');
$array['element']['allowoverlap']['default'] = 'yes';
$array['element']['allowoverlap']['set'] = true;

$array['element']['promiscredir'] = array();
$array['element']['promiscredir']['value'] = array('no','yes');
$array['element']['promiscredir']['default'] = 'no';
$array['element']['promiscredir']['set'] = true;

$array['element']['autodomain'] = array();
$array['element']['autodomain']['value'] = array('no','yes');
$array['element']['autodomain']['default'] = 'no';
$array['element']['autodomain']['set'] = true;

$array['element']['domain'] = array();
$array['element']['domain']['default'] = '';
$array['element']['domain']['null'] = true;

$array['element']['allowexternaldomains'] = array();
$array['element']['allowexternaldomains']['value'] = array('no','yes');
$array['element']['allowexternaldomains']['default'] = 'yes';
$array['element']['allowexternaldomains']['set'] = true;

$array['element']['usereqphone'] = array();
$array['element']['usereqphone']['value'] = array('no','yes');
$array['element']['usereqphone']['default'] = 'no';
$array['element']['usereqphone']['set'] = true;

$array['element']['realm'] = array();
$array['element']['realm']['default'] = 'xivo';

$array['element']['alwaysauthreject'] = array();
$array['element']['alwaysauthreject']['value'] = array('no','yes');
$array['element']['alwaysauthreject']['default'] = 'no';
$array['element']['alwaysauthreject']['set'] = true;

$array['element']['limitonpeer'] = array();
$array['element']['limitonpeer']['value'] = array('no','yes');
$array['element']['limitonpeer']['default'] = 'yes';
$array['element']['limitonpeer']['set'] = true;

$array['element']['useragent'] = array();
$array['element']['useragent']['default'] = 'XIVO PBX';

$array['element']['checkmwi'] = array();
$array['element']['checkmwi']['value'] = array(0,5,10,15,20,25,30,35,40,45,50,55,60);
$array['element']['checkmwi']['default'] = 10;

$array['element']['buggymwi'] = array();
$array['element']['buggymwi']['value'] = array('no','yes');
$array['element']['buggymwi']['default'] = 'no';
$array['element']['buggymwi']['set'] = true;

$array['element']['regcontext'] = array();
$array['element']['regcontext']['default'] = '';
$array['element']['regcontext']['null'] = true;

$array['element']['callerid'] = array();
$array['element']['callerid']['default'] = 'xivo';

$array['element']['fromdomain'] = array();
$array['element']['fromdomain']['default'] = '';
$array['element']['fromdomain']['null'] = true;

$array['element']['sipdebug'] = array();
$array['element']['sipdebug']['value'] = array('no','yes');
$array['element']['sipdebug']['default'] = 'no';
$array['element']['sipdebug']['set'] = true;

$array['element']['dumphistory'] = array();
$array['element']['dumphistory']['value'] = array('no','yes');
$array['element']['dumphistory']['default'] = 'no';
$array['element']['dumphistory']['set'] = true;

$array['element']['recordhistory'] = array();
$array['element']['recordhistory']['value'] = array('no','yes');
$array['element']['recordhistory']['default'] = 'no';
$array['element']['recordhistory']['set'] = true;

$array['element']['callevents'] = array();
$array['element']['callevents']['value'] = array('no','yes');
$array['element']['callevents']['default'] = 'yes';
$array['element']['callevents']['set'] = true;
$array['element']['callevents']['forcedef'] = true;

$array['element']['tos_sip'] = array();
$array['element']['tos_sip']['value'] = array('CS0','CS1','CS2','CS3','CS4','CS5','CS6','CS7',
					      'AF11','AF12','AF13','AF21','AF22','AF23',
					      'AF31', 'AF32','AF33','AF41','AF42','AF43','EF');
$array['element']['tos_sip']['default'] = '';
$array['element']['tos_sip']['null'] = true;

$array['element']['tos_audio'] = array();
$array['element']['tos_audio']['value'] = array('CS0','CS1','CS2','CS3','CS4','CS5','CS6','CS7',
						'AF11','AF12','AF13','AF21','AF22','AF23',
						'AF31', 'AF32','AF33','AF41','AF42','AF43','EF');
$array['element']['tos_audio']['default'] = '';
$array['element']['tos_audio']['null'] = true;

$array['element']['tos_video'] = array();
$array['element']['tos_video']['value'] = array('CS0','CS1','CS2','CS3','CS4','CS5','CS6','CS7',
						'AF11','AF12','AF13','AF21','AF22','AF23',
						'AF31', 'AF32','AF33','AF41','AF42','AF43','EF');
$array['element']['tos_video']['default'] = '';
$array['element']['tos_video']['null'] = true;

$array['element']['t38pt_udptl'] = array();
$array['element']['t38pt_udptl']['value'] = array('no','yes');
$array['element']['t38pt_udptl']['default'] = 'no';
$array['element']['t38pt_udptl']['set'] = true;

$array['element']['t38pt_rtp'] = array();
$array['element']['t38pt_rtp']['value'] = array('no','yes');
$array['element']['t38pt_rtp']['default'] = 'no';
$array['element']['t38pt_rtp']['set'] = true;

$array['element']['t38pt_tcp'] = array();
$array['element']['t38pt_tcp']['value'] = array('no','yes');
$array['element']['t38pt_tcp']['default'] = 'no';
$array['element']['t38pt_tcp']['set'] = true;

$array['element']['t38pt_usertpsource'] = array();
$array['element']['t38pt_usertpsource']['value'] = array('no','yes');
$array['element']['t38pt_usertpsource']['default'] = 'no';
$array['element']['t38pt_usertpsource']['set'] = true;

$array['element']['localnet'] = array();
$array['element']['localnet']['default'] = '';
$array['element']['localnet']['null'] = true;
$array['element']['localnet']['multi'] = true;

$array['element']['externip'] = array();
$array['element']['externip']['default'] = '';
$array['element']['externip']['null'] = true;

$array['element']['externhost'] = array();
$array['element']['externhost']['default'] = '';
$array['element']['externhost']['null'] = true;

$array['element']['externrefresh'] = array();
$array['element']['externrefresh']['value'] = array(1,10,20,30,40,50,60,70,80,90,
						    100,110,120,180,240,300,600,900,
						    1200,1800);
$array['element']['externrefresh']['default'] = 10;

$array['element']['matchexterniplocally'] = array();
$array['element']['matchexterniplocally']['value'] = array('no','yes');
$array['element']['matchexterniplocally']['default'] = 'no';
$array['element']['matchexterniplocally']['set'] = true;

$array['element']['outboundproxy'] = array();
$array['element']['outboundproxy']['default'] = '';
$array['element']['outboundproxy']['null'] = true;

$array['element']['outboundproxyport'] = array();
$array['element']['outboundproxyport']['default'] = '';
$array['element']['outboundproxyport']['null'] = true;

$array['element']['g726nonstandard'] = array();
$array['element']['g726nonstandard']['value'] = array('no','yes');
$array['element']['g726nonstandard']['default'] = 'no';
$array['element']['g726nonstandard']['set'] = true;

$array['element']['disallow'] = array();
$array['element']['disallow']['value'] = array('all');
$array['element']['disallow']['default'] = 'all';
$array['element']['disallow']['unset'] = true;

$array['element']['allow'] = array();
$array['element']['allow']['value'] = array(
					XIVO_SRE_IPBX_AST_CODEC_AUDIO_G723,
					XIVO_SRE_IPBX_AST_CODEC_AUDIO_GSM,
					XIVO_SRE_IPBX_AST_CODEC_AUDIO_ULAW,
					XIVO_SRE_IPBX_AST_CODEC_AUDIO_ALAW,
					XIVO_SRE_IPBX_AST_CODEC_AUDIO_ADPCM,
					XIVO_SRE_IPBX_AST_CODEC_AUDIO_SLIN,
					XIVO_SRE_IPBX_AST_CODEC_AUDIO_LPC10,
					XIVO_SRE_IPBX_AST_CODEC_AUDIO_G729,
					XIVO_SRE_IPBX_AST_CODEC_AUDIO_SPEEX,
					XIVO_SRE_IPBX_AST_CODEC_AUDIO_ILBC,
					XIVO_SRE_IPBX_AST_CODEC_AUDIO_G726,
					XIVO_SRE_IPBX_AST_CODEC_AUDIO_G722,
					XIVO_SRE_IPBX_AST_CODEC_IMAGE_JPEG,
					XIVO_SRE_IPBX_AST_CODEC_IMAGE_PNG,
					XIVO_SRE_IPBX_AST_CODEC_VIDEO_H261,
					XIVO_SRE_IPBX_AST_CODEC_VIDEO_H263,
					XIVO_SRE_IPBX_AST_CODEC_VIDEO_H263P,
					XIVO_SRE_IPBX_AST_CODEC_VIDEO_H264);
$array['element']['allow']['default'] = '';
$array['element']['allow']['null'] = true;

$array['element']['t1min'] = array();
$array['element']['t1min']['value'] = array(0,10,20,30,40,50,60,70,80,90,100,
					    200,300,400,500,600,700,800,900,
					    1000,1100,1200,1300,1400,1500,1600,1700,1800,1900,
					    2000,2500,3000,3500,4000);
$array['element']['t1min']['default'] = 100;

$array['element']['relaxdtmf'] = array();
$array['element']['relaxdtmf']['value'] = array('no','yes');
$array['element']['relaxdtmf']['default'] = 'no';
$array['element']['relaxdtmf']['set'] = true;

$array['element']['rfc2833compensate'] = array();
$array['element']['rfc2833compensate']['value'] = array('no','yes');
$array['element']['rfc2833compensate']['default'] = 'no';
$array['element']['rfc2833compensate']['set'] = true;

$array['element']['compactheaders'] = array();
$array['element']['compactheaders']['value'] = array('no','yes');
$array['element']['compactheaders']['default'] = 'no';
$array['element']['compactheaders']['set'] = true;

$array['element']['rtptimeout'] = array();
$array['element']['rtptimeout']['value'] = array(0,5,10,15,20,25,30,35,40,45,50,55,60);
$array['element']['rtptimeout']['default'] = 0;

$array['element']['rtpholdtimeout'] = array();
$array['element']['rtpholdtimeout']['value'] = array(0,5,10,15,20,25,30,35,40,45,50,55,60);
$array['element']['rtpholdtimeout']['default'] = 0;

$array['element']['rtpkeepalive'] = array();
$array['element']['rtpkeepalive']['value'] = array(0,5,10,15,20,25,30,35,40,45,50,55,60);
$array['element']['rtpkeepalive']['default'] = 0;

$array['element']['directrtpsetup'] = array();
$array['element']['directrtpsetup']['value'] = array('no','yes');
$array['element']['directrtpsetup']['default'] = 'no';
$array['element']['directrtpsetup']['set'] = true;

$array['element']['notifymimetype'] = array();
$array['element']['notifymimetype']['default'] = 'application/simple-message-summary';
$array['element']['notifymimetype']['setdef'] = true;

$array['element']['srvlookup'] = array();
$array['element']['srvlookup']['value'] = array('no','yes');
$array['element']['srvlookup']['default'] = 'no';
$array['element']['srvlookup']['set'] = true;

$array['element']['pedantic'] = array();
$array['element']['pedantic']['value'] = array('no','yes');
$array['element']['pedantic']['default'] = 'no';
$array['element']['pedantic']['set'] = true;

$array['element']['minexpiry'] = array();
$array['element']['minexpiry']['value'] = array(1,10,20,30,40,50,60,70,80,90,
						100,110,120,130,140,150,160,170,180,190,
						200,210,220,230,240,300,600,900,
						1200,1800,2400,2700,3000,3600,4500,
						5400,6300,7200);
$array['element']['minexpiry']['default'] = 60;

$array['element']['maxexpiry'] = array();
$array['element']['maxexpiry']['value'] = array(1,10,20,30,40,50,60,70,80,90,
						100,110,120,130,140,150,160,170,180,190,
						200,210,220,230,240,300,600,900,
						1200,1800,2400,2700,3000,3600,4500,
						5400,6300,7200);
$array['element']['maxexpiry']['default'] = 3600;

$array['element']['defaultexpiry'] = array();
$array['element']['defaultexpiry']['value'] = array(1,10,20,30,40,50,60,70,80,90,
						    100,110,120,130,140,150,160,170,180,190,
						    200,210,220,230,240,300,600,900,
						    1200,1800,2400,2700,3000,3600,4500,
						    5400,6300,7200);
$array['element']['defaultexpiry']['default'] = 120;

$array['element']['registertimeout'] = array();
$array['element']['registertimeout']['value'] = array(1,10,20,30,40,50,60,70,80,90,100,110,120);
$array['element']['registertimeout']['default'] = 20;

$array['element']['registerattempts'] = array();
$array['element']['registerattempts']['value'] = array(0,5,10,15,20,25,30,35,40,45,
						       50,55,60,65,70,75,80,85,90,95,
						       100,105,110,115,120,125,130,135,140,145,
						       150,155,160,165,170,175,180,185,190,195,200);
$array['element']['registerattempts']['default'] = 0;

$array['element']['notifyringing'] = array();
$array['element']['notifyringing']['value'] = array('no','yes');
$array['element']['notifyringing']['default'] = 'yes';
$array['element']['notifyringing']['set'] = true;

$array['element']['notifyhold'] = array();
$array['element']['notifyhold']['value'] = array('no','yes');
$array['element']['notifyhold']['default'] = 'no';
$array['element']['notifyhold']['set'] = true;

$array['element']['allowtransfer'] = array();
$array['element']['allowtransfer']['value'] = array('no','yes');
$array['element']['allowtransfer']['default'] = 'yes';
$array['element']['allowtransfer']['set'] = true;

$array['element']['maxcallbitrate'] = array();
$array['element']['maxcallbitrate']['default'] = 384;

$array['element']['autoframing'] = array();
$array['element']['autoframing']['value'] = array('no','yes');
$array['element']['autoframing']['default'] = 'yes';
$array['element']['autoframing']['set'] = true;

$array['element']['jbenable'] = array();
$array['element']['jbenable']['value'] = array('no','yes');
$array['element']['jbenable']['default'] = 'no';
$array['element']['jbenable']['set'] = true;

$array['element']['jbforce'] = array();
$array['element']['jbforce']['value'] = array('no','yes');
$array['element']['jbforce']['default'] = 'no';
$array['element']['jbforce']['set'] = true;

$array['element']['jbmaxsize'] = array();
$array['element']['jbmaxsize']['value'] = array(0,100,150,200,250,500,750,1000,1250,1500,1750,2000);
$array['element']['jbmaxsize']['default'] = 200;

$array['element']['jbresyncthreshold'] = array();
$array['element']['jbresyncthreshold']['value'] = array(-1,0,250,500,750,1000,1250,1500,1750,2000);
$array['element']['jbresyncthreshold']['default'] = 1000;

$array['element']['jbimpl'] = array();
$array['element']['jbimpl']['value'] = array('fixed','adaptative');
$array['element']['jbimpl']['default'] = 'fixed';

$array['element']['jblog'] = array();
$array['element']['jblog']['value'] = array('no','yes');
$array['element']['jblog']['default'] = 'no';
$array['element']['jblog']['set'] = true;

$array['element']['context'] = array();
$array['element']['context']['default'] = '';
$array['element']['context']['null'] = true;

$array['element']['nat'] = array();
$array['element']['nat']['value'] = array('no','yes','never','route');
$array['element']['nat']['default'] = 'no';

$array['element']['dtmfmode'] = array();
$array['element']['dtmfmode']['value'] = array('rfc2833','inband','info','auto');
$array['element']['dtmfmode']['default'] = 'info';

$array['element']['qualify'] = array();
$array['element']['qualify']['value'] = array('no','yes',500,1000,1500,2000,2500,3000,3500,4000);
$array['element']['qualify']['default'] = 'no';

$array['element']['useclientcode'] = array();
$array['element']['useclientcode']['value'] = array('no','yes');
$array['element']['useclientcode']['default'] = 'no';
$array['element']['useclientcode']['set'] = true;

$array['element']['progressinband'] = array();
$array['element']['progressinband']['value'] = array('no','yes','never');
$array['element']['progressinband']['default'] = 'never';

$array['element']['language'] = array();
$array['element']['language']['value'] = array('en','fr');
$array['element']['language']['default'] = 'fr';

$array['element']['mohinterpret'] = array();
$array['element']['mohinterpret']['default'] = XIVO_SRE_IPBX_AST_MOH_DEFAULT;
$array['element']['mohinterpret']['setdef'] = true;

$array['element']['mohsuggest'] = array();
$array['element']['mohsuggest']['default'] = '';
$array['element']['mohsuggest']['null'] = true;

$array['element']['vmexten'] = array();
$array['element']['vmexten']['default'] = '*98';

$array['element']['trustrpid'] = array();
$array['element']['trustrpid']['value'] = array('no','yes');
$array['element']['trustrpid']['default'] = 'no';
$array['element']['trustrpid']['set'] = true;

$array['element']['sendrpid'] = array();
$array['element']['sendrpid']['value'] = array('no','yes');
$array['element']['sendrpid']['default'] = 'no';
$array['element']['sendrpid']['set'] = true;

$array['element']['assertedidentity'] = array();
$array['element']['assertedidentity']['value'] = array('no','yes');
$array['element']['assertedidentity']['default'] = 'no';
$array['element']['assertedidentity']['set'] = true;

$array['element']['canreinvite'] = array();
$array['element']['canreinvite']['value'] = array('no'			=> 'no',
						  'yes'			=> 'yes',
						  'update'		=> 'update',
						  'nonat'		=> 'nonat',
						  'update,nonat'	=> 'update-nonat');
$array['element']['canreinvite']['default'] = 'no';

$array['element']['insecure'] = array();
$array['element']['insecure']['value'] = array('no'		=> 'no',
					       'port'		=> 'port',
					       'invite'		=> 'invite',
					       'port,invite'	=> 'all');
$array['element']['insecure']['default'] = 'no';

$array['element']['rtcachefriends'] = array();
$array['element']['rtcachefriends']['value'] = array('no','yes');
$array['element']['rtcachefriends']['default'] = 'yes';
$array['element']['rtcachefriends']['set'] = true;

$array['element']['rtupdate'] = array();
$array['element']['rtupdate']['value'] = array('no','yes');
$array['element']['rtupdate']['default'] = 'yes';
$array['element']['rtupdate']['set'] = true;

$array['element']['ignoreregexpire'] = array();
$array['element']['ignoreregexpire']['value'] = array('no','yes');
$array['element']['ignoreregexpire']['default'] = 'no';
$array['element']['ignoreregexpire']['set'] = true;

$array['element']['rtsavesysname'] = array();
$array['element']['rtsavesysname']['value'] = array('no','yes');
$array['element']['rtsavesysname']['default'] = 'no';
$array['element']['rtsavesysname']['set'] = true;

$array['element']['rtautoclear'] = array();
$array['element']['rtautoclear']['value'] = array('no','yes',1,10,20,30,40,50,60,70,80,90,
						  100,110,120,130,140,150,160,170,180,190,
						  200,210,220,230,240);
$array['element']['rtautoclear']['default'] = 'no';

$array['element']['subscribecontext'] = array();
$array['element']['subscribecontext']['default'] = '';
$array['element']['subscribecontext']['forcedef'] = true;
$array['element']['subscribecontext']['null'] = true;


dwho::load_class('dwho_network');

$array['filter'] = array();
$array['filter']['bindport'] = array('set' => false,'chk' => 2,'cast' => 'uint','between' => array(1,65535));
$array['filter']['bindaddr'] = array('set'		=> false,
				     'chk'		=> 2,
				     'callback'		=> 'dwho_network::valid_host',
				     'callbackarg'	=> array((DWHO_NETWORK_HOST_IPV4_DOTDEC |
								  DWHO_NETWORK_HOST_HOSTNAME)),
				     'maxlen'		=> 64);
$array['filter']['videosupport'] = array('bool' => true);
$array['filter']['autocreatepeer'] = array('bool' => true);
$array['filter']['allowguest'] = array('bool' => true);
$array['filter']['allowsubscribe'] = array('bool' => true);
$array['filter']['allowoverlap'] = array('bool' => true);
$array['filter']['promiscredir'] = array('bool' => true);
$array['filter']['autodomain'] = array('bool' => true);
$array['filter']['domain'] = array('set'		=> false,
				   'chk'		=> 2,
				   'callback'		=> 'dwho_network::valid_host',
				   'callbackarg'	=> array((DWHO_NETWORK_HOST_IPV4_DOTDEC |
								  DWHO_NETWORK_HOST_HOSTNAME)),
				   'maxlen'		=> 64);
$array['filter']['allowexternaldomains'] = array('bool' => true);
$array['filter']['usereqphone'] = array('bool' => true);
$array['filter']['realm'] = array('regexp' => '/[^"]/','maxlen' => 64);
$array['filter']['alwaysauthreject'] = array('bool' => true);
$array['filter']['limitonpeer'] = array('bool' => true);
$array['filter']['useragent'] = array('cast' => 'sgraph','maxlen' => 80);
$array['filter']['checkmwi'] = array('cast' => 'uint','between' => array(0,60,5));
$array['filter']['buggymwi'] = array('bool' => true);
$array['filter']['regcontext'] = array('set' => false,'chk' => 2,
				       'callback' => 'xivo_service_asterisk::chk_context',
				       'callbackarg' => array(array(XIVO_SRE_IPBX_AST_CONTEXT_AUTHENTICATION)));
$array['filter']['callerid'] = array('maxlen'	=> 80,
				     'regexp'	=> '/^'.XIVO_SRE_IPBX_AST_REGEXP_CALLERID.'$/u',
				     'notag'	=> false);
$array['filter']['fromdomain'] = array('set' => false,'chk' => 2,'regexp' => '/^[a-z0-9_-\.]+$/i','maxlen' => 80);
$array['filter']['sipdebug'] = array('bool' => true);
$array['filter']['dumphistory'] = array('bool' => true);
$array['filter']['recordhistory'] = array('bool' => true);
$array['filter']['callevents'] = array('bool' => true);
$array['filter']['tos_sip'] = array('set' => false,'chk' => 2,'regexp' => '/^(CS[0-7]|AF[1-4][1-3]|EF)$/');
$array['filter']['tos_audio'] = array('set' => false,'chk' => 2,'regexp' => '/^(CS[0-7]|AF[1-4][1-3]|EF)$/');
$array['filter']['tos_video'] = array('set' => false,'chk' => 2,'regexp' => '/^(CS[0-7]|AF[1-4][1-3]|EF)$/');
$array['filter']['t38pt_udptl'] = array('bool' => true);
$array['filter']['t38pt_rtp'] = array('bool' => true);
$array['filter']['t38pt_tcp'] = array('bool' => true);
$array['filter']['t38pt_usertpsource'] = array('bool' => true);
$array['filter']['localnet'] = array('set'		=> false,
				     'chk'		=> 2,
				     'callback'		=> 'dwho_network::valid_host_subnet_ipv4',
				     'callbackarg'	=> array(null,null,DWHO_NETWORK_HOST_IPV4_DOTDEC),
				     'maxlen'		=> 128);
$array['filter']['externip'] = array('set'		=> false,
				     'chk'		=> 2,
				     'callback'		=> 'dwho_network::valid_host',
				     'callbackarg'	=> array((DWHO_NETWORK_HOST_IPV4_DOTDEC |
								  DWHO_NETWORK_HOST_HOSTNAME)),
				     'maxlen'		=> 64);
$array['filter']['externhost'] = array('set'		=> false,
				       'chk'		=> 2,
				       'callback'	=> 'dwho_network::valid_host',
				       'callbackarg'	=> array((DWHO_NETWORK_HOST_IPV4_DOTDEC |
								  DWHO_NETWORK_HOST_HOSTNAME)),
				       'maxlen'		=> 64);
$array['filter']['externrefresh'] = array('key' => array(1,10,20,30,40,50,60,70,80,90,100,110,120,180,240,300,600,900,1200,1800));
$array['filter']['matchexterniplocally'] = array('bool' => true);
$array['filter']['outboundproxy'] = array('set'		=> false,
					  'chk'		=> 2,
					  'callback'	=> 'dwho_network::valid_host',
					  'callbackarg'	=> array((DWHO_NETWORK_HOST_IPV4_DOTDEC |
								  DWHO_NETWORK_HOST_HOSTNAME)),
					  'maxlen'	=> 64);
$array['filter']['outboundproxyport'] = array('set' => false,'chk' => 2,'cast' => 'uint','between' => array(1,65535));
$array['filter']['g726nonstandard'] = array('bool' => true);
$array['filter']['disallow'] = array('set' => false,'chk' => 2,'key' => array('all'));
$array['filter']['allow'] = array('set' => false,'chk' => 2,'uvalue' => array(
						XIVO_SRE_IPBX_AST_CODEC_AUDIO_G723,
						XIVO_SRE_IPBX_AST_CODEC_AUDIO_GSM,
						XIVO_SRE_IPBX_AST_CODEC_AUDIO_ULAW,
						XIVO_SRE_IPBX_AST_CODEC_AUDIO_ALAW,
						XIVO_SRE_IPBX_AST_CODEC_AUDIO_ADPCM,
						XIVO_SRE_IPBX_AST_CODEC_AUDIO_SLIN,
						XIVO_SRE_IPBX_AST_CODEC_AUDIO_LPC10,
						XIVO_SRE_IPBX_AST_CODEC_AUDIO_G729,
						XIVO_SRE_IPBX_AST_CODEC_AUDIO_SPEEX,
						XIVO_SRE_IPBX_AST_CODEC_AUDIO_ILBC,
						XIVO_SRE_IPBX_AST_CODEC_AUDIO_G726,
						XIVO_SRE_IPBX_AST_CODEC_AUDIO_G722,
						XIVO_SRE_IPBX_AST_CODEC_IMAGE_JPEG,
						XIVO_SRE_IPBX_AST_CODEC_IMAGE_PNG,
						XIVO_SRE_IPBX_AST_CODEC_VIDEO_H261,
						XIVO_SRE_IPBX_AST_CODEC_VIDEO_H263,
						XIVO_SRE_IPBX_AST_CODEC_VIDEO_H263P,
						XIVO_SRE_IPBX_AST_CODEC_VIDEO_H264));
$array['filter']['t1min'] = array('key' => array(0,10,20,30,40,50,60,70,80,90,100,
						 200,300,400,500,600,700,800,900,
						 1000,1100,1200,1300,1400,1500,1600,1700,1800,1900,
						 2000,2500,3000,3500,4000));
$array['filter']['relaxdtmf'] = array('bool' => true);
$array['filter']['rfc2833compensate'] = array('bool' => true);
$array['filter']['compactheaders'] = array('bool' => true);
$array['filter']['rtptimeout'] = array('cast' => 'uint','between' => array(0,60,5));
$array['filter']['rtpholdtimeout'] = array('cast' => 'uint','between' => array(0,60,5));
$array['filter']['rtpkeepalive'] = array('cast' => 'uint','between' => array(0,60,5));
$array['filter']['directrtpsetup'] = array('bool' => true);
$array['filter']['notifymimetype'] = array('set' => false,'chk' => 2,'maxlen' => 80,
					   'regexp' => '#^(?:text|application)/[a-zA-Z][a-zA-Z0-9][a-zA-Z0-9\+][a-zA-Z0-9\-\+\.]*$#');
$array['filter']['srvlookup'] = array('bool' => true);
$array['filter']['pedantic'] = array('bool' => true);
$array['filter']['minexpiry'] = array('key' => array(1,10,20,30,40,50,60,
						     70,80,90,100,110,120,
						     130,140,150,160,170,
						     180,190,200,210,220,230,
						     240,300,600,900,1200,1800,
						     2400,2700,3000,3600,4500,
						     5400,6300,7200));
$array['filter']['maxexpiry'] = array('key' => array(1,10,20,30,40,50,60,
						     70,80,90,100,110,120,
						     130,140,150,160,170,
						     180,190,200,210,220,230,
						     240,300,600,900,1200,1800,
						     2400,2700,3000,3600,4500,
						     5400,6300,7200));
$array['filter']['defaultexpiry'] = array('key' => array(1,10,20,30,40,50,60,
							 70,80,90,100,110,120,
							 130,140,150,160,170,
							 180,190,200,210,220,230,
							 240,300,600,900,1200,1800,
							 2400,2700,3000,3600,4500,
							 5400,6300,7200));
$array['filter']['registertimeout'] = array('cast' => 'uint','between' => array(1,120,10));
$array['filter']['registerattempts'] = array('cast' => 'uint','between' => array(0,200,5));
$array['filter']['notifyringing'] = array('bool' => true);
$array['filter']['notifyhold'] = array('bool' => true);
$array['filter']['allowtransfer'] = array('bool' => true);
$array['filter']['maxcallbitrate'] = array('cast' => 'uint','between' => array(1,1860));
$array['filter']['autoframing'] = array('bool' => true);
$array['filter']['jbenable'] = array('bool' => true);
$array['filter']['jbforce'] = array('bool' => true);
$array['filter']['jbmaxsize'] = array('key' => array(0,100,150,200,250,500,750,1000,1250,1500,1750,2000));
$array['filter']['jbresyncthreshold'] = array('key' => array(-1,0,250,500,750,1000,1250,1500,1750,2000));
$array['filter']['jbimpl'] = array('key' => array('fixed','adaptative'));
$array['filter']['jblog'] = array('bool' => true);
$array['filter']['context'] = array('set' => false,'chk' => 2,
				    'callback' => 'xivo_service_asterisk::chk_context',
				    'callbackarg' => array(array(XIVO_SRE_IPBX_AST_CONTEXT_AUTHENTICATION)));
$array['filter']['nat'] = array('key' => array('no','yes','never','route'));
$array['filter']['dtmfmode'] = array('key' => array('rfc2833','inband','info','auto'));
$array['filter']['qualify'] = array('key' => array('no','yes',500,1000,1500,2000,2500,3000,3500,4000));
$array['filter']['useclientcode'] = array('bool' => true);
$array['filter']['progressinband'] = array('key' => array('no','yes','never'));
$array['filter']['language'] = array('key' => array('en','fr'));
$array['filter']['mohinterpret'] = array('set' => false,'chk' => 2,'maxlen' => 20);
$array['filter']['mohsuggest'] = array('set' => false,'chk' => 2,'maxlen' => 20);
$array['filter']['vmexten'] = array('regexp' => '/^[0-9\*]+$/','maxlen' => 40);
$array['filter']['trustrpid'] = array('bool' => true);
$array['filter']['sendrpid'] = array('bool' => true);
$array['filter']['assertedidentity'] = array('bool' => true);
$array['filter']['canreinvite'] = array('key' => array('no','yes','update','nonat','update,nonat'));
$array['filter']['insecure'] = array('key' => array('no','port','invite','port,invite'));
$array['filter']['rtcachefriends'] = array('bool' => true);
$array['filter']['rtupdate'] = array('bool' => true);
$array['filter']['ignoreregexpire'] = array('bool' => true);
$array['filter']['rtsavesysname'] = array('bool' => true);
$array['filter']['rtautoclear'] = array('key' => array('yes','no',1,10,20,30,40,50,60,70,80,90,
						       100,110,120,130,140,150,160,170,180,
						       190,200,210,220,230,240));
$array['filter']['subscribecontext'] = array('set' => false,'chk' => 0);

?>

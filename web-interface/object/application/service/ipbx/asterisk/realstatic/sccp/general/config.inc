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

$array['element']['servername'] = array();
$array['element']['servername']['default'] = 'Asterisk';

$array['element']['keepalive'] = array();
$array['element']['keepalive']['default'] = 60;
$array['element']['keepalive']['value'] = array(0,5,10,15,20,25,30,35,40,45,
								50,55,60,65,70,75,80,85,90,95,
								100,105,110,115,120,125,130,135,140,145,
								150,155,160,165,170,175,180,185,190,195,200);

$array['element']['debug'] = array();
$array['element']['debug']['default'] = 'core';

$array['element']['context'] = array();
$array['element']['context']['default'] = '';
$array['element']['context']['null'] = true;

$array['element']['dateFormat'] = array();
$array['element']['dateFormat']['default'] = 'D.M.Y';

$array['element']['bindaddr'] = array();
$array['element']['bindaddr']['default'] = '0.0.0.0';

$array['element']['port'] = array();
$array['element']['port']['default'] = 2000;

$array['element']['disallow'] = array();
$array['element']['disallow']['default'] = 'all';
$array['element']['disallow']['value'] = array('all');

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

$array['element']['firstdigittimeout'] = array();
$array['element']['firstdigittimeout']['default'] = 16;

$array['element']['digittimeout'] = array();
$array['element']['digittimeout']['default'] = 8;

$array['element']['digittimeoutchar'] = array();
$array['element']['digittimeoutchar']['default'] = '#';

$array['element']['autoanswer_ring_time'] = array();
$array['element']['autoanswer_ring_time']['default'] = 0;
$array['element']['autoanswer_ring_time']['value'] = array(0,5,10,15,20,25,30,35,40,45,
								50,55,60,65,70,75,80,85,90,95,
								100,105,110,115,120,125,130,135,140,145,
								150,155,160,165,170,175,180,185,190,195,200);

$array['element']['autoanswer_tone'] = array();
$array['element']['autoanswer_tone']['default'] = '0x32';
$array['element']['autoanswer_tone']['value'] = array(0,1,2,3,4,5,6,7,8,9,'0xa','0xe','0xf','0x10','0x11','0x12','0x13',
								'0x21','0x22','0x23','0x24','0x25','0x26','0x27','0x28','0x29','0x2a','0x2b','0x2c','0x2d','0x2e','0x2f',
								'0x30','0x31','0x32','0x33','0x34','0x35','0x36',
								'0x40','0x41','0x42','0x43','0x44','0x45','0x46',
								'0x50','0x51','0x52','0x53','0x54','0x55','0x56','0x57','0x58','0x59','0x5a','0x5b','0x5c','0x5d','0x5e','0x5f',
								'0x60','0x61','0x62','0x63','0x64','0x65','0x66','0x67','0x68','0x69','0x6a',
								'0x7f');

$array['element']['remotehangup_tone'] = array();
$array['element']['remotehangup_tone']['default'] = '0x32';
$array['element']['remotehangup_tone']['value'] = array(0,1,2,3,4,5,6,7,8,9,'0xa','0xe','0xf','0x10','0x11','0x12','0x13',
								'0x21','0x22','0x23','0x24','0x25','0x26','0x27','0x28','0x29','0x2a','0x2b','0x2c','0x2d','0x2e','0x2f',
								'0x30','0x31','0x32','0x33','0x34','0x35','0x36',
								'0x40','0x41','0x42','0x43','0x44','0x45','0x46',
								'0x50','0x51','0x52','0x53','0x54','0x55','0x56','0x57','0x58','0x59','0x5a','0x5b','0x5c','0x5d','0x5e','0x5f',
								'0x60','0x61','0x62','0x63','0x64','0x65','0x66','0x67','0x68','0x69','0x6a',
								'0x7f');

$array['element']['transfer_tone'] = array();
$array['element']['transfer_tone']['default'] = 0;
$array['element']['transfer_tone']['value'] = array(0,1,2,3,4,5,6,7,8,9,'0xa','0xe','0xf','0x10','0x11','0x12','0x13',
								'0x21','0x22','0x23','0x24','0x25','0x26','0x27','0x28','0x29','0x2a','0x2b','0x2c','0x2d','0x2e','0x2f',
								'0x30','0x31','0x32','0x33','0x34','0x35','0x36',
								'0x40','0x41','0x42','0x43','0x44','0x45','0x46',
								'0x50','0x51','0x52','0x53','0x54','0x55','0x56','0x57','0x58','0x59','0x5a','0x5b','0x5c','0x5d','0x5e','0x5f',
								'0x60','0x61','0x62','0x63','0x64','0x65','0x66','0x67','0x68','0x69','0x6a',
								'0x7f');

$array['element']['callwaiting_tone'] = array();
$array['element']['callwaiting_tone']['default'] = '0x2d';
$array['element']['callwaiting_tone']['value'] = array(0,1,2,3,4,5,6,7,8,9,'0xa','0xe','0xf','0x10','0x11','0x12','0x13',
								'0x21','0x22','0x23','0x24','0x25','0x26','0x27','0x28','0x29','0x2a','0x2b','0x2c','0x2d','0x2e','0x2f',
								'0x30','0x31','0x32','0x33','0x34','0x35','0x36',
								'0x40','0x41','0x42','0x43','0x44','0x45','0x46',
								'0x50','0x51','0x52','0x53','0x54','0x55','0x56','0x57','0x58','0x59','0x5a','0x5b','0x5c','0x5d','0x5e','0x5f',
								'0x60','0x61','0x62','0x63','0x64','0x65','0x66','0x67','0x68','0x69','0x6a',
								'0x7f');

$array['element']['musicclass'] = array();
$array['element']['musicclass']['default'] = 'default';

$array['element']['language'] = array();
$array['element']['language']['default'] = 'fr';
$array['element']['language']['value'] = array('fr', 'en');

$array['element']['callevents'] = array();
$array['element']['callevents']['default'] = 'no';
$array['element']['callevents']['value'] = array('no', 'yes');

$array['element']['accountcode'] = array();
$array['element']['accountcode']['default'] = 'skinny';

$array['element']['deny'] = array();
$array['element']['deny']['default'] = '';
$array['element']['deny']['null'] = true;

$array['element']['permit'] = array();
$array['element']['permit']['default'] = '';
$array['element']['permit']['null'] = true;

$array['element']['localnet'] = array();
$array['element']['localnet']['default'] = '';
$array['element']['localnet']['null'] = true;

$array['element']['externip'] = array();
$array['element']['externip']['default'] = '';
$array['element']['externip']['null'] = true;

$array['element']['externhost'] = array();
$array['element']['externhost']['default'] = '';
$array['element']['externhost']['null'] = true;

$array['element']['externrefresh'] = array();
$array['element']['externrefresh']['default'] = 60;
$array['element']['externrefresh']['value'] = array(1,10,20,30,40,50,60,70,80,90,
						    100,110,120,180,240,300,600,900,1200,1800);

$array['element']['dnd'] = array();
$array['element']['dnd']['default'] = 'off';
$array['element']['dnd']['value'] = array('off', 'on', 'reject', 'silent');

$array['element']['sccp_tos'] = array();
$array['element']['sccp_tos']['default'] = '0x68';

$array['element']['sccp_cos'] = array();
$array['element']['sccp_cos']['default'] = 4;
$array['element']['sccp_cos']['value'] = array(1,2,3,4,5,6,7);

$array['element']['audio_tos'] = array();
$array['element']['audio_tos']['default'] = '0xB8';

$array['element']['audio_cos'] = array();
$array['element']['audio_cos']['default'] = 6;
$array['element']['audio_cos']['value'] = array(1,2,3,4,5,6,7);

$array['element']['video_tos'] = array();
$array['element']['video_tos']['default'] = '0x88';

$array['element']['video_cos'] = array();
$array['element']['video_cos']['default'] = 5;
$array['element']['video_cos']['value'] = array(1,2,3,4,5,6,7);

$array['element']['echocancel'] = array();
$array['element']['echocancel']['default'] = 'on';
$array['element']['echocancel']['value'] = array('off', 'on');

$array['element']['silencesuppression'] = array();
$array['element']['silencesuppression']['default'] = 'off';
$array['element']['silencesuppression']['value'] = array('off', 'on');

$array['element']['callgroup'] = array();
$array['element']['callgroup']['default'] = '1,3-4';

$array['element']['pickupgroup'] = array();
$array['element']['pickupgroup']['default'] = '1,3-5';

$array['element']['amaflags'] = array();
$array['element']['amaflags']['default'] = '';

$array['element']['trustphoneip'] = array();
$array['element']['trustphoneip']['default'] = 'no';
$array['element']['trustphoneip']['value'] = array('no', 'yes');

$array['element']['earlyrtp'] = array();
$array['element']['earlyrtp']['default'] = 'none';
$array['element']['earlyrtp']['value'] = array('none','offhook','dial','ringout');

$array['element']['private'] = array();
$array['element']['private']['default'] = 'on';
$array['element']['private']['value'] = array('off', 'on');

$array['element']['mwilamp'] = array();
$array['element']['mwilamp']['default'] = 'on';
$array['element']['mwilamp']['value'] = array('off', 'on');

$array['element']['mwioncall'] = array();
$array['element']['mwioncall']['default'] = 'off';
$array['element']['mwioncall']['value'] = array('off', 'on');

$array['element']['blindtransferindication'] = array();
$array['element']['blindtransferindication']['default'] = 'ring';
$array['element']['blindtransferindication']['value'] = array('ring', 'moh');

$array['element']['protocolversion'] = array();
$array['element']['protocolversion']['default'] = 11;

$array['element']['cfwdall'] = array();
$array['element']['cfwdall']['default'] = 'off';
$array['element']['cfwdall']['value'] = array('off', 'on');

$array['element']['cfwdbusy'] = array();
$array['element']['cfwdbusy']['default'] = 'off';
$array['element']['cfwdbusy']['value'] = array('off', 'on');

$array['element']['cfwdnoanswer'] = array();
$array['element']['cfwdnoanswer']['default'] = 'off';
$array['element']['cfwdnoanswer']['value'] = array('off', 'on');

$array['element']['devicetable'] = array();
$array['element']['devicetable']['default'] = 'sccpdevice';

$array['element']['linetable'] = array();
$array['element']['linetable']['default'] = 'sccpline';

$array['element']['nat'] = array();
$array['element']['nat']['default'] = 'off';
$array['element']['nat']['value'] = array('off', 'on');

$array['element']['directrtp'] = array();
$array['element']['directrtp']['default'] = 'off';
$array['element']['directrtp']['value'] = array('off', 'on');

$array['element']['allowoverlap'] = array();
$array['element']['allowoverlap']['default'] = 'off';
$array['element']['allowoverlap']['value'] = array('off', 'on');

$array['element']['callanswerorder'] = array();
$array['element']['callanswerorder']['default'] = 'olderfirst';
$array['element']['callanswerorder']['value'] = array('olderfirst', 'lastestfirst');

$array['element']['jbenable'] = array();
$array['element']['jbenable']['default'] = 'no';
$array['element']['jbenable']['value'] = array('no', 'yes');

$array['element']['jbforce'] = array();
$array['element']['jbforce']['default'] = 'no';
$array['element']['jbforce']['value'] = array('no', 'yes');

$array['element']['jbmaxsize'] = array();
$array['element']['jbmaxsize']['default'] = 200;
$array['element']['jbmaxsize']['value'] = array(0,100,150,200,250,500,750,1000,1250,1500,1750,2000);

$array['element']['jbresyncthreshold'] = array();
$array['element']['jbresyncthreshold']['default'] = 1000;
$array['element']['jbresyncthreshold']['value'] = array(-1,0,250,500,750,1000,1250,1500,1750,2000);

$array['element']['jbimpl'] = array();
$array['element']['jbimpl']['default'] = 'fixed';
$array['element']['jbimpl']['value'] = array('fixed', 'adaptative');

$array['element']['jblog'] = array();
$array['element']['jblog']['default'] = 'no';
$array['element']['jblog']['value'] = array('no', 'yes');

$array['element']['hotline_enabled'] = array();
$array['element']['hotline_enabled']['default'] = 'yes';
$array['element']['hotline_enabled']['value'] = array('no', 'yes');

$array['element']['hotline_context'] = array();
$array['element']['hotline_context']['default'] = 'default';

$array['element']['hotline_extension'] = array();
$array['element']['hotline_extension']['default'] = 111;

dwho::load_class('dwho_network');

$array['filter'] = array();

##############################################################
# GENERAL PART

$array['filter']['servername'] = array('set' => false,'chk' => 2,'regexp' => '/^[a-z0-9_-]+$/i','maxlen' => 80);
$array['filter']['keepalive'] = array('key' => array(0,5,10,15,20,25,30,35,40,45,50,55,60,65,70,75,80,85,90,95,100,105,110,115,120,125,130,135,140,145,150,155,160,165,170,175,180,185,190,195,200));
$array['filter']['context'] = array('set' => false,'chk' => 2,
				    'callback' => 'xivo_service_asterisk::chk_context',
				    'callbackarg' => array(array(XIVO_SRE_IPBX_AST_CONTEXT_AUTHENTICATION)));
$array['filter']['dateFormat'] = array('set' => false,'chk' => 5,'regexp' => '/^[-DMYA\/\.]+$/','maxlen' => 6);
$array['filter']['bindaddr'] = array('set' => false,'chk' => 2,'callback' => 'dwho_network::valid_host',
				     'callbackarg'	=> array((DWHO_NETWORK_HOST_IPV4_DOTDEC | DWHO_NETWORK_HOST_HOSTNAME)),
				     'maxlen'		=> 64);
$array['filter']['port'] = array('set' => false,'chk' => 2,'cast' => 'uint','between' => array(1,65535));
$array['filter']['firstdigittimeout'] = array('set' => false,'chk' => 2,'cast' => 'uint','between' => array(1,65535));
$array['filter']['digittimeout'] = array('set' => false,'chk' => 2,'cast' => 'uint','between' => array(1,65535));
$array['filter']['digittimeoutchar'] = array('key' => array('#','*'));
$array['filter']['debug'] = array('set' => false,'chk' => 2,'regexp' => '/^[a-z,\.]+$/i','maxlen' => 80);
$array['filter']['language'] = array('key' => array('en','fr'));

##############################################################
# NETWORK PART

$array['filter']['deny'] = array('set' => false,'chk' => 2,'callback' => 'dwho_network::valid_host',
				     'callbackarg'	=> array((DWHO_NETWORK_HOST_IPV4_DOTDEC | DWHO_NETWORK_HOST_HOSTNAME)),
				     'maxlen'		=> 64);
$array['filter']['permit'] = array('set' => false,'chk' => 2,'callback' => 'dwho_network::valid_host',
				     'callbackarg'	=> array((DWHO_NETWORK_HOST_IPV4_DOTDEC | DWHO_NETWORK_HOST_HOSTNAME)),
				     'maxlen'		=> 64);
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
/*
$array['filter']['sccp_tos'] = array('set' => false,'chk' => 2,'regexp' => '/^(0x[*])$/');
$array['filter']['audio_tos'] = array('set' => false,'chk' => 2,'regexp' => '/^(0x[*])$/');
$array['filter']['video_tos'] = array('set' => false,'chk' => 2,'regexp' => '/^(0x[*])$/');*/

##############################################################
# SIGNALING PART

$array['filter']['allowoverlap'] = array('key' => array('on','off'));

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


##############################################################
# JITTER BUFFER PART

$array['filter']['jbenable'] = array('key' => array('yes','no'));
$array['filter']['jbforce'] = array('key' => array('yes','no'));
$array['filter']['jbmaxsize'] = array('key' => array(0,100,150,200,250,500,750,1000,1250,1500,1750,2000));
$array['filter']['jbresyncthreshold'] = array('key' => array(-1,0,250,500,750,1000,1250,1500,1750,2000));
$array['filter']['jbimpl'] = array('key' => array('fixed','adaptative'));
$array['filter']['jblog'] = array('key' => array('yes','no'));

##############################################################
# DEFAULT PART



/*
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

$array['filter']['t1min'] = array('key' => array(0,10,20,30,40,50,60,70,80,90,100,
						 200,300,400,500,600,700,800,900,
						 1000,1100,1200,1300,1400,1500,1600,1700,1800,1900,
						 2000,2500,3000,3500,4000));
$array['filter']['rtptimeout'] = array('cast' => 'uint','between' => array(0,60,5));
$array['filter']['rtpholdtimeout'] = array('cast' => 'uint','between' => array(0,60,5));
$array['filter']['rtpkeepalive'] = array('cast' => 'uint','between' => array(0,60,5));
$array['filter']['srvlookup'] = array('bool' => true);
$array['filter']['registertimeout'] = array('cast' => 'uint','between' => array(1,120,10));
$array['filter']['registerattempts'] = array('cast' => 'uint','between' => array(0,200,5));
$array['filter']['notifyringing'] = array('bool' => true);
$array['filter']['notifyhold'] = array('bool' => true);
$array['filter']['allowtransfer'] = array('bool' => true);
$array['filter']['maxcallbitrate'] = array('cast' => 'uint','between' => array(1,1860));
$array['filter']['autoframing'] = array('bool' => true);
$array['filter']['context'] = array('set' => false,'chk' => 2,
				    'callback' => 'xivo_service_asterisk::chk_context',
				    'callbackarg' => array(array(XIVO_SRE_IPBX_AST_CONTEXT_AUTHENTICATION)));
$array['filter']['nat'] = array('key' => array('on','off'));
$array['filter']['dtmfmode'] = array('key' => array('rfc2833','inband','info','auto'));
$array['filter']['qualify'] = array('key' => array('no','yes',500,1000,1500,2000,2500,3000,3500,4000));
$array['filter']['useclientcode'] = array('bool' => true);
$array['filter']['progressinband'] = array('key' => array('no','yes','never'));
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
*/
?>

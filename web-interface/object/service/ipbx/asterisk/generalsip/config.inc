<?php

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
$array['element']['allowguest']['value'] = array('no','yes','osp');
$array['element']['allowguest']['default'] = 'no';

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

$array['element']['useragent'] = array();
$array['element']['useragent']['default'] = 'XIVO PBX';

$array['element']['checkmwi'] = array();
$array['element']['checkmwi']['value'] = array(0,5,10,15,20,25,30,35,40,45,50,55,60);
$array['element']['checkmwi']['default'] = 10;

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
$array['element']['callevents']['default'] = 'no';
$array['element']['callevents']['set'] = true;

$array['element']['tos'] = array();
$array['element']['tos']['value'] = array('none','lowdelay','throughput','reliability','mincost',
					  1,2,3,4,5,6,7,8,9,10,
					  11,12,13,14,15,16,17,18,19,20,
					  21,22,23,24,25,26,27,28,29,30,31);
$array['element']['tos']['default'] = 'lowdelay';

$array['element']['ospauth'] = array();
$array['element']['ospauth']['value'] = array('no','proxy','gateway','exclusive');
$array['element']['ospauth']['default'] = 'no';

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
$array['element']['externrefresh']['value'] = array(1 => 1,10 => 10,20 => 20,30 => 30,
						    40 => 40,50 => 50,60 => 1,70 => array(1,10),
						    80 => array(1,20),90 => array(1,30),100 => array(1,40),
						    110 => array(1,50),120 => 2);
$array['element']['externrefresh']['default'] = 10;

$array['element']['outboundproxy'] = array();
$array['element']['outboundproxy']['default'] = '';
$array['element']['outboundproxy']['null'] = true;

$array['element']['outboundproxyport'] = array();
$array['element']['outboundproxyport']['default'] = '';
$array['element']['outboundproxyport']['null'] = true;

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
					XIVO_SRE_IPBX_AST_CODEC_AUDIO_G726,
					XIVO_SRE_IPBX_AST_CODEC_AUDIO_ADPCM,
					XIVO_SRE_IPBX_AST_CODEC_AUDIO_SLIN,
					XIVO_SRE_IPBX_AST_CODEC_AUDIO_LPC10,
					XIVO_SRE_IPBX_AST_CODEC_AUDIO_G729,
					XIVO_SRE_IPBX_AST_CODEC_AUDIO_SPEEX,
					XIVO_SRE_IPBX_AST_CODEC_AUDIO_ILBC,
					XIVO_SRE_IPBX_AST_CODEC_IMAGE_JPEG,
					XIVO_SRE_IPBX_AST_CODEC_IMAGE_PNG,
					XIVO_SRE_IPBX_AST_CODEC_VIDEO_H261,
					XIVO_SRE_IPBX_AST_CODEC_VIDEO_H263,
					XIVO_SRE_IPBX_AST_CODEC_VIDEO_H263P);
$array['element']['allow']['default'] = '';
$array['element']['allow']['null'] = true;

$array['element']['relaxdtmf'] = array();
$array['element']['relaxdtmf']['value'] = array('no','yes');
$array['element']['relaxdtmf']['default'] = 'no';
$array['element']['relaxdtmf']['set'] = true;

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

$array['element']['maxexpiry'] = array();
$array['element']['maxexpiry']['value'] = array(1 => 1,900 => 15,1800 => 30,2700 => 45,3600 => 1,
						4500 => array(1,15),5400 => array(1,30),
						6300 => array(1,45),7200 => 2);
$array['element']['maxexpiry']['default'] = 3600;

$array['element']['defaultexpiry'] = array();
$array['element']['defaultexpiry']['value'] = array(1 => 1,10 => 10,20 => 20,30 => 30,40 => 40,50 => 50,60 => 1,
						    70 => array(1,10),80 => array(1,20),90 => array(1,30),
						    100 => array(1,40),110 => array(1,50),120 => 2,130 => array(2,10),
						    140 => array(2,20),150 => array(2,30),160 => array(2,40),
						    170 => array(2,50),180 => 3,190 => array(3,10),200 => array(3,20),
						    210 => array(3,30),220 => array(3,40),230 => array(3,50),240 => 4,
						    300 => 5,600 => 10,900 => 15,1200 => 20,1800 => 30,2400 => 40,
						    2700 => 45,3000 => 50,3600 => 1,4500 => array(1,15),
						    5400 => array(1,30),6300 => array(1,45),7200 => 2);
$array['element']['defaultexpiry']['default'] = 120;

$array['element']['registertimeout'] = array();
$array['element']['registertimeout']['value'] = array(1 => 1,10 => 10,20 => 20,30 => 30,
						    40 => 40,50 => 50,60 => 1,70 => array(1,10),
						    80 => array(1,20),90 => array(1,30),100 => array(1,40),
						    110 => array(1,50),120 => 2);
$array['element']['registertimeout']['default'] = 20;

$array['element']['registerattempts'] = array();
$array['element']['registerattempts']['value'] = array(0 => 0,10 => 10,20 => 20,30 => 30,
						    40 => 40,50 => 50,60 => 1,70 => array(1,10),
						    80 => array(1,20),90 => array(1,30),100 => array(1,40),
						    110 => array(1,50),120 => 2);
$array['element']['registerattempts']['default'] = 0;

$array['element']['notifyringing'] = array();
$array['element']['notifyringing']['value'] = array('no','yes');
$array['element']['notifyringing']['default'] = 'yes';
$array['element']['notifyringing']['set'] = true;

$array['element']['context'] = array();
$array['element']['context']['default'] = 'default';
$array['element']['context']['setdef'] = true;

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

$array['element']['musiconhold'] = array();
$array['element']['musiconhold']['default'] = 'default';
$array['element']['musiconhold']['setdef'] = true;

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

$array['element']['canreinvite'] = array();
$array['element']['canreinvite']['value'] = array('no','yes','update');
$array['element']['canreinvite']['default'] = 'yes';

$array['element']['insecure'] = array();
$array['element']['insecure']['value'] = array('port' => 'port','invite' => 'invite','port,invite' => 'all');
$array['element']['insecure']['default'] = '';
$array['element']['insecure']['null'] = true;

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

$array['element']['rtautoclear'] = array();
$array['element']['rtautoclear']['value'] = array('no' => 'no','yes' => 'yes',1 => 1,10 => 10,20 => 20,
						  30 => 30,40 => 40,50 => 50,60 => 1,
						  70 => array(1,10),80 => array(1,20),90 => array(1,30),
						  100 => array(1,40),110 => array(1,50),120 => 2,130 => array(2,10),
						  140 => array(2,20),150 => array(2,30),160 => array(2,40),
						  170 => array(2,50),180 => 3,190 => array(3,10),200 => array(3,20),
						  210 => array(3,30),220 => array(3,40),230 => array(3,50),240 => 4);
$array['element']['rtautoclear']['default'] = 'no'; 

$array['element']['register'] = array();
$array['element']['register']['default'] = '';

$array['element']['register_username'] = array();
$array['element']['register_username']['unset'] = true;

$array['element']['register_password'] = array();
$array['element']['register_password']['unset'] = true;

$array['element']['register_authuser'] = array();
$array['element']['register_authuser']['unset'] = true;

$array['element']['register_host'] = array();
$array['element']['register_host']['unset'] = true;

$array['element']['register_port'] = array();
$array['element']['register_port']['unset'] = true;

$array['element']['register_contact'] = array();
$array['element']['register_contact']['unset'] = true;

$array['filter'] = array();
$array['filter']['bindport'] = array('set' => false,'chk' => 2,'cast' => 'uint','between' => array(1,65535));
$array['filter']['bindaddr'] = array('set' => false,'chk' => 2,'callback' => 'xivo_filter::chk_host_ipv4_strict','maxlen' => 64);
$array['filter']['videosupport'] = array('bool' => true);
$array['filter']['autocreatepeer'] = array('bool' => true);
$array['filter']['allowguest'] = array('key' => array('no','yes','osp'));
$array['filter']['promiscredir'] = array('bool' => true);
$array['filter']['autodomain'] = array('bool' => true);
$array['filter']['domain'] = array('set' => false,'chk' => 2,'callback' => 'xivo_filter::chk_host_ipv4_strict','maxlen' => 64);
$array['filter']['allowexternaldomains'] = array('bool' => true);
$array['filter']['usereqphone'] = array('bool' => true);
$array['filter']['realm'] = array('regexp' => '/[^"]/','maxlen' => 64);
$array['filter']['alwaysauthreject'] = array('bool' => true);
$array['filter']['useragent'] = array('cast' => 'sgraph','maxlen' => 80);
$array['filter']['checkmwi'] = array('key' => array(0,5,10,15,20,25,30,35,40,45,50,55,60));
$array['filter']['regcontext'] = array('set' => false,'chk' => 2,
				       'callback' => 'xivo_service_asterisk::chk_context',
				       'callbackparam' => array(XIVO_SRE_IPBX_AST_CONTEXT_AUTHENTIFICATION));
$array['filter']['callerid'] = array('regexp' => '/^[a-z0-9_-]+$/i','maxlen' => 80);
$array['filter']['fromdomain'] = array('set' => false,'chk' => 2,'regexp' => '/^[a-z0-9_-\.]+$/i','maxlen' => 80);
$array['filter']['sipdebug'] = array('bool' => true);
$array['filter']['dumphistory'] = array('bool' => true);
$array['filter']['recordhistory'] = array('bool' => true);
$array['filter']['callevents'] = array('bool' => true);
$array['filter']['tos'] = array('key' => array('none','lowdelay','throughput','reliability','mincost',
					  1,2,3,4,5,6,7,8,9,10,
					  11,12,13,14,15,16,17,18,19,20,
					  21,22,23,24,25,26,27,28,29,30,31));
$array['filter']['ospauth'] = array('key' => array('no','proxy','gateway','exclusive'));
$array['filter']['localnet'] = array('set' => false,'chk' => 2,'regexp' => '/^[0-9\.\/]+$/','maxlen' => 128);
$array['filter']['externip'] = array('set' => false,'chk' => 2,'callback' => 'xivo_filter::chk_host_ipv4_strict','maxlen' => 64);
$array['filter']['externhost'] = array('set' => false,'chk' => 2,'callback' => 'xivo_filter::chk_host_ipv4_strict','maxlen' => 64);
$array['filter']['externrefresh'] = array('key' => array(1,10,20,30,40,50,60,70,80,90,100,110,120));
$array['filter']['outboundproxy'] = array('set' => false,'chk' => 2,'callback' => 'xivo_filter::chk_host_ipv4_strict','maxlen' => 64);
$array['filter']['outboundproxyport'] = array('set' => false,'chk' => 2,'cast' => 'uint','between' => array(1,65535));
$array['filter']['disallow'] = array('set' => false,'chk' => 2,'key' => array('all'));
$array['filter']['allow'] = array('set' => false,'chk' => 2,'value' => array(
						XIVO_SRE_IPBX_AST_CODEC_AUDIO_G723,
						XIVO_SRE_IPBX_AST_CODEC_AUDIO_GSM,
						XIVO_SRE_IPBX_AST_CODEC_AUDIO_ULAW,
						XIVO_SRE_IPBX_AST_CODEC_AUDIO_ALAW,
						XIVO_SRE_IPBX_AST_CODEC_AUDIO_G726,
						XIVO_SRE_IPBX_AST_CODEC_AUDIO_ADPCM,
						XIVO_SRE_IPBX_AST_CODEC_AUDIO_SLIN,
						XIVO_SRE_IPBX_AST_CODEC_AUDIO_LPC10,
						XIVO_SRE_IPBX_AST_CODEC_AUDIO_G729,
						XIVO_SRE_IPBX_AST_CODEC_AUDIO_SPEEX,
						XIVO_SRE_IPBX_AST_CODEC_AUDIO_ILBC,
						XIVO_SRE_IPBX_AST_CODEC_IMAGE_JPEG,
						XIVO_SRE_IPBX_AST_CODEC_IMAGE_PNG,
						XIVO_SRE_IPBX_AST_CODEC_VIDEO_H261,
						XIVO_SRE_IPBX_AST_CODEC_VIDEO_H263,
						XIVO_SRE_IPBX_AST_CODEC_VIDEO_H263P));
$array['filter']['relaxdtmf'] = array('bool' => true);
$array['filter']['compactheaders'] = array('bool' => true);
$array['filter']['rtptimeout'] = array('key' => array(0,5,10,15,20,25,30,35,40,45,50,55,60));
$array['filter']['rtpholdtimeout'] = array('key' => array(0,5,10,15,20,25,30,35,40,45,50,55,60));
$array['filter']['rtpkeepalive'] = array('key' => array(0,5,10,15,20,25,30,35,40,45,50,55,60));
$array['filter']['notifymimetype'] = array('set' => false,'chk' => 2,'maxlen' => 80,
					   'regexp' => '#^(?:text|application)/[a-zA-Z][a-zA-Z0-9][a-zA-Z0-9\+][a-zA-Z0-9\-\+\.]*$#');
$array['filter']['srvlookup'] = array('bool' => true);
$array['filter']['pedantic'] = array('bool' => true);
$array['filter']['maxexpiry'] = array('key' => array(1,900,1800,2700,3600,4500,5400,6300,7200));
$array['filter']['defaultexpiry'] = array('key' => array(1,10,20,30,40,50,60,
							 70,80,90,100,110,120,
							 130,140,150,160,170,
							 180,190,200,210,220,230,
							 240,300,600,900,1200,1800,
							 2400,2700,3000,3600,4500,
							 5400,6300,7200));
$array['filter']['registertimeout'] = array('key' => array(1,10,20,30,40,50,60,70,80,90,100,110,120));
$array['filter']['registerattempts'] = array('key' => array(0,10,20,30,40,50,60,70,80,90,100,110,120));
$array['filter']['notifyringing'] = array('bool' => true);
$array['filter']['context'] = array('set' => false,'chk' => 2,
				    'callback' => 'xivo_service_asterisk::chk_context',
				    'callbackparam' => array(XIVO_SRE_IPBX_AST_CONTEXT_AUTHENTIFICATION));
$array['filter']['nat'] = array('key' => array('no','yes','never','route'));
$array['filter']['dtmfmode'] = array('key' => array('rfc2833','inband','info','auto'));
$array['filter']['qualify'] = array('key' => array('no','yes',500,1000,1500,2000,2500,3000,3500,4000));
$array['filter']['useclientcode'] = array('bool' => true);
$array['filter']['progressinband'] = array('key' => array('yes','no','never'));
$array['filter']['language'] = array('key' => array('en','fr'));
$array['filter']['musiconhold'] = array('set' => false,'chk' => 2,'maxlen' => 20);
$array['filter']['vmexten'] = array('regexp' => '/^[0-9#\*]+$/','maxlen' => 40);
$array['filter']['trustrpid'] = array('bool' => true);
$array['filter']['sendrpid'] = array('bool' => true);
$array['filter']['canreinvite'] = array('key' => array('yes','no','update'));
$array['filter']['insecure'] = array('set' => false,'chk' => 2,'key' => array('port','invite','port,invite'));
$array['filter']['rtcachefriends'] = array('bool' => true);
$array['filter']['rtupdate'] = array('bool' => true);
$array['filter']['ignoreregexpire'] = array('bool' => true);
$array['filter']['rtautoclear'] = array('yes','no',1,10,20,30,40,50,60,70,80,90,
					100,110,120,130,140,150,160,170,180,
					190,200,210,220,230,240);

$array['filter']['register_username'] = array('set' => false,'regexp' => '/^[a-z0-9_@\.-]+$/i','maxlen' => 80);
$array['filter']['register_password'] = array('set' => false,'chk' => 2,'regexp' => '/^[a-z0-9_\.-]+$/i','maxlen' => 80);
$array['filter']['register_authuser'] = array('set' => false,'chk' => 2,'regexp' => '/^[a-z0-9_\.-]+$/i','maxlen' => 80);
$array['filter']['register_host'] = array('set' => false,'callback' => 'xivo_filter::chk_host_ipv4_strict','maxlen' => 64);
$array['filter']['register_port'] = array('set' => false,'chk' => 2,'cast' => 'uint','between' => array(1,65535));
$array['filter']['register_contact'] = array('set' => false,'chk' => 2,'regexp' => '/^[a-z0-9]+$/i','maxlen' => 250);

?>

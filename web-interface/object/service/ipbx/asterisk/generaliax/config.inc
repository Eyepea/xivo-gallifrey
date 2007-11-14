<?php

$array = array();
$array['element'] = array();

$array['element']['bindport'] = array();
$array['element']['bindport']['default'] = 4569;
$array['element']['bindport']['setdef'] = true;

$array['element']['bindaddr'] = array();
$array['element']['bindaddr']['default'] = '0.0.0.0';
$array['element']['bindaddr']['setdef'] = true;

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
$array['element']['minregexpire']['value'] = array(0 => 0,10 => 10,20 => 20,30 => 30,
						    40 => 40,50 => 50,60 => 1,70 => array(1,10),
						    80 => array(1,20),90 => array(1,30),100 => array(1,40),
						    110 => array(1,50),120 => 2);
$array['element']['minregexpire']['default'] = 60;

$array['element']['maxregexpire'] = array();
$array['element']['maxregexpire']['value'] = array(0 => 0,10 => 10,20 => 20,30 => 30,
						    40 => 40,50 => 50,60 => 1,70 => array(1,10),
						    80 => array(1,20),90 => array(1,30),100 => array(1,40),
						    110 => array(1,50),120 => 2);
$array['element']['maxregexpire']['default'] = 60;

$array['element']['bandwidth'] = array();
$array['element']['bandwidth']['value'] = array('low','medium','high');
$array['element']['bandwidth']['default'] = 'high';

$array['element']['tos'] = array();
$array['element']['tos']['value'] = array('none','lowdelay','throughput','reliability','mincost',
					  1,2,3,4,5,6,7,8,9,10,
					  11,12,13,14,15,16,17,18,19,20,
					  21,22,23,24,25,26,27,28,29,30,31);
$array['element']['tos']['default'] = 'lowdelay';

$array['element']['jitterbuffer'] = array();
$array['element']['jitterbuffer']['value'] = array('no','yes');
$array['element']['jitterbuffer']['default'] = 'no';
$array['element']['jitterbuffer']['set'] = true;

$array['element']['forcejitterbuffer'] = array();
$array['element']['forcejitterbuffer']['value'] = array('no','yes');
$array['element']['forcejitterbuffer']['default'] = 'no';
$array['element']['forcejitterbuffer']['set'] = true;

$array['element']['dropcount'] = array();
$array['element']['dropcount']['value'] = array(0,1,2,3,4,5,6,7,8,9,10);
$array['element']['dropcount']['default'] = 3;

$array['element']['maxjitterbuffer'] = array();
$array['element']['maxjitterbuffer']['value'] = array(0,250,500,750,1000,1250,1500,1750,2000);
$array['element']['maxjitterbuffer']['default'] = 1000;

$array['element']['maxjitterinterps'] = array();
$array['element']['maxjitterinterps']['value'] = array(0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20);
$array['element']['maxjitterinterps']['default'] = 10;

$array['element']['resyncthreshold'] = array();
$array['element']['resyncthreshold']['value'] = array(-1,0,250,500,750,1000,1250,1500,1750,2000);
$array['element']['resyncthreshold']['default'] = 1000;

$array['element']['minexcessbuffer'] = array();
$array['element']['minexcessbuffer']['value'] = array(0,5,10,15,20,25,30,35,40,45,50,55,60,65,70,75,80,85,90,95,100);
$array['element']['minexcessbuffer']['default'] = 10;

$array['element']['maxexcessbuffer'] = array();
$array['element']['maxexcessbuffer']['value'] = array(0,5,10,15,20,25,30,35,40,45,50,55,60,65,70,75,80,85,90,95,100);
$array['element']['maxexcessbuffer']['default'] = 50;

$array['element']['jittershrinkrate'] = array();
$array['element']['jittershrinkrate']['value'] = array(0,1,2,3,4,5,6,7,8,9,10);
$array['element']['jittershrinkrate']['default'] = 2;

$array['element']['accountcode'] = array();
$array['element']['accountcode']['default'] = '';
$array['element']['accountcode']['null'] = true;

$array['element']['amaflags'] = array();
$array['element']['amaflags']['value'] = array(
				XIVO_SRE_IPBX_AST_CDR_AMAFLAGS_META_DEFAULT,
				XIVO_SRE_IPBX_AST_CDR_AMAFLAGS_META_OMIT,
				XIVO_SRE_IPBX_AST_CDR_AMAFLAGS_META_BILLING,
				XIVO_SRE_IPBX_AST_CDR_AMAFLAGS_META_DOCUMENTATION);
$array['element']['amaflags']['default'] = XIVO_SRE_IPBX_AST_CDR_AMAFLAGS_META_DEFAULT;

$array['element']['mailboxdetail'] = array();
$array['element']['mailboxdetail']['value'] = array('no','yes');
$array['element']['mailboxdetail']['default'] = 'yes';
$array['element']['mailboxdetail']['set'] = true;

$array['element']['notransfer'] = array();
$array['element']['notransfer']['value'] = array('no','yes');
$array['element']['notransfer']['default'] = 'no';
$array['element']['notransfer']['set'] = true;

$array['element']['language'] = array();
$array['element']['language']['value'] = array('en','fr');
$array['element']['language']['default'] = 'fr';

$array['element']['encryption'] = array();
$array['element']['encryption']['value'] = array('no','yes','aes128');
$array['element']['encryption']['default'] = 'no';

$array['element']['maxauthreq'] = array();
$array['element']['maxauthreq']['value'] = array(0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20);
$array['element']['maxauthreq']['default'] = 0;

$array['element']['codecpriority'] = array();
$array['element']['codecpriority']['value'] = array('disabled','host','caller','reqonly');
$array['element']['codecpriority']['default'] = 'host';

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

$array['element']['rtcachefriends'] = array();
$array['element']['rtcachefriends']['value'] = array('no','yes');
$array['element']['rtcachefriends']['default'] = 'yes';
$array['element']['rtcachefriends']['set'] = true;

$array['element']['rtupdate'] = array();
$array['element']['rtupdate']['value'] = array('no','yes');
$array['element']['rtupdate']['default'] = 'no';
$array['element']['rtupdate']['set'] = true;

$array['element']['rtignoreregexpire'] = array();
$array['element']['rtignoreregexpire']['value'] = array('no','yes');
$array['element']['rtignoreregexpire']['default'] = 'no';
$array['element']['rtignoreregexpire']['set'] = true;

$array['element']['rtautoclear'] = array();
$array['element']['rtautoclear']['value'] = array('no' => 'no','yes' => 'yes',1 => 1,10 => 10,20 => 20,30 => 30,40 => 40,50 => 50,60 => 1,
						    70 => array(1,10),80 => array(1,20),90 => array(1,30),
						    100 => array(1,40),110 => array(1,50),120 => 2,130 => array(2,10),
						    140 => array(2,20),150 => array(2,30),160 => array(2,40),
						    170 => array(2,50),180 => 3,190 => array(3,10),200 => array(3,20),
						    210 => array(3,30),220 => array(3,40),230 => array(3,50),240 => 4);
$array['element']['rtautoclear']['default'] = 'yes'; 

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

$array['element']['register'] = array();
$array['element']['register']['default'] = '';

$array['element']['register_username'] = array();
$array['element']['register_username']['unset'] = true;

$array['element']['register_password'] = array();
$array['element']['register_password']['unset'] = true;

$array['element']['register_host'] = array();
$array['element']['register_host']['unset'] = true;

$array['element']['register_port'] = array();
$array['element']['register_port']['unset'] = true;

$array['filter'] = array();
$array['filter']['bindport'] = array('set' => false,'chk' => 2,'cast' => 'uint','between' => array(1,65535));
$array['filter']['bindaddr'] = array('set' => false,'chk' => 2,'func' => 'xivo_filter::chk_host_ipv4_strict','maxlen' => 64);
$array['filter']['iaxcompat'] = array('bool' => true);
$array['filter']['authdebug'] = array('bool' => true);
$array['filter']['delayreject'] = array('bool' => true);
$array['filter']['trunkfreq'] = array('key' => array(10,15,20,25,30,35,40,45,50,55,60,65,70,75,80,85,90,95,100));
$array['filter']['trunktimestamps'] = array('bool' => true);
$array['filter']['regcontext'] = array('set' => false,'chk' => 2,'maxlen' => 39,
					'regexp' => '/^(?!macro-)[a-z0-9_-]+$/',
					'nokey' => array('general','global','globals','hints','features'));
$array['filter']['minregexpire'] = array('key' => array(0,10,20,30,40,50,60,70,80,90,100,110,120));
$array['filter']['maxregexpire'] = array('key' => array(0,10,20,30,40,50,60,70,80,90,100,110,120));
$array['filter']['bandwidth'] = array('key' => array('low','medium','high'));
$array['filter']['tos'] = array('key' => array('none','lowdelay','throughput','reliability','mincost',
					  1,2,3,4,5,6,7,8,9,10,
					  11,12,13,14,15,16,17,18,19,20,
					  21,22,23,24,25,26,27,28,29,30,31));
$array['filter']['jitterbuffer'] = array('bool' => true);
$array['filter']['forcejitterbuffer'] = array('bool' => true);
$array['filter']['dropcount'] = array('key' => array(0,1,2,3,4,5,6,7,8,9,10));
$array['filter']['maxjitterbuffer'] = array('key' => array(0,250,500,750,1000,1250,1500,1750,2000));
$array['filter']['maxjitterinterps'] = array('key' => array(0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20));
$array['filter']['resyncthreshold'] = array('key' => array(-1,0,250,500,750,1000,1250,1500,1750,2000));
$array['filter']['minexcessbuffer'] = array('key' => array(0,5,10,15,20,25,30,35,40,45,50,55,60,65,70,75,80,85,90,95,100));
$array['filter']['maxexcessbuffer'] = array('key' => array(0,5,10,15,20,25,30,35,40,45,50,55,60,65,70,75,80,85,90,95,100));
$array['filter']['jittershrinkrate'] = array('key' => array(0,1,2,3,4,5,6,7,8,9,10));
$array['filter']['accountcode'] = array('set' => false,'chk' => 2,'maxlen' => 20,'regexp' => '/^[a-z0-9_-]+$/i');
$array['filter']['amaflags'] = array('key' => array(
				XIVO_SRE_IPBX_AST_CDR_AMAFLAGS_META_DEFAULT,
				XIVO_SRE_IPBX_AST_CDR_AMAFLAGS_META_OMIT,
				XIVO_SRE_IPBX_AST_CDR_AMAFLAGS_META_BILLING,
				XIVO_SRE_IPBX_AST_CDR_AMAFLAGS_META_DOCUMENTATION));
$array['filter']['mailboxdetail'] = array('bool' => true);
$array['filter']['notransfer'] = array('bool' => true);
$array['filter']['language'] = array('key' => array('en','fr'));
$array['filter']['encryption'] = array('key' => array('no','yes','aes128'));
$array['filter']['maxauthreq'] = array('key' => array(0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20));
$array['filter']['codecpriority'] = array('key' => array('disabled','host','caller','reqonly'));
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
$array['filter']['rtcachefriends'] = array('bool' => true);
$array['filter']['rtupdate'] = array('bool' => true);
$array['filter']['rtignoreregexpire'] = array('bool' => true);
$array['filter']['rtautoclear'] = array('yes','no',1,10,20,30,40,50,60,70,80,90,
					100,110,120,130,140,150,160,170,180,
					190,200,210,220,230,240);
$array['filter']['pingtime'] = array('key' => array(0,5,10,15,20,25,30,35,40,45,50,55,60));
$array['filter']['lagrqtime'] = array('key' => array(0,5,10,15,20,25,30,35,40,45,50,55,60));
$array['filter']['nochecksums'] = array('bool' => true);
$array['filter']['autokill'] = array('key' => array('no','yes',500,1000,1500,2000,2500,3000,3500,4000));

$array['filter']['register_username'] = array('set' => false,'regexp' => '/^[a-z0-9_@\.-]+$/i','maxlen' => 80);
$array['filter']['register_password'] = array('set' => false,'chk' => 2,'regexp' => '/^([a-z0-9_\.-]+|\[[a-z0-9\+\/]+={0,2}\])$/i','maxlen' => 80);
$array['filter']['register_host'] = array('set' => false,'func' => 'xivo_filter::chk_host_ipv4_strict','maxlen' => 64);
$array['filter']['register_port'] = array('set' => false,'chk' => 2,'cast' => 'uint','between' => array(1,65535));

?>

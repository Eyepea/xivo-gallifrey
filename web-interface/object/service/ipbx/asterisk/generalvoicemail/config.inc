<?php

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
$array['element']['minmessage']['value'] = array(0 => 0,1 => 1,2 => 2,3 => 3,4 => 4,5 => 5,6 => 6,7 => 7,8 => 8,
						    9 => 9,10 => 10,20 => 20,30 => 30,40 => 40,50 => 50,60 => 1,
						    70 => array(1,10),80 => array(1,20),90 => array(1,30),
						    100 => array(1,40),110 => array(1,50),120 => 2,130 => array(2,10),
						    140 => array(2,20),150 => array(2,30),160 => array(2,40),
						    170 => array(2,50),180 => 3,190 => array(3,10),200 => array(3,20),
						    210 => array(3,30),220 => array(3,40),230 => array(3,50),240 => 4,
						    250 => array(4,10),260 => array(4,20),270 => array(4,30),
						    280 => array(4,40),290 => array(4,50),300 => 5,310 => array(5,10),
						    320 => array(5,20),330 => array(5,30),340 => array(5,40),
						    350 => array(5,50),360 => 6);
$array['element']['minmessage']['default'] = 2;

$array['element']['maxmessage'] = array();
$array['element']['maxmessage']['value'] = array(0 => 0,1 => 1,2 => 2,3 => 3,4 => 4,5 => 5,6 => 6,7 => 7,8 => 8,
						    9 => 9,10 => 10,20 => 20,30 => 30,40 => 40,50 => 50,60 => 1,
						    70 => array(1,10),80 => array(1,20),90 => array(1,30),
						    100 => array(1,40),110 => array(1,50),120 => 2,130 => array(2,10),
						    140 => array(2,20),150 => array(2,30),160 => array(2,40),
						    170 => array(2,50),180 => 3,190 => array(3,10),200 => array(3,20),
						    210 => array(3,30),220 => array(3,40),230 => array(3,50),240 => 4,
						    250 => array(4,10),260 => array(4,20),270 => array(4,30),
						    280 => array(4,40),290 => array(4,50),300 => 5,310 => array(5,10),
						    320 => array(5,20),330 => array(5,30),340 => array(5,40),
						    350 => array(5,50),360 => 6);
$array['element']['maxmessage']['default'] = 0;

$array['element']['maxsilence'] = array();
$array['element']['maxsilence']['value'] = array(1,2,3,4,5,6,7,8,9,10,
						11,12,13,14,15,16,17,18,19,20,
						21,22,23,24,25,26,27,28,29,30);
$array['element']['maxsilence']['default'] = 1;

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
$array['element']['maxlogins']['value'] = array(1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20);
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
$array['element']['cidinternalcontexts']['default'] = 'default';
$array['element']['cidinternalcontexts']['forcedef'] = true;

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

$array['element']['maxgreet'] = array();
$array['element']['maxgreet']['value'] = array(0 => 0,1 => 1,2 => 2,3 => 3,4 => 4,5 => 5,6 => 6,7 => 7,8 => 8,
						    9 => 9,10 => 10,20 => 20,30 => 30,40 => 40,50 => 50,60 => 1,
						    70 => array(1,10),80 => array(1,20),90 => array(1,30),
						    100 => array(1,40),110 => array(1,50),120 => 2,130 => array(2,10),
						    140 => array(2,20),150 => array(2,30),160 => array(2,40),
						    170 => array(2,50),180 => 3,190 => array(3,10),200 => array(3,20),
						    210 => array(3,30),220 => array(3,40),230 => array(3,50),240 => 4,
						    250 => array(4,10),260 => array(4,20),270 => array(4,30),
						    280 => array(4,40),290 => array(4,50),300 => 5,310 => array(5,10),
						    320 => array(5,20),330 => array(5,30),340 => array(5,40),
						    350 => array(5,50),360 => 6);
$array['element']['maxgreet']['default'] = 0;

$array['element']['skipms'] = array();
$array['element']['skipms']['value'] = array(1000 => 1,2000 => 2,3000 => 3,4000 => 4,5000 => 5,
						6000 => 6,7000 => 7,8000 => 8,9000 => 9,
						10000 => 10,15000 => 15,20000 => 20,25000 => 25,
						30000 => 30,35000 => 35,40000 => 40,
						45000 => 45,50000 => 50,55000 => 55,60000 => 60);
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
$array['element']['emaildateformat']['default'] = '%A, %B %d, %Y à %r';
$array['element']['emaildateformat']['setdef'] = true;

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
$array['filter']['minmessage'] = array('key' => array(0,1,2,3,4,5,6,7,8,9,
							10,20,30,40,50,60,70,80,90,
							100,110,120,130,140,150,160,170,180,190,
							200,210,220,230,240,250,260,270,280,290,
							300,310,320,330,340,350,360));
$array['filter']['maxmessage'] = array('key' => array(0,1,2,3,4,5,6,7,8,9,
							10,20,30,40,50,60,70,80,90,
							100,110,120,130,140,150,160,170,180,190,
							200,210,220,230,240,250,260,270,280,290,
							300,310,320,330,340,350,360));
$array['filter']['maxsilence'] = array('key' => array(1,2,3,4,5,6,7,8,9,10,
						11,12,13,14,15,16,17,18,19,20,
						21,22,23,24,25,26,27,28,29,30));
$array['filter']['review'] = array('bool' => true);
$array['filter']['operator'] = array('bool' => true);
$array['filter']['format'] = array('set' => false,'chk' => 2,'value' => array(
					XIVO_SRE_IPBX_AST_FORMAT_AUDIO_WAV,
					XIVO_SRE_IPBX_AST_FORMAT_AUDIO_WAV49,
					XIVO_SRE_IPBX_AST_FORMAT_AUDIO_GSM,
					XIVO_SRE_IPBX_AST_FORMAT_AUDIO_OGG,
					XIVO_SRE_IPBX_AST_FORMAT_AUDIO_ULAW,
					XIVO_SRE_IPBX_AST_FORMAT_AUDIO_ALAW,
					XIVO_SRE_IPBX_AST_FORMAT_AUDIO_AU,
					XIVO_SRE_IPBX_AST_FORMAT_AUDIO_VOX,
					XIVO_SRE_IPBX_AST_FORMAT_AUDIO_SLIN));
$array['filter']['maxlogins'] = array('key' => array(1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20));
$array['filter']['envelope'] = array('bool' => true);
$array['filter']['saycid'] = array('bool' => true);
$array['filter']['cidinternalcontexts'] = array('set' => false,'chk' => 2,'key' => array('default'));
$array['filter']['sayduration'] = array('bool' => true);
$array['filter']['saydurationm'] = array('key' => array(1,2,3,4,5,10,15,20,25,30,35,40,45,50,55,60));
$array['filter']['forcename'] = array('bool' => true);
$array['filter']['forcegreetings'] = array('bool' => true);
$array['filter']['maxgreet'] = array('key' => array(0,1,2,3,4,5,6,7,8,9,
							10,20,30,40,50,60,70,80,90,
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
//$array['filter']['mailcmd'] = array('set' => false,'chk' => 2,'cast' => 'sgraph','maxlen' => 160);
$array['filter']['mailcmd'] = array('set' => false);
$array['filter']['serveremail'] = array('set' => false,'chk' => 2,'regexp' => '/^[a-z0-9@\-_\.]+$/i','maxlen' => 80);
$array['filter']['charset'] = array('set' => false);
$array['filter']['fromstring'] = array('set' => false,'chk' => 2,'regexp' => '/^[a-z0-9\-_\. ]+$/i','maxlen' => 100);
$array['filter']['emaildateformat'] = array('set' => false,'chk' => 2,'cast' => 'sgraph','maxlen' => 32);
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
$array['filter']['odbcstorage'] = array('set' => false,'chk' => 2,'cast' => 'sgraph','maxlen' => 80);
$array['filter']['odbctable'] = array('set' => false,'chk' => 2,'cast' => 'sgraph','maxlen' => 80);

?>

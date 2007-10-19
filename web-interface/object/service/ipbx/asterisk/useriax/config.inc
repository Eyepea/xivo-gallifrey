<?php

$array = array();

$array['initialized'] = 'iax';

$array['element'] = array();

$array['element']['name'] = array();
$array['element']['name']['default'] = '';

$array['element']['host'] = array();
$array['element']['host']['value'] = array('dynamic');
$array['element']['host']['default'] = 'dynamic';
$array['element']['host']['setdef'] = true;

$array['element']['amaflags'] = array();
$array['element']['amaflags']['value'] = array(
					XIVO_SRE_IPBX_AST_CDR_AMAFLAGS_META_DEFAULT,
					XIVO_SRE_IPBX_AST_CDR_AMAFLAGS_META_OMIT,
					XIVO_SRE_IPBX_AST_CDR_AMAFLAGS_META_BILLING,
					XIVO_SRE_IPBX_AST_CDR_AMAFLAGS_META_DOCUMENTATION);
$array['element']['amaflags']['default'] = XIVO_SRE_IPBX_AST_CDR_AMAFLAGS_META_DOCUMENTATION;
$array['element']['amaflags']['setdef'] = true;

$array['element']['qualify'] = array();
$array['element']['qualify']['value'] = array('no','yes');
$array['element']['qualify']['default'] = 'no';
$array['element']['qualify']['set'] = true;

$array['element']['disallow'] = array();
$array['element']['disallow']['value'] = array('all');
$array['element']['disallow']['default'] = 'all';
$array['element']['disallow']['null'] = true;

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

$array['element']['port'] = array();
$array['element']['port']['default'] = '';
$array['element']['port']['null'] = true;

$array['element']['ipaddr'] = array();
$array['element']['ipaddr']['default'] = '';
$array['element']['ipaddr']['null'] = true;

$array['element']['callgroup'] = array();
$array['element']['callgroup']['default'] = '';

$array['element']['context'] = array();
$array['element']['context']['default'] = 'default';
$array['element']['context']['setdef'] = true;

$array['element']['username'] = array();
$array['element']['username']['default'] = '';

$array['element']['mailbox'] = array();
$array['element']['mailbox']['default'] = '';

$array['element']['host-static'] = array();
$array['element']['host-static']['default'] = '';
$array['element']['host-static']['unset'] = true;

$array['element']['host-dynamic'] = array();
$array['element']['host-dynamic']['value'] = array('dynamic','static');
$array['element']['host-dynamic']['unset'] = true;

$array['element']['call-limit'] = array();
$array['element']['call-limit']['default'] = 0;
$array['element']['call-limit']['setdef'] = true;

$array['filter'] = array();
$array['filter']['callerid'] = array('chk' => 2,'maxlen' => 160,'notag' => false);
$array['filter']['name'] = array('minlen' => 1,'maxlen' => 40,'regexp' => '/^[a-z0-9_-]+$/i');
$array['filter']['secret'] = array('set' => false,'chk' => 2,'minlen' => 1,'maxlen' => 80);
$array['filter']['host'] = array('set' => false,'chk' => 2,'key' => array('dynamic','static'),'strict' => false,'func' => 'xivo_filter::chk_host_ipv4_strict');
$array['filter']['amaflags'] = array('set' => false,'chk' => 2,'key' => array(
						XIVO_SRE_IPBX_AST_CDR_AMAFLAGS_META_DEFAULT,
						XIVO_SRE_IPBX_AST_CDR_AMAFLAGS_META_OMIT,
						XIVO_SRE_IPBX_AST_CDR_AMAFLAGS_META_BILLING,
						XIVO_SRE_IPBX_AST_CDR_AMAFLAGS_META_DOCUMENTATION));
$array['filter']['accountcode'] = array('set' => false,'chk' => 2,'maxlen' => 20);
$array['filter']['callgroup'] = array('set' => false,'chk' => 2,'regexp' => '/^([0-9]+[,]?)*$/','maxlen' => 180);
$array['filter']['context'] = array('set' => false,'chk' => 2,'nokey' => array('general','globals','initconfig','hints','features','zonemessages'),'regexp' => '/^(?!macro-)[a-z0-9_-]+$/','maxlen' => 39);
$array['filter']['call-limit'] = array('set' => false,'chk' => 2,'cast' => 'uint','between' => array(0,30));
$array['filter']['qualify'] = array('bool' => true);
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
$array['filter']['port'] = array('set' => false,'chk' => 2);
$array['filter']['ipaddr'] = array('set' => false,'chk' => 2);
$array['filter']['username'] = array('set' => false,'chk' => 2,'maxlen' => 80);
$array['filter']['mailbox'] = array('set' => false,'chk' => 2,'regexp' => '/^[0-9#\*]{1,40}(@[a-z0-9@_\-]{1,39})?$/','maxlen' => 80);

?>

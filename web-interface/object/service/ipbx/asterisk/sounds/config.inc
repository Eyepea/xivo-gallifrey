<?php

$array = array();

$array['option'] = array();

$array['option']['defdir'] = array();
$array['option']['defdir']['acd'] = 1;
$array['option']['defdir']['beep'] = 1;
$array['option']['defdir']['features'] = 1;
$array['option']['defdir']['goodbye'] = 1;
$array['option']['defdir']['monitor'] = 1;
$array['option']['defdir']['playback'] = 1;
$array['option']['defdir']['recordings'] = 1;

$array['option']['dir'] = array();
$array['option']['dir']['name'] = '/^[a-z0-9_-]{1,50}$/i';

$array['option']['file'] = array();
$array['option']['file']['name'] = '/^['.XIVO_SRE_IPBX_AST_REGEXP_AUDIO_FILE_PATTERN.']'.
				    XIVO_SRE_IPBX_AST_REGEXP_AUDIO_FILE_LENGTH.'\.[a-zA-Z0-9]{2,5}$/';
$array['option']['file']['extension'] = array();
$array['option']['file']['extension'][XIVO_SRE_IPBX_AST_FORMAT_AUDIO_WAV] = true;
$array['option']['file']['extension'][XIVO_SRE_IPBX_AST_FORMAT_AUDIO_WAV49] = true;
$array['option']['file']['extension'][XIVO_SRE_IPBX_AST_FORMAT_AUDIO_GSM] = true;
$array['option']['file']['extension'][XIVO_SRE_IPBX_AST_FORMAT_AUDIO_OGG] = true;
$array['option']['file']['extension'][XIVO_SRE_IPBX_AST_FORMAT_AUDIO_ULAW] = true;
$array['option']['file']['extension'][XIVO_SRE_IPBX_AST_FORMAT_AUDIO_ALAW] = true;
$array['option']['file']['extension'][XIVO_SRE_IPBX_AST_FORMAT_AUDIO_AU] = true;
$array['option']['file']['extension'][XIVO_SRE_IPBX_AST_FORMAT_AUDIO_VOX] = true;
$array['option']['file']['extension'][XIVO_SRE_IPBX_AST_FORMAT_AUDIO_SLIN] = true;
$array['option']['file']['extension'][XIVO_SRE_IPBX_AST_FORMAT_AUDIO_MP3] = true;

$array['filter'] = array();
$array['filter']['filename'] = array('regexp' => '/^['.XIVO_SRE_IPBX_AST_REGEXP_AUDIO_FILE_PATTERN.']'.
						 XIVO_SRE_IPBX_AST_REGEXP_AUDIO_FILE_LENGTH.'\.[a-zA-Z0-9]{2,5}$/','minlen' => 4,'maxlen' => 56); 
$array['filter']['dirname'] = array('regexp' => '/^[a-z0-9_-]+$/i','minlen' => 1,'maxlen' => 50); 

?>

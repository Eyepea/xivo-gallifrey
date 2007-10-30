<?php

$array = array();

$array['option'] = array();
$array['option']['dir'] = array();
$array['option']['dir']['name'] = '/^[a-z0-9_-]{1,50}$/i';

$array['option']['file'] = array();
$array['option']['file']['name'] = '/^['.XIVO_SRE_IPBX_AST_REGEXP_AUDIO_FILE_PATTERN.']'.
				    XIVO_SRE_IPBX_AST_REGEXP_AUDIO_FILE_LENGTH.'\.(wav|gsm)$/';
$array['option']['file']['extension'] = array();
$array['option']['file']['extension']['wav'] = array('audio/wav','regexp' => '/wav/i');
$array['option']['file']['extension']['gsm'] = array('application/octet-stream','audio/x-gsm');

$array['filter'] = array();
$array['filter']['filename'] = array('regexp' => '/^['.XIVO_SRE_IPBX_AST_REGEXP_AUDIO_FILE_PATTERN.']'.
						XIVO_SRE_IPBX_AST_REGEXP_AUDIO_FILE_LENGTH.'\.(wav|gsm)$/','minlen' => 1,'maxlen' => 54); 
$array['filter']['dirname'] = array('regexp' => '/^[a-z0-9_-]+$/i','minlen' => 1,'maxlen' => 50); 

?>

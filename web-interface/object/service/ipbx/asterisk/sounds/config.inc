<?php

$array = array();

$array['option'] = array();
$array['option']['dir'] = array();
$array['option']['dir']['name'] = '/^[a-z0-9_-]{1,50}$/i';

$array['option']['file'] = array();
$array['option']['file']['name'] = '/^['.XIVO_SRE_IPBX_AST_REGEX_AUDIO_FILE_PATTERN.']'.
				    XIVO_SRE_IPBX_AST_REGEX_AUDIO_FILE_LENGTH.'\.(wav|gsm)$/';
$array['option']['file']['extension'] = array();
$array['option']['file']['extension']['wav'] = true;
$array['option']['file']['extension']['gsm'] = array('application/octet-stream','audio/x-gsm');

$array['filter'] = array();
$array['filter']['filename'] = array('preg' => '/^['.XIVO_SRE_IPBX_AST_REGEX_AUDIO_FILE_PATTERN.']'.
						XIVO_SRE_IPBX_AST_REGEX_AUDIO_FILE_LENGTH.'\.(wav|gsm)$/','minlen' => 1,'maxlen' => 54); 
$array['filter']['dirname'] = array('preg' => '/^[a-z0-9_-]+$/i','minlen' => 1,'maxlen' => 50); 

?>

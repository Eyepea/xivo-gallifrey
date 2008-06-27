<?php

$form = &$this->get_module('form');

$element = $this->get_var('element');
$event = $this->get_var('event');
$action = $this->get_varra('dialaction',array($event,'action'));

echo $form->select(array('desc'		=> $this->bbf('fm_dialaction_actiontype'),
			 'name'		=> 'dialaction['.$event.'][actiontype]',
			 'labelid'	=> 'dialaction-'.$event.'-actiontype',
			 'bbf'		=> array('concatvalue','fm_dialaction_actiontype-opt-',null,XIVO_SRE_IPBX_LABEL),
			 'key'		=> false,
			 'default'	=> $element['dialaction']['actiontype']['default'],
			 'value'	=> $action),
		   $element['voicemenuevent']['actiontype'],
		   'onchange="xivo_ast_chg_dialaction(\''.$event.'\',this);"');

?>

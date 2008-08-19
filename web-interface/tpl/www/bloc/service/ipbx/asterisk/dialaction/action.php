<?php

$form = &$this->get_module('form');

$element = $this->get_var('element');
$event = $this->get_var('event');
$action = $this->get_varra('dialaction',array($event,'action'));

if($this->get_var('dialaction_from') === 'incall' && $event === 'answer'):
	$onchange = 'xivo_ast_incall_chg_dialaction_answer(this);';
else:
	$onchange = 'xivo_ast_chg_dialaction(\''.$event.'\',this);';
endif;

echo $form->select(array('desc'		=> $this->bbf('fm_dialaction_actiontype'),
			 'name'		=> 'dialaction['.$event.'][actiontype]',
			 'labelid'	=> 'dialaction-'.$event.'-actiontype',
			 'bbf'		=> array('concatvalue','fm_dialaction_actiontype-opt-',null,XIVO_SRE_IPBX_LABEL),
			 'key'		=> false,
			 'default'	=> $element['dialaction']['actiontype']['default'],
			 'value'	=> $action),
		   $element['dialaction']['actiontype']['value'],
		   'onchange="'.$onchange.'"');

?>

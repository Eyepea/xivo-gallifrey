<?php
$form = &$this->get_module('form');
$ipbx_engines = $this->get_var('ipbx-engines');
if(isset($_SESSION['_wizard']['ipbx-engine']))
	$ipbx_engine = $_SESSION['_wizard']['ipbx-engine'];
else
	$ipbx_engine = 'asterisk';

echo 
	$form->hidden(array('name' => 'fm_send',
						'value' => 1)),

	$form->hidden(array('name' => 'step',
						'value' => $this->get_var('step'))),
	
	$form->select(array('desc' => $this->bbf('fm-ipbx-engine'),
						'name' => 'ipbx-engine',
						'id' => 'ipbx-engine',
						'selected' => $ipbx_engine),
						$ipbx_engines);

?>

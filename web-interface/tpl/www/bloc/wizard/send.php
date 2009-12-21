<?php
$form = &$this->get_module('form');

echo
		$form->hidden(array('name'	=> 'step',
							'value'	=> $this->get_var('step')));

$url = "http://" . $_SESSION['_wizard']['server']['ip'] . "/";

if($this->get_var('send-result') === true)
{
	echo 
		$this->bbf('wz-send-success'), "<br><br>\n",
		$this->bbf('wz-send-xivo-wi-url'),
		"<a href=\"$url\">$url</a>";

}
else
	echo $this->bbf('wz-send-failed');

?>

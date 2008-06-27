<?php

$json = &$this->get_module('json');
die($this->get_var('callback').'('.$json->encode($this->get_var('info')).');');

?>

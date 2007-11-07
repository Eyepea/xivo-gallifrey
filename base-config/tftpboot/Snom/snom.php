<?php

$agent = $HTTP_SERVER_VARS['HTTP_USER_AGENT'];

$info = explode(';', $agent);

$phone_type = "snom";

if (strpos($agent, "snom300") > 0) {
	$phone_type = "snom300";
	} 
else if (strpos($agent, "snom320") > 0) {
	$phone_type = "snom320";
	} 
else if (strpos($agent, "snom360") > 0) {
	$phone_type = "snom360";
	}

if (isset($_GET['mac'])) {
	if (is_file("$phone_type-" . $_GET['mac'] . ".htm")) {
		include("$phone_type-" . $_GET['mac'] . ".htm");
		exit();
	} else {
		include("$phone_type.htm");
		exit();
	}
}

?>

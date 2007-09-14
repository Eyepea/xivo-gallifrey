<?php

require_once('xivo.php');

xivo_user::logout();
$_QRY->go($_HTML->url('index'));

?>

<?php

require_once('xivo.php');

xivo_user::logoff();
$_QRY->go($_HTML->url('index'));

?>

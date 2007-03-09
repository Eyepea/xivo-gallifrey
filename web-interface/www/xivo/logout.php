<?php

require_once('xivo.php');

xivo_user::logout();
xivo_go($_HTML->url('index'));

?>

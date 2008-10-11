<?php

$menu = &$this->get_module('menu');
$this->file_include('bloc/head');

?>
<div id="bc-body">

<div id="bc-head">
	<div id="b-tmenu">
<?php
	$menu->mk_top();
?>
	</div>
</div>
<div id="bc-main">
	<div id="b-lmenu">
<?php
	$menu->mk_left();
?>
	</div>
	<div id="bc-content">
		<div id="b-content">
<?php
	$this->mk_struct();
?>
		</div>
	</div>
</div>

<div id="bc-foot">
	<div id="b-bmenu">
<?php
	$menu->mk_bottom();
?>
	</div>
</div>
</div>
<?php

$this->file_include('bloc/foot');

?>

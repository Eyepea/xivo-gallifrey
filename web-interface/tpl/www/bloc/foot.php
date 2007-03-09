<?php
	$dhtml = &$this->get_module('dhtml');
	$dhtml->load_js('foot');
?>
	<h6 id="version-copyright"><?=XIVO_SOFT_LABEL?> - <?=$this->bbf('info_version');?> <?=XIVO_SOFT_VERSION?> | <?=$this->bbf('visit_for_information','<a href="http://'.XIVO_SOFT_URL.'" title="'.XIVO_SOFT_LABEL.'" target="_blank">'.XIVO_SOFT_URL.'</a>');?> | <?=$this->bbf('info_copyright','<a href="http://'.XIVO_CORP_URL.'" title="'.XIVO_CORP_LABEL.'" target="_blank">'.XIVO_CORP_LABEL.'</a>');?></h6>
	</body>
</html>

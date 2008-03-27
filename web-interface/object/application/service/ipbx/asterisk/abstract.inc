<?php

require_once(XIVO_PATH_OBJECT.XIVO_SEP_DIR.'application'.XIVO_SEP_DIR.'helper.inc');

class xivo_application_service_asterisk_abstract extends xivo_application_helper
{
	var $_service	= null;

	function xivo_application_service_asterisk_abstract(&$ipbx)
	{
		if(xivo_instanceof($ipbx,'xivo_service_asterisk') === false)
			trigger_error('Invalid service in '.__CLASS__,E_USER_ERROR);

		$this->_service = &$ipbx;
	}
}

?>

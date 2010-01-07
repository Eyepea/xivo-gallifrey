<?php

#
# XiVO Web-Interface
# Copyright (C) 2006-2010  Proformatique <technique@proformatique.com>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#

$dhtml = &$this->get_module('dhtml');
$dhtml->load_js('foot');

?>
		<h6 id="version-copyright">
<?php
		echo	XIVO_SOFT_LABEL,' - ',
			$this->bbf('info_version'),' ',
			XIVO_SOFT_VERSION,' "',XIVO_SOFT_CODENAME,'" | ',
			$this->bbf('visit_for_information',
				   '<a href="http://'.XIVO_SOFT_URL.'" title="'.XIVO_SOFT_LABEL.'" target="_blank">'.XIVO_SOFT_URL.'</a>'),' | ',
			$this->bbf('info_copyright',
				   array(2006,dwho_i18n::strftime_l('%Y',null),
				   '<a href="http://'.XIVO_CORP_URL.'" title="'.XIVO_CORP_LABEL.'" target="_blank">'.XIVO_CORP_LABEL.'</a>'));
?>
		</h6>
	</body>
</html>

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

	$url = $this->get_module('url');
?>
<div class="b-infos">
	<h3 class="sb-top xspan"><span class="span-left">&nbsp;</span><span class="span-center"><?=$this->bbf('title_content_name');?></span><span class="span-right">&nbsp;</span></h3>
	<div class="sb-content">
	<div>
		<?=$url->img_html('img/site/proformatique.png',XIVO_CORP_LABEL);?>
	</div>
<dl>
	<dt><?=$this->bbf('info_address');?></dt>
	<dd><?=XIVO_CORP_ADDRESS?></dd>
	<dd><?=XIVO_CORP_ZIPCODE?> <?=XIVO_CORP_CITY?></dd>
	<dd><?=XIVO_CORP_COUNTRY?></dd>
	<dt><?=$this->bbf('info_phone');?></dt>
	<dd><?=XIVO_CORP_PHONE?></dd>
	<dt><?=$this->bbf('info_fax');?></dt>
	<dd><?=XIVO_CORP_FAX?></dd>
	<dt><?=$this->bbf('info_e-mail');?></dt>
	<dd><?='<a href="mailto:',XIVO_CORP_EMAIL,'" title="',XIVO_CORP_EMAIL,'">',XIVO_CORP_EMAIL,'</a>'?></dd>
	<dt><?=$this->bbf('info_websites');?></dt>
	<dd><?='<a href="http://',XIVO_CORP_URL,'" title="',XIVO_CORP_LABEL,'" target="_blank">',XIVO_CORP_URL,'</a>'?></dd>
	<dd><?='<a href="http://',XIVO_SOFT_URL,'" title="',XIVO_SOFT_LABEL,'" target="_blank">',XIVO_SOFT_URL,'</a>'?></dd>
</dl>
	</div>
	<div class="sb-foot xspan"><span class="span-left">&nbsp;</span><span class="span-center">&nbsp;</span><span class="span-right">&nbsp;</span></div>
</div>

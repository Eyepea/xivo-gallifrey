<?php
	$switchboard_url = $this->bbf('download_soft_url_xivo-switchboard',XIVO_SOFT_URL);
	$client_url = $this->bbf('download_soft_url_xivo-client',XIVO_SOFT_URL);
?>
<div class="b-infos">
	<h3 class="sb-top xspan"><span class="span-left">&nbsp;</span><span class="span-center"><?=$this->bbf('title_content_name');?></span><span class="span-right">&nbsp;</span></h3>
	<div class="sb-content">
<dl>
	<dt><?=$this->bbf('info_download_xivo-switchboard');?></dt>
	<dd><?='<a href="',$switchboard_url,'" title="',XIVO_SOFT_LABEL,'" target="_blank">',$switchboard_url,'</a>'?></dd>
	<dt><?=$this->bbf('info_download_xivo-client');?></dt>
	<dd><?='<a href="',$client_url,'" title="',XIVO_SOFT_LABEL,'" target="_blank">',$client_url,'</a>'?></dd>
</dl>
	</div>
	<div class="sb-foot xspan"><span class="span-left">&nbsp;</span><span class="span-center">&nbsp;</span><span class="span-right">&nbsp;</span></div>
</div>

<?php
	$download = $this->bbf('download_soft_url',XIVO_SOFT_URL);
?>
<div class="b-infos">
	<h3 class="sb-top xspan"><span class="span-left">&nbsp;</span><span class="span-center"><?=$this->bbf('title_content_name');?></span><span class="span-right">&nbsp;</span></h3>
	<div class="sb-content">
<dl>
	<dt><?=$this->bbf('info_download');?></dt>
	<dd><?='<a href="',$download,'" title="',XIVO_SOFT_LABEL,'" target="_blank">',$download,'</a>'?></dd>
</dl>
	</div>
	<div class="sb-foot xspan"><span class="span-left">&nbsp;</span><span class="span-center">&nbsp;</span><span class="span-right">&nbsp;</span></div>
</div>

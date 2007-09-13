<?php
	$url = &$this->get_module('url');
?>
<div id="index" class="b-infos">
	<h3 class="sb-top xspan"><span class="span-left">&nbsp;</span><span class="span-center"><?=$this->bbf('title_content_name');?></span><span class="span-right">&nbsp;</span></h3>
	<div class="sb-content">
		<div id="ipbx-logo">
			<?=$url->img_html('img/service/ipbx/asterisk.png',XIVO_SRE_IPBX_LABEL);?>
		</div>
		<dl>
			<dd><b><?=$this->bbf('info_service_label');?></b> <?=XIVO_SRE_IPBX_LABEL?></dd>
			<dd><b><?=$this->bbf('info_service_version');?></b> <?=XIVO_SRE_IPBX_VERSION?></dd>
			<dt><?=$this->bbf('quick_service');?></dt>
<?php
			if($this->chk_acl('pbx_settings','users') === true):
				echo '<dd>',$url->href_html($this->bbf('service_add_user'),'service/ipbx/pbx_settings/users','act=add'),'</dd>';
			endif;

			if($this->chk_acl('pbx_settings','groups') === true):
				echo '<dd>',$url->href_html($this->bbf('service_add_group'),'service/ipbx/pbx_settings/groups','act=add'),'</dd>';
			endif;
?>
		</dl>
		<div class="clearboth"></div>
	</div>
	<div class="sb-foot xspan"><span class="span-left">&nbsp;</span><span class="span-center">&nbsp;</span><span class="span-right">&nbsp;</span></div>
</div>

<?php
	$url = &$this->get_module('url');

	$userstat = $this->vars('userstat');
	$groupstat = $this->vars('groupstat');
	$queuestat = $this->vars('queuestat');
	$meetmestat = $this->vars('meetmestat');
?>
<div id="index" class="b-infos">
	<h3 class="sb-top xspan"><span class="span-left">&nbsp;</span><span class="span-center"><?=$this->bbf('title_content_name');?></span><span class="span-right">&nbsp;</span></h3>
	<div class="sb-content sb-list">
		<div id="ipbx-stats">
			<table border="0" cellpadding="0" cellspacing="0">
				<tr class="sb-top">
					<th class="th-left"><?=$this->bbf('stats_col_type');?></th>
					<th class="th-center"><?=$this->bbf('stats_col_enable');?></th>
					<th class="th-center"><?=$this->bbf('stats_col_disable');?></th>
					<th class="th-right"><?=$this->bbf('stats_col_total');?></th>
				</tr>
				<tr class="l-infos-1on2">
					<td class="td-left txt-left">
<?php
	if($this->chk_acl('pbx_settings','users') === true):
		echo $url->href_html($this->bbf('stats_users'),'service/ipbx/pbx_settings/users','act=add');
	else:
		echo $this->bbf('stats_users');
	endif;
?>
					</td>
					<td class="td-center"><?=$userstat['enable']?></td>
					<td class="td-center"><?=$userstat['disable']?></td>
					<td class="td-right"><?=$userstat['total']?></td>
				</tr>
				<tr class="l-infos-2on2">
					<td class="td-left txt-left">
<?php
	if($this->chk_acl('pbx_settings','groups') === true):
		echo $url->href_html($this->bbf('stats_groups'),'service/ipbx/pbx_settings/groups','act=add');
	else:
		echo $this->bbf('stats_groups');
	endif;
?>
					</td>
					<td class="td-center"><?=$groupstat['enable']?></td>
					<td class="td-center"><?=$groupstat['disable']?></td>
					<td class="td-right"><?=$groupstat['total']?></td>
				</tr>
				<tr class="l-infos-1on2">
					<td class="td-left txt-left">
<?php
	if($this->chk_acl('pbx_settings','queues') === true):
		echo $url->href_html($this->bbf('stats_queues'),'service/ipbx/pbx_settings/queues','act=add');
	else:
		echo $this->bbf('stats_queues');
	endif;
?>
					</td>
					<td class="td-center"><?=$queuestat['enable']?></td>
					<td class="td-center"><?=$queuestat['disable']?></td>
					<td class="td-right"><?=$queuestat['total']?></td>
				</tr>
				<tr class="l-infos-2on2">
					<td class="td-left txt-left">
<?php
	if($this->chk_acl('pbx_settings','meetme') === true):
		echo $url->href_html($this->bbf('stats_meetme'),'service/ipbx/pbx_settings/meetme','act=add');
	else:
		echo $this->bbf('stats_meetme');
	endif;
?>
					</td>
					<td class="td-center"><?=$meetmestat['enable']?></td>
					<td class="td-center"><?=$meetmestat['disable']?></td>
					<td class="td-right"><?=$meetmestat['total']?></td>
				</tr>
			</table>
		</div>
		<div id="ipbx-logo">
			<?=$url->img_html('img/service/ipbx/asterisk.png',XIVO_SRE_IPBX_LABEL);?>
			<ul>
				<li><b><?=$this->bbf('info_service_label');?></b> <?=XIVO_SRE_IPBX_LABEL?></li>
				<li><b><?=$this->bbf('info_service_version');?></b> <?=XIVO_SRE_IPBX_VERSION?></li>
			</ul>
		</div>
		<div class="clearboth"></div>
	</div>
	<div class="sb-foot xspan"><span class="span-left">&nbsp;</span><span class="span-center">&nbsp;</span><span class="span-right">&nbsp;</span></div>
</div>

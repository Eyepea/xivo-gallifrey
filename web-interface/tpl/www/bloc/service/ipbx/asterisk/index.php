<?php
	$url = &$this->get_module('url');

	$userstat = $this->get_var('userstat');
	$groupstat = $this->get_var('groupstat');
	$queuestat = $this->get_var('queuestat');
	$meetmestat = $this->get_var('meetmestat');
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
	if(xivo_user::chk_acl('pbx_settings','users') === true):
		echo $url->href_html($this->bbf('stats_type-user'),'service/ipbx/pbx_settings/users','act=add');
	else:
		echo $this->bbf('stats_type-user');
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
	if(xivo_user::chk_acl('pbx_settings','groups') === true):
		echo $url->href_html($this->bbf('stats_type-group'),'service/ipbx/pbx_settings/groups','act=add');
	else:
		echo $this->bbf('stats_type-group');
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
	if(xivo_user::chk_acl('pbx_settings','queues') === true):
		echo $url->href_html($this->bbf('stats_type-queue'),'service/ipbx/pbx_settings/queues','act=add');
	else:
		echo $this->bbf('stats_type-queue');
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
	if(xivo_user::chk_acl('pbx_settings','meetme') === true):
		echo $url->href_html($this->bbf('stats_type-meetme'),'service/ipbx/pbx_settings/meetme','act=add');
	else:
		echo $this->bbf('stats_type-meetme');
	endif;
?>
					</td>
					<td class="td-center"><?=$meetmestat['enable']?></td>
					<td class="td-center"><?=$meetmestat['disable']?></td>
					<td class="td-right"><?=$meetmestat['total']?></td>
				</tr>
			</table>

			<table border="0" cellpadding="0" cellspacing="0">
				<tr class="sb-top">
					<th class="th-left"><?=$this->bbf('stats_col_status');?></th>
					<th class="th-center"><?=$this->bbf('stats_col_user');?></th>
					<th class="th-right"><?=$this->bbf('stats_col_total');?></th>
				</tr>
				<tr class="l-infos-1on2">
					<td class="td-left txt-left"><?=$this->bbf('stats_status-initialized');?></td>
					<td class="td-center"><?=$userstat['initialized']?></td>
					<td class="td-right"><?=$userstat['total']?></td>
				</tr>
			</table>

			<table border="0" cellpadding="0" cellspacing="0">
				<tr class="l-infos-1on2">
					<td class="td-singlenotop"><?=$this->bbf('stats_calls-active',$this->get_var('activecalls'));?></td>
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

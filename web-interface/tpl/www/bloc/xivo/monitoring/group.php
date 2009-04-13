<?php

$url = &$this->get_module('url');
$dhtml = &$this->get_module('dhtml');

$grpname = $this->get_var('group_name');
$grpdata = $this->get_var($grpname);
$memtotal = $this->get_var('memtotal');

if(is_array($grpdata) === true && ($nb = count($grpdata)) > 0):

?>
	<div class="monit-group">
		<table border="0" cellpadding="0" cellspacing="0">
			<tr class="sb-top">
				<th colspan="8" class="th-left th-right"><?=$this->bbf('sysinfos_'.$grpname);?></th>
			</tr>
			<tr class="l-subth">
				<td><?=$this->bbf('sysinfos_col_process');?></td>
				<td><?=$this->bbf('sysinfos_col_status');?></td>
				<td><?=$this->bbf('sysinfos_col_uptime');?></td>
				<td><?=$this->bbf('sysinfos_col_cpu');?></td>
				<td colspan="3"><?=$this->bbf('sysinfos_col_memory');?></td>
				<td class="td-right"><?=$this->bbf('sysinfos_col_action');?></td>
			</tr>
<?php
		for($i = 0;$i < $nb;$i++):
			$ref = &$grpdata[$i];

			$mempx = 0;
			$uptime = $cpupcent = $mempcent = $memsize = $mempcent = $nummempcent = '-';

			if($ref['monitor'] === 1 && $ref['status'] === 0):
				$status = 'running';

				if($ref['type'] === 3):
					$uptime = $this->bbf('sysinfos_uptime-duration',
							     xivo_calc_time('second',
									    $ref['uptime'],
									    '%d%H%M%s'));
					$cpupcent = $this->bbf('number_percent',$ref['cpu']['percenttotal']);
					$membyte = xivo_size_si_to_byte('KB',$ref['memory']['kilobytetotal']);
					$mem = xivo_size_iec($membyte);
					$memsize = $this->bbf('size_iec_'.$mem[1],$mem[0]);

					if($memtotal > 0):
						$mempcent = ($membyte / $memtotal * 100);
					else:
						$mempcent = 0;
					endif;

					$nummempcent = $this->bbf('number_percent',$mempcent);
				endif;
			else:
				if($ref['monitor'] === 1):
					$status = 'notrunning';
				elseif($ref['monitor'] === 2):
					$status = 'initializing';
				else:
					$status = 'notmonitored';
				endif;
			endif;
?>
			<tr class="l-infos-<?=(($i % 2) + 1)?>on2">
				<td><?=xivo_trunc(xivo_htmlen($ref['name']),20,'...',false);?></td>
				<td class="monit-status-<?=$status?>"><b><?=$this->bbf('sysinfos_status-opt',$status);?></b></td>
				<td class="txt-right"><?=$uptime?></td>
				<td class="txt-right"><?=$cpupcent?></td>
				<td class="gauge">
					<div><div style="width: <?=round($mempcent);?>px;">&nbsp;</div></div>
				</td>
				<td class="gaugepercent txt-right"><?=$nummempcent?></td>
				<td class="txt-right"><?=$memsize?></td>
				<td class="td-right">
<?php
			$actionable = false;

			if($ref['startable'] === true && xivo_user::chk_acl('control_system','start','service/monitoring') === true):
				$actionable = true;
				echo	$url->href_html($url->img_html('img/site/button/start.gif',
								       $this->bbf('sysinfos_opt_start'),
								       'border="0"'),
							'xivo',
							array('service'	=> $ref['name'],
							      'action'	=> 'start'),
							'onclick="return(confirm(\''.$dhtml->escape($this->bbf('sysinfos_opt_start_confirm',
												    $ref['name'])).'\'));"',
							$this->bbf('sysinfos_opt_start')),"\n";
			endif;

			if($ref['restartable'] === true && xivo_user::chk_acl('control_system','restart','service/monitoring') === true):
				$actionable = true;
				echo	$url->href_html($url->img_html('img/site/button/restart.gif',
								       $this->bbf('sysinfos_opt_restart'),
								       'border="0"'),
							'xivo',
							array('service'	=> $ref['name'],
							      'action'	=> 'restart'),
							'onclick="return(confirm(\''.$dhtml->escape($this->bbf('sysinfos_opt_restart_confirm',
													       $ref['name'])).'\'));"',
							$this->bbf('sysinfos_opt_restart')),"\n";
			endif;

			if($ref['stoppable'] === true && xivo_user::chk_acl('control_system','stop','service/monitoring') === true):
				$actionable = true;
				echo	$url->href_html($url->img_html('img/site/button/stop.gif',
								       $this->bbf('sysinfos_opt_stop'),
								       'border="0"'),
							'xivo',
							array('service'	=> $ref['name'],
							      'action'	=> 'stop'),
							'onclick="return(confirm(\''.$dhtml->escape($this->bbf('sysinfos_opt_stop_confirm',
													       $ref['name'])).'\'));"',
							$this->bbf('sysinfos_opt_stop')),"\n";
			endif;

			if($actionable === false):
				echo '-';
			endif;
?>
				</td>
			</tr>
<?php
		endfor;
?>
		</table>
	</div>
<?php

endif;

?>

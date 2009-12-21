<?php
$this->load_i18n_file('bloc/wizard/packages');
$form = &$this->get_module('form');
$hw_info = $this->get_var('hw-info');
$sw_info = $this->get_var('sw-required');

$dbcnx_check_res = $this->get_var('db-cnx-check-res');

echo 
	$form->hidden(array('name' => 'fm_send',
						'value' => 1)),
	
	$form->hidden(array('name' => 'ck_host',
						'value' => $this->get_var('ckhost'))),

	$form->hidden(array('name' => 'step',
						'value' => $this->get_var('step')));

?>
<div class="wz-check-tb-title"><?=$this->bbf('wz-check-hardware')?></div>
<div class="wz-check-tb-left"><?=$this->bbf('wz-check-memory')?></div>
<div class="wz-check-tb-right"><?=$hw_info['memsize']?></div>
<div class="wz-check-tb-left"><?=$this->bbf('wz-check-network')?></div>
<div class="wz-check-tb-right">
	<div class="wz-check-tb-left"><?=$this->bbf('wz-check-net-vendor');?></div>
	<div class="wz-check-tb-right"><?=$hw_info['net-vnd']?></div>
	<div class="wz-check-tb-left"><?=$this->bbf('wz-check-net-iface');?></div>
	<div class="wz-check-tb-right"><?=$hw_info['net-ifc']?></div>
	<div class="wz-check-tb-left"><?=$this->bbf('wz-check-net-driver');?></div>
	<div class="wz-check-tb-right"><?=$hw_info['net-drv']?></div>
	<div class="wz-check-tb-left"><?=$this->bbf('wz-check-net-address');?></div>
	<div class="wz-check-tb-right"><?=$hw_info['net-mac']?></div>	
	<div class="wz-check-tb-left"><?=$this->bbf('wz-check-net-ip');?></div>
	<div class="wz-check-tb-right"><?=$hw_info['net-ipa']?></div>
	<div class="wz-check-tb-left"><?=$this->bbf('wz-check-net-speed');?></div>
	<div class="wz-check-tb-right"><?=$hw_info['net-spd']?></div>
	<div class="wz-check-tb-left"><?=$this->bbf('wz-check-net-autoneg');?></div>
	<div class="wz-check-tb-right"><?=$hw_info['net-aut']?></div>
</div>
<div class="wz-check-tb-title"><?=$this->bbf('wz-check-software')?></div>
<!-- 
	<div class="wz-check-tb-left">
	<div class="wz-check-tb-title"><?=$this->bbf('wz-check-pkg-required')?></div>
</div>
<div class="wz-check-tb-right">
	<div class="wz-check-tb-title"><?=$this->bbf('wz-check-pkg-installed')?></div>
</div>
-->
<div class="wz-check-tb-left">
	<div class="wz-check-tb-title"><?=$this->bbf('wz-check-pkg-required-base')?></div>
	<?php
	# check base required packages
	#
	foreach(array('base', 'ipbx') as $pkg_cat)
	{
		foreach($sw_info[$pkg_cat] as $pkgname => $pkgstatus)
		{
			if($pkgstatus === 'installed')
				$hint = 'green';
			else
				$hint = 'red';
			echo "<div class=\"wz-check-tb-left\"><span style=\"color: $hint\">$pkgname</span></div>\n";
			echo "<div class=\"wz-check-tb-right\">". $this->bbf($pkgname) ."</div>\n";
		}
	}
	?>
</div>
<div class="wz-check-tb-right">
	<div class="wz-check-tb-title"><?=$this->bbf('wz-check-pkg-required-db')?></div>
	<?php
	# check required database packages
	# depending on db backend previously selected 
	#
	foreach($sw_info['db'] as $pkgname => $pkgstatus)
	{
		if($pkgstatus === 'installed')
			$hint = 'green';
		else
			$hint = 'red';
		echo "<div class=\"wz-check-tb-left\"><span style=\"color: $hint\">$pkgname</span></div>\n";
		echo "<div class=\"wz-check-tb-right\">". $this->bbf($pkgname) ."</div>\n";
	}
	if($_SESSION['_wizard']['db']['backend'] === 'mysql')
	{
		echo 
			"<div class=\"wz-check-tb-left\">",
			$form->submit(array('name' => 'db-cnx-check',
								'value' => $this->bbf('wz-check-db-cnx'),
								'paragraph' => false)),
			"</div>\n",
			"<div class=\"wz-check-tb-right\">";
		if(isset($dbcnx_check_res))
			echo $this->bbf('wz-check-db-cnx-' . $dbcnx_check_res);
		echo "</div>\n";
	}
	?>
</div>
<div class="wz-check-tb-title">
<?php
#	echo $this->bbf('wz-check-last') . " " . $this->get_var('timestamp'),

	echo $form->submit(array('name' => 'reload',
							'value' => $this->bbf('wz-check-reload')));
?>
</div>

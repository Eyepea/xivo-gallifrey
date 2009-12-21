<?php

$form = &$this->get_module('form');

echo	$form->hidden(array('name'	=> 'fm_send',
							'value'	=> 1)),

		$form->hidden(array('name'	=> 'step',
							'value'	=> $this->get_var('step')));

echo $this->bbf('wz-validate-text');

?>
<div class="wz-check-tb-title"><?=$this->bbf('wz-validate-general')?></div>
<div class="wz-check-tb-left"><?=$this->bbf('wz-validate-lang')?></div>
<div class="wz-check-tb-right"><?=$_SESSION['_wizard']['lang']?></div>
<div class="wz-check-tb-left"><?=$this->bbf('wz-validate-ipbx-engine')?></div>
<div class="wz-check-tb-right"><?=$_SESSION['_wizard']['ipbx-engine']?></div>

<div class="wz-check-tb-title"><?=$this->bbf('wz-validate-db')?></div>
<div class="wz-check-tb-left"><?=$this->bbf('wz-validate-db-backend')?></div>
<div class="wz-check-tb-right"><?=$_SESSION['_wizard']['db']['backend']?></div>
<?php
$db_backend = $_SESSION['_wizard']['db']['backend'];
foreach($_SESSION['_wizard']['db'][$db_backend] as $k => $v)
{
	echo
		"<div class=\"wz-check-tb-left\">",
		$k,
		"</div>\n",
		"<div class=\"wz-check-tb-right\">",
		$v,
		"</div>\n";
}
?>
<div class="wz-check-tb-title"><?=$this->bbf('wz-validate-server')?></div>
<div class="wz-check-tb-left"><?=$this->bbf('wz-validate-server-name')?></div>
<div class="wz-check-tb-right"><?=$_SESSION['_wizard']['server']['name']?></div>
<div class="wz-check-tb-left"><?=$this->bbf('wz-validate-server-admin-pwd')?></div>
<div class="wz-check-tb-right"><?=$_SESSION['_wizard']['server']['admin-pwd']?></div>
<div class="wz-check-tb-left"><?=$this->bbf('wz-validate-server-ip')?></div>
<div class="wz-check-tb-right"><?=$_SESSION['_wizard']['server']['ip']?></div>
<div class="wz-check-tb-left"><?=$this->bbf('wz-validate-server-mask')?></div>
<div class="wz-check-tb-right"><?=$_SESSION['_wizard']['server']['netmask']?></div>
<div class="wz-check-tb-left"><?=$this->bbf('wz-validate-server-gw')?></div>
<div class="wz-check-tb-right"><?=$_SESSION['_wizard']['server']['gw']?></div>
<div class="wz-check-tb-left"><?=$this->bbf('wz-validate-server-dns')?></div>
<div class="wz-check-tb-right">
<?php
if(isset($_SESSION['_wizard']['server']['dns1']))
	echo $_SESSION['_wizard']['server']['dns1'], '&nbsp;';
if(isset($_SESSION['_wizard']['server']['dns2']))
	echo $_SESSION['_wizard']['server']['dns2']
?>
</div>

<div class="wz-check-tb-title"><?=$this->bbf('wz-validate-entity')?></div>
<div class="wz-check-tb-left"><?=$this->bbf('wz-validate-entity-name')?></div>
<div class="wz-check-tb-right"><?=$_SESSION['_wizard']['entity']['name']?></div>
<div class="wz-check-tb-left"><?=$this->bbf('wz-validate-entity-dispname')?></div>
<div class="wz-check-tb-right"><?=$_SESSION['_wizard']['entity']['dispname']?></div>

<div class="wz-check-tb-title"><?=$this->bbf('wz-validate-context')?></div>
<?php
$ctx = $_SESSION['_wizard']['context'];
if((count($ctx['internal']) === 0) && (count($ctx['incoming']) === 0) && (count($ctx['outgoing']) === 0))
	echo "<div class=\"wz-check-tb-left\">", $this->bbf('wz-validate-context-none'), "</div>";
else
{
	foreach($_SESSION['_wizard']['context'] as $ctx => $ctxinfo)
	{
		if(dwho_has_len($ctxinfo, 'name') && dwho_has_len($ctxinfo, 'numbeg') && dwho_has_len($ctxinfo, 'numend'))
		{
			echo 
				"<div class=\"wz-check-tb-left\">",
				$this->bbf('wz-validate-context-' . $ctx),
				"</div>\n",
				"<div class=\"wz-check-tb-right\">";
	
			echo 
				$this->bbf('wz-validate-context-name'),
				'&nbsp;:&nbsp;',
				$ctxinfo['name'],
				'&nbsp;',
				$this->bbf('wz-validate-context-numbers-from'),
				'&nbsp;',
				$ctxinfo['numbeg'],
				'&nbsp;',
				$this->bbf('wz-validate-context-numbers-to'),
				'&nbsp;',
				$ctxinfo['numend'],
				"<br>\n";
	
			echo "</div>\n";
		}
	}
}
?>
<div class="wz-check-tb-title">&nbsp;</div>
			

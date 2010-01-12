<?php
$form = &$this->get_module('form');
$msg = $this->get_var('wz-message');

$admin_pwd = isset($_SESSION['_wizard']['server']['admin-pwd']) === true ? $_SESSION['_wizard']['server']['admin-pwd'] : '';
$server_name = isset($_SESSION['_wizard']['server']['name']) === true ? $_SESSION['_wizard']['server']['name'] : '';
#$server_ip = isset($_SESSION['_wizard']['network']['ip']) === true ? $_SESSION['_wizard']['network']['ip'] : '';
#$server_mask = isset($_SESSION['_wizard']['network']['mask']) === true ? $_SESSION['_wizard']['network']['mask'] : '';
#$server_gw = isset($_SESSION['_wizard']['network']['gw']) === true ? $_SESSION['_wizard']['network']['gw'] : '';
$server_iface = isset($_SESSION['_wizard']['network']['iface']) === true ? $_SESSION['_wizard']['network']['iface'] : '';
echo 
	$form->hidden(array('name' => 'fm_send',
						'value' => 1)),

	$form->hidden(array('name' => 'step',
						'value' => $this->get_var('step'))),
	
	$form->text(array('desc' => $this->bbf('fm-admin-pwd'),
						'name' => 'fm-admin-pwd',
						'labelid' => 'fm-admin-pwd',
						'size' => 15,
						'required' => true,
						'help' => $this->bbf('fm-help-admin-pwd'),
						'comment' => $this->bbf('fm-comment-admin-pwd'),
						'regexp' => '/^[a-zA-Z0-9-\._~\!\$&\'\(\)\*\+,;=%]+$/',
						'default' => $admin_pwd)),

	$form->text(array('desc' => $this->bbf('fm-admin-pwd-retype'),
						'name' => 'fm-admin-pwd-retype',
						'labelid' => 'fm-admin-pwd-retype',
						'size' => 15)),

	$form->text(array('desc' => $this->bbf('fm-server-name'),
						'name' => 'fm-server-name',
						'labelid' => 'fm-server-name',
						'help' => $this->bbf('hlp_fm-server-name'),
						'comment' => $this->bbf('cmt_fm-server-name'),
						'size' => 15,
						'default' => $server_name)),

	"<div class=\"wz-check-tb-title\">", $this->bbf('wz-net-iface'), " ", $server_iface, "</div>\n",
	$form->text(array('desc' => $this->bbf('fm-server-ip'),
						'name' => 'fm-server-ip',
						'labelid' => 'fm-server-ip',
						'help' => $this->bbf('hlp_fm-server-ip'),
						'comment' => $this->bbf('cmt_fm-server-ip'),
						'size' => 15,
						'default' => $this->get_var('ip'))),
	
	$form->text(array('desc' => $this->bbf('fm-server-netmask'),
						'name' => 'fm-server-netmask',
						'labelid' => 'fm-server-netmask',
						'help' => $this->bbf('hlp_fm-server-netmask'),
						'comment' => $this->bbf('cmt_fm-server-netmask'),
						'size' => 15,
						'default' => $this->get_var('mask'))),

	$form->text(array('desc' => $this->bbf('fm-server-gw'),
						'name' => 'fm-server-gw',
						'labelid' => 'fm-server-gw',
						'help' => $this->bbf('hlp_fm-server-gw'),
						'comment' => $this->bbf('cmt_fm-server-gw'),
						'size' => 15,
						'default' => $this->get_var('gw'))),
	
	$form->text(array('desc' => $this->bbf('fm-server-dns1'),
						'name' => 'fm-server-dns1',
						'labelid' => 'fm-server-dns1',
						'help' => $this->bbf('hlp_fm-server-dns1'),
						'comment' => $this->bbf('cmt_fm-server-dns1'),
						'size' => 15,
						'default' => $this->get_var('dns1'))),

	$form->text(array('desc' => $this->bbf('fm-server-dns2'),
						'name' => 'fm-server-dns2',
						'labelid' => 'fm-server-dns2',
						'size' => 15,
						'default' => $this->get_var('dns2')));
?>

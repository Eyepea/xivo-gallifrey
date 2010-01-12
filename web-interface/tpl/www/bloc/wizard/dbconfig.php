<?php
$form = &$this->get_module('form');
$db_backends 	= $this->get_var('db-backends');

$db_backend 	= isset($_SESSION['_wizard']['db']['backend']) === true ? $_SESSION['_wizard']['db']['backend'] : 'sqlite';
$db_file_xivo	= isset($_SESSION['_wizard']['db']['sqlite']['xivo']) === true 
					? $_SESSION['_wizard']['db']['sqlite']['xivo'] : '/var/lib/pf-xivo-web-interface/sqlite/xivo.db';
$db_file_ipbx	= isset($_SESSION['_wizard']['db']['sqlite']['ipbx']) === true 
					? $_SESSION['_wizard']['db']['sqlite']['ipbx'] 
					: ((isset($_SESSION['_wizard']['ipbx-engine']) && $_SESSION['_wizard']['ipbx-engine'] === 'asterisk')
						? '/var/lib/asterisk/astsqlite' : '');
$db_host 		= isset($_SESSION['_wizard']['db']['mysql']['host']) === true ? $_SESSION['_wizard']['db']['mysql']['host'] : 'localhost';
$db_host_port 	= isset($_SESSION['_wizard']['db']['mysql']['host-port']) === true ? $_SESSION['_wizard']['db']['mysql']['host-port'] : '3306';
$db_dbname_xivo = isset($_SESSION['_wizard']['db']['mysql']['xivo']['db-dbname']) === true ? $_SESSION['_wizard']['db']['mysql']['xivo']['db-dbname'] : 'xivo';
$db_user_xivo	= isset($_SESSION['_wizard']['db']['mysql']['xivo']['db-user']) === true ? $_SESSION['_wizard']['db']['mysql']['xivo']['db-user'] : 'root';
$db_pwd_xivo	= isset($_SESSION['_wizard']['db']['mysql']['xivo']['db-pwd']) === true ? $_SESSION['_wizard']['db']['mysql']['xivo']['db-pwd'] : '';
$db_dbname_ipbx = isset($_SESSION['_wizard']['db']['mysql']['ipbx']['db-dbname']) === true 
					? $_SESSION['_wizard']['db']['mysql']['ipbx']['db-dbname'] 
					: isset($_SESSION['_wizard']['ipbx-engine'])
						? $_SESSION['_wizard']['ipbx-engine']
						: '';
$db_user_ipbx	= isset($_SESSION['_wizard']['db']['mysql']['ipbx']['db-user']) === true ? $_SESSION['_wizard']['db']['mysql']['ipbx']['db-user'] : 'root';
$db_pwd_ipbx	= isset($_SESSION['_wizard']['db']['mysql']['ipbx']['db-pwd']) === true ? $_SESSION['_wizard']['db']['mysql']['ipbx']['db-pwd'] : '';
	
echo 
	$form->hidden(array('name' => 'fm_send',
						'value' => 1)),

	$form->hidden(array('name' => 'step',
						'value' => $this->get_var('step'))),

	$form->select(array('desc' => $this->bbf('fm-db-backend'),
						'name' => 'db-backend',
						'labelid' => 'db-backend',
						'help' => $this->bbf('hlp_fm-db-backend'),
						'selected' => $db_backend),
						$db_backends),

	$form->text(array('desc' => $this->bbf('fm-db-file-xivo'),
						'name' => 'db-file-xivo',
						'labelid' => 'db-file-xivo',
						'help' => $this->bbf('hlp_fm-db-file-xivo'),
						'comment' => $this->bbf('cmt_fm-db-file-xivo'),
						'size' => 20,
						'value' => $db_file_xivo)),
	
	$form->text(array('desc' => $this->bbf('fm-db-file-ipbx'),
						'name' => 'db-file-ipbx',
						'labelid' => 'db-file-ipbx',
						'help' => $this->bbf('hlp_fm-db-file-ipbx'),
						'comment' => $this->bbf('cmt_fm-db-file-ipbx'),
						'size' => 20,
						'value' => $db_file_ipbx)),
	
	$form->text(array('desc' => $this->bbf('fm-db-hostname'),
						'name' => 'db-host',
						'labelid' => 'db-host',
						'help' => $this->bbf('hlp_fm-db-hostname'),
						'comment' => $this->bbf('cmt_fm-db-hostname'),
						'size' => 20,
						'default' => $db_host)),

	$form->text(array('desc' => $this->bbf('fm-db-hostport'),
						'name' => 'db-host-port',
						'labelid' => 'db-host-port',
						'help' => $this->bbf('hlp_fm-db-hostport'),
						'comment' => $this->bbf('cmt_fm-db-hostport'),
						'size' => 20,
						'default' => $db_host_port)),

	$form->text(array('desc' => $this->bbf('fm-db-dbname-xivo'),
						'name' => 'db-dbname-xivo',
						'labelid' => 'db-dbname-xivo',
						'help' => $this->bbf('hlp_fm-db-dbname-xivo'),
						'comment' => $this->bbf('cmt_fm-db-dbname-xivo'),
						'size' => 20,
						'default' => $db_dbname_xivo)),

	$form->text(array('desc' => $this->bbf('fm-db-user-xivo'),
						'name' => 'db-user-xivo',
						'labelid' => 'db-user-xivo',
						'help' => $this->bbf('hlp_fm-db-user-xivo'),
						'comment' => $this->bbf('cmt_fm-db-user-xivo'),
						'size' => 20,
						'default' => $db_user_xivo)),

	$form->text(array('desc' => $this->bbf('fm-db-pwd-xivo'),
						'name' => 'db-pwd-xivo',
						'labelid' => 'db-pwd-xivo',
						'help' => $this->bbf('hlp_fm-db-pwd-xivo'),
						'comment' => $this->bbf('cmt_fm-db-pwd-xivo'),
						'size' => 20,
						'default' => $db_pwd_xivo)),

	$form->text(array('desc' => $this->bbf('fm-db-dbname-ipbx'),
						'name' => 'db-dbname-ipbx',
						'labelid' => 'db-dbname-ipbx',
						'help' => $this->bbf('hlp_fm-db-dbname-ipbx'),
						'comment' => $this->bbf('cmt_fm-db-dbname-ipbx'),
						'size' => 20,
						'default' => $db_dbname_ipbx)),

	$form->text(array('desc' => $this->bbf('fm-db-user-ipbx'),
						'name' => 'db-user-ipbx',
						'labelid' => 'db-user-ipbx',
						'help' => $this->bbf('hlp_fm-db-user-ipbx'),
						'comment' => $this->bbf('cmt_fm-db-user-ipbx'),
						'size' => 20,
						'default' => $db_user_ipbx)),

	$form->text(array('desc' => $this->bbf('fm-db-pwd-ipbx'),
						'name' => 'db-pwd-ipbx',
						'labelid' => 'db-pwd-ipbx',
						'help' => $this->bbf('hlp_fm-db-pwd-ipbx'),
						'comment' => $this->bbf('cmt_fm-db-pwd-ipbx'),
						'size' => 20,
						'default' => $db_pwd_ipbx));

?>

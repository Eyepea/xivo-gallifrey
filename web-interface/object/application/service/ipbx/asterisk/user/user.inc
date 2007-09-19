<?php

require_once(XIVO_PATH_OBJECT.XIVO_SEP_DIR.'application'.XIVO_SEP_DIR.'abstract.inc');

class xivo_application_service_asterisk_user extends xivo_application_abstract
{
	var $_ipbx	= null;
	var $_protocol	= false;
	var $_origin	= array();

	function xivo_application_service_asterisk_user(&$ipbx)
	{
		if(is_object($ipbx) === false || get_class($ipbx) !== 'xivo_service_asterisk')
			trigger_error('Invalid service in '.__CLASS__,E_USER_ERROR);

		$this->_ipbx = &$ipbx;
	}

	function _reset()
	{
		$this->_protocol = false;

		return(parent::_reset());
	}

	function get($id,$disable=null)
	{
		$this->_info = $this->_origin = array();

		if(($ufeatures = &$this->_ipbx->get_module('userfeatures')) === false
		|| ($this->_info['ufeatures'] = $ufeatures->get($id)) === false
		|| ($protocol = &$this->_ipbx->get_protocol_module($this->_info['ufeatures']['protocol'])) === false
		|| ($this->_info['protocol'] = $protocol->get($this->_info['ufeatures']['protocolid'],$disable)) === false)
			return(false);

		$this->_origin['protocol'] = $protocol->get_origin();
		$this->_origin['ufeatures'] = $ufeatures->get_origin();

		$this->_get_localexten();
		$this->_get_hints();
		$this->_get_extenumbers();
		$this->_get_groupmember();
		$this->_get_queuemember();
		$this->_get_usergroup();
		$this->_get_voicemail();
		$this->_get_autoprov();

		return($this->_info);
	}

	function _get_localexten()
	{
		$this->_info['localexten'] = false;
		$this->_origin['localexten'] = false;

		if(($extensions = &$this->_ipbx->get_module('extensions')) === false)
			return(false);

		$this->_info['localexten'] = $extensions->get_exten('macro',
								    $this->_info['ufeatures']['number'],
								    $this->_info['protocol']['context']);

		$this->_origin['localexten'] = $extensions->get_origin();

		return($this->_info['localexten']);
	}

	function _get_hints()
	{
		$this->_info['hints'] = false;
		$this->_origin['hints'] = false;

		if(($extensions = &$this->_ipbx->get_module('extensions')) === false)
			return(false);

		$this->_info['hints'] = $extensions->get_hints($this->_info['protocol']['name'],
							       $this->_info['ufeatures']['protocol'],
							       $this->_info['ufeatures']['number']);
			
		$this->_origin['hints'] = $extensions->get_origin();

		return($this->_info['hints']);
	}

	function _get_extenumbers()
	{
		$this->_info['extenumbers'] = false;
		$this->_origin['extenumbers'] = false;

		if(($extenumbers = &$this->_ipbx->get_module('extenumbers')) === false)
			return(false);

		$extenumbers_where = array();
		$extenumbers_where['exten'] = $this->_info['ufeatures']['number'];
		$extenumbers_where['context'] = $this->_info['protocol']['context'];

		$this->_info['extenumbers'] = $extenumbers->get_where($extenumbers_where);
		$this->_origin['extenumbers'] = $extenumbers->get_origin();

		return($this->_info['extenumbers']);
	}

	function _get_groupmember()
	{
		return($this->_get_member_type('group'));
	}

	function _get_queuemember()
	{
		return($this->_get_member_type('queue'));
	}

	function _get_member_type($type)
	{
		if($type !== 'group' && $type !== 'queue')
			return(false);

		$membertype = $type.'member';

		$this->_info[$membertype] = false;
		$this->_origin[$membertype] = false;

		$where = array();
		$where['category'] = $type;
		$where['usertype'] = 'user';
		$where['userid'] = $this->_info['ufeatures']['id'];

		if(($qmember = &$this->_ipbx->get_module('queuemember')) === false
		|| ($this->_info[$membertype] = $qmember->get_all_where($where)) === false)
			return(false);

		$this->_origin[$membertype] = $qmember->get_origin_list();

		return($this->_info[$membertype]);
	}

	function _get_usergroup()
	{
		$this->_info['usergroup'] = false;
		$this->_origin['usergroup'] = false;

		if(($ugroup = &$this->_ipbx->get_module('usergroup')) === false)
			return(false);

		$ugroup_where = array();
		$ugroup_where['userid'] = $this->_info['ufeatures']['id'];
		$this->_info['usergroup'] = $ugroup->get_where($ugroup_where);
		$this->_origin['usergroup'] = $ugroup->get_origin();

		return($this->_info['usergroup']);
	}

	function _get_voicemail()
	{
		$this->_info['voicemail'] = false;
		$this->_origin['voicemail'] = false;

		if(($voicemail = &$this->_ipbx->get_module('uservoicemail')) === false)
			return(false);

		$voicemail_where = array();
		$voicemail_where['mailbox'] = $this->_info['ufeatures']['number'];
		$voicemail_where['context'] = $this->_info['ufeatures']['context'];
		$this->_info['voicemail'] = $voicemail->get_where($voicemail_where);
		$this->_origin['voicemail'] = $voicemail->get_origin();

		return($this->_info['voicemail']);
	}

	function _get_autoprov()
	{
		$this->_info['autoprov'] = false;
		$this->_origin['autoprov'] = false;

		if(($autoprov = &$this->_ipbx->get_module('autoprov')) === false)
			return(false);

		$autoprov_where = array();
		$autoprov_where['iduserfeatures'] = $this->_info['ufeatures']['id'];
		$this->_info['autoprov'] = $autoprov->get_where($autoprov_where);
		$this->_origin['autoprov'] = $autoprov->get_origin();

		return($this->_info['autoprov']);
	}

	function get_element()
	{
		$r = array();

		$r['protocol'] = $this->_ipbx->get_protocol_element();

		if(($ufeatures = &$this->_ipbx->get_module('userfeatures')) !== false)
			$r['ufeatures'] = $ufeatures->get_element();
		else
			$r['ufeatures'] = false;

		if(($qmember = &$this->_ipbx->get_module('queuemember')) !== false)
			$r['qmember'] = $qmember->get_element();
		else
			$r['qmember'] = false;

		if(($voicemail = &$this->_ipbx->get_module('uservoicemail')) !== false)
			$r['voicemail'] = $voicemail->get_element();
		else
			$r['voicemail'] = false;

		if(($autoprov = &$this->_ipbx->get_module('autoprov')) !== false)
			$r['autoprov'] = $autoprov->get_element();
		else
			$r['autoprov'] = false;

		return($r);
	}

	function set_add($arr,$protocol)
	{
		return($this->_set('add',$arr,$protocol));
	}

	function set_edit($arr,$protocol)
	{
		if(empty($this->_info) === true)
			return(false);

		return($this->_set('edit',$arr,$protocol));
	}

	function _set($action,$arr,$protocol)
	{
		$this->_reset();

		if(($action !== 'add' && $action !== 'edit') === true
		|| xivo_issa('protocol',$arr) === false
		|| xivo_issa('ufeatures',$arr) === false)
			return(false);

		$this->_protocol = $protocol;

		$this->set_protocol($arr['protocol'],$protocol);

		$arr['ufeatures']['protocol'] = $protocol;
		$arr['ufeatures']['protocolid'] = 0;

		$name = $this->get_result_var('protocol','name');
		$context = $this->get_result_var('protocol','context');

		$arr['ufeatures']['name'] = $name;
		$arr['ufeatures']['context'] = $context;

		$this->set_userfeatures($arr['ufeatures']);

		$number = (string) $this->get_result_var('ufeatures','number');
		$protocol = (string) $this->get_result_var('ufeatures','protocol');

		$this->_set_result_var('protocol','mailbox',$number);

		if($this->_errnb === 0 && $number !== '')
		{
			$appdata = array('appdata' => 'superuser');

			if(($localexten = $this->set_macro($action,'localexten',$number,$appdata,$context)) === false)
				$this->_set_error('localexten');

			$this->_set_result('localexten',$localexten);

			$callerid = (string) $this->get_result_var('protocol','callerid');
			$callerid = $this->_ipbx->mk_callerid($number,$callerid);

			$this->_set_result_var('protocol','callerid',$callerid);

			if(($hints = $this->set_hints($action,$name,$protocol,$number)) === false)
				$this->_set_error('hints');

			$this->_set_result('hints',$hints);

			$this->set_extenumbers($action,$number,$context);
		}

		if(xivo_issa('group',$arr) === true
		&& xivo_issa('group-select',$arr) === true
		&& $this->set_groupmember($arr['group'],$arr['group-select'],$name,$context,$number,$protocol) === true)
		{
			$callgroup = $this->get_result('callgroup');

			if(is_array($callgroup) === true && isset($callgroup[0]) === true)
				$callgroup = implode(',',$callgroup);
			else
				$callgroup = '';

			$this->_set_result_var('protocol','callgroup',$callgroup);
		}

		if(xivo_issa('queue',$arr) === true && xivo_issa('queue-select',$arr) === true)
			$this->set_queuemember($arr['queue'],$arr['queue-select'],$name,$context,$number,$protocol);

		if(xivo_issa('voicemail',$arr) === true)
			$this->set_voicemail($arr['voicemail'],$context,$number);

		if(xivo_issa('autoprov',$arr) === true)
			$this->set_autoprov($arr['autoprov'],$protocol);

		if($this->get_errnb() > 0)
			$this->_protocol = $r = false;
		else
			$r = true;

		return($r);
	}

	function set_protocol($arr,$protocol)
	{
		if(is_array($arr) === false
		|| ($proto = &$this->_ipbx->get_protocol_module($protocol)) === false)
		{
			$this->_set_result('protocol',null);
			$this->_set_error('protocol','invalid');
			return(false);
		}

		if(($rs = $proto->chk_values($arr)) === false)
		{
			$this->_set_result('protocol',$proto->get_filter_result());
			$this->_set_error('protocol',$proto->get_filter_error());

			return(false);
		}

		if(is_array($rs['allow']) === true)
			$rs['allow'] = implode(',',$rs['allow']);

		$this->_set_result('protocol',$rs);

		return(true);
	}

	function set_userfeatures($arr)
	{
		if(is_array($arr) === false
		|| ($ufeatures = &$this->_ipbx->get_module('userfeatures')) === false)
		{
			$this->_set_result('ufeatures',null);
			$this->_set_error('ufeatures','invalid');
			return(false);
		}

		if(($rs = $ufeatures->chk_values($arr)) === false)
		{
			$this->_set_result('ufeatures',$ufeatures->get_filter_result());
			$this->_set_error('ufeatures',$ufeatures->get_filter_error());

			return(false);
		}

		$this->_set_result('ufeatures',$rs);

		return(true);
	}

	function set_macro($action,$name,$exten,$arr,$context)
	{
		if(($action !== 'add' && $action !== 'edit') === true
		|| is_string($name) === false
		|| ($extensions = &$this->_ipbx->get_module('extensions')) === false)
			return(false);

		if($action === 'edit' && xivo_issa($name,$this->_info) !== false)
			$rs = $extensions->chk_exten('macro',$arr,$exten,$context);
		else
			$rs = $extensions->new_exten('macro',$arr,$exten,$context);

		return($rs);
	}

	function set_hints($action,$name,$protocol,$number)
	{
		if(($action !== 'add' && $action !== 'edit') === true
		|| ($extensions = &$this->_ipbx->get_module('extensions')) === false)
			return(false);

		if($action === 'edit' && xivo_issa('hints',$this->_info) !== false)
			$rs = $extensions->chk_hints($name,$protocol,$number);
		else
			$rs = $extensions->new_hints($name,$protocol,$number);

		return($rs);
	}

	function set_extenumbers($action,$number,$context)
	{
		if(($action !== 'add' && $action !== 'edit') === true
		|| ($extenumbers = &$this->_ipbx->get_module('extenumbers')) === false)
			return(false);

		if($action === 'edit' && xivo_issa('extenumbers',$this->_info) === true)
			$id = $this->_info['extenumbers']['id'];
		else
			$id = null;

		$extenum = array();
		$extenum['exten'] = $number;
		$extenum['context'] = $context;

		$exists = false;

		if(($rs = $extenumbers->chk_values($extenum)) === false
		|| ($exists = $extenumbers->exists($rs,$id)) !== false)
		{
			$this->_set_result('extenumbers',$extenumbers->get_filter_result());

			if($exists === false)
				$this->_set_error('extenumbers',$extenumbers->get_filter_error());
			else
				$this->_set_error('extenumbers','exists');

			return(false);
		}

		$rs = $this->_set_result('extenumbers',$rs);

		return(true);
	}

	function set_groupmember($group,$select,$name,$context='',$number='',$protocol='')
	{
		$r = $this->_set_member_type('group',$group,$select,$name,$context,$number,$protocol);

		return($r);
	}

	function set_queuemember($queue,$select,$name,$context='',$number='',$protocol='')
	{
		$r = $this->_set_member_type('queue',$queue,$select,$name,$context,$number,$protocol);

		return($r);
	}

	function _set_member_type($type,$list,$select,$name,$context='',$number='',$protocol='')
	{
		if(($type !== 'group' && $type !== 'queue') === true
		|| is_array($list) === false
		|| is_array($select) === false
		|| ($module = &$this->_ipbx->get_module($type.'features')) === false
		|| ($qmember = &$this->_ipbx->get_module('queuemember')) === false)
			return(false);

		$slt = array_values($select);

		if(($nb = count($slt)) === false)
			return(null);

		$info = array();
		$info['usertype'] = 'user';
		$info['userid'] = 0;
		$info['category'] = $type;

		$key = $type.'member';

		$tmp = $type_list = $callgroup = array();

		for($i = 0;$i < $nb;$i++)
		{
			$qname = &$slt[$i];

			if(isset($tmp[$qname]) === true
			|| isset($list[$qname]) === false
			|| ($chantype = xivo_ak('chantype',$list[$qname],true)) === false
			|| ($interface = $this->mk_interface($name,$chantype,$context,$number,$protocol)) === false
			|| ($typeid = $module->get_id(array('name' => $qname))) === false)
				continue;

			$info['queue_name'] = $qname;
			$info['interface'] = $interface['interface'];
			$info['channel'] = $interface['channel'];

			$values = array_merge($list[$qname],$info);

			if(($rs = $qmember->chk_values($values)) === false)
				continue;

			$tmp[$qname] = 1;
			$type_list[] = $rs;
			$callgroup[] = $typeid;		
		}

		if(isset($type_list[0]) === false)
		{
			$this->_set_result($key,false);

			if($type === 'group')
				$this->_set_result('callgroup','');
			return(false);
		}

		$this->_set_result($key,$type_list);

		if($type === 'group')
			$this->_set_result('callgroup',$callgroup);

		return(true);
	}

	function set_voicemail($arr,$context,$number='')
	{
		if(is_array($arr) === false
		|| ($voicemail = &$this->_ipbx->get_module('uservoicemail')) === false)
			return(false);

		$arr['context'] = $context;

		if($number !== '')
			$arr['mailbox'] = $number;

		if(($rs = $voicemail->chk_values($arr)) === false)
		{
			$rs = array_merge($voicemail->get_filter_result(),array('commented' => 0));

			$this->_set_result('voicemail',$rs);
			$this->_set_error('voicemail',$voicemail->get_filter_error());

			return(false);
		}

		$rs['commented'] = 0;
		$this->_set_result('voicemail',$rs);

		return(true);
	}

	function set_autoprov($arr,$protocol)
	{
		if(is_array($arr) === false
		|| ($autoprov = &$this->_ipbx->get_module('autoprov')) === false
		|| ($list = $autoprov->get_autoprov_list()) === false)
			return(false);

		if(xivo_haslen($arr,'vendormodel') === false
		|| xivo_haslen($arr,'macaddr') === false)
			return(null);

		if(($pos = strpos($arr['vendormodel'],'.')) === false)
			$arr['vendormodel'] = $arr['modact'] = '';
		else
		{
			$vendor = substr($arr['vendormodel'],0,$pos);
			$model = substr($arr['vendormodel'],$pos+1);

			if(xivo_issa($vendor,$list) === true
			&& xivo_issa('model',$list[$vendor]) === true
			&& xivo_issa($model,$list[$vendor]['model']) === true)
			{
				$arr['vendor'] = $vendor;
				$arr['model'] = $model;
			}

			if(($macaddr = xivo_filter::chk_macaddr($arr['macaddr'])) !== false)
				$arr['macaddr'] = $macaddr;
			else
				$arr['macaddr'] = '';

			$arr['modact'] = 'prov';
			$arr['proto'] = $protocol;
		}

		$autoprov = &$this->_ipbx->get_module('autoprov');

		if(($rs = $autoprov->chk_values($arr)) === false)
		{
			$this->_set_result('autoprov',$autoprov->get_filter_result());
			$this->_set_error('autoprov',$autoprov->get_filter_error());

			return(false);
		}

		$this->_set_result('autoprov',$rs);
		$this->_set_result_var('autoprov','modact',$arr['modact']);

		return(true);
	}

	function mk_interface($name,$chantype,$context='',$number='',$protocol='')
	{
		$chantype = strval($chantype);
		$number = strval($number);

		$r = array();

		switch($chantype)
		{
			case XIVO_SRE_IPBX_AST_CHAN_LOCAL:
				$r['interface'] = $this->_ipbx->mk_interface($name,null,$number,$context);

				if($number === '' || $r['interface'] === false)
					return(false);

				$r['channel'] = XIVO_SRE_IPBX_AST_CHAN_LOCAL;
				break;
			default:
				$r['interface'] = $this->_ipbx->mk_interface($name,$protocol);
				$r['channel'] = $this->_ipbx->get_channel_by_protocol($protocol);

				if($r['interface'] === false || $r['channel'] === false)
					return(false);
		}

		return($r);
	}

	function add()
	{
		$this->_return = array();
		$ufeaturesid = null;

		if($this->get_errnb() > 0
		|| $this->_protocol === false
		|| ($protocolid = $this->add_protocol()) === false
		|| ($ufeaturesid = $this->add_userfeatures($protocolid)) === false
		|| $this->add_macro('localexten') === false
		|| $this->add_hints() === false
		|| $this->add_extenumbers() === false
		|| $this->add_groupmember($ufeaturesid) === false
		|| $this->add_queuemember($ufeaturesid) === false
		|| $this->add_voicemail() === false
		|| $this->add_phone($ufeaturesid) === false)
		{
			if($ufeaturesid !== null)
				$this->_reverse_add();

			return(false);
		}

		return(true);
	}

	function add_protocol()
	{
		if(($rs = $this->get_result('protocol')) === null
		|| ($protocol = &$this->_ipbx->get_protocol_module($this->_protocol)) === false)
			return(false);

		$this->_return['protocol'] = $protocol->add($rs);

		return($this->_return['protocol']);
	}

	function add_userfeatures($protocolid)
	{
		$rs = false;

		if(($protocolid = xivo_uint($protocolid)) === 0
		|| ($rs = $this->get_result('ufeatures')) === null
		|| $rs === false)
			return($rs);

		$ufeatures = &$this->_ipbx->get_module('userfeatures');

		$rs['protocolid'] = $protocolid;
		$this->_return['userfeatures'] = $ufeatures->add($rs);

		return($this->_return['userfeatures']);
	}

	function add_macro($name)
	{
		if(($rs = $this->get_result($name)) === null || $rs === false)
			return($rs);

		$extensions = &$this->_ipbx->get_module('extensions');
		$this->_return[$name] = $extensions->add_exten($rs);

		return($this->_return[$name]);
	}

	function add_hints()
	{
		if(($rs = $this->get_result('hints')) === null || $rs === false)
			return($rs);

		$extensions = &$this->_ipbx->get_module('extensions');
		$this->_return['hints'] = $extensions->add_hints($rs);

		return($this->_return['hints']);
	}

	function add_extenumbers()
	{
		if(($rs = $this->get_result('extenumbers')) === null || $rs === false)
			return($rs);

		$extenumbers = &$this->_ipbx->get_module('extenumbers');
		$this->_return['extenumbers'] = $extenumbers->add($rs);

		return($this->_return['extenumbers']);
	}

	function add_groupmember($ufeaturesid)
	{
		return($this->_add_member_type('group',$ufeaturesid));
	}

	function add_queuemember($ufeaturesid)
	{
		return($this->_add_member_type('queue',$ufeaturesid));
	}

	function _add_member_type($type,$ufeaturesid)
	{
		if(($type !== 'group' && $type !== 'queue') === true
		|| ($ufeaturesid = xivo_uint($ufeaturesid)) === 0)
			return(false);

		$key = $type.'member';

		if(($rs = $this->get_result($key)) === null || $rs === false)
			return($rs);
		
		$qmember = &$this->_ipbx->get_module('queuemember');

		$this->_return[$key] = array();

		$nb = count($rs);

		for($i = 0;$i < $nb;$i++)
		{
			$rs[$i]['userid'] = $ufeaturesid;

			if(($this->_return[$key][] = $qmember->add($rs[$i])) === false)
				return(false);
		}

		if(isset($this->_return[$key][0]) === false)
			$this->_return[$key] = false;

		return(true);
	}

	function add_voicemail()
	{
		if(($rs = $this->get_result('voicemail')) === null)
			return($rs);

		$voicemail = &$this->_ipbx->get_module('uservoicemail');

		$this->_return['uservoicemail'] = $voicemail->add($rs);

		return($this->_return['uservoicemail']);
	}

	function add_phone($ufeaturesid)
	{
		$rs = false;

		if(($ufeaturesid = xivo_uint($ufeaturesid)) === 0
		|| ($rs = $this->get_result('autoprov')) === null
		|| $rs === false)
			return($rs);

		$autoprov = &$this->_ipbx->get_module('autoprov');

		$rs['iduserfeatures'] = $ufeaturesid;

		if($autoprov->authoritative($rs,$rs['modact']) === false)
			$this->_return['autoprov'] = false;
		else
			$this->_return['autoprov'] = $ufeaturesid;

		return($this->_return['autoprov']);
	}

	function _reverse_add()
	{
		if($this->get_errnb() > 0
		|| $this->_protocol === false
		|| ($arr = xivo_get_aks($this->_return)) === false)
			return(false);

		$nb = count($arr);

		for($i = 0;$i < $nb;$i++)
		{
			$key = &$arr['keys'][$i];
			$val = &$this->_return[$key];

			if($val === false)
				continue;

			switch($key)
			{
				case 'protocol':
					$protocol = &$this->_ipbx->get_protocol_module($this->_protocol);
					$protocol->delete($val);
					break;
				case 'localexten':
					$extensions = &$this->_ipbx->get_module('extensions');
					$extensions->delete_exten($val);
					break;
				case 'hints':
					$extensions = &$this->_ipbx->get_module('extensions');
					$extensions->delete_hints($val);
					break;
				case 'groupmember':
				case 'queuemember':
					$qmember = &$this->_ipbx->get_module('queuemember');

					if(is_array($val) === false || ($cnt = count($val)) === 0)
						continue;

					for($j = 0;$j < $cnt;$j++)
					{
						if($val[$j] !== false)
							$qmember->delete($val[$j]);
					}
					break;
				case 'autoprov':
					$autoprov = &$this->_ipbx->get_module('autoprov');

					$autoprov->userdeleted($val);
				default:
					if(($module = &$this->_ipbx->get_module($key)) === false)
						continue;

					$module->delete($val);
			}
		}
		return(true);
	}

	function edit()
	{
		$this->_return = array();

		if(is_array($this->_info) === false
		|| empty($this->_info) === true
		|| $this->get_errnb() > 0
		|| $this->_protocol === false
		|| ($protocolret = $this->edit_protocol()) === false)
			return(false);

		if($this->edit_userfeatures($protocolret) === false)
		{
			$this->edit_origin_protocol($protocolret);
			return(false);
		}

		$delete_links = false;

		if($this->_info['ufeatures']['number'] !== '')
		{
			if($this->get_result('localexten') === null
			|| $this->get_result('hints') === null
			|| $this->get_result('extenumbers') === null)
			{
				$delete_links = true;
				$this->delete_macro('localexten');
				$this->delete_hints();
				$this->delete_extenumbers();
			}
			else if($this->edit_macro('localexten') === false
			|| $this->edit_hints() === false
			|| $this->edit_extenumbers() === false)
			{
				$this->_reverse_edit($protocolret);
				return(false);
			}
		}
		else if($this->add_macro('localexten') === false
		|| $this->add_hints() === false
		|| $this->add_extenumbers() === false)
		{
			$this->_reverse_edit($protocolret);
			return(false);
		}

		if($this->edit_groupmember() === false
		|| $this->edit_queuemember() === false
		|| $this->edit_voicemail() === false)
		{
			$this->_reverse_edit($protocolret);
			return(false);
		}

		$this->edit_phone();

		if($delete_links === true)
			$this->delete_links();

		return(true);
	}

	function edit_protocol()
	{
		if(xivo_issa('protocol',$this->_info) === false
		|| xivo_issa('ufeatures',$this->_info) === false
		|| ($protocol = &$this->_ipbx->get_protocol_module($this->_protocol)) === false)
			return(false);

		if($this->_info['ufeatures']['protocol'] === $this->_protocol)
		{
			if(($rs = $this->get_result('protocol')) === null)
				return(false);

			$this->_return['protocol'] = $protocol->edit($this->_info['protocol']['id'],$rs);

			return($this->_return['protocol']);
		}

		if(($return = $this->add_protocol()) === false)
			return(false);

		if(($proto = &$this->_ipbx->get_protocol_module($this->_info['ufeatures']['protocol'])) === false
		|| $proto->delete($this->_info['protocol']['id']) === false)
		{
			$protocol->delete($return);
			return(false);
		}

		$this->_return['protocol'] = $return;

		return($this->_return['protocol']);
	}

	function edit_origin_protocol($protocolid=0)
	{
		if(xivo_issa('protocol',$this->_origin) === false
		|| xivo_issa('ufeatures',$this->_info) === false
		|| ($protocol = &$this->_ipbx->get_protocol_module($this->_info['ufeatures']['protocol'])) === false)
			return(false);

		if($this->_info['ufeatures']['protocol'] === $this->_protocol)
		{
			$protocol->set_origin($this->_origin['protocol']);
			return($protocol->edit_origin());
		}

		if(($protocolid = xivo_uint($protocolid)) === 0
		|| ($proto = &$this->_ipbx->get_protocol($this->_protocol)) === false
		|| $proto->delete($protocolid) === false)
			return(false);

		$proto->set_origin($this->_origin['protocol']);

		return($proto->add_origin());
	}

	function edit_userfeatures($protocolid)
	{
		$rs = false;

		if(xivo_issa('ufeatures',$this->_info) === false
		|| xivo_issa('protocol',$this->_info) === false
		|| ($rs = $this->get_result('ufeatures')) === null
		|| $rs === false)
			return($rs);

		if($this->_info['ufeatures']['protocol'] === $this->_protocol)
		{
			$rs['protocolid'] = $this->_info['protocol']['id'];
			$provisioning = false;
		}
		else if(($protocolid = xivo_uint($protocolid)) !== 0)
		{
			$rs['protocolid'] = $protocolid;
			$provisioning = true;
		}
		else
			return(false);

		$ufeatures = &$this->_ipbx->get_module('userfeatures');
		$this->_return['userfeatures'] = $ufeatures->edit($this->_info['ufeatures']['id'],$rs,$provisioning);

		return($this->_return['userfeatures']);
	}

	function edit_macro($name)
	{
		$rs = false;

		if(xivo_issa($name,$this->_info) === false
		|| ($rs = $this->get_result($name)) === null
		|| $rs === false)
			return($rs);

		$extensions = &$this->_ipbx->get_module('extensions');
		$this->_return[$name] = $extensions->edit($this->_info[$name]['id'],$rs);

		return($this->_return[$name]);
	}

	function edit_hints()
	{
		$rs = false;

		if(xivo_issa('hints',$this->_info) === false
		|| ($rs = $this->get_result('hints')) === null
		|| $rs === false)
			return($rs);

		$extensions = &$this->_ipbx->get_module('extensions');
		$this->_return['hints'] = $extensions->edit($this->_info['hints']['id'],$rs);

		return($this->_return['hints']);
	}

	function edit_extenumbers()
	{
		$rs = false;

		if(xivo_issa('extenumbers',$this->_info) === false
		|| ($rs = $this->get_result('extenumbers')) === null
		|| $rs === false)
			return($rs);

		$extenumbers = &$this->_ipbx->get_module('extenumbers');
		$this->_return['extenumbers'] = $extenumbers->edit($this->_info['extenumbers']['id'],$rs);

		return($this->_return['extenumbers']);
	}

	function edit_groupmember()
	{
		if(xivo_issa('ufeatures',$this->_info) === false)
			return(false);

		if(xivo_issa('groupmember',$this->_info) === false)
			return($this->add_groupmember($this->_info['ufeatures']['id']));

		$this->_return['groupmember'] = $this->delete_groupmember();

		if($this->get_result('groupmember') === null)
			return($this->_return['groupmember']);

		return($this->_edit_member_type('group'));
	}

	function edit_queuemember()
	{
		if(xivo_issa('ufeatures',$this->_info) === false)
			return(false);

		if(xivo_issa('queuemember',$this->_info) === false)
			return($this->add_queuemember($this->_info['ufeatures']['id']));

		$this->_return['queuemember'] = $this->delete_queuemember();

		if($this->get_result('queuemember') === null)
			return($this->_return['queuemember']);

		return($this->_edit_member_type('queue'));
	}

	function _edit_member_type($type)
	{
		if(($type !== 'group' && $type !== 'queue') === true
		|| xivo_issa('ufeatures',$this->_info) === false)
			return(false);

		$key = $type.'member';

		if(($rs = $this->get_result($key)) === null || $rs === false)
			return($rs);
	
		$qmember = &$this->_ipbx->get_module('queuemember');

		$this->_return[$key] = array();

		$nb = count($rs);

		for($i = 0;$i < $nb;$i++)
		{
			$rs[$i]['userid'] = $this->_info['ufeatures']['id'];

			if(($this->_return[$key][] = $qmember->add($rs[$i])) === false)
				return(false);
		}

		if(isset($this->_return[$key][0]) === false)
			$this->_return[$key] = false;

		return(true);
	}

	function edit_voicemail()
	{
		if(xivo_issa('voicemail',$this->_info) === false)
			return($this->add_voicemail());

		if(xivo_issa('ufeatures',$this->_info) === false
		|| ($voicemail = &$this->_ipbx->get_module('uservoicemail')) === false)
			return(false);

		if($this->_info['ufeatures']['number'] === '')
			$r = $voicemail->delete($this->_info['voicemail']['id']);
		else if($this->get_result('voicemail') === null)
			$r = $voicemail->disable($this->_info['voicemail']['id']);
		else
			$r = true;

		return(($this->_return['uservoicemail'] = $r));
	}

	function edit_phone()
	{
		$rs = false;

		if(xivo_issa('ufeatures',$this->_info) === false
		|| ($rs = $this->get_result('autoprov')) === null
		|| $rs === false)
			return($rs);

		$autoprov = &$this->_ipbx->get_module('autoprov');

		$rs['iduserfeatures'] = $this->_info['ufeatures']['id'];

		if(xivo_issa('autoprov',$this->_info) !== false
		&& (bool) $this->_info['autoprov']['isinalan'] === true)
			$this->_return['autoprov'] = $autoprov->notification($rs,$rs['modact']);
		else
			$this->_return['autoprov'] = $autoprov->authoritative($rs,$rs['modact']);

		return($this->_return['autoprov']);
	}

	function _reverse_edit($protocolid=0)
	{
		if($this->get_errnb() > 0
		|| $this->_protocol === false
		|| ($arr = xivo_get_aks($this->_return)) === false)
			return(false);

		$nb = count($arr);

		for($i = 0;$i < $nb;$i++)
		{
			$key = &$arr['keys'][$i];
			$val = &$this->_return[$key];

			if($val === false)
				continue;

			switch($key)
			{
				case 'protocol':
					$this->edit_origin_protocol($protocolid);
					break;
				case 'userfeatures':
					$ufeatures = &$this->_ipbx->get_module('userfeatures');
					$ufeatures->set_origin($this->_origin['ufeatures']);
					$ufeatures->edit_origin();
					break;
				case 'uservoicemail':
					$voicemail = &$this->_ipbx->get_module('uservoicemail');

					if($this->_origin['voicemail'] === false)
						$voicemail->delete($val);
					else
					{
						$voicemail->set_origin($this->_origin['voicemail']);
						$voicemail->edit_origin();
					}
					break;
				case 'localexten':
					$extensions = &$this->_ipbx->get_module('extensions');

					if($this->_origin['localexten'] === false)
						$extensions->delete_exten($val);
					else
					{
						$extensions->set_origin($this->_origin['localexten']);
						$extensions->edit_origin();
					}
					break;
				case 'hints':
					$extensions = &$this->_ipbx->get_module('extensions');

					if($this->_origin['hints'] === false)
						$extensions->delete_hints($val);
					else
					{
						$extensions->set_origin($this->_origin['hints']);
						$extensions->edit_origin();
					}
					break;
				case 'groupmember':
				case 'queuemember':
					if($key === 'groupmember')
						$this->delete_groupmember();
					else
						$this->delete_queuemember();

					if($this->_origin[$key] === false
					|| ($nb = count($this->_origin[$key])) === 0)
						continue;

					$qmember = &$this->_ipbx->get_module('queuemember');

					for($i = 0;$i < $nb;$i++)
						$qmember->add($this->_origin[$key][$i]);
					break;
				case 'autoprov':
					break;
				default:
					if(($module = &$this->_ipbx->get_module($key)) === false)
						continue;

					if($this->_origin[$key] === false)
						$module->delete($val);
					else
					{
						$module->set_origin($this->_origin[$key]);
						$module->edit_origin();
					}
			}
		}
		return(true);
	}

	function delete_macro($name)
	{
		if(xivo_issa($name,$this->_info) === false
		|| ($extensions = &$this->_ipbx->get_module('extensions')) === false)
			return(false);

		return($extensions->delete($this->_info[$name]['id']));
	}

	function delete_hints()
	{
		if(xivo_issa('hints',$this->_info) === false
		|| ($extensions = &$this->_ipbx->get_module('extensions')) === false)
			return(false);

		return($extensions->delete($this->_info['hints']['id']));
	}

	function delete_extenumbers()
	{
		if(xivo_issa('extenumbers',$this->_info) === false
		|| ($extenumbers = &$this->_ipbx->get_module('extenumbers')) === false)
			return(false);

		return($extenumbers->delete($this->_info['extenumbers']['id']));
	}

	function delete_groupmember()
	{
		return($this->_delete_member_type('group'));
	}

	function delete_queuemember()
	{
		return($this->_delete_member_type('queue'));
	}

	function _delete_member_type($type)
	{
		if(($type !== 'group' && $type !== 'queue') === true
		|| xivo_issa('ufeatures',$this->_info) === false
		|| ($qmember = &$this->_ipbx->get_module('queuemember')) === false)
			return(false);

		$qmember_where = array();
		$qmember_where['type'] = 'user';
		$qmember_where['category'] = $type;
		$qmember_where['userid'] = $this->_info['ufeatures']['id'];

		return($qmember->delete_where($qmember_where));
	}

	function delete_links()
	{
		if(xivo_issa('ufeatures',$this->_info) === false)
			return(false);

		if(($incall = &$this->_ipbx->get_module('incall')) !== false)
		{
			$incall_where = array();
			$incall_where['type'] = 'user';
			$incall_where['typeval'] = $this->_info['ufeatures']['id'];

			$incall->unlinked_where($incall_where);
		}

		if(($schedule = &$this->_ipbx->get_module('schedule')) === false)
			return(true);
		
		$schedule_where = array();
		$schedule_where['typetrue'] = 'user';
		$schedule_where['typevaltrue'] = $this->_info['ufeatures']['id'];

		$schedule->unlinked_where($schedule_where);

		$schedule_where = array();
		$schedule_where['typefalse'] = 'user';
		$schedule_where['typevalfalse'] = $this->_info['ufeatures']['id'];

		$schedule->unlinked_where($schedule_where);
	}
}

?>

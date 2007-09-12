<?php

require_once(XIVO_PATH_OBJECT.XIVO_SEP_DIR.'application'.XIVO_SEP_DIR.'abstract.inc');

class xivo_application_service_asterisk_user extends xivo_application_abstract
{
	var $_ipbx	= null;
	var $_protocol	= false;

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

	function get($id)
	{
		if(($ufeatures = $this->_ipbx->get_module('userfeatures')) === false
		|| ($this->_info['ufeatures'] = $ufeatures->get($id)) === false
		|| ($protocol = &$this->_ipbx->get_protocol_module($this->_info['ufeatures']['protocol'])) === false
		|| ($this->_info['protocol'] = $protocol->get($this->_info['ufeatures']['protocolid'])) === false)
			return(false);

		return($this->_info);
	}

	function set($arr,$protocol)
	{
		$this->_reset();

		if(xivo_issa('protocol',$arr) === false
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

			if(($localexten = $this->set_macro($number,$appdata,$context)) === false)
				$this->_set_error('localexten');

			$this->_set_result('localexten',$localexten);

			$callerid = (string) $this->get_result_var('protocol','callerid');
			$callerid = $this->_ipbx->mk_callerid($number,$callerid);

			$this->_set_result_var('protocol','callerid',$callerid);

			if(($hints = $this->set_hints($name,$protocol,$number)) === false)
				$this->_set_error('hints');

			$this->_set_result('hints',$hints);

			$this->set_extenumbers($number,$context);
		}

		if(xivo_issa('group',$arr) === true
		&& xivo_issa('group-select',$arr) === true
		&& $this->set_group_list($arr['group'],$arr['group-select'],$name,$context,$number,$protocol) === true)
		{
			$callgroup = $this->get_result('callgroup');

			if(is_array($callgroup) === true && isset($callgroup[0]) === true)
				$callgroup = implode(',',$callgroup);
			else
				$callgroup = '';

			$this->_set_result_var('protocol','callgroup',$callgroup);
		}

		if(xivo_issa('queue',$arr) === true && xivo_issa('queue-select',$arr) === true)
			$this->set_queue_list($arr['queue'],$arr['queue-select'],$name,$context,$number,$protocol);

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
			return(false);

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
			return(false);

		if(($rs = $ufeatures->chk_values($arr)) === false)
		{
			$this->_set_result('ufeatures',$ufeatures->get_filter_result());
			$this->_set_error('ufeatures',$ufeatures->get_filter_error());

			return(false);
		}

		$this->_set_result('ufeatures',$rs);

		return(true);
	}

	function set_macro($exten,$arr,$context)
	{
		if(($extensions = &$this->_ipbx->get_module('extensions')) === false)
			return(false);

		$rs = $extensions->new_exten('macro',$arr,$exten,$context);

		return($rs);
	}

	function set_hints($name,$protocol,$number)
	{
		if(($extensions = &$this->_ipbx->get_module('extensions')) === false)
			return(false);

		$rs = $extensions->new_hints($name,$protocol,$number);

		return($rs);
	}

	function set_extenumbers($number,$context)
	{
		if(($extenumbers = &$this->_ipbx->get_module('extenumbers')) === false)
			return(false);

		$extenum = array();
		$extenum['exten'] = $number;
		$extenum['context'] = $context;

		$exists = false;

		if(($rs = $extenumbers->chk_values($extenum)) === false
		|| ($exists = $extenumbers->exists($rs)) !== false)
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

	function set_group_list($group,$select,$name,$context='',$number='',$protocol='')
	{
		$r = $this->_set_group_queue_list('group',$group,$select,$name,$context,$number,$protocol);

		return($r);
	}

	function set_queue_list($queue,$select,$name,$context='',$number='',$protocol='')
	{
		$r = $this->_set_group_queue_list('queue',$queue,$select,$name,$context,$number,$protocol);

		return($r);
	}

	function _set_group_queue_list($type,$list,$select,$name,$context='',$number='',$protocol='')
	{
		if(($type !== 'group' && $type !== 'queue') === true
		|| is_array($list) === false
		|| is_array($select) === false
		|| ($typefeatures = &$this->_ipbx->get_module($type.'features')) === false
		|| ($qmember = &$this->_ipbx->get_module('queuemember')) === false)
			return(false);

		$slt = array_values($select);

		if(($nb = count($slt)) === false)
			return(null);

		$info = array();
		$info['usertype'] = 'user';
		$info['userid'] = 0;
		$info['category'] = $type;

		$key = $type.'_list';

		$tmp = $type_list = $callgroup = array();

		for($i = 0;$i < $nb;$i++)
		{
			$name = &$slt[$i];

			$info['queue_name'] = $name;

			if(is_scalar($name) === false
			|| isset($tmp[$name]) === true
			|| isset($list[$name]) === false
			|| ($chantype = xivo_ak('chantype',$list[$name],true)) === false
			|| ($interface = $this->mk_interface($name,$chantype,$context,$number,$protocol)) === false
			|| ($typeid = $typefeatures->get_id(array('name' => $name))) === false)
				continue;

			$info['interface'] = $interface['interface'];
			$info['channel'] = $interface['channel'];

			$values = array_merge($list[$name],$info);

			if(($rs = $qmember->chk_values($values)) === false)
				continue;

			$tmp[$name] = 1;
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
		|| $this->add_group_list($ufeaturesid) === false
		|| $this->add_queue_list($ufeaturesid) === false
		|| $this->add_voicemail() === false
		|| $this->send_autoprov($ufeaturesid) === false)
		{
			if($ufeaturesid !== null)
				$this->_reverse_add();

			return(false);
		}

		return(true);
	}

	function add_protocol()
	{
		if(($rs = $this->get_result('protocol')) === null)
			return(false);

		$protocol = &$this->_ipbx->get_protocol_module($this->_protocol);
		$this->_return['protocol'] = $protocol->add($rs);

		return($this->_return['protocol']);
	}

	function add_userfeatures($protocolid)
	{
		if(($rs = $this->get_result('ufeatures')) === null 
		|| ($protocolid = xivo_uint($protocolid)) === 0)
			return(false);

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

	function add_group_list($ufeaturesid)
	{
		return($this->_add_group_queue_list('group',$ufeaturesid));
	}

	function add_queue_list($ufeaturesid)
	{
		return($this->_add_group_queue_list('queue',$ufeaturesid));
	}

	function _add_group_queue_list($type,$ufeaturesid)
	{
		if(($type !== 'group' && $type !== 'queue') === true
		|| ($ufeaturesid = xivo_uint($ufeaturesid)) === 0)
			return(false);

		$key = $type.'_list';

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

	function send_autoprov($ufeaturesid)
	{
		if(($ufeaturesid = xivo_uint($ufeaturesid)) === 0)
			return(false);

		if(($rs = $this->get_result('autoprov')) === null || $rs === false)
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
		|| ($arr = xivo_get_aks($this->_return)) === true)
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
				case 'group_list':
				case 'queue_list':
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
	}
}

?>

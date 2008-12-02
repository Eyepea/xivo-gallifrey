<?php

xivo_file::required(array(XIVO_PATH_OBJECT,'application','service','ipbx','asterisk','abstract','datastorage','abstract','sql.inc'),true);

class xivo_application_service_asterisk_trunk_sql extends xivo_application_service_asterisk_abstract_sql
{
	var $_table	= array(
				'trunkfeatures'	=> '',
				'trunksip'	=> '',
				'trunkiax'	=> '',
				'trunkcustom'	=> '');

	function get_all($protocols,$disable=null,$order=null,$limit=null)
	{
		$arr = array();

		$this->_dso->new_select($this->_table['trunkfeatures'],array('trunkfeaturesid'	=> 'id',
									     'protocol',
									     'protocolid'));

		if(is_array($protocols) === false)
			$use_sip = $use_iax = $use_custom = true;
		else
		{
			$use_sip = in_array(XIVO_SRE_IPBX_AST_PROTO_SIP,$protocols);
			$use_iax = in_array(XIVO_SRE_IPBX_AST_PROTO_IAX,$protocols);
			$use_custom = in_array(XIVO_SRE_IPBX_AST_PROTO_CUSTOM,$protocols);
			$this->_dso->wherein('protocol',$protocols,$this->_table['trunkfeatures']);
		}

		if($use_sip === false && $use_iax === false && $use_custom === false)
			return(false);

		$sip = $iax = $custom = array();
		$sip['category'] = $iax['category'] = $custom['category'] = 'trunk';

		if($disable !== null)
		{
			$disable = intval((bool) $disable);
			$arr['commented'] = $disable;
			$sip['commented'] = $disable;
			$iax['commented'] = $disable;
			$custom['commented'] = $disable;
		}

		$this->_dso->andwhere($arr,$this->_table['trunkfeatures']);

		if($use_sip === true)
		{
			$this->_dso->leftjoin($this->_table['trunksip'],
					      $this->_table['trunkfeatures'].'.protocolid = '.$this->_table['trunksip'].'.id',
					      null,
					      array('sipname'		=> 'name',
						    'siptype'		=> 'type',
						    'sipcontext'	=> 'context',
						    'siphost'		=> 'host',
						    'sipcalllimit'	=> 'call-limit',
						    'sipcommented'	=> 'commented'));

			$this->_dso->joinwhere($this->_table['trunksip'],$sip);

			$this->_dso->andjoinwhere($this->_table['trunksip'],
						  $this->_table['trunkfeatures'].'.protocol = '.$this->_table['trunksip'].'.protocol');
		}

		if($use_iax === true)
		{
			$this->_dso->leftjoin($this->_table['trunkiax'],
					      $this->_table['trunkfeatures'].'.protocolid = '.$this->_table['trunkiax'].'.id',
					      null,
					      array('iaxname'		=> 'name',
						    'iaxtype'		=> 'type',
						    'iaxcontext'	=> 'context',
						    'iaxhost'		=> 'host',
						    'iaxcommented'	=> 'commented'));

			$this->_dso->joinwhere($this->_table['trunkiax'],$iax);

			$this->_dso->andjoinwhere($this->_table['trunkiax'],
						  $this->_table['trunkfeatures'].'.protocol = '.$this->_table['trunkiax'].'.protocol');
		}

		if($use_custom === true)
		{
			$this->_dso->leftjoin($this->_table['trunkcustom'],
					      $this->_table['trunkfeatures'].'.protocolid = '.$this->_table['trunkcustom'].'.id',
					      null,
					      array('customname'	=> 'name',
						    'customcontext'	=> 'context',
						    'custominterface'	=> 'interface',
						    'customintfsuffix'	=> 'intfsuffix',
						    'customcommented'	=> 'commented'));

			$this->_dso->joinwhere($this->_table['trunkcustom'],$custom);

			$this->_dso->andjoinwhere($this->_table['trunkcustom'],
						  $this->_table['trunkfeatures'].'.protocol = '.$this->_table['trunkcustom'].'.protocol');
		}

		$this->_dso->aggregate_where();

		if($use_sip === true)
			$this->_dso->where($this->_table['trunksip'].'.id IS NOT NULL');

		if($use_iax === true)
		{
			if($use_sip === false)
				$this->_dso->where($this->_table['trunkiax'].'.id IS NOT NULL');
			else
				$this->_dso->orwhere($this->_table['trunkiax'].'.id IS NOT NULL');
		}

		if($use_custom === true)
		{
			if($use_sip === false && $use_iax === false)
				$this->_dso->where($this->_table['trunkcustom'].'.id IS NOT NULL');
			else
				$this->_dso->orwhere($this->_table['trunkcustom'].'.id IS NOT NULL');
		}

		$this->_dso->aggregate_where();

		$this->_dso->new_order($order,null,$this->_table['trunkfeatures']);
		$this->_dso->limit($limit);

		$r = $this->_dso->select_all();

		$this->_dso->reset_order();
		$this->_dso->reset_limit();

		if(($this->_cnt = $this->_dso->select_count(null,'id',$this->_table['trunkfeatures'])) === false
		|| isset($r[0]) === false)
			$r = false;

		return($r);
	}
}

?>

<?php

xivo_file::required(array(XIVO_PATH_OBJECT,'service','ipbx','asterisk','abstract.inc'),true);

class xivo_service_asterisk_cdr extends xivo_service_asterisk_abstract
{
	var $_dso	= null;
	var $_name	= 'cdr';
	var $_filter	= null;

	function xivo_service_asterisk_cdr(&$sre,&$dso)
	{
		if(is_object($sre) === false)
			trigger_error('Invalid service in '.__CLASS__,E_USER_ERROR);

		if(is_object($dso) === false)
			trigger_error('Invalid datastorage in '.__CLASS__,E_USER_ERROR);

		$this->_sre = &$sre;
		$this->_dso = &$dso;

		$this->_load_config();
	}

	function get($id)
	{
		if(($id = xivo_uint($id)) === 0 || ($r = $this->_dso->get($id)) === false)
			return(false);

		$this->_origin = $r;

		return($r);
	}

	function search($arr)
	{
		$r = false;

		if(is_array($arr) === false)
			return(false);

		$search = array();

		if(isset($arr['dbeg'],$arr['dbeg']{0}) === true)
		{
			if(($dbeg = $this->_chk_date($arr['dbeg'])) === false)
				return(false);

			$search['dbeg'] = $dbeg['date'];

			if(isset($arr['dend'],$arr['dend']{0}) === true)
			{
				if(($dend = $this->_chk_date($arr['dend'],$dbeg['dateint'])) === false)
					return(false);

				$search['dend'] = $dend['date'];
			}
		}

		if(isset($arr['channel'],$arr['channel']{0}) === true)
			$search['channel'] = $arr['channel'];

		if(isset($arr['disposition'],$arr['disposition']{0}) === true)
			$search['disposition'] = $arr['disposition'];

		if(isset($arr['src'],$arr['src']{0},$arr['srcformat']) === true)
			$search['src'] = array('pattern' => $arr['src'],'type' => $arr['srcformat']);

		if(isset($arr['clid'],$arr['clid']{0},$arr['clidformat']) === true)
			$search['clid'] = array('pattern' => $arr['clid'],'type' => $arr['clidformat']);

		if(isset($arr['accountcode'],$arr['accountcode']{0},$arr['accountcodeformat']) === true)
			$search['accountcode'] = array('pattern' => $arr['accountcode'],'type' => $arr['accountcodeformat']);

		if(isset($arr['userfield'],$arr['userfield']{0},$arr['userfieldformat']) === true)
			$search['userfield'] = array('pattern' => $arr['userfield'],'type' => $arr['userfieldformat']);

		$r = $this->_dso->search($search);

		return($r);
	}

	function _chk_date($date,$dbeg='')
	{
		$r = array();

		$date = (string) $date;
		$dbeg = (string) $dbeg;

		if($dbeg !== '' && ($dbeg = xivo_uint($dbeg)) === 0)
			return(false);

		if($dbeg === 0)
			$dbeg = '';

		if(preg_match('/^(2[0-9]{3})(?:-(0?[1-9]|1[0-2])(?:-(0?[1-9]|1[0-9]|2[0-9]|3[0-1]))?)?$/',$date,$match) !== 1)
			return(false);

		$timeformat = '%Y';
		$r['date'] = $r['dateint'] = $match[1];

		if(isset($match[2]) === true)
		{
			$timeformat .= '%m';
			$month = sprintf('%02u',$match[2]);
			$r['date'] .= '-'.$month;
			$r['dateint'] .= $month;
		}

		if(isset($match[3]) === true)
		{
			$timeformat .= '%d';
			$day = sprintf('%02u',$match[3]);
			$r['date'] .= '-'.$day;
			$r['dateint'] .= $day;
		}

		if($r['dateint'] > strftime($timeformat))
			return(false);

		if($dbeg !== '' && (strlen($r['dateint']) !== strlen($dbeg) || $r['dateint'] > $dbeg) === true)
			return(false);

		return($r);
	}
}

?>

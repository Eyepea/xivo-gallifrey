<?php

$act = isset($_QR['act']) === true ? $_QR['act'] : '';
$page = isset($_QR['page']) === true ? xivo_uint($_QR['page'],1) : 1;

$meetme = &$ipbx->get_module('meetme');
$mfeatures = &$ipbx->get_module('meetmefeatures');
$extensions = &$ipbx->get_module('extensions');
$extenumbers = &$ipbx->get_module('extenumbers');

$param = array();
$param['act'] = 'list';

$info = $result = array();

switch($act)
{
	case 'add':
		$add = true;
		$result = null;
		$meetmeval = '';

		$musiconhold = &$ipbx->get_module('musiconhold');

		if(($moh_list = $musiconhold->get_all_category(null,false)) !== false)
			ksort($moh_list);

		do
		{
			if(isset($_QR['fm_send']) === false || xivo_issa('meetme',$_QR) === false || xivo_issa('mfeatures',$_QR) === false)
				break;

			if($moh_list === false || isset($_QR['mfeatures']['musiconhold'],$moh_list[$_QR['mfeatures']['musiconhold']]) === false)
				$_QR['mfeatures']['musiconhold'] = '';

			if(($result['meetme'] = $meetme->chk_values($_QR['meetme'])) === false)
			{
				$add = false;
				$result['meetme'] = $meetme->get_filter_result();
			}
			else
			{
				$meetmeval = $result['meetme']['number'].',';

				if($result['meetme']['pin'] !== '')
					$meetmeval .= $result['meetme']['pin'];

				if($result['meetme']['admin-pin'] !== '')
					$meetmeval .= ','.$result['meetme']['admin-pin'];

				$meetmeval = rtrim($meetmeval,',');
			}

			$_QR['mfeatures']['meetmeid'] = 0;
			$_QR['mfeatures']['number'] = $result['meetme']['number'];

			if(($result['mfeatures'] = $mfeatures->chk_values($_QR['mfeatures'])) === false)
			{
				$add = false;
				$result['mfeatures'] = $mfeatures->get_filter_result();
			}

			$local_exten = $exten_numbers = null;

			if($add === true && $result['mfeatures']['number'] !== '')
			{
				$local_exten = array();
				$local_exten['exten'] = $result['mfeatures']['number'];
				$local_exten['appdata'] = 'supermeetme';

				if($result['mfeatures']['context'] === '')
					$local_exten['context'] = 'local-extensions';
				else
					$local_exten['context'] = $result['mfeatures']['context'];

				if(($result['local_exten'] = $extensions->chk_macro($local_exten)) === false)
				{
					$add = false;
					$result['local_exten'] = $extensions->get_filter_result();
				}

				$exten_numbers = array();
				$exten_numbers['exten'] = $result['local_exten']['exten'];
				$exten_numbers['context'] = $result['local_exten']['context'];

				if(($result['extenumbers'] = $extenumbers->chk_values($exten_numbers)) === false
				|| $extenumbers->exists($result['extenumbers']) !== false)
				{
					$add = false;
					$result['extenumbers'] = $extenumbers->get_filter_result();
				}
			}

			if($add === false || ($meetmeid = $meetme->add($meetmeval)) === false)
				break;

			$result['mfeatures']['meetmeid'] = $meetmeid;

			if(($mfeaturesid = $mfeatures->add($result['mfeatures'])) === false)
			{
				$meetme->delete($meetmeid);
				break;
			}

			if($local_exten !== null && ($local_extenid = $extensions->add($result['local_exten'])) === false)
			{
				$meetme->delete($meetmeid);
				$mfeatures->delete($mfeaturesid);
				break;
			}

			if($exten_numbers !== null && ($extenumid = $extenumbers->add($result['extenumbers'])) === false)
			{
				$meetme->delete($meetmeid);
				$mfeatures->delete($mfeaturesid);

				if($local_exten !== null)
					$extensions->delete($local_extenid);
				break;
			}

			xivo_go($_HTML->url('service/ipbx/pbx_settings/meetme'),$param);
		}
		while(false);

		$element = array();
		$element['meetme'] = $meetme->get_element();
		$element['mfeatures'] = $mfeatures->get_element();

		$_HTML->assign('info',$result);
		$_HTML->assign('moh_list',$moh_list);
		$_HTML->assign('element',$element);
		break;
	case 'edit':
		$edit = true;
		$meetmeval = '';

		$return = &$info;

		if(isset($_QR['id']) === false
		|| ($info['meetme'] = $meetme->get($_QR['id'])) === false
		|| ($info['mfeatures'] = $mfeatures->get_where(array('meetmeid' => $info['meetme']['id']))) === false)
			xivo_go($_HTML->url('service/ipbx/pbx_settings/meetme'),$param);

		$musiconhold = &$ipbx->get_module('musiconhold');

		if(($moh_list = $musiconhold->get_all_category(null,false)) !== false)
			ksort($moh_list);

		$status = array();
		$status['localexten'] = $status['extenumbers'] = false;

		do
		{
			if(isset($_QR['fm_send']) === false || xivo_issa('meetme',$_QR) === false || xivo_issa('mfeatures',$_QR) === false)
				break;

			if($moh_list === false || isset($_QR['mfeatures']['musiconhold'],$moh_list[$_QR['mfeatures']['musiconhold']]) === false)
				$_QR['mfeatures']['musiconhold'] = '';

			if(($result['meetme'] = $meetme->chk_values($_QR['meetme'])) === false)
			{
				$edit = false;
				$result['meetme'] = $meetme->get_filter_result();
			}
			else
			{
				$meetmeval = $result['meetme']['number'].',';

				if($result['meetme']['pin'] !== '')
					$meetmeval .= $result['meetme']['pin'];

				if($result['meetme']['admin-pin'] !== '')
					$meetmeval .= ','.$result['meetme']['admin-pin'];

				$meetmeval = rtrim($meetmeval,',');
			}

			$_QR['mfeatures']['meetmeid'] = $info['meetme']['id'];
			$_QR['mfeatures']['number'] = $result['meetme']['number'];

			if(($result['mfeatures'] = $mfeatures->chk_values($_QR['mfeatures'])) === false)
			{
				$edit = false;
				$result['mfeatures'] = $mfeatures->get_filter_result();
			}

			$exten_where = array();
			$exten_where['exten'] = $info['mfeatures']['number'];
			$exten_where['app'] = 'Macro';
			$exten_where['appdata'] = 'supermeetme';

			if($info['mfeatures']['context'] === '')
				$exten_where['context'] = 'local-extensions';
			else
				$exten_where['context'] = $info['mfeatures']['context'];

			if(($info['localexten'] = $extensions->get_where($exten_where)) !== false)
			{
				if($result['mfeatures']['number'] === '')
					$status['localexten'] = 'delete';
				else
				{
					$status['localexten'] = 'edit';

					$local_exten = $info['localexten'];
					$local_exten['exten'] = $result['mfeatures']['number'];

					if($result['mfeatures']['context'] === '')
						$local_exten['context'] = 'local-extensions';
					else
						$local_exten['context'] = $result['mfeatures']['context'];

					if(($result['localexten'] = $extensions->chk_values($local_exten)) === false)
					{
						$edit = false;
						$result['localexten'] = array_merge($info['localexten'],$extensions->get_filter_result());
					}
				}
			}
			else if($result['mfeatures']['number'] !== '')
			{
				$status['localexten'] = 'add';

				$local_exten = $exten_where;
				$local_exten['exten'] = $result['mfeatures']['number'];
				$local_exten['priority'] = 1;

				if($result['mfeatures']['context'] === '')
					$local_exten['context'] = 'local-extensions';
				else
					$local_exten['context'] = $result['mfeatures']['context'];

				if(($result['localexten'] = $extensions->chk_values($local_exten)) === false)
				{
					$edit = false;
					$result['localexten'] = $extensions->get_filter_result();
				}
			}

			$exten_numbers = array();
			$exten_numbers['exten'] = $result['mfeatures']['number'];

			if($result['mfeatures']['context'] === '')
				$exten_numbers['context'] = 'local-extensions';
			else
				$exten_numbers['context'] = $result['mfeatures']['context'];

			$exten_where = array();
			$exten_where['exten'] = $info['mfeatures']['number'];

			if($info['mfeatures']['context'] === '')
				$exten_where['context'] = 'local-extensions';
			else
				$exten_where['context'] = $info['mfeatures']['context'];

			if(($info['extenumbers'] = $extenumbers->get_where($exten_where)) !== false)
			{
				if($result['mfeatures']['number'] === '')
					$status['extenumbers'] = 'delete';
				else
				{
					$status['extenumbers'] = 'edit';

					if(($result['extenumbers'] = $extenumbers->chk_values($exten_numbers)) === false
					|| $extenumbers->exists($result['extenumbers'],$info['extenumbers']['id']) !== false)
					{
						$edit = false;
						$result['extenumbers'] = array_merge($info['extenumbers'],$extenumbers->get_filter_result());
					}
				}
			}
			else if($result['mfeatures']['number'] !== '')
			{
				$status['extenumbers'] = 'add';

				if(($result['extenumbers'] = $extenumbers->chk_values($exten_numbers)) === false
				|| $extenumbers->exists($result['extenumbers']) !== false)
				{
					$edit = false;
					$result['extenumbers'] = $extenumbers->get_filter_result();
				}
			}

			if($edit === false || $meetme->edit($info['meetme']['id'],$meetmeval) === false)
				break;

			if($mfeatures->edit($info['mfeatures']['id'],$result['mfeatures']) === false)
			{
				$meetme->edit_origin();
				break;
			}

			switch($status['localexten'])
			{
				case 'add':
					$rs_localexten = $extensions->add($result['localexten']);
					break;
				case 'edit':
					$rs_localexten = $extensions->edit($info['localexten']['id'],$result['localexten']);
					break;
				case 'delete':
					$rs_localexten = $extensions->delete($info['localexten']['id']);
					break;
				default:
					$rs_localexten = null;
			}

			if($rs_localexten === false)
			{
				$meetme->edit_origin();
				$mfeatures->edit_origin();
				break;
			}

			$rs_dfeatures = null;

			$dfeatures = &$ipbx->get_module('didfeatures');
			$dfeatures_where = array();
			$dfeatures_where['type'] = 'meetme';
			$dfeatures_where['typeid'] = $info['mfeatures']['id'];
			$dfeatures_where['commented'] = 0;

			switch($status['extenumbers'])
			{
				case 'add':
					$rs_extenumbers = $extenumbers->add($result['extenumbers']);
					break;
				case 'edit':
					$rs_extenumbers = $extenumbers->edit($info['extenumbers']['id'],$result['extenumbers']);
					break;
				case 'delete':
					if(($rs_extenumbers = $extenumbers->delete($info['extenumbers']['id'])) !== false
					&& ($info['dfeatures'] = $dfeatures->get_list_where($dfeatures_where,false)) !== false
					&& ($rs_dfeatures = $dfeatures->edit_where($dfeatures_where,array('commented' => 1))) === false)
						$rs_extenumbers = false;
					break;
				default:
					$rs_extenumbers = null;
			}

			if($rs_extenumbers === false)
			{
				$meetme->edit_origin();
				$mfeatures->edit_origin();

				if($rs_dfeatures === false)
					$extenumbers->add_origin();

				if($rs_localexten === null)
					break;

				switch($status['localexten'])
				{
					case 'add':
						$extensions->delete($rs_localexten);
						break 2;
					case 'edit':
						$extensions->edit_origin();
						break 2;
					case 'delete':
						$extensions->add_origin();
						break 2;
					default:
						break 2;
				}
			}

			xivo_go($_HTML->url('service/ipbx/pbx_settings/meetme'),$param);
		}
		while(false);

		$element = array();
		$element['meetme'] = $meetme->get_element();
		$element['mfeatures'] = $mfeatures->get_element();

		$_HTML->assign('id',$info['meetme']['id']);
		$_HTML->assign('info',$return);
		$_HTML->assign('moh_list',$moh_list);
		$_HTML->assign('element',$element);
		break;
	case 'delete':
		$param['page'] = $page;

		if(isset($_QR['id']) === false
		|| ($info['meetme'] = $meetme->get($_QR['id'])) === false
		|| ($info['mfeatures'] = $mfeatures->get_where(array('meetmeid' => $info['meetme']['id']))) === false)
			xivo_go($_HTML->url('service/ipbx/pbx_settings/meetme'),$param);

		do
		{
			if($meetme->delete($info['meetme']['id']) === false)
				break;

			if($mfeatures->delete($info['mfeatures']['id']) === false)
			{
				$meetme->add_origin();
				break;
			}

			$localexten_where = array();
			$localexten_where['exten'] = $info['mfeatures']['number'];
			$localexten_where['app'] = 'Macro';
			$localexten_where['appdata'] = 'supermeetme';

			if($info['mfeatures']['context'] === '')
				$localexten_where['context'] = 'local-extensions';
			else
				$localexten_where['context'] = $info['mfeatures']['context'];

			if(($info['extensions'] = $extensions->get_where($localexten_where)) !== false
			&& $extensions->delete($info['extensions']['id']) === false)
			{
				$meetme->add_origin();
				$mfeatures->add_origin();
				break;
			}

			$extenum_where = array();
			$extenum_where['exten'] = $localexten_where['exten'];
			$extenum_where['context'] = $localexten_where['context'];

			$info['dfeatures'] = false;

			if(($info['extenumbers'] = $extenumbers->get_where($extenum_where)) !== false)
			{
				$dfeatures = &$ipbx->get_module('didfeatures');
				$dfeatures_where = array();
				$dfeatures_where['type'] = 'meetme';
				$dfeatures_where['typeid'] = $info['mfeatures']['id'];
				$dfeatures_where['commented'] = 0;

				if($extenumbers->delete($info['extenumbers']['id']) === false
				|| (($info['dfeatures'] = $dfeatures->get_list_where($dfeatures_where,false)) !== false
				   && $dfeatures->edit_where($dfeatures_where,array('commented' => 1)) === false) === true)
				{
					$meetme->add_origin();
					$mfeatures->add_origin();

					if($info['localexten'] !== false)
						$extensions->add_origin();

					if($info['dfeatures'] !== false)
						$extenumbers->add_origin();
					break;
				}
			}
		}
		while(false);

		xivo_go($_HTML->url('service/ipbx/pbx_settings/meetme'),$param);
		break;
	case 'enables':
	case 'disables':
		$param['page'] = $page;
		$disable = $act === 'disables' ? true : false;

		do
		{
			if(($val = xivo_issa_val('meetme',$_QR)) === false)
				break;

			$nb = count($val);

			for($i = 0;$i < $nb;$i++)
			{
				$id = strval($val[$i]);

				if($disable === true)
					$meetme->disable($id);
				else
					$meetme->enable($id);
			}
		}
		while(false);
		
		xivo_go($_HTML->url('service/ipbx/pbx_settings/meetme'),$param);
		break;
	case 'deletes':
		$param['page'] = $page;
		$dfeatures = &$ipbx->get_module('didfeatures');

		$localexten_where = $extenum_where = $dfeatures_where = array();

		$localexten_where['app'] = 'Macro';
		$localexten_where['appdata'] = 'supermeetme';

		$dfeatures_where['type'] = 'meetme';
		$dfeatures_where['commented'] = 0;

		if(($val = xivo_issa_val('meetme',$_QR)) !== false)
		{
			$nb = count($val);

			for($i = 0;$i < $nb;$i++)
			{
				$id = strval($val[$i]);

				if(($info['meetme'] = $meetme->get($id)) === false
				|| ($info['mfeatures'] = $mfeatures->get_where(array('meetmeid' => $info['meetme']['id']))) === false
				|| $meetme->delete($info['meetme']['id']) === false)
					continue;

				if($mfeatures->delete($info['mfeatures']['id']) === false)
				{
					$meetme->add_origin();
					continue;
				}

				$localexten_where['exten'] = $info['mfeatures']['number'];

				if($info['mfeatures']['context'] === '')
					$localexten_where['context'] = 'local-extensions';
				else
					$localexten_where['context'] = $info['mfeatures']['context'];

				if(($info['extensions'] = $extensions->get_where($localexten_where)) !== false
				&& $extensions->delete($info['extensions']['id']) === false)
				{
					$meetme->add_origin();
					$mfeatures->add_origin();
					continue;
				}

				$extenum_where['exten'] = $localexten_where['exten'];
				$extenum_where['context'] = $localexten_where['context'];

				$info['dfeatures'] = false;

				$dfeatures_where['typeid'] = $info['mfeatures']['id'];

				if(($info['extenumbers'] = $extenumbers->get_where($extenum_where)) !== false)
				{
					if($extenumbers->delete($info['extenumbers']['id']) === false
					|| (($info['dfeatures'] = $dfeatures->get_list_where($dfeatures_where,false)) !== false
					   && $dfeatures->edit_where($dfeatures_where,array('commented' => 1)) === false) === true)
					{
						$meetme->add_origin();
						$mfeatures->add_origin();

						if($info['localexten'] !== false)
							$extensions->add_origin();

						if($info['dfeatures'] !== false)
							$extenumbers->add_origin();
						continue;
					}
				}
			}
		}
		
		xivo_go($_HTML->url('service/ipbx/pbx_settings/meetme'),$param);
		break;
	default:
		$act = 'list';
		$total = 0;

		if(($meets = $ipbx->get_meetme_list()) !== false)
		{
			$total = count($meets);
			xivo::load_class('xivo_sort');
			$sort = new xivo_sort(array('browse' => 'mfeatures','key' => 'name'));
			usort($meets,array(&$sort,'str_usort'));
		}

		$_HTML->assign('pager',xivo_calc_page($page,20,$total));
		$_HTML->assign('list',$meets);
}

$_HTML->assign('act',$act);

$menu = &$_HTML->get_module('menu');
$menu->set_top('top/user/'.$_USR->get_infos('meta'));
$menu->set_left('left/service/ipbx/asterisk');
$menu->set_toolbar('toolbar/service/ipbx/asterisk/pbx_settings/meetme');

$_HTML->assign('bloc','pbx_settings/meetme/'.$act);
$_HTML->assign('service_name',$service_name);
$_HTML->set_struct('service/ipbx/index');
$_HTML->display('index');

?>

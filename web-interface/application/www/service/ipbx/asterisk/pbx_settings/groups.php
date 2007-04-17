<?php

$act = isset($_QR['act']) === true ? $_QR['act'] : '';
$page = isset($_QR['page']) === true ? xivo_uint($_QR['page'],1) : 1;

$gfeatures = &$ipbx->get_module('groupfeatures');
$queue = &$ipbx->get_module('queue');
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

		do
		{
			if(isset($_QR['fm_send']) === false || xivo_issa('gfeatures',$_QR) === false || xivo_issa('queue',$_QR) === false)
				break;

			$result = array();

			if(($result['gfeatures'] = $gfeatures->chk_values($_QR['gfeatures'],true,true)) === false)
			{
				$add = false;
				$result['gfeatures'] = $gfeatures->get_filter_result();
			}

			$_QR['queue']['name'] = $result['gfeatures']['name'];

			if(($result['queue'] = $queue->chk_values($_QR['queue'],true,true)) === false)
			{
				$add = false;
				$result['queue'] = $queue->get_filter_result();
			}

			$local_exten = $exten_numbers = null;

			if($add === true && $result['gfeatures']['number'] !== '')
			{
				$local_exten = array();
				$local_exten['exten'] = $result['gfeatures']['number'];
				$local_exten['priority'] = 1;
				$local_exten['app'] = 'Macro';
				$local_exten['appdata'] = 'supergroup';

				if($result['gfeatures']['context'] === '')
					$local_exten['context'] = 'local-extensions';
				else
					$local_exten['context'] = $result['gfeatures']['context'];

				if(($result['local_exten'] = $extensions->chk_values($local_exten,true,true)) === false)
				{
					$add = false;
					$result['local_exten'] = $extensions->get_filter_result();
				}

				$exten_numbers = array();
				$exten_numbers['number'] = $result['local_exten']['exten'];
				$exten_numbers['context'] = $result['local_exten']['context'];

				if(($result['extenumbers'] = $extenumbers->chk_values($exten_numbers,true,true)) === false
				|| $extenumbers->get_where($result['extenumbers']) !== false)
				{
					$add = false;
					$result['extenumbers'] = $extenumbers->get_filter_result();
				}
			}

			if($add === false || ($gfeaturesid = $gfeatures->add($result['gfeatures'])) === false)
				break;

			if(($queueid = $queue->add($result['queue'])) === false)
			{
				$gfeatures->delete($gfeaturesid);
				break;
			}

			if($local_exten !== null && ($local_extenid = $extensions->add($result['local_exten'])) === false)
			{
				$gfeatures->delete($gfeaturesid);
				$queue->delete($queueid);
				break;
			}

			if($exten_numbers !== null && ($extenumid = $extenumbers->add($result['extenumbers'])) === false)
			{
				$gfeatures->delete($gfeaturesid);
				$queue->delete($queueid);

				if($local_exten !== null)
					$extensions->delete($local_extenid);
				break;
			}

			xivo_go($_HTML->url('service/ipbx/pbx_settings/groups'),$param);
		}
		while(false);

		$element = array();
		$element['queue'] = $queue->get_element();
		$element['gfeatures'] = $gfeatures->get_element();

		$_HTML->assign('element',$element);
		$_HTML->assign('info',$result);
		break;
	case 'edit':
		$edit = true;

		$return = &$info;

		if(isset($_QR['id']) === false
		|| ($info['gfeatures'] = $gfeatures->get($_QR['id'],false)) === false
		|| ($info['queue'] = $queue->get($info['gfeatures']['name'],false)) === false)
			xivo_go($_HTML->url('service/ipbx/pbx_settings/groups'),$param);

		$status = array();
		$status['local_exten'] = $status['extenumbers'] = false;

		do
		{
			if(isset($_QR['fm_send']) === false || xivo_issa('gfeatures',$_QR) === false || xivo_issa('queue',$_QR) === false)
					break;

			$return = &$result;

			if(($result['gfeatures'] = $gfeatures->chk_values($_QR['gfeatures'],true,true)) === false)
			{
				$edit = false;
				$result['mfeatures'] = array_merge($info['gfeatures'],$gfeatures->get_filter_result());
			}

			$_QR['queue']['name'] = $result['gfeatures']['name'];

			if(($result['queue'] = $queue->chk_values($_QR['queue'],true,true)) === false)
			{
				$edit = false;
				$result['queue'] = $queue->get_filter_result();
			}

			$exten_where = array();
			$exten_where['exten'] = $info['gfeatures']['number'];
			$exten_where['app'] = 'Macro';
			$exten_where['appdata'] = 'supergroup';

			if($info['gfeatures']['context'] === '')
				$exten_where['context'] = 'local-extensions';
			else
				$exten_where['context'] = $info['gfeatures']['context'];

			if(($info['localexten'] = $extensions->get_where($exten_where)) !== false)
			{
				if($result['gfeatures']['number'] === '')
					$status['localexten'] = 'delete';
				else
				{
					$status['localexten'] = 'edit';

					$local_exten = $info['localexten'];
					$local_exten['exten'] = $result['gfeatures']['number'];

					if($result['gfeatures']['context'] === '')
						$local_exten['context'] = 'local-extensions';
					else
						$local_exten['context'] = $result['gfeatures']['context'];

					if(($result['localexten'] = $extensions->chk_values($local_exten,true,true)) === false)
					{
						$edit = false;
						$result['localexten'] = array_merge($info['localexten'],$extensions->get_filter_result());
					}
				}
			}
			else if($result['gfeatures']['number'] !== '')
			{
				$status['localexten'] = 'add';

				$local_exten = $exten_where;
				$local_exten['exten'] = $result['gfeatures']['number'];
				$local_exten['priority'] = 1;

				if($result['gfeatures']['context'] === '')
					$local_exten['context'] = 'local-extensions';
				else
					$local_exten['context'] = $result['gfeatures']['context'];

				if(($result['localexten'] = $extensions->chk_values($local_exten,true,true)) === false)
				{
					$edit = false;
					$result['localexten'] = $extensions->get_filter_result();
				}
			}

			$exten_numbers = array();
			$exten_numbers['number'] = $result['gfeatures']['number'];

			if($result['gfeatures']['context'] === '')
				$exten_numbers['context'] = 'local-extensions';
			else
				$exten_numbers['context'] = $result['gfeatures']['context'];

			$exten_where = array();
			$exten_where['number'] = $info['gfeatures']['number'];

			if($info['gfeatures']['context'] === '')
				$exten_where['context'] = 'local-extensions';
			else
				$exten_where['context'] = $info['gfeatures']['context'];

			if(($info['extenumbers'] = $extenumbers->get_where($exten_where)) !== false)
			{
				if($result['gfeatures']['number'] === '')
					$status['extenumbers'] = 'delete';
				else
				{
					$status['extenumbers'] = 'edit';

					if(($result['extenumbers'] = $extenumbers->chk_values($exten_numbers,true,true)) === false
					|| (($extenum = $extenumbers->get_where($result['extenumbers'])) !== false
					   && (int) $extenum['id'] !== (int) $info['extenumbers']['id']) === true)
					{
						$edit = false;
						$result['extenumbers'] = array_merge($info['extenumbers'],$extenumbers->get_filter_result());
					}
				}
			}
			else if($result['gfeatures']['number'] !== '')
			{
				$status['extenumbers'] = 'add';

				if(($result['extenumbers'] = $extenumbers->chk_values($exten_numbers,true,true)) === false)
				{
					$edit = false;
					$result['extenumbers'] = $extenumbers->get_filter_result();
				}
			}

			if($edit === false || $gfeatures->edit($info['gfeatures']['id'],$result['gfeatures']) === false)
				break;

			if($queue->edit($info['queue']['name'],$result['queue']) === false)
			{
				$gfeatures->edit_origin();
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
				$gfeatures->edit_origin();
				$queue->edit_origin();
				break;
			}

			$rs_dfeatures = null;

			$dfeatures = &$ipbx->get_module('didfeatures');
			$dfeatures_where = array();
			$dfeatures_where['type'] = 'group';
			$dfeatures_where['typeid'] = $info['gfeatures']['id'];
			$dfeatures_where['disable'] = 0;

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
					&& ($rs_dfeatures = $dfeatures->edit_where($dfeatures_where,array('disable' => 1))) === false)
						$rs_extenumbers = false;
					break;
				default:
					$rs_extenumbers = null;
			}

			if($rs_extenumbers === false)
			{
				$gfeatures->edit_origin();
				$queue->edit_origin();

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

			if($info['queue']['name'] === $result['queue']['name'])
				xivo_go($_HTML->url('service/ipbx/pbx_settings/groups'),$param);

			$qmember = &$ipbx->get_module('queuemember');

			if($qmember->edit_where(array('queue_name' => $info['queue']['name']),array('queue_name' => $result['queue']['name'])) === false)
			{
				$gfeatures->edit_origin();
				$queue->edit_origin();

				if($rs_localexten !== null)
				{
					switch($status['localexten'])
					{
						case 'add':
							$extensions->delete($rs_localexten);
							break;
						case 'edit':
							$extensions->edit_origin();
							break;
						case 'delete':
							$extensions->add_origin();
							break;
						default:
							break;
					}
				}

				if($rs_extenumbers === null)
					break;

				switch($status['extenumbers'])
				{
					case 'add':
						$extenumbers->delete($rs_extenumbers);
						break 2;
					case 'edit':
						$extenumbers->edit_origin();
						break 2;
					case 'delete':
						$extenumbers->add_origin();

						if($rs_dfeatures === true)
							$dfeatures->edit_list_where($info['dfeatures'],array('disable' => 0));
						break 2;
					default:
						break 2;
				}
			}

			xivo_go($_HTML->url('service/ipbx/pbx_settings/groups'),$param);
		}
		while(false);

		$element = array();
		$element['queue'] = $queue->get_element();
		$element['gfeatures'] = $gfeatures->get_element();

		$_HTML->assign('id',$info['gfeatures']['id']);
		$_HTML->assign('info',$return);
		$_HTML->assign('element',$element);
		break;
	case 'delete':
		$param['page'] = $page;
		$qmember = &$ipbx->get_module('queuemember');

		if(isset($_QR['id']) === false
		|| ($info['gfeatures'] = $gfeatures->get($_QR['id'],false)) === false
		|| ($info['queue'] = $queue->get($info['gfeatures']['name'],false)) === false
		|| $qmember->get_nb_by_name($info['queue']['name']) !== 0)
			xivo_go($_HTML->url('service/ipbx/pbx_settings/groups'),$param);

		do
		{
			if($gfeatures->disable($info['gfeatures']['id']) === false)
				break;

			if($queue->delete($info['queue']['name']) === false)
			{
				$gfeatures->enable($info['gfeatures']['id']);
				break;
			}

			$localexten_where = array();
			$localexten_where['exten'] = $info['gfeatures']['number'];
			$localexten_where['app'] = 'Macro';
			$localexten_where['appdata'] = 'supergroup';

			if($info['gfeatures']['context'] === '')
				$localexten_where['context'] = 'local-extensions';
			else
				$localexten_where['context'] = $info['gfeatures']['context'];

			if(($info['extensions'] = $extensions->get_where($localexten_where)) !== false
			&& $extensions->delete($info['extensions']['id']) === false)
			{
				$gfeatures->enable($info['gfeatures']['id']);
				$queue->add_origin();
				break;
			}

			$extenum_where = array();
			$extenum_where['number'] = $localexten_where['exten'];
			$extenum_where['context'] = $localexten_where['context'];

			$info['dfeatures'] = false;

			if(($info['extenumbers'] = $extenumbers->get_where($extenum_where)) !== false)
			{
				$dfeatures = &$ipbx->get_module('didfeatures');
				$dfeatures_where = array();
				$dfeatures_where['type'] = 'group';
				$dfeatures_where['typeid'] = $info['gfeatures']['id'];
				$dfeatures_where['disable'] = 0;

				if($extenumbers->delete($info['extenumbers']['id']) === false
				|| (($info['dfeatures'] = $dfeatures->get_list_where($dfeatures_where,false)) !== false
				   && $dfeatures->edit_where($dfeatures_where,array('disable' => 1)) === false) === true)
				{
					$gfeatures->enable($info['gfeatures']['id']);
					$queue->add_origin();

					if($info['localexten'] !== false)
						$extensions->add_origin();

					if($info['dfeatures'] !== false)
						$extenumbers->add_origin();
					break;
				}
			}
		}
		while(false);

		xivo_go($_HTML->url('service/ipbx/pbx_settings/groups'),$param);
		break;
	default:
		$act = 'list';
		$total = 0;

		if(($groups = $ipbx->get_groups_list(false)) !== false)
		{
			$total = count($groups);
			xivo::load_class('xivo_sort');
			$sort = new xivo_sort(array('browse' => 'gfeatures','key' => 'name'));
			usort($groups,array(&$sort,'str_usort'));
		}

		$_HTML->assign('pager',xivo_calc_page($page,20,$total));
		$_HTML->assign('list',$groups);
}

$_HTML->assign('act',$act);

$menu = &$_HTML->get_module('menu');
$menu->set_top('top/user/'.$_USR->get_infos('meta'));
$menu->set_left('left/service/ipbx/asterisk');
$menu->set_toolbar('toolbar/service/ipbx/asterisk/pbx_settings/groups');

$_HTML->assign('bloc','pbx_settings/groups/'.$act);
$_HTML->assign('service_name',$service_name);
$_HTML->set_struct('service/ipbx/index');
$_HTML->display('index');

?>

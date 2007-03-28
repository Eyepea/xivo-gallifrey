<?php

$act = isset($_QR['act']) === true ? $_QR['act'] : '';
$gfeatures = &$ipbx->get_module('groupfeatures');
$queue = &$ipbx->get_module('queue');

$result = $info = array();

$local_extenid = false;

switch($act)
{
	case 'add':
		$extensions = &$ipbx->get_module('extensions');

		do
		{
			if(isset($_QR['fm_send']) === false || xivo_issa('gfeatures',$_QR) === false || xivo_issa('queue',$_QR) === false)
				break;

			if(($result['gfeatures'] = $gfeatures->chk_values($_QR['gfeatures'],true,true)) === false
			|| ($gid = $gfeatures->add($result['gfeatures'])) === false)
			{
				$info['gfeatures'] = $gfeatures->get_filter_result();
				break;
			}

			if($result['gfeatures']['number'] !== '')
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

				if(($result['local_exten'] = $extensions->chk_values($local_exten,true,true)) === false
				|| ($local_extenid = $extensions->add($result['local_exten'])) === false)
				{
					$gfeatures->delete($gid);
					break;
				}
			}

			$_QR['queue']['name'] = $result['gfeatures']['name'];

			if($queue->add($_QR['queue']) === false)
			{
				$gfeatures->delete($gid);

				if($local_extenid !== false)
					$extensions->delete($local_extenid);

				break;
			}

			xivo_go($_HTML->url('service/ipbx/pbx_settings/groups'),'act=list');
		}
		while(false);

		$_HTML->assign('queue_elt',$queue->get_element());
		$_HTML->assign('gfeatures_elt',$gfeatures->get_element());
		break;
	case 'edit':
		$extensions = &$ipbx->get_module('extensions');

		if(isset($_QR['id']) === false
		|| ($info['gfeatures'] = $gfeatures->get($_QR['id'],false)) === false
		|| ($info['queue'] = $queue->get($info['gfeatures']['name'],false)) === false)
			xivo_go($_HTML->url('service/ipbx/pbx_settings/groups'),'act=list');

		$localexten_status = false;

		do
		{
			if(isset($_QR['fm_send']) === false || xivo_issa('gfeatures',$_QR) === false || xivo_issa('queue',$_QR) === false)
					break;

			if(($result['gfeatures'] = $gfeatures->chk_values($_QR['gfeatures'],true,true)) === false
			|| $gfeatures->edit($info['gfeatures']['id'],$result['gfeatures']) === false)
			{
				$info['mfeatures'] = array_merge($info['gfeatures'],$gfeatures->get_filter_result());
				break;
			}

			$exten_where = array();
			$exten_where['exten'] = $info['gfeatures']['number'];
			$exten_where['app'] = 'Macro';
			$exten_where['appdata'] = 'supergroup';

			if($info['gfeatures']['context'] === '')
				$exten_where['context'] = 'local-extensions';
			else
				$exten_where['context'] = $info['gfeatures']['context'];

			if(($info['extensions'] = $extensions->get_where($exten_where)) !== false)
			{
				$local_exten = $info['extensions'];
				$local_exten['exten'] = $result['gfeatures']['number'];

				if($result['gfeatures']['context'] === '')
					$local_exten['context'] = 'local-extensions';
				else
					$local_exten['context'] = $result['gfeatures']['context'];

				if($result['gfeatures']['number'] === '')
				{
					if($extensions->delete($info['extensions']['id']) !== false)
						$localexten_status = 'delete';
				}
				else
				{
					if(($result['local_exten'] = $extensions->chk_values($local_exten,true,true)) === false
					|| $extensions->edit($info['extensions']['id'],$result['local_exten']) === false)
					{
						$gfeatures->edit_origin();
						break;
					}

					$localexten_status = 'edit';
				}
			}
			else if($result['gfeatures']['number'] !== '')
			{
				$local_exten = $exten_where;
				$local_exten['exten'] = $result['gfeatures']['number'];
				$local_exten['priority'] = 1;

				if($result['gfeatures']['context'] === '')
					$local_exten['context'] = 'local-extensions';
				else
					$local_exten['context'] = $result['gfeatures']['context'];

				if(($result['local_exten'] = $extensions->chk_values($local_exten,true,true)) === false
				|| ($local_extenid = $extensions->add($result['local_exten'])) === false)
				{
					$gfeatures->edit_origin();
					break;
				}

				$localexten_status = 'add';
			}

			$_QR['queue']['name'] = $_QR['gfeatures']['name'];

			if($queue->edit($info['queue']['name'],$_QR['queue']) === false)
			{
				$gfeatures->edit_origin();

				switch($localexten_status)
				{
					case 'add':
						$extensions->delete($local_extenid);
						break;
					case 'edit':
						$extensions->edit_origin();
						break;
					case 'delete':
						$extensions->add_origin();
						break;
				}

				break;
			}

			if($info['queue']['name'] === $_QR['queue']['name'])
				xivo_go($_HTML->url('service/ipbx/pbx_settings/groups'),'act=list');

			$qmember = &$ipbx->get_module('queuemember');

			if($qmember->update_by_name($info['queue']['name'],array('queue_name' => $_QR['queue']['name'])) === false)
			{
				$gfeatures->edit_origin();

				switch($localexten_status)
				{
					case 'add':
						$extensions->delete($local_extenid);
						break;
					case 'edit':
						$extensions->edit_origin();
						break;
					case 'delete':
						$extensions->add_origin();
						break;
				}

				$queue->edit_origin();
				break;
			}

			xivo_go($_HTML->url('service/ipbx/pbx_settings/groups'),'act=list');
		}
		while(false);

		$_HTML->assign('info',$info);
		$_HTML->assign('queue_elt',$queue->get_element());
		$_HTML->assign('gfeatures_elt',$gfeatures->get_element());
		break;
	case 'delete':
		$extensions = &$ipbx->get_module('extensions');
		$qmember = &$ipbx->get_module('queuemember');

		if(isset($_QR['id']) === false
		|| ($info['gfeatures'] = $gfeatures->get($_QR['id'],false)) === false
		|| ($info['queue'] = $queue->get($info['gfeatures']['name'],false)) === false
		|| $qmember->get_nb_by_name($info['queue']['name']) !== 0)
			xivo_go($_HTML->url('service/ipbx/pbx_settings/groups'),'act=list');

		$localexten_where = array();
		$localexten_where['exten'] = $info['gfeatures']['number'];
		$localexten_where['app'] = 'Macro';
		$localexten_where['appdata'] = 'supergroup';

		if($info['gfeatures']['context'] === '')
			$localexten_where['context'] = 'local-extensions';
		else
			$localexten_where['context'] = $info['gfeatures']['context'];

		do
		{
			if($gfeatures->disable($info['gfeatures']['id']) === false)
				break;

			if(($info['extensions'] = $extensions->get_where($localexten_where)) !== false
			&& $extensions->delete($info['extensions']['id']) === false)
			{
				$gfeatures->enable($info['gfeatures']['id']);
				break;
			}

			if($queue->delete($info['queue']['name']) === false)
			{
				$gfeatures->enable($info['gfeatures']['id']);

				if($info['extensions'] !== false)
					$extensions->add_origin();
				break;
			}
		}
		while(false);

		xivo_go($_HTML->url('service/ipbx/pbx_settings/groups'),'act=list');
		break;
	default:
		$act = 'list';
		$total = 0;
		$page = 1;

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

$menu = &$_HTML->get_module('menu');
$menu->set_top('top/user/'.$_USR->get_infos('meta'));
$menu->set_left('left/service/ipbx/asterisk');
$menu->set_toolbar('toolbar/service/ipbx/asterisk/pbx_settings/groups');

$_HTML->assign('bloc','pbx_settings/groups/'.$act);
$_HTML->assign('service_name',$service_name);
$_HTML->set_struct('service/ipbx/index');
$_HTML->display('index',true);

?>

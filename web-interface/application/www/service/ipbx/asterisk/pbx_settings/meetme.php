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

			$localextenid = $exten_numbers = null;

			if($add === true && $result['mfeatures']['number'] !== '')
			{
				if($result['mfeatures']['context'] === '')
					$localextencontext = 'local-extensions';
				else
					$localextencontext = $result['mfeatures']['context'];

				if(($localextenid = $extensions->new_exten('macro',
								array('appdata' => 'supermeetme'),
								$result['mfeatures']['number'],
								$localextencontext)) === false)
					$add = false;

				$exten_numbers = array();
				$exten_numbers['exten'] = $result['mfeatures']['number'];
				$exten_numbers['context'] = $localextencontext;

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

			if($localextenid !== null && $extensions->add_exten($localextenid) === false)
			{
				$meetme->delete($meetmeid);
				$mfeatures->delete($mfeaturesid);
				break;
			}

			if($exten_numbers !== null && ($extenumid = $extenumbers->add($result['extenumbers'])) === false)
			{
				$meetme->delete($meetmeid);
				$mfeatures->delete($mfeaturesid);

				if($localextenid !== null)
					$extensions->delete_exten($localextenid);
				break;
			}

			$_QRY->go($_HTML->url('service/ipbx/pbx_settings/meetme'),$param);
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
			$_QRY->go($_HTML->url('service/ipbx/pbx_settings/meetme'),$param);

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

			if($info['mfeatures']['number'] !== '')
			{
				if($info['mfeatures']['context'] === '')
					$localextencontext = 'local-extensions';
				else
					$localextencontext = $info['mfeatures']['context'];

				if($result['mfeatures']['context'] === '')
					$localexteneditcontext = 'local-extensions';
				else
					$localexteneditcontext = $result['mfeatures']['context'];

				if(($info['localexten'] = $extensions->get_exten('macro',
										 $info['mfeatures']['number'],
										 $localextencontext,
										 array('appdata' => 'supermeetme'))) === false)
					$edit = false;
				else if($result['mfeatures']['number'] === '')
					$status['localexten'] = 'delete';
				else if(($localexten_edit = $extensions->chk_exten('macro',
										   null,
										   $result['mfeatures']['number'],
										   $localexteneditcontext)) === false)
					$edit = false;
				else
					$status['localexten'] = 'edit';
			}
			else if($result['mfeatures']['number'] !== '')
			{
				$status['localexten'] = 'add';
	
				if($result['mfeatures']['context'] === '')
					$localextencontext = 'local-extensions';
				else
					$localextencontext = $result['mfeatures']['context'];
	
				if(($localextenid = $extensions->new_exten('macro',
							array('appdata' => 'supermeetme'),
							$result['mfeatures']['number'],
							$localextencontext)) === false)
					$edit = false;
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
					$rs_localexten = $extensions->add_exten($localextenid);
					break;
				case 'edit':
					$rs_localexten = $extensions->edit($info['localexten']['id'],$localexten_edit);
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

			switch($status['extenumbers'])
			{
				case 'add':
					$rs_extenumbers = $extenumbers->add($result['extenumbers']);
					break;
				case 'edit':
					$rs_extenumbers = $extenumbers->edit($info['extenumbers']['id'],$result['extenumbers']);
					break;
				case 'delete':
					$rs_extenumbers = $extenumbers->delete($info['extenumbers']['id']);
					break;
				default:
					$rs_extenumbers = null;
			}

			if($rs_extenumbers === false)
			{
				$meetme->edit_origin();
				$mfeatures->edit_origin();

				if($rs_localexten === null)
					break;

				switch($status['localexten'])
				{
					case 'add':
						$extensions->delete_exten($localextenid);
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

			if($status['extenumbers'] === 'delete')
			{
				$incall = &$ipbx->get_module('incall');

				$incall->unlinked_where(array('type' => 'meetme',
								 'typeval' => $info['mfeatures']['id']));

				$schedule = &$ipbx->get_module('schedule');

				$schedule->unlinked_where(array('typetrue' => 'meetme',
								'typevaltrue' => $info['mfeatures']['id']));
	
				$schedule->unlinked_where(array('typefalse' => 'meetme',
				   				'typevalfalse' => $info['mfeatures']['id']));
			}

			$_QRY->go($_HTML->url('service/ipbx/pbx_settings/meetme'),$param);
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
		
		$_QRY->go($_HTML->url('service/ipbx/pbx_settings/meetme'),$param);
		break;
	case 'delete':
		$param['page'] = $page;

		if(isset($_QR['id']) === false
		|| ($info['meetme'] = $meetme->get($_QR['id'])) === false
		|| ($info['mfeatures'] = $mfeatures->get_where(array('meetmeid' => $info['meetme']['id']))) === false)
			$_QRY->go($_HTML->url('service/ipbx/pbx_settings/meetme'),$param);

		do
		{
			if($meetme->delete($info['meetme']['id']) === false)
				break;

			if($mfeatures->delete($info['mfeatures']['id']) === false)
			{
				$meetme->add_origin();
				break;
			}

			if($info['mfeatures']['context'] === '')
				$localextencontext = 'local-extensions';
			else
				$localextencontext = $info['mfeatures']['context'];

			if(($info['localexten'] = $extensions->get_exten('macro',
								 $info['mfeatures']['number'],
								 $localextencontext,
								 array('appdata' => 'supermeetme'))) !== false
			&& $extensions->delete($info['localexten']['id']) === false)
			{
				$meetme->add_origin();
				$mfeatures->add_origin();
				break;
			}

			$extenum_where = array();
			$extenum_where['exten'] = $info['mfeatures']['number'];
			$extenum_where['context'] = $localextencontext;

			if(($info['extenumbers'] = $extenumbers->get_where($extenum_where)) !== false)
			{
				if($extenumbers->delete($info['extenumbers']['id']) === false)
				{
					$meetme->add_origin();
					$mfeatures->add_origin();

					if($info['localexten'] !== false)
						$extensions->add_origin();
					break;
				}

				$incall = &$ipbx->get_module('incall');

				$incall->unlinked_where(array('type' => 'meetme',
							      'typeval' => $info['mfeatures']['id']));

				$schedule = &$ipbx->get_module('schedule');

				$schedule->unlinked_where(array('typetrue' => 'meetme',
								'typevaltrue' => $info['mfeatures']['id']));
	
				$schedule->unlinked_where(array('typefalse' => 'meetme',
				   				'typevalfalse' => $info['mfeatures']['id']));
			}
		}
		while(false);

		$_QRY->go($_HTML->url('service/ipbx/pbx_settings/meetme'),$param);
		break;
	case 'deletes':
		$param['page'] = $page;
		$incall = &$ipbx->get_module('incall');
		$schedule = &$ipbx->get_module('schedule');

		$localexten_where = $extenum_where = array();

		$incall_where['type'] = $schedule_true_where['typetrue'] = $schedule_false_where['typefalse'] = 'meetme';

		$localexten_where['app'] = 'Macro';
		$localexten_where['appdata'] = 'supermeetme';

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

				if(($info['extenumbers'] = $extenumbers->get_where($extenum_where)) !== false)
				{
					if($extenumbers->delete($info['extenumbers']['id']) === false)
					{
						$meetme->add_origin();
						$mfeatures->add_origin();

						if($info['localexten'] !== false)
							$extensions->add_origin();
						continue;
					}

					$incall->unlinked_where(array('type' => 'meetme',
								      'typeval' => $info['mfeatures']['id']));

					$schedule->unlinked_where(array('typetrue' => 'meetme',
									'typevaltrue' => $info['mfeatures']['id']));

					$schedule->unlinked_where(array('typefalse' => 'meetme',
									'typevalfalse' => $info['mfeatures']['id']));
				}
			}
		}
		
		$_QRY->go($_HTML->url('service/ipbx/pbx_settings/meetme'),$param);
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
$menu->set_left('left/service/ipbx/'.$ipbx->get_name());
$menu->set_toolbar('toolbar/service/ipbx/'.$ipbx->get_name().'/pbx_settings/meetme');

$_HTML->set_bloc('main','service/ipbx/'.$ipbx->get_name().'/pbx_settings/meetme/'.$act);
$_HTML->set_struct('service/ipbx/'.$ipbx->get_name());
$_HTML->display('index');

?>

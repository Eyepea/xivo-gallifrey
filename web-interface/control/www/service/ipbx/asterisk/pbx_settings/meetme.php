<?php

$act = isset($_QR['act']) === true ? $_QR['act'] : '';
$page = isset($_QR['page']) === true ? xivo_uint($_QR['page'],1) : 1;

$meetme = &$ipbx->get_module('meetme');
$mfeatures = &$ipbx->get_module('meetmefeatures');

$result = $info = array();

$meetme_val = '';

switch($act)
{
	case 'add':
		$musiconhold = &$ipbx->get_module('musiconhold');
		$extensions = &$ipbx->get_module('extensions');

		if(($moh_list = $musiconhold->get_all_category()) !== false)
			ksort($moh_list);

		do
		{
			if(isset($_QR['fm_send']) === false || xivo_issa('meetme',$_QR) === false || xivo_issa('mfeatures',$_QR) === false)
				break;

			if($moh_list === false || isset($_QR['mfeatures']['musiconhold'],$moh_list[$_QR['mfeatures']['musiconhold']]) === false)
				$_QR['mfeatures']['musiconhold'] = '';

			if(($result['meetme'] = $meetme->chk_values($_QR['meetme'],true,true)) !== false)
			{
				$meetme_val = $result['meetme']['number'].',';

				if($result['meetme']['pin'] !== '')
					$meetme_val .= $result['meetme']['pin'];

				if($result['meetme']['admpin'] !== '')
					$meetme_val .= ','.$result['meetme']['admpin'];

				$meetme_val = rtrim($meetme_val,',');
			}

			if($result['meetme'] === false || ($mid = $meetme->add($meetme_val)) === false)
			{
				$info['meetme'] = $meetme->get_filter_result();
				break;
			}

			$_QR['mfeatures']['number'] = $result['meetme']['number'];
			$_QR['mfeatures']['meetmeid'] = $mid;

			if(($result['mfeatures'] = $mfeatures->chk_values($_QR['mfeatures'],true,true)) === false
			|| $mfeatures->add($result['mfeatures']) === false)
			{
				$info['mfeatures'] = $mfeatures->get_filter_result();
				$meetme->delete($mid);
				break;
			}

			if($result['meetme']['number'] !== '')
			{
				$local_exten = array();
				$local_exten['exten'] = $result['meetme']['number'];
				$local_exten['priority'] = 1;
				$local_exten['app'] = 'Macro';
				$local_exten['appdata'] = 'supermeetme';

				if($result['mfeatures']['context'] === '')
					$local_exten['context'] = 'local-extensions';
				else
					$local_exten['context'] = $result['mfeatures']['context'];

				if(($result['local_exten'] = $extensions->chk_values($local_exten,true,true)) === false
				|| ($local_extenid = $extensions->add($result['local_exten'])) === false)
				{
					$meetme->delete($mid);
					$mfeatures->delete($gid);
					break;
				}
			}

			xivo_go($_HTML->url('service/ipbx/pbx_settings/meetme'),'act=list');
		}
		while(false);

		$_HTML->assign('info',$info);
		$_HTML->assign('moh_list',$moh_list);
		$_HTML->assign('mfeatures_elt',$mfeatures->get_element());
		break;
	case 'edit':
		$musiconhold = &$ipbx->get_module('musiconhold');
		$extensions = &$ipbx->get_module('extensions');

		if(($moh_list = $musiconhold->get_all_category()) !== false)
			ksort($moh_list);

		$info = array();

		if(isset($_QR['id']) === false
		|| ($info['meetme'] = $meetme->get($_QR['id'])) === false
		|| ($info['mfeatures'] = $mfeatures->get_by_meetme($info['meetme']['id'])) === false)
			xivo_go($_HTML->url('service/ipbx/pbx_settings/meetme'),'act=list');

		do
		{
			if(isset($_QR['fm_send']) === false || xivo_issa('meetme',$_QR) === false || xivo_issa('mfeatures',$_QR) === false)
				break;

			if($moh_list === false || isset($_QR['mfeatures']['musiconhold'],$moh_list[$_QR['mfeatures']['musiconhold']]) === false)
				$_QR['mfeatures']['musiconhold'] = '';

			if(($result['meetme'] = $meetme->chk_values($_QR['meetme'],true,true)) !== false)
			{
				$meetme_val = $result['meetme']['number'].',';

				if($result['meetme']['pin'] !== '')
					$meetme_val .= $result['meetme']['pin'];

				if($result['meetme']['admpin'] !== '')
					$meetme_val .= ','.$result['meetme']['admpin'];

				$meetme_val = rtrim($meetme_val,',');
			}

			if($result['meetme'] === false || $meetme->edit($info['meetme']['id'],$meetme_val) === false)
			{
				$info['meetme'] = $meetme->get_filter_result();
				break;
			}

			$_QR['mfeatures']['number'] = $result['meetme']['number'];
			$_QR['mfeatures']['meetmeid'] = $info['meetme']['id'];

			if(($result['mfeatures'] = $mfeatures->chk_values($_QR['mfeatures'],true,true)) === false
			|| $mfeatures->edit($info['mfeatures']['id'],$result['mfeatures']) === false)
			{
				$info['mfeatures'] = $mfeatures->get_filter_result();
				$meetme->edit_origin();
				break;
			}

			$exten_where = array();
			$exten_where['exten'] = $info['meetme']['number'];
			$exten_where['app'] = 'Macro';
			$exten_where['appdata'] = 'supermeetme';

			if($info['mfeatures']['context'] === '')
				$exten_where['context'] = 'local-extensions';
			else
				$exten_where['context'] = $info['mfeatures']['context'];

			if(($info['extensions'] = $extensions->get_where($exten_where)) !== false)
			{
				$local_exten = $info['extensions'];
				$local_exten['exten'] = $result['meetme']['number'];

				if($result['mfeatures']['context'] === '')
					$local_exten['context'] = 'local-extensions';
				else
					$local_exten['context'] = $result['mfeatures']['context'];

				if($result['meetme']['number'] === '')
					$extensions->delete($info['extensions']['id']);
				else if(($result['local_exten'] = $extensions->chk_values($local_exten,true,true)) === false
				|| $extensions->edit($info['extensions']['id'],$result['local_exten']) === false)
				{
					$meetme->edit_origin();
					$mfeatures->edit_origin();
					break;
				}
			}
			else if($result['meetme']['number'] !== '')
			{
				$local_exten = $exten_where;
				$local_exten['exten'] = $result['meetme']['number'];
				$local_exten['priority'] = 1;

				if($result['mfeatures']['context'] === '')
					$local_exten['context'] = 'local-extensions';
				else
					$local_exten['context'] = $result['mfeatures']['context'];

				if(($result['local_exten'] = $extensions->chk_values($local_exten,true,true)) === false
				|| ($local_extenid = $extensions->add($result['local_exten'])) === false)
				{
					$meetme->edit_origin();
					$mfeatures->edit_origin();
					break;
				}
			}

			xivo_go($_HTML->url('service/ipbx/pbx_settings/meetme'),'act=list');
		}
		while(false);

		$_HTML->assign('info',$info);
		$_HTML->assign('moh_list',$moh_list);
		$_HTML->assign('mfeatures_elt',$mfeatures->get_element());
		break;
	case 'delete':
		$extensions = &$ipbx->get_module('extensions');

		$info = array();

		if(isset($_QR['id']) === false
		|| ($info['meetme'] = $meetme->get($_QR['id'])) === false
		|| ($info['mfeatures'] = $mfeatures->get_by_meetme($info['meetme']['id'])) === false)
			xivo_go($_HTML->url('service/ipbx/pbx_settings/meetme'),'act=list&page='.$page);

		$exten_where = array();
		$exten_where['exten'] = $info['meetme']['number'];
		$exten_where['app'] = 'Macro';
		$exten_where['appdata'] = 'supermeetme';

		if($info['mfeatures']['context'] === '')
			$exten_where['context'] = 'local-extensions';
		else
			$exten_where['context'] = $info['mfeatures']['context'];

		do
		{
			if($meetme->delete($info['meetme']['id']) === false)
				break;

			if($mfeatures->delete($info['mfeatures']['id']) === false)
			{
				$meetme->add_origin();
				break;
			}

			if(($info['extensions'] = $extensions->get_where($exten_where)) !== false
			&& $extensions->delete($info['extensions']['id']) === false)
			{
				$meetme->add_origin();
				$mfeatures->add_origin();
			}
		}
		while(false);

		xivo_go($_HTML->url('service/ipbx/pbx_settings/meetme'),'act=list&page='.$page);
		break;
	case 'enables':
	case 'disables':
		$disable = $act === 'disables' ? true : false;

		do
		{
			if(xivo_issa('meetme',$_QR) === false)
				break;

			$val = array_values($_QR['meetme']);

			if(($nb = count($val)) === false)
				break;

			for($i = 0;$i < $nb;$i++)
			{
				if($disable === true)
					$meetme->disable($val[$i]);
				else
					$meetme->enable($val[$i]);
			}
		}
		while(false);
		
		xivo_go($_HTML->url('service/ipbx/pbx_settings/meetme'),'act=list&page='.$page);
		break;
	case 'deletes':
		$extensions = &$ipbx->get_module('extensions');

		$info = $exten_where = array();

		$exten_where['app'] = 'Macro';
		$exten_where['appdata'] = 'supermeetme';

		do
		{
			if(xivo_issa('meetme',$_QR) === false)
				break;

			$val = array_values($_QR['meetme']);

			if(($nb = count($val)) === false)
				break;

			for($i = 0;$i < $nb;$i++)
			{
				if(($info['meetme'] = $meetme->get($val[$i])) === false
				|| ($info['mfeatures'] = $mfeatures->get_by_meetme($info['meetme']['id'])) === false)
					continue;

				if($meetme->delete($info['meetme']['id']) === false)
					continue;

				if($mfeatures->delete($info['mfeatures']['id']) === false)
				{
					$meetme->add_origin();
					continue;
				}

				$exten_where['exten'] = $info['meetme']['number'];

				if($info['mfeatures']['context'] === '')
					$exten_where['context'] = 'local-extensions';
				else
					$exten_where['context'] = $info['mfeatures']['context'];

				if(($info['extensions'] = $extensions->get_where($exten_where)) !== false
				&& $extensions->delete($info['extensions']['id']) === false)
				{
					$meetme->add_origin();
					$mfeatures->add_origin();
				}
			}
		}
		while(false);
		
		xivo_go($_HTML->url('service/ipbx/pbx_settings/meetme'),'act=list&page='.$page);
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

$menu = &$_HTML->get_module('menu');
$menu->set_top('top/user/'.$_USR->get_infos('meta'));
$menu->set_left('left/service/ipbx/asterisk');
$menu->set_toolbar('toolbar/service/ipbx/asterisk/pbx_settings/meetme');

$_HTML->assign('act',$act);
$_HTML->assign('bloc','pbx_settings/meetme/'.$act);
$_HTML->assign('service_name',$service_name);
$_HTML->set_struct('service/ipbx/index');
$_HTML->display('index');

?>

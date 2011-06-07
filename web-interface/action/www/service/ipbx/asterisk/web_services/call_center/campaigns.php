<?php

#
# XiVO Web-Interface
# Copyright (C) 2010  Proformatique <technique@proformatique.com>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#

$access_category    = 'call_center';
$access_subcategory = 'campaigns';

include(dwho_file::joinpath(dirname(__FILE__),'..','_common.php'));
$act = $_QRY->get('act');

switch($act)
{
	default:
	case 'view':
		$act = 'view';

		$camp = &$ipbx->get_module('campaign_general');
		$gene = $camp->get(1);

    $info = array(
      'campaigns'          => array(),

      'tags'               => array(
        'notag'            => array(
          'label'          => 'no tag',
          'action'         => 'purge'
         )
       ),

      'purges'             => array(
        'syst'             => array(
          'tagged'         => array(
            'delay'        => intval($gene['purge_syst_tagged_delay']) * 86400,
            'when'         => $gene['purge_syst_tagged_at']
          ),
          'untagged'       => array(
            'delay'        => intval($gene['purge_syst_untagged_delay']) * 86400,
            'when'         => $gene['purge_syst_untagged_at']
					)
				),
				'punct'            => array(
					'delay'          => intval($gene['purge_punct_delay']) * 86400,
					'when'           => $gene['purge_punct_at']
				)
			),

      'dialplan_variables' => array(
        'svientries'       => $gene['svientries'],
        'svivariables'     => $gene['svivariables'],
        'svichoices'       => $gene['svichoices']
      ),

      'records_path'       => $gene['records_path'],
      'records_announce'   => $gene['records_announce']
		);

		$tagmod = &$ipbx->get_module('campaign_tag');
		$tags 	= $tagmod->get_all(null,true);
		foreach($tags as $tag)
			$info['tags'][$tag['name']] = array('label' => $tag['label'], 'action' => $tag['action']);

		$campmod    = &$ipbx->get_module('campaign_campaign');
		$filtermod  = &$ipbx->get_module('campaign_campaign_filter');
		$campaigns = $campmod->get_all(null,true);
		foreach($campaigns as $campaign)
		{
			$key = $campaign['created_at'];

			// get filters
			$filters = &$filtermod->get_all_where(array('campaign_id' => $campaign['id']));
			$campaign['filters'] = array('agents' => array(), 'queues' => array(), 'skills' => array(), 'directions' => null);
			foreach($filters as $filter)
			{
				switch($filter['type']) 
				{
				case 'agent':
				case 'queue':
					$campaign['filters'][$filter['type'].'s'][] = intval($filter['value']);
					break;

				case 'skill':
					$campaign['filters'][$filter['type'].'s'][] = split("\n", str_replace("\r", "", $filter['value']));
					break;

				case 'way':
					$campaign['filters']['directions'] = $filter['value'] == 'in'?'I':($filter['value'] == 'out'?'O':'I && O');
					break;

				case 'competences':
					// NOT YET IMPLEMENTED
				default:
				}
			}

			$info['campaigns'][$key] = $campaign;
		}

		//$http_response->set_status_line(204);
		//$http_response->send(true);
		$_TPL->set_var('info',$info);
}

$_TPL->set_var('act',$act);
$_TPL->set_var('sum',$_QRY->get('sum'));
$_TPL->display('/service/ipbx/'.$ipbx->get_name().'/generic');

?>

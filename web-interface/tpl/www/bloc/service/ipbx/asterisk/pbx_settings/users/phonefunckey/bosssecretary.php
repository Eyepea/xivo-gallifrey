<?php

#
# XiVO Web-Interface
# Copyright (C) 2006-2009  Proformatique <technique@proformatique.com>
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

$form = &$this->get_module('form');
$url = &$this->get_module('url');

$list = $this->get_var('bsfilter_list');
$fkdata = $this->get_var('fkdata');

$select = array();
$select['paragraph'] = false;
$select['name'] = 'phonefunckey[typeval][]';
$select['label'] = false;
$select['key'] = 'callfilteridentity';
$select['altkey'] = 'id';
$select['id'] = 'it-phonefunckey-extenfeatures-bsfilter-typeval';

if($fkdata['ex'] === false):
	$incr = dwho_uint($fkdata['incr']);
	$select['id'] .= '-'.$incr;

	if($fkdata['type'] === 'extenfeatures-bsfilter'):
		$select['invalid'] = true;
		$select['selected'] = $fkdata['typeval'];
	endif;

	$selectoptattr = '';

	$hrefstyle = 'id="fd-phonefunckey-extenfeatures-bsfilter-typeval-'.$incr.'"';
else:
	$select['disabled'] = true;

	$selectoptattr = 'style="display: none;"';

	$hrefstyle = 'id="fd-phonefunckey-extenfeatures-bsfilter-typeval" style="display: none;"';
endif;

if(empty($list) === false):
	echo	$form->select($select,$list,$selectoptattr);
else:
	echo	$form->hidden($select),
		$url->href_html($this->bbf('create_callfilter'),
				'service/ipbx/call_management/callfilter',
				'act=add',
				$hrefstyle);
endif;

?>

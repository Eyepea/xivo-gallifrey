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

$list = $this->get_varra('fkdest_list','queues');
$fkdata = $this->get_var('fkdata');

$select = array();
$select['field'] = false;
$select['name'] = 'phonefunckey[typeval][]';
$select['label'] = false;
$select['key'] = 'identity';
$select['altkey'] = 'id';
$select['id'] = 'it-phonefunckey-queue-typeval';

if($fkdata['ex'] === false):
	$incr = xivo_uint($fkdata['incr']);
	$select['id'] .= '-'.$incr;

	if($fkdata['type'] === 'queue'):
		$select['invalid'] = true;
		$select['value'] = $fkdata['queue'];
	endif;

	$selectoptattr = '';

	$hrefstyle = 'id="fd-phonefunckey-queue-typeval-'.$incr.'"';
else:
	$select['disabled'] = true;

	$selectoptattr = ' onfocus="xivo_fm_set_onfocus(this);"'.
			 ' onblur="xivo_fm_set_onblur(this);"'.
			 ' style="display: none;"';

	$hrefstyle = 'id="fd-phonefunckey-queue-typeval" style="display: none;"';
endif;

if(empty($list) === false):
	echo	$form->select($select,$list,$selectoptattr);
else:
	echo	$form->hidden($select),
		$url->href_html($this->bbf('create_queue'),
				'service/ipbx/pbx_settings/queues',
				'act=add',
				$hrefstyle);
endif;

?>

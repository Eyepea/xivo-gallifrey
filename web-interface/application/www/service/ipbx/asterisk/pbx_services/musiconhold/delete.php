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

$param['page'] = $page;

if(isset($_QR['id']) === false
|| ($infos = $musiconhold->get_category($_QR['id'],null)) === false)
	$_QRY->go($_TPL->url('service/ipbx/pbx_services/musiconhold'),$param);

$musiconhold->delete_category($infos['cat']['category']);

$_QRY->go($_TPL->url('service/ipbx/pbx_services/musiconhold'),$param);

?>

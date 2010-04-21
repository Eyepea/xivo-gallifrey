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
/*
$this->file_include('xivo/monitoring/systems');
$this->file_include('xivo/monitoring/memstats');
$this->file_include('xivo/monitoring/services');
*/

?>

<div id="systems"><?php $this->file_include('xivo/monitoring/systems'); ?></div>
<div id="memstats"><?php $this->file_include('xivo/monitoring/memstats'); ?></div>
<div id="services"><?php $this->file_include('xivo/monitoring/services'); ?></div>

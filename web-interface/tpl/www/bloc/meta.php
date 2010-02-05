<?php

#
# XiVO Web-Interface
# Copyright (C) 2006-2010  Proformatique <technique@proformatique.com>
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

?>
<meta http-equiv="content-type" content="text/html; charset=utf-8">
<meta http-equiv="content-language" content="<?=DWHO_I18N_BABELFISH_LANGUAGE?>">

<meta name="charset" content="utf-8">
<meta name="robots" content="none">
<meta name="author" content="<?=XIVO_CORP_LABEL?>">
<meta name="publisher" content="<?=XIVO_CORP_LABEL?>">
<meta name="distribution" content="iu">
<meta name="copyright" content="Copyright <?=dwho_i18n::strftime_l('%Y',null),
					     ' ',
					     XIVO_CORP_LABEL?>">
<meta name="title" content="<?=dwho_htmlsc($this->bbf('page_title',php_uname('n')));?>">

<link rel="icon" href="<?=$this->file_time($this->url('favicon.ico'));?>">
<link rel="shortcut icon" href="<?=$this->file_time($this->url('favicon.ico'));?>">

<link rel="stylesheet" type="text/css" charset="utf-8" href="<?=$this->file_time($this->url('css/xivo.css'));?>">
<script type="text/javascript" charset="utf-8" src="<?=$this->file_time($this->url('js/dwho.js'));?>"></script>
<script type="text/javascript" charset="utf-8" src="<?=$this->file_time($this->url('js/dwho/dom.js'));?>"></script>
<script type="text/javascript" charset="utf-8" src="<?=$this->file_time($this->url('js/dwho/form.js'));?>"></script>
<script type="text/javascript" charset="utf-8" src="<?=$this->file_time($this->url('js/xivo.js'));?>"></script>

<?php

$dhtml = &$this->get_module('dhtml');

echo $dhtml->mk_js(),"\n",$dhtml->mk_css();

?>

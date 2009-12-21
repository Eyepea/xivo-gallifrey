<?php
$url = &$this->get_module('url');
$form = &$this->get_module('form');
#$_LANG = &dwho_gat::load_get('language',XIVO_PATH_OBJECTCONF);
$lang_list = $this->get_var('lang-list'); 

echo 
	$form->hidden(array('name' => 'fm_send',
						'value' => 1)),

	$form->hidden(array('name' => 'step',
						'value' => $this->get_var('step'))),

	$form->select(array('desc'  => $this->bbf('fm_language'),
						'name'  => 'language',
						'id'    => 'it-language',
						'selected'  => DWHO_I18N_BABELFISH_LANGUAGE),
						$lang_list);

?>

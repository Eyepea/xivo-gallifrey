<?php
	$form = &$this->get_module('form');
	$url = &$this->get_module('url');
	$dhtml = &$this->get_module('dhtml');

	$element = $this->get_var('element');
	$type = $this->get_var('type');
?>
<div class="fm-field fm-multilist">

	<div class="slt-list">

		<?=$form->select(array('name'		=> 'contextentity['.$type.'][]',
				       'label'		=> false,
				       'id'		=> 'it-contextentity-'.$type,
				       'key'		=> 'typeval',
				       'altkey'		=> 'typeval',
				       'multiple'	=> true,
				       'size'		=> 5,
				       'field'		=> false),
				 $this->get_varra('info',array('contextentity',$type)));?>

		<div class="bt-adddelete">

				<a href="#" onclick="xivo_fm_select_add_contextentity_type('it-contextentity-<?=$type?>',prompt('<?=$dhtml->escape($this->bbf('add_contextentity-'.$type));?>')); return(false);" title="<?=$this->bbf('bt_contextentity-'.$type.'-add');?>"><?=$url->img_html('img/site/button/mini/blue/add.gif',$this->bbf('bt_contextentity-'.$type.'-add'),'class="bt-addlist" id="bt-contextentity-'.$type.'-add" border="0"');?></a><br />

				<a href="#" onclick="xivo_fm_select_delete_entry('it-contextentity-<?=$type?>'); return(false);" title="<?=$this->bbf('bt_delete_contextentity-'.$type);?>"><?=$url->img_html('img/site/button/mini/blue/delete.gif',$this->bbf('bt_delete_contextentity-'.$type),'class="bt-deletelist" id="bt-contextentity-'.$type.'-delete" border="0"');?></a>

		</div>

	</div>

</div>
<div class="clearboth"></div>

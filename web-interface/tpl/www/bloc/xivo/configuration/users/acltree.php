<?php
	$form = &$this->get_module('form');
	$url = &$this->get_module('url');

	$tree = $this->get_var('tree');

	if(($parent = $this->get_var('parent')) === null):
		$pid = '';
		$plevel = 0;
	else:
		$pid = $parent['id'];
		$plevel = $parent['level'];
	endif;

	if(is_array($tree) === true && empty($tree) === false):
		if($pid === '' && $plevel === 0):
			echo '<tr><td>';
		endif;

		$keys = array_keys($tree);
		$nb = count($keys);
		$cnt = $nb - 1;

		for($i = 0;$i < $nb;$i++):
			$v = &$tree[$keys[$i]];

			$mod9 = $i % 9;
			$mod3 = $i % 3;

			if($v['level'] === 3):
				echo '<div class="acl-category"><div><h4>',$form->checkbox(array('desc' => array('%s%s',$this->bbf('acl_'.$v['id']),1),'name' => 'tree[]','label' => 'lb-'.$v['id'],'id' => $v['id'],'field' => false,'value' => $v['path'],'checked' => $v['access']),'onclick="xivo_fm_mk_acl(this);"'),'</h4>',(isset($v['child']) === true ? '<span><a href="#" title="'.$this->bbf('opt_browse').'" onclick="xivo_eid(\'table-'.$v['id'].'\').style.display = xivo_eid(\'table-'.$v['id'].'\').style.display == \'block\' ? \'none\' : \'block\'; return(false);">'.$url->img_html('img/site/button/more.gif',$this->bbf('opt_browse'),'border="0"').'</a></span>' : ''),'</div>',"\n";
			else:
				if($i === 0):
					echo '<table cellspacing="0" cellpadding="0" border="0" id="table-'.$v['parent']['id'].'"><tr><td>',"\n";
				elseif($mod9 === 0):
					echo '</td></tr><tr><td>',"\n";
				elseif($mod3 === 0):
					echo '</td><td>';
				endif;

				echo '<div class="acl-func">',$form->checkbox(array('desc' => array('%s%s',$this->bbf('acl_'.$v['id']),1),'name' => 'tree[]','label' => 'lb-'.$v['id'],'id' => $v['id'],'field' => false,'value' => $v['path'],'checked' => $v['access']),'onclick="xivo_fm_mk_acl(this);"'),'</div>',"\n";

				if($cnt === $i):
					if($mod9 < 3):
						$repeat = 2;
					elseif($mod9 < 6):
						$repeat = 1;
					else:
						echo '</td>';
						$repeat = 0;
					endif;
					echo str_repeat('<td>&nbsp;</td>',$repeat),'</tr></table>',"\n";
				endif;

			endif;

			if(isset($v['child']) === true):

				if(isset($v['parent']) === true):
					$parent = $v['parent'];
				else:
					$parent = null;
				endif;

				$this->file_include('bloc/xivo/configuration/users/acltree',array('tree' => $v['child'],'parent' => $parent));
			endif;
			if($v['level'] === 3):
				echo '</div>';
			endif;
		endfor;
		if($pid === '' && $plevel === 0):
			echo '</td></tr>';
		endif;
	endif;
?>

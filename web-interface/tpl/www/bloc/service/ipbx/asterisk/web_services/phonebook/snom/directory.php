<?php
header('Content-Type: text/xml; charset=utf-8');

$list = $this->get_var('list');
?>
<SnomIPPhoneDirectory>
<?php
if(is_array($list) === false || ($nb = count($list)) === 0):
?>
	<DirectoryEntry>
		<Name><?=$this->bbf('phone_noentry');?></Name>
	</DirectoryEntry>
<?php
else:
	for($i = 0;$i < $nb;$i++):
?>
	<DirectoryEntry>
		<Name><?=$this->bbf('phone_name-'.$list[$i]['type'],$list[$i]['name']);?></Name>
		<Telephone><?=$list[$i]['phone']?></Telephone>
	</DirectoryEntry>
<?php
	endfor;
endif;
?>
</SnomIPPhoneDirectory>

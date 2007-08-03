<?php

$url = &$this->get_module('url');

$result = $this->vars('result');
$info = $this->vars('info');

if($result === false)
	die();

header('Expires: 0');
header('Last-Modified: '.gmdate('D, d M Y H:i:s',mktime()).' '.strftime('%z'));
header('Cache-Control: must-revalidate');
header('Content-Type: text/csv');
header('Content-Disposition: attachment; filename=xivo_cdr-'.strftime('%Y-%m-%d-%H:%M:%S').'.csv');

echo '"',$this->bbf('fm_dbeg'),'";"',$info['dbeg'],'"',"\n";
	
if($info['dend'] !== '')
	echo '"',$this->bbf('fm_dend'),'";"',$info['dend'],'"',"\n";

if($info['channel'] !== '')
{
	if($info['channel'] === XIVO_SRE_IPBX_AST_CHAN_UNKNOWN)
		$info['channel'] = $this->bbf('fm_channel-optunknown');

	echo '"',$this->bbf('fm_channel'),'";"',$info['channel'],'"',"\n";
}

if($info['disposition'] !== '')
	echo '"',$this->bbf('fm_disposition'),'";"',$this->bbf('fm_disposition-opt-'.$info['disposition']),'"',"\n";

if($info['amaflags'] !== '')
	echo '"',$this->bbf('fm_amaflags'),'";"',$this->bbf('fm_amaflags-opt-'.$info['amaflagsmeta']),'"',"\n";

if($info['src'] !== '')
	echo '"',$this->bbf('fm_src'),'";"',$info['src'],'"',"\n";

if($info['dst'] !== '')
	echo '"',$this->bbf('fm_dst'),'";"',$info['dst'],'"',"\n";

if($info['clid'] !== '')
	echo '"',$this->bbf('fm_clid'),'";"',$info['clid'],'"',"\n";

if($info['accountcode'] !== '')
	echo '"',$this->bbf('fm_accountcode'),'";"',$info['accountcode'],'"',"\n";

if($info['userfield'] !== '')
	echo '"',$this->bbf('fm_userfield'),'";"',$info['userfield'],'"',"\n";

if($info['dubeg'] !== '')
	echo '"',$this->bbf('fm_dubeg'),'";"',$this->bbf('fm_dubeg-'.$info['dubegunit'],$info['dubeg']),'"',"\n";

if($info['duend'] !== '')
	echo '"',$this->bbf('fm_duend'),'";"',$this->bbf('fm_duend-'.$info['duendunit'],$info['duend']),'"',"\n";

echo "\n";

if($result === null || ($nb = count($result)) === 0)
{
	echo $this->bbf('no_cdr-result');
	die();
}

echo	'"',$this->bbf('col_calldate'),'";"',$this->bbf('col_src'),'";"',$this->bbf('col_dst'),'";"',$this->bbf('col_duration'),'";'.
	'"',$this->bbf('col_channel'),'";"',$this->bbf('col_disposition'),'";"',$this->bbf('col_amaflags'),'";"',$this->bbf('col_clid'),'";'.
	'"',$this->bbf('col_accountcode'),'";"',$this->bbf('col_userfield'),'";"',$this->bbf('col_dcontext'),'";"',$this->bbf('col_dstchannel'),'";'.
	'"',$this->bbf('col_billsec'),'";"',$this->bbf('col_lastapp'),'";"',$this->bbf('col_lastdata'),'";"',$this->bbf('col_uniqueid'),'"',"\n";

for($i = 0;$i < $nb;$i++)
{
	$ref = &$result[$i];	

	if($ref['channel'] === XIVO_SRE_IPBX_AST_CHAN_UNKNOWN)
		$ref['channel'] = $this->bbf('entry_channel-unknown');

	echo	'"',strftime($this->bbf('date_format_yymmddhhiiss'),$ref['callunixtime']),'";'.
		'"',$ref['src'],'";"',$ref['dst'],'";"',$ref['duration'],'";"',$ref['channel'],'";',
		'"',$this->bbf('entry_disposition-'.$ref['disposition']),'";"',$this->bbf('entry_amaflagsmeta-'.$ref['amaflagsmeta']),'";',
		'"',$ref['clid'],'";"',$ref['accountcode'],'";"',$ref['userfield'],'";"',$ref['dcontext'],'";"',$ref['dstchannel'],'";',
		'"',$ref['billsec'],'";"',$ref['lastapp'],'";"',$ref['lastdata'],'";"',$ref['uniqueid'],'"',"\n";
}

die();

?>

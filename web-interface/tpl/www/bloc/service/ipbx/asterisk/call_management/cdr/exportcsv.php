<?php

$url = &$this->get_module('url');

$result = $this->get_var('result');
$info = $this->get_var('info');

if($result === false)
	die();

header('Pragma: no-cache');
header('Cache-Control: private, must-revalidate');
header('Last-Modified: '.date('D, d M Y H:i:s',mktime()).' '.strftime('%Z'));
header('Content-Disposition: attachment; filename=xivo_cdr-'.strftime('%Y-%m-%d-%H:%M:%S').'.csv');
header('Content-Type: text/csv; charset=UTF-8');

ob_start();

echo	'"',str_replace('"','""',$this->bbf('fm_dbeg')),'";',
	'"',str_replace('"','""',$info['dbeg']),'"',"\n";
	
if($info['dend'] !== '')
{
	echo	'"',str_replace('"','""',$this->bbf('fm_dend')),'";',
		'"',str_replace('"','""',$info['dend']),'"',"\n";
}

if($info['channel'] !== '')
{
	if($info['channel'] === XIVO_SRE_IPBX_AST_CHAN_UNKNOWN)
		$info['channel'] = $this->bbf('fm_channel-optunknown');

	echo	'"',str_replace('"','""',$this->bbf('fm_channel')),'";',
		'"',str_replace('"','""',$info['channel']),'"',"\n";
}

if($info['disposition'] !== '')
{
	echo	'"',str_replace('"','""',$this->bbf('fm_disposition')),'";',
		'"',str_replace('"','""',$this->bbf('fm_disposition-opt-'.$info['disposition'])),'"',"\n";
}

if($info['amaflags'] !== '')
{
	echo	'"',str_replace('"','""',$this->bbf('fm_amaflags')),'";',
		'"',str_replace('"','""',$this->bbf('ast_amaflag_name_info-'.$info['amaflagsmeta'])),'"',"\n";
}

if($info['dcontext'] !== '')
{
	echo	'"',str_replace('"','""',$this->bbf('fm_dcontext')),'";',
		'"',str_replace('"','""',$info['dcontext']),'"',"\n";
}

if($info['src'] !== '')
{
	echo	'"',str_replace('"','""',$this->bbf('fm_src')),'";',
		'"',str_replace('"','""',$info['src']),'"',"\n";
}

if($info['dst'] !== '')
{
	echo	'"',str_replace('"','""',$this->bbf('fm_dst')),'";',
		'"',str_replace('"','""',$info['dst']),'"',"\n";
}

if($info['clid'] !== '')
{
	echo	'"',str_replace('"','""',$this->bbf('fm_clid')),'";',
		'"',str_replace('"','""',$info['clid']),'"',"\n";
}

if($info['accountcode'] !== '')
{
	echo	'"',str_replace('"','""',$this->bbf('fm_accountcode')),'";',
		'"',str_replace('"','""',$info['accountcode']),'"',"\n";
}

if($info['userfield'] !== '')
{
	echo	'"',str_replace('"','""',$this->bbf('fm_userfield')),'";',
		'"',str_replace('"','""',$info['userfield']),'"',"\n";
}

if($info['dubeg'] !== '')
{
	echo	'"',str_replace('"','""',$this->bbf('fm_dubeg')),'";',
		'"',str_replace('"','""',$this->bbf('fm_dubeg-'.$info['dubegunit'],$info['dubeg'])),'"',"\n";
}

if($info['duend'] !== '')
{
	echo	'"',str_replace('"','""',$this->bbf('fm_duend')),'";',
		'"',str_replace('"','""',$this->bbf('fm_duend-'.$info['duendunit'],$info['duend'])),'"',"\n";
}

echo "\n";

if($result === null || ($nb = count($result)) === 0)
{
	echo	$this->bbf('no_cdr-result');
	header('Content-Length: '.ob_get_length());
	ob_end_flush();
	die();
}

echo	'"',str_replace('"','""',$this->bbf('col_calldate')),'";',
	'"',str_replace('"','""',$this->bbf('col_src')),'";',
	'"',str_replace('"','""',$this->bbf('col_dst')),'";',
	'"',str_replace('"','""',$this->bbf('col_duration')),'";',
	'"',str_replace('"','""',$this->bbf('col_channel')),'";',
	'"',str_replace('"','""',$this->bbf('col_disposition')),'";',
	'"',str_replace('"','""',$this->bbf('col_amaflags')),'";',
	'"',str_replace('"','""',$this->bbf('col_clid')),'";',
	'"',str_replace('"','""',$this->bbf('col_accountcode')),'";',
	'"',str_replace('"','""',$this->bbf('col_userfield')),'";',
	'"',str_replace('"','""',$this->bbf('col_dcontext')),'";',
	'"',str_replace('"','""',$this->bbf('col_dstchannel')),'";',
	'"',str_replace('"','""',$this->bbf('col_billsec')),'";',
	'"',str_replace('"','""',$this->bbf('col_lastapp')),'";',
	'"',str_replace('"','""',$this->bbf('col_lastdata')),'";',
	'"',str_replace('"','""',$this->bbf('col_uniqueid')),'"',"\n";

for($i = 0;$i < $nb;$i++)
{
	$ref = &$result[$i];	

	if($ref['channel'] === XIVO_SRE_IPBX_AST_CHAN_UNKNOWN)
		$ref['channel'] = $this->bbf('entry_channel-unknown');

	echo	'"',str_replace('"','""',strftime($this->bbf('date_format_yymmddhhiiss'),strtotime($ref['calldate']))),'";',
		'"',str_replace('"','""',$ref['src']),'";',
		'"',str_replace('"','""',$ref['dst']),'";',
		'"',str_replace('"','""',$ref['duration']),'";',
		'"',str_replace('"','""',$ref['channel']),'";',
		'"',str_replace('"','""',$this->bbf('entry_disposition-'.$ref['disposition'])),'";',
		'"',str_replace('"','""',$this->bbf('ast_amaflag_name_info-'.$ref['amaflagsmeta'])),'";',
		'"',str_replace('"','""',$ref['clid']),'";',
		'"',str_replace('"','""',$ref['accountcode']),'";',
		'"',str_replace('"','""',$ref['userfield']),'";',
		'"',str_replace('"','""',$ref['dcontext']),'";',
		'"',str_replace('"','""',$ref['dstchannel']),'";',
		'"',str_replace('"','""',$ref['billsec']),'";',
		'"',str_replace('"','""',$ref['lastapp']),'";',
		'"',str_replace('"','""',$ref['lastdata']),'";',
		'"',str_replace('"','""',$ref['uniqueid']),'"',"\n";
}

header('Content-Length: '.ob_get_length());
ob_end_flush();
die();

?>

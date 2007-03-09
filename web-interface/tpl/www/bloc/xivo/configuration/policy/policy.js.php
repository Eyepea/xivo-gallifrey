function xivo_fm_mk_policy(tree)
{
	var ref = tree.form[tree.name];
	var value = tree.value;
	var len = value.length;
	var nb = ref.length
	var sub = 0;
	var rs = false;

	for(i = 0;i < nb;i++)
	{
		sub = ref[i].value.substring(0,len);
		rs = tree.checked == true ? true : false;

		if(value == sub)
			ref[i].checked = rs;
	}
}

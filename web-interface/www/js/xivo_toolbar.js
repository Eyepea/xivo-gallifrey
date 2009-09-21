var xivo_toolbar_fn_adv_menu_enable = function(e)
{
   	if(xivo_is_function(e.preventDefault) === true)
		e.preventDefault();

	xivo_fm[xivo_toolbar_form_name]['act'].value = 'enables';
	xivo_fm[xivo_toolbar_form_name].submit();
}

var xivo_toolbar_fn_adv_menu_disable = function(e)
{
   	if(xivo_is_function(e.preventDefault) === true)
		e.preventDefault();

	xivo_fm[xivo_toolbar_form_name]['act'].value = 'disables';
	xivo_fm[xivo_toolbar_form_name].submit();
}

var xivo_toolbar_fn_adv_menu_delete = function(e)
{
   	if(xivo_is_function(e.preventDefault) === true)
		e.preventDefault();

	if(confirm(xivo_toolbar_adv_menu_delete_confirm) === true)
	{
		if(xivo_is_undef(xivo_fm[xivo_toolbar_form_name]['search']) === false
		&& typeof(xivo_toolbar_fm_search) !== 'undefined')
			xivo_fm[xivo_toolbar_form_name]['search'].value = xivo_toolbar_fm_search;

		xivo_fm[xivo_toolbar_form_name]['act'].value = 'deletes';
		xivo_fm[xivo_toolbar_form_name].submit();
	}
}

xivo.dom.set_onload(function()
{
	if(typeof(xivo_toolbar_fm_search) === 'undefined' || xivo_has_len(xivo_toolbar_fm_search) === false)
		xivo_fm_set_events_text_helper('it-toolbar-search');

	xivo.dom.add_event('mouseover',
			   xivo_eid('toolbar-bt-add'),
			   function()
			   {
			   	if((add_menu = xivo_eid('toolbar-add-menu')) !== false)
			   		add_menu.style.display = 'block';
			   });

	xivo.dom.add_event('mouseout',
			   xivo_eid('toolbar-bt-add'),
			   function()
			   {
			   	if((add_menu = xivo_eid('toolbar-add-menu')) !== false)
			   		add_menu.style.display = 'none';
			   });

	xivo.dom.add_event('mouseover',
			   xivo_eid('toolbar-add-menu'),
			   function()
			   {
				this.style.display = 'block';
			   });

	xivo.dom.add_event('mouseout',
			   xivo_eid('toolbar-add-menu'),
			   function()
			   {
				this.style.display = 'none';
			   });

	xivo.dom.add_event('mouseover',
			   xivo_eid('toolbar-bt-advanced'),
			   function()
			   {
			   	if((advanced_menu = xivo_eid('toolbar-advanced-menu')) !== false)
			   		advanced_menu.style.display = 'block';
			   });

	xivo.dom.add_event('mouseout',
			   xivo_eid('toolbar-bt-advanced'),
			   function()
			   {
			   	if((advanced_menu = xivo_eid('toolbar-advanced-menu')) !== false)
			   		advanced_menu.style.display = 'none';
			   });

	xivo.dom.add_event('mouseover',
			   xivo_eid('toolbar-advanced-menu'),
			   function()
			   {
				this.style.display = 'block';
			   });

	xivo.dom.add_event('mouseout',
			   xivo_eid('toolbar-advanced-menu'),
			   function()
			   {
				this.style.display = 'none';
			   });

	xivo.dom.add_event('click',
			   xivo_eid('toolbar-advanced-menu-enable'),
			   xivo_toolbar_fn_adv_menu_enable);

	xivo.dom.add_event('click',
			   xivo_eid('toolbar-advanced-menu-disable'),
			   xivo_toolbar_fn_adv_menu_disable);

	xivo.dom.add_event('click',
			   xivo_eid('toolbar-advanced-menu-select-all'),
			   function(e)
			   {
			   	if(xivo_is_function(e.preventDefault) === true)
					e.preventDefault();

				xivo_fm_checked_all(xivo_toolbar_form_name,
						    xivo_toolbar_form_list);
			   });

	xivo.dom.add_event('click',
			   xivo_eid('toolbar-advanced-menu-delete'),
			   xivo_toolbar_fn_adv_menu_delete);
});

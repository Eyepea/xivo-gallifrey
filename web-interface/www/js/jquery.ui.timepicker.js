/*
 * jQuery UI Timepicker 0.0.1
 *
 * Copyright 2010, Francois Gelinas
 * Dual licensed under the MIT or GPL Version 2 licenses.
 * http://jquery.org/license
 *
 * http://fgelinas.com
 *
 * Depends:
 *	jquery.ui.core.js
 */

/*
 * As it is a timepicker, I insipred most of the code from the datepicker
 * Francois Gelinas - Nov 2010
 */

(function( $, undefined ) {

$.extend($.ui, { timepicker: { version: "0.0.1" } });

var PROP_NAME = 'timepicker';
var tpuuid = new Date().getTime();

/* Time picker manager.
   Use the singleton instance of this class, $.timepicker, to interact with the time picker.
   Settings for (groups of) time pickers are maintained in an instance object,
   allowing multiple different settings on the same page. */

function Timepicker() {
	this.debug = true; // Change this to true to start debugging
	this._curInst = null; // The current instance in use

    this._disabledInputs = []; // List of time picker inputs that have been disabled
	this._timepickerShowing = false; // True if the popup picker is showing , false if not
	this._inDialog = false; // True if showing within a "dialog", false if not
	this._dialogClass = 'ui-timepicker-dialog'; // The name of the dialog marker class
    this._mainDivId = 'ui-timepicker-div'; // The ID of the main timepicker division
	this._currentClass = 'ui-timepicker-current'; // The name of the current hour / minutes marker class
	this._dayOverClass = 'ui-timepicker-days-cell-over'; // The name of the day hover marker class
    
	this.regional = []; // Available regional settings, indexed by language code
	this.regional[''] = { // Default regional settings
		hourText: 'Heure', // Display text for hours section
		minuteText: 'Minutes', // Display text for minutes link
        amPmText: ['', ''] // Display text for AM PM
		
	};
	this._defaults = { // Global defaults for all the time picker instances
		showOn: 'focus', // 'focus' for popup on focus,
			// 'button' for trigger button, or 'both' for either
		showAnim: 'fadeIn', // Name of jQuery animation for popup
		showOptions: {}, // Options for enhanced animations
		appendText: '', // Display text following the input box, e.g. showing the format
		onSelect: null, // Define a callback function when a hour / minutes is selected
		onClose: null, // Define a callback function when the timepicker is closed
        timeSeparator: ':' // The caracter to use to separate hours and minutes
	};
	$.extend(this._defaults, this.regional['']);
	this.tpDiv = $('<div id="' + this._mainDivId + '" class="ui-timepicker ui-widget ui-widget-content ui-helper-clearfix ui-corner-all ui-helper-hidden-accessible"></div>');
}

$.extend(Timepicker.prototype, {
	/* Class name added to elements to indicate already configured with a time picker. */
	markerClassName: 'hasTimepicker',

    /* Debug logging (if enabled). */
	log: function () {
		if (this.debug)
			console.log.apply('', arguments);
	},

    // TODO rename to "widget" when switching to widget factory
	_widgetTimepicker: function() {
		return this.tpDiv;
	},

	/* Override the default settings for all instances of the time picker.
	   @param  settings  object - the new settings to use as defaults (anonymous object)
	   @return the manager object */
	setDefaults: function(settings) {
		extendRemove(this._defaults, settings || {});
		return this;
	},

    /* Attach the time picker to a jQuery selection.
	   @param  target    element - the target input field or division or span
	   @param  settings  object - the new settings to use for this time picker instance (anonymous) */
	_attachTimepicker: function(target, settings) {
		// check for settings on the control itself - in namespace 'time:'
		var inlineSettings = null;
		for (var attrName in this._defaults) {
			var attrValue = target.getAttribute('time:' + attrName);
			if (attrValue) {
				inlineSettings = inlineSettings || {};
				try {
					inlineSettings[attrName] = eval(attrValue);
				} catch (err) {
					inlineSettings[attrName] = attrValue;
				}
			}
		}
		var nodeName = target.nodeName.toLowerCase();
		var inline = (nodeName == 'div' || nodeName == 'span');
		if (!target.id) {
			this.uuid += 1;
			target.id = 'tp' + this.uuid;
		}
		var inst = this._newInst($(target), inline);
		inst.settings = $.extend({}, settings || {}, inlineSettings || {});
		if (nodeName == 'input') {
			this._connectTimepicker(target, inst);
		} else if (inline) {
			this._inlineTimepicker(target, inst);
		}
	},

	/* Create a new instance object. */
	_newInst: function(target, inline) {
		var id = target[0].id.replace(/([^A-Za-z0-9_-])/g, '\\\\$1'); // escape jQuery meta chars
		return {id: id, input: target, // associated target
			selectedDay: 0, selectedMonth: 0, selectedYear: 0, // current selection
			drawMonth: 0, drawYear: 0, // month being drawn
			inline: inline, // is timepicker inline or not
			tpDiv: (!inline ? this.tpDiv : // presentation div
			$('<div class="' + this._inlineClass + ' ui-timepicker ui-widget ui-widget-content ui-helper-clearfix ui-corner-all"></div>'))};
	},

	/* Attach the time picker to an input field. */
	_connectTimepicker: function(target, inst) {
		var input = $(target);
		inst.append = $([]);
		inst.trigger = $([]);
		if (input.hasClass(this.markerClassName))
			return;
		this._attachments(input, inst);
		input.addClass(this.markerClassName).keydown(this._doKeyDown).
			bind("setData.timepicker", function(event, key, value) {
				inst.settings[key] = value;
			}).bind("getData.timepicker", function(event, key) {
				return this._get(inst, key);
			});
		//this._autoSize(inst);
		$.data(target, PROP_NAME, inst);
	},

    /* Handle keystrokes. */
	_doKeyDown: function(event) {
		var inst = $.timepicker._getInst(event.target);
		var handled = true;
		inst._keyEvent = true;
		if ($.timepicker._timepickerShowing)
			switch (event.keyCode) {
				case 9: $.timepicker._hideTimepicker();
						handled = false;
						break; // hide on tab out
				case 27: $.timepicker._hideTimepicker();
						break; // hide on escape
				default: handled = false;
			}
		else if (event.keyCode == 36 && event.ctrlKey) // display the time picker on ctrl+home
			$.timepicker._showTimepicker(this);
		else {
			handled = false;
		}
		if (handled) {
			event.preventDefault();
			event.stopPropagation();
		}
	},

	/* Make attachments based on settings. */
	_attachments: function(input, inst) {
		var appendText = this._get(inst, 'appendText');
		var isRTL = this._get(inst, 'isRTL');
		if (inst.append)
			inst.append.remove();
		if (appendText) {
			inst.append = $('<span class="' + this._appendClass + '">' + appendText + '</span>');
			input[isRTL ? 'before' : 'after'](inst.append);
		}
		input.unbind('focus', this._showTimepicker);
		if (inst.trigger)
			inst.trigger.remove();
		var showOn = this._get(inst, 'showOn');
		if (showOn == 'focus' || showOn == 'both') // pop-up time picker when in the marked field
			input.focus(this._showTimepicker);
		if (showOn == 'button' || showOn == 'both') { // pop-up time picker when button clicked
			var buttonText = this._get(inst, 'buttonText');
			var buttonImage = this._get(inst, 'buttonImage');
			inst.trigger = $(this._get(inst, 'buttonImageOnly') ?
				$('<img/>').addClass(this._triggerClass).
					attr({ src: buttonImage, alt: buttonText, title: buttonText }) :
				$('<button type="button"></button>').addClass(this._triggerClass).
					html(buttonImage == '' ? buttonText : $('<img/>').attr(
					{ src:buttonImage, alt:buttonText, title:buttonText })));
			input[isRTL ? 'before' : 'after'](inst.trigger);
			inst.trigger.click(function() {
				if ($.timepicker._timepickerShowing && $.timepicker._lastInput == input[0])
					$.timepicker._hideTimepicker();
				else
					$.timepicker._showTimepicker(input[0]);
				return false;
			});
		}
	},


    /* Pop-up the time picker for a given input field.
	   @param  input  element - the input field attached to the time picker or
	                  event - if triggered by focus */
	_showTimepicker: function(input) {
		input = input.target || input;
		if (input.nodeName.toLowerCase() != 'input') // find from button/image trigger
			input = $('input', input.parentNode)[0];
		if ($.timepicker._isDisabledTimepicker(input) || $.timepicker._lastInput == input) // already here
			return;
		var inst = $.timepicker._getInst(input);
		if ($.timepicker._curInst && $.timepicker._curInst != inst) {
			$.timepicker._curInst.tpDiv.stop(true, true);
		}
		var beforeShow = $.timepicker._get(inst, 'beforeShow');
		extendRemove(inst.settings, (beforeShow ? beforeShow.apply(input, [input, inst]) : {}));
		inst.lastVal = null;
		$.timepicker._lastInput = input;

		$.timepicker._setTimeFromField(inst);
		if ($.timepicker._inDialog) // hide cursor
			input.value = '';
		if (!$.timepicker._pos) { // position below input
			$.timepicker._pos = $.timepicker._findPos(input);
			$.timepicker._pos[1] += input.offsetHeight; // add the height
		}
		var isFixed = false;
		$(input).parents().each(function() {
			isFixed |= $(this).css('position') == 'fixed';
			return !isFixed;
		});
		if (isFixed && $.browser.opera) { // correction for Opera when fixed and scrolled
			$.timepicker._pos[0] -= document.documentElement.scrollLeft;
			$.timepicker._pos[1] -= document.documentElement.scrollTop;
		}
		var offset = {left: $.timepicker._pos[0], top: $.timepicker._pos[1]};
		$.timepicker._pos = null;
		// determine sizing offscreen
		inst.tpDiv.css({position: 'absolute', display: 'block', top: '-1000px'});
		$.timepicker._updateTimepicker(inst);

        // reset clicked state
        inst._hoursClicked = false;
        inst._minutesClicked = false;

		// fix width for dynamic number of time pickers
		// and adjust position before showing
		offset = $.timepicker._checkOffset(inst, offset, isFixed);
		inst.tpDiv.css({position: ($.timepicker._inDialog && $.blockUI ?
			'static' : (isFixed ? 'fixed' : 'absolute')), display: 'none',
			left: offset.left + 'px', top: offset.top + 'px'});
		if (!inst.inline) {
			var showAnim = $.timepicker._get(inst, 'showAnim');
			var duration = $.timepicker._get(inst, 'duration');
			var postProcess = function() {
				$.timepicker._timepickerShowing = true;
				var borders = $.timepicker._getBorders(inst.tpDiv);
				inst.tpDiv.find('iframe.ui-timepicker-cover'). // IE6- only
					css({left: -borders[0], top: -borders[1],
						width: inst.tpDiv.outerWidth(), height: inst.tpDiv.outerHeight()});
			};
			inst.tpDiv.zIndex($(input).zIndex()+1);
			if ($.effects && $.effects[showAnim])
				inst.tpDiv.show(showAnim, $.timepicker._get(inst, 'showOptions'), duration, postProcess);
			else
				inst.tpDiv[showAnim || 'show']((showAnim ? duration : null), postProcess);
			if (!showAnim || !duration)
				postProcess();
			if (inst.input.is(':visible') && !inst.input.is(':disabled'))
				inst.input.focus();
			$.timepicker._curInst = inst;

			$("#"+ $.timepicker._mainDivId)[0].style.visibility = 'visible';
		}
	},

	/* Generate the time picker content. */
	_updateTimepicker: function(inst) {
		var self = this;
		var borders = $.timepicker._getBorders(inst.tpDiv);
		inst.tpDiv.empty().append(this._generateHTML(inst))
			.find('iframe.ui-timepicker-cover') // IE6- only
				.css({left: -borders[0], top: -borders[1],
					width: inst.tpDiv.outerWidth(), height: inst.tpDiv.outerHeight()})
			.end()
			.find('.ui-timepicker td a')
				.bind('mouseout', function(){
					$(this).removeClass('ui-state-hover');
					if(this.className.indexOf('ui-timepicker-prev') != -1) $(this).removeClass('ui-timepicker-prev-hover');
					if(this.className.indexOf('ui-timepicker-next') != -1) $(this).removeClass('ui-timepicker-next-hover');
				})
				.bind('mouseover', function(){
					if (!self._isDisabledTimepicker( inst.inline ? inst.tpDiv.parent()[0] : inst.input[0])) {
						$(this).parents('.ui-timepicker-calendar').find('a').removeClass('ui-state-hover');
						$(this).addClass('ui-state-hover');
						if(this.className.indexOf('ui-timepicker-prev') != -1) $(this).addClass('ui-timepicker-prev-hover');
						if(this.className.indexOf('ui-timepicker-next') != -1) $(this).addClass('ui-timepicker-next-hover');
					}
				})
			.end()
			.find('.' + this._dayOverClass + ' a')
				.trigger('mouseover')
			.end();
		//var numMonths = this._getNumberOfMonths(inst);
		//var cols = numMonths[1];
		//var width = 17;
		//if (cols > 1)
		//	inst.tpDiv.addClass('ui-timepicker-multi-' + cols).css('width', (width * cols) + 'em');
		//else
		//	inst.tpDiv.removeClass('ui-timepicker-multi-2 ui-timepicker-multi-3 ui-timepicker-multi-4').width('');
		//inst.tpDiv[(numMonths[0] != 1 || numMonths[1] != 1 ? 'add' : 'remove') +
		//	'Class']('ui-timepicker-multi');
//		inst.tpDiv[(this._get(inst, 'isRTL') ? 'add' : 'remove') +
//			'Class']('ui-timepicker-rtl');
//		if (inst == $.timepicker._curInst && $.timepicker._timepickerShowing && inst.input &&
//				inst.input.is(':visible') && !inst.input.is(':disabled'))
//			inst.input.focus();
	},
    
    /* Generate the HTML for the current state of the date picker. */
	_generateHTML: function(inst) {
        var h, m, html = '';

        html = '<div id="ui-timepicker-hours" >';
        html += '<div class="ui-timepicker-title ui-widget-header ui-helper-clearfix ui-corner-all">' + this._get(inst, 'hourText') + '</div>';

        html += '<table class="ui-timepicker">';
        
        // AM
        html += '<tr>';
        for ( h = 0; h<= 5; h++) 
           html += this._generateHTMLHourCell(inst, h);
        
        html += '</tr><tr>';
        for ( h = 6; h<= 11; h++) 
           html += this._generateHTMLHourCell(inst, h);
        

        // PM
        html += '<tr>';
        for ( h = 12; h<= 17; h++) 
           html += this._generateHTMLHourCell(inst, h);
        
        html += '</tr><tr>';
        for ( h = 18; h<= 23; h++) 
           html += this._generateHTMLHourCell(inst, h);
        html += '</tr></table></div>'; // close the Hour div

        html += '<div id="ui-timepicker-minutes">';

        /* Add the minutes */
        html += '<div class="ui-timepicker-title ui-widget-header ui-helper-clearfix ui-corner-all">' + this._get(inst, 'minuteText') + '</div>';

        html += '<table class="ui-timepicker">';

        html += '<tr>';
        for ( m = 0; m <= 10; m += 5) 
            html += this._generateHTMLMinuteCell(inst, m);
        html += '</tr><tr>';
        for ( m = 15; m <= 25; m += 5)
            html += this._generateHTMLMinuteCell(inst, m);
        html += '</tr><tr>';
        for ( m = 30; m <= 40; m += 5)
            html += this._generateHTMLMinuteCell(inst, m);
        html += '</tr><tr>';
        for ( m = 45; m <= 55; m += 5)
            html += this._generateHTMLMinuteCell(inst, m);
        html += '</tr></table></div>';

        html += '<div style="clear: both"></div>';
        return html;
    },

    /* Generate the content of a "Hour" cell */
    _generateHTMLHourCell: function(inst, hour) {
        var html = '<td ' +
                   'onclick="TP_jQuery_' + tpuuid + '.timepicker.selectHours(\'#' + inst.id + '\', ' + hour.toString() + ', this ); return false;" ' +
                   'ondblclick="TP_jQuery_' + tpuuid + '.timepicker.selectHours(\'#' + inst.id + '\', ' + hour.toString() + ', this, true ); return false;" ' +
                   '>' +
                   '<a href="#" class="ui-state-default ' +
                   (hour == inst.hours ? 'ui-state-active' : '') +
                   '">' +
                   hour.toString() +
                   '</a></td>';
        return html;
    },
    /* Generate the content of a "Hour" cell */
    _generateHTMLMinuteCell: function(inst, minute) {
        var html = '<td ' +
                   'onclick="TP_jQuery_' + tpuuid + '.timepicker.selectMinutes(\'#' + inst.id + '\', ' + minute.toString() + ', this ); return false;" ' +
                   'ondblclick="TP_jQuery_' + tpuuid + '.timepicker.selectMinutes(\'#' + inst.id + '\', ' + minute.toString() + ', this, true ); return false;" ' +
                   '>' +
                   '<a href="#" class="ui-state-default ' +
                   (minute == inst.minutes ? 'ui-state-active' : '') +
                   '" >' +
                   (minute < 10 ? '0' : '') +
                   minute.toString() +
                   '</a></td>';
        return html;
    },
    
    /* Is the first field in a jQuery collection disabled as a timepicker?
	   @param  target    element - the target input field or division or span
	   @return boolean - true if disabled, false if enabled */
	_isDisabledTimepicker: function(target) {
		if (!target) {
			return false;
		}
		for (var i = 0; i < this._disabledInputs.length; i++) {
			if (this._disabledInputs[i] == target)
				return true;
		}
		return false;
	},

    /* Check positioning to remain on screen. */
	_checkOffset: function(inst, offset, isFixed) {
		var tpWidth = inst.tpDiv.outerWidth();
		var tpHeight = inst.tpDiv.outerHeight();
		var inputWidth = inst.input ? inst.input.outerWidth() : 0;
		var inputHeight = inst.input ? inst.input.outerHeight() : 0;
		var viewWidth = document.documentElement.clientWidth + $(document).scrollLeft();
		var viewHeight = document.documentElement.clientHeight + $(document).scrollTop();

		offset.left -= (this._get(inst, 'isRTL') ? (tpWidth - inputWidth) : 0);
		offset.left -= (isFixed && offset.left == inst.input.offset().left) ? $(document).scrollLeft() : 0;
		offset.top -= (isFixed && offset.top == (inst.input.offset().top + inputHeight)) ? $(document).scrollTop() : 0;

		// now check if datepicker is showing outside window viewport - move to a better place if so.
		offset.left -= Math.min(offset.left, (offset.left + tpWidth > viewWidth && viewWidth > tpWidth) ?
			Math.abs(offset.left + tpWidth - viewWidth) : 0);
		offset.top -= Math.min(offset.top, (offset.top + tpHeight > viewHeight && viewHeight > tpHeight) ?
			Math.abs(tpHeight + inputHeight) : 0);

		return offset;
	},

	/* Find an object's position on the screen. */
	_findPos: function(obj) {
		var inst = this._getInst(obj);
		var isRTL = this._get(inst, 'isRTL');
        while (obj && (obj.type == 'hidden' || obj.nodeType != 1)) {
            obj = obj[isRTL ? 'previousSibling' : 'nextSibling'];
        }
        var position = $(obj).offset();
	    return [position.left, position.top];
	},

	/* Retrieve the size of left and top borders for an element.
	   @param  elem  (jQuery object) the element of interest
	   @return  (number[2]) the left and top borders */
	_getBorders: function(elem) {
		var convert = function(value) {
			return {thin: 1, medium: 2, thick: 3}[value] || value;
		};
		return [parseFloat(convert(elem.css('border-left-width'))),
			parseFloat(convert(elem.css('border-top-width')))];
	},


    /* Close time picker if clicked elsewhere. */
	_checkExternalClick: function(event) {
		if (!$.timepicker._curInst)
			return;
		var $target = $(event.target);
		if ($target[0].id != $.timepicker._mainDivId &&
				$target.parents('#' + $.timepicker._mainDivId).length == 0 &&
				!$target.hasClass($.timepicker.markerClassName) &&
				!$target.hasClass($.timepicker._triggerClass) &&
				$.timepicker._timepickerShowing && !($.timepicker._inDialog && $.blockUI))
			$.timepicker._hideTimepicker();
	},

    /* Hide the time picker from view.
	   @param  input  element - the input field attached to the time picker */
	_hideTimepicker: function(input) {
		var inst = this._curInst;
		if (!inst || (input && inst != $.data(input, PROP_NAME)))
			return;
		if (this._timepickerShowing) {
			var showAnim = this._get(inst, 'showAnim');
			var duration = this._get(inst, 'duration');
			var postProcess = function() {
				$.timepicker._tidyDialog(inst);
				this._curInst = null;
			};
			if ($.effects && $.effects[showAnim])
				inst.tpDiv.hide(showAnim, $.timepicker._get(inst, 'showOptions'), duration, postProcess);
			else
				inst.tpDiv[(showAnim == 'slideDown' ? 'slideUp' :
					(showAnim == 'fadeIn' ? 'fadeOut' : 'hide'))]((showAnim ? duration : null), postProcess);
			if (!showAnim)
				postProcess();
			var onClose = this._get(inst, 'onClose');
			if (onClose)
				onClose.apply((inst.input ? inst.input[0] : null),
					[(inst.input ? inst.input.val() : ''), inst]);  // trigger custom callback
			this._timepickerShowing = false;
			this._lastInput = null;
			if (this._inDialog) {
				this._dialogInput.css({ position: 'absolute', left: '0', top: '-100px' });
				if ($.blockUI) {
					$.unblockUI();
					$('body').append(this.tpDiv);
				}
			}
			this._inDialog = false;
		}
	},

	/* Tidy up after a dialog display. */
	_tidyDialog: function(inst) {
		inst.tpDiv.removeClass(this._dialogClass).unbind('.ui-timepicker');
	},

    /* Retrieve the instance data for the target control.
	   @param  target  element - the target input field or division or span
	   @return  object - the associated instance data
	   @throws  error if a jQuery problem getting data */
	_getInst: function(target) {
		try {
			return $.data(target, PROP_NAME);
		}
		catch (err) {
			throw 'Missing instance data for this timepicker';
		}
	},

    /* Get a setting value, defaulting if necessary. */
	_get: function(inst, name) {
		return inst.settings[name] !== undefined ?
			inst.settings[name] : this._defaults[name];
	},

    /* Parse existing time and initialise time picker. */
	_setTimeFromField: function(inst) {
		if (inst.input.val() == inst.lastVal) {
			return;
		}
        var timeVal = inst.lastVal = inst.input ? inst.input.val() : null;
        var time = this.parseTime(timeVal);
        inst.hours = time.hours;
        inst.minutes = time.minutes;
	},

    /*
     * Pase a time string into hours and minutes
     */
    parseTime: function(timeVal) {
        var retVal = new Object();
        retVal.hours = -1;
        retVal.minutes = -1;

        var p = timeVal.search(/[hH:]/);
        if (p == -1)
            return retVal;
        
        retVal.hours = parseInt(timeVal.substr(0,p),10);
        retVal.minutes = parseInt(timeVal.substr(p+1),10);
        return retVal;
    },

	/* Update or retrieve the settings for a time picker attached to an input field or division.
	   @param  target  element - the target input field or division or span
	   @param  name    object - the new settings to update or
	                   string - the name of the setting to change or retrieve,
	                   when retrieving also 'all' for all instance settings or
	                   'defaults' for all global defaults
	   @param  value   any - the new value for the setting
	                   (omit if above is an object or to retrieve a value) */
	_optionTimepicker: function(target, name, value) {
		var inst = this._getInst(target);
		if (arguments.length == 2 && typeof name == 'string') {
			return (name == 'defaults' ? $.extend({}, $.timepicker._defaults) :
				(inst ? (name == 'all' ? $.extend({}, inst.settings) :
				this._get(inst, name)) : null));
		}
		var settings = name || {};
		if (typeof name == 'string') {
			settings = {};
			settings[name] = value;
		}
		if (inst) {
			if (this._curInst == inst) {
				this._hideTimepicker();
			}
			var date = this._getDateTimepicker(target, true);
			extendRemove(inst.settings, settings);
			this._attachments($(target), inst);
			this._autoSize(inst);
			//this._setDateTimepicker(target, date);
			//this._updateTimepicker(inst);
		}
	},

    selectHours: function(id, newHours, td, fromDoubleClick) {
        var target = $(id);
		var inst = this._getInst(target[0]);
        $('#ui-timepicker-hours a').removeClass('ui-state-active');
        $(td).children('a').addClass('ui-state-active');

        inst.hours = newHours;
        this._updateSelectedValue(inst);

        inst._hoursClicked = true;


        if ((inst._minutesClicked) || (fromDoubleClick))
            $.timepicker._hideTimepicker();

    },

    selectMinutes: function(id, newMinutes, td, fromDoubleClick) {
        var target = $(id);
        var inst = this._getInst(target[0]);
        $('#ui-timepicker-minutes a').removeClass('ui-state-active');
        $(td).children('a').addClass('ui-state-active');

        inst.minutes = newMinutes;
        this._updateSelectedValue(inst);

        inst._minutesClicked = true;
        if ((inst._hoursClicked) || (fromDoubleClick))
            $.timepicker._hideTimepicker();

    },

    _updateSelectedValue: function(inst) {
        if ((inst.hours < 0) || (inst.hours > 23))
            inst.hours = 12;
        if ((inst.minutes < 0) || (inst.minutes > 59))
            inst.minutes = 00;

        var h = inst.hours.toString();
        if (inst.hours < 10)
            h = '0' + h;

        var m = inst.minutes.toString();
        if (inst.minutes < 10)
            m = '0' + m;
        
        var newTime = h + this._get(inst, 'timeSeparator') + m;

        if (inst.input)
            inst.input.val(newTime);

		var onSelect = this._get(inst, 'onSelect');
		if (onSelect)
			onSelect.apply((inst.input ? inst.input[0] : null), [newTime, inst]);  // trigger custom callback

        return newTime;
    }


});

/* Invoke the timepicker functionality.
   @param  options  string - a command, optionally followed by additional parameters or
                    Object - settings for attaching new datepicker functionality
   @return  jQuery object */
$.fn.timepicker = function(options){

	/* Initialise the date picker. */
	if (!$.timepicker.initialized) {
		$(document).mousedown($.timepicker._checkExternalClick).
			find('body').append($.timepicker.tpDiv);
		$.timepicker.initialized = true;
	}

	var otherArgs = Array.prototype.slice.call(arguments, 1);
	if (typeof options == 'string' && (options == 'isDisabled' || options == 'getDate' || options == 'widget'))
		return $.timepicker['_' + options + 'Datepicker'].
			apply($.timepicker, [this[0]].concat(otherArgs));
	if (options == 'option' && arguments.length == 2 && typeof arguments[1] == 'string')
		return $.timepicker['_' + options + 'Datepicker'].
			apply($.timepicker, [this[0]].concat(otherArgs));
	return this.each(function() {
		typeof options == 'string' ?
			$.timepicker['_' + options + 'Datepicker'].
				apply($.timepicker, [this].concat(otherArgs)) :
			$.timepicker._attachTimepicker(this, options);
	});
};

/* jQuery extend now ignores nulls! */
function extendRemove(target, props) {
	$.extend(target, props);
	for (var name in props)
		if (props[name] == null || props[name] == undefined)
			target[name] = props[name];
	return target;
};

$.timepicker = new Timepicker(); // singleton instance
$.timepicker.initialized = false;
$.timepicker.uuid = new Date().getTime();
$.timepicker.version = "1.8.6";

// Workaround for #4055
// Add another global to avoid noConflict issues with inline event handlers
window['TP_jQuery_' + tpuuid] = $;

})(jQuery);

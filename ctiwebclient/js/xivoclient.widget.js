//
// XiVO webclient-widget
// Copyright (C) 2009  Proformatique <technique@proformatique.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

var xivoc_i18n = {};
function _xivoc_i18n(text) { if (xivoc_i18n[text] !== undefined) return xivoc_i18n[text]; else return "'" + text + "`nt"; }

var xivoc_i18n_lang = 'fr';
var xivoc_i18n_lang_possible = { fr: "french", en: "english" };



if (typeof Array.prototype.reduce !== "function")
  Array.prototype.reduce = function (fun,s) {
    var i=0,e=0;
    var a=s;

    for (i=0,e=this.length;i<e;i++) {
      a = fun(a,this[i],i,this);
    }

    return a;
  };

// @private method
// @argument text: string
// extract the number from a string
XiVOHelper.prototype.parse_num = function (text) {
    var i,e,s = "";

    for (i=0,e=text.length;i < e;i++) 
      if ((text[i] <= '9') && (text[i] >= '0'))
        s += text[i];

    return s;
};


// @public method
// @return DOM-element that self update
XiVOClient.prototype.spawn_widget = function (kind,decorator,hidout) {

  if (this.widget === undefined)
    this.widget = {};

  decorator = (decorator !== undefined ) ? decorator : function (c) { return c; };
  this.decorator = decorator; // a decorator is a function that make apear a widget in a window or ...
  this.hidout = hidout;
  var widget = new XiVOCWidget(this,kind,decorator,hideout);
  if (widget.have_decoration())
    widget.win = decorator(widget.get_dom(),widget.get_title(),widget.get_size(),widget.get_resizable(),widget.get_closable());

  this.widget[kind] = widget;

  if (this.widget.respawner !== undefined )
    this.widget.respawner.spawned_widget_list.push([widget,kind]);

  return widget;
};

// @private method
function XiVOCWidget(xivoclient_instance,kind_of_widget,decorator,hideout) {
  this.id = "xivoc-widget-" + this.unid;
  this.uid = this.unid;
  this.title = "unnamed window";
  this.to_watch = xivoclient_instance;
  this.refresh = true;
  this.widget_dom = undefined;
  this.interval = undefined;
  this.hideout = hideout;
  this.resizable = true;
  this.decoration = true;
  this.closable = "&nbsp;";
  this.size = { width: "440px", height: "200px" };
  this.limit = 1; // we must spawn only one instance of each kind of widget..

  if (typeof this.widget[kind_of_widget] === "object")
    this.widget[kind_of_widget].start(this);
  else
    alert("sigh, you tryed to spawn a widget that doesn't exist.. (" + kind_of_widget + ")");

  XiVOCWidget.prototype.unid = this.unid + 1;
};

XiVOCWidget.prototype.have_decoration = function () {
  return this.decoration;
};

XiVOCWidget.prototype.get_title = function () {
  return this.title;
};

XiVOCWidget.prototype.get_closable = function () {
  return this.closable;
};

XiVOCWidget.prototype.get_size = function () {
  return this.size;
};

XiVOCWidget.prototype.get_resizable = function () {
  return this.resizable;
};

XiVOCWidget.prototype.unid = 0;

XiVOCWidget.prototype.widget_status = function () {
};

XiVOCWidget.prototype.widget = {};

// display a login widget
XiVOCWidget.prototype.widget.login = {};

// @private method
XiVOCWidget.prototype.widget.login._ondc = function (us,code) {
  window.location.reload();
};

// @private method
XiVOCWidget.prototype.widget.login.start = function (us) {
  us.refresh = true;
  us.closable = false;
  us.resizable = false;
  us.size = { width: "420px", height: "141px" };
  us.widget_dom = us.widget.login.build(us);
  us.to_watch.register_disconnect_function(function (code) {  us.widget.login._ondc(us,code); });
  us.to_watch.spawn_widget('footer',us.to_watch.decorator);
};

// @private method
XiVOCWidget.prototype.widget.login.on_connect = function (us,connected) {
  if (connected) {
    us.request_everything(function (us) {
      var widget_list = [];
      var capawidget = { search:      "user_around",
                         identity:    "common_info",
                         dial:        "dial"       ,
                         history:     "history"    ,
                         directory:   "directory"  ,
                         features:    "service"    ,
                         conference:  null
                       };
      var i,e;

      capa = us.get_capaxlets();

      for (i=0,e=capa.length;i<e;i++) {
        capa[i] = capa[i].substr(0,capa[i].indexOf('-'));
        if ((capawidget[capa[i]] !== null) && (typeof capawidget[capa[i]] !== "undefined")) {
          widget_list.push([us.spawn_widget(capawidget[capa[i]],us.decorator),capawidget[capa[i]]]);
        }
      }

      var spawner = us.spawn_widget('respawner',us.decorator);
      spawner.widget_list = widget_list;
      spawner.spawned_widget_list = widget_list; // @bug, it should be cloned!!

      us.widget["login"].hideout(us.widget["login"].win,true);
      delete us.widget["login"];
    });
  } else
    alert('something has failed somewhere');
};

// @private method
XiVOCWidget.prototype.widget.login.rebuild = function (us) {
  // gentle cleaning
  for (var widget_name in us.to_watch.widget) {
    if (widget_name !== 'footer')
    us.to_watch.widget[widget_name].hideout(us.to_watch.widget[widget_name].win);
    if (typeof us.to_watch.widget[widget_name].interval !== "undefined") {
      clearInterval(us.to_watch.widget[widget_name].interval);
    }
    delete us.to_watch.widget[widget_name];
  };
  us.to_watch.spawn_widget("login",us.to_watch.decorator,us.to_watch.hideout);
};

// @private method
XiVOCWidget.prototype.widget.login.build = function (us) {
  us.title = _xivoc_i18n("authentication");
  var awidget = { div: { attr: { id: us.id, 'class': "login-widget" }, child: [
                  { div: { attr: { 'class': "login-widget-fields" }, child: [
                    { input: { attr: { type: "text", id: "login-widget-login-" + us.uid, value: _xivoc_i18n("your login"),
                                       'class': "login-widget-login" + " widget-field" + " widget-field-blured" },
                               ev: { focus: function () {
                                 XiVOH.delClass(this,"widget-field-blured");
                                 if (this.value === _xivoc_i18n("your login"))
                                   this.value = "";
                               }, blur: function () {
                                 XiVOH.addClass(this,"widget-field-blured");
                                 if (this.value === "") {
                                   this.value = _xivoc_i18n("your login");
                                 } else {
                                   XiVOH.delClass(this,"widget-field-blured");
                                 }
                               }}
                             }},
                    { input: { attr: { type: "text", id: "login-widget-password-" + us.uid, value: _xivoc_i18n("your password"),
                                       'class': "login-widget-password" + " widget-field" + " widget-field-blured" },
                               ev: { focus: function () {
                                 XiVOH.delClass(this,"widget-field-blured");
                                 this.setAttribute("type","password");
                                 if (this.value === _xivoc_i18n("your password"))
                                   this.value = "";
                               }, blur: function () {
                                 XiVOH.addClass(this,"widget-field-blured");
                                 if (this.value === "") {
                                   this.value = _xivoc_i18n("your password");
                                   this.setAttribute("type","text");
                                 } else {
                                   XiVOH.delClass(this,"widget-field-blured");
                                 }
                               }, keydown: function (event) {
                                 if (event.keyCode === 13) {
                                   XiVOH.addClass(document.getElementById("login-widget-connect-" + us.uid),"login-widget-connect-on-progress");
                                   XiVOH.addClass(document.getElementById("login-widget-loading-" + us.uid),
                                                                            "visible");
                                   us.to_watch.try_login({ user: document.getElementById("login-widget-login-" + us.uid).value,
                                                           pass: document.getElementById("login-widget-password-" + us.uid).value,
                                                           keep_alive: false,
                                                           state: 'available' }, us.widget.login.on_connect);
                                 }
                               }}
                    }},
                    { span: { attr: { 'class': "login-widget-language-label" },
                              text: _xivoc_i18n("language: ") }},
                    { select: { ev: { change: function () {
                                  xivoc_i18n = undefined;
                                  xivoc_i18n_lang = this.value;
                                  XiVOH.load_script('js/xivoc-i18n-' + this.value + '.js');
                                  XiVOH.call_me_asap(function () { return (typeof xivoc_i18n === "object"); },function () { 
                                    us.widget.login.rebuild(us);
                                  },100);
                                }}, attr: { 'class': "login-widget-language" + " widget-field" }, child: [
                    ]}},
                  ]}},
                  { input: { ev: { click: function () {
                                     XiVOH.addClass(document.getElementById("login-widget-loading-" + us.uid),
                                                                            "visible");
                                     XiVOH.addClass(document.getElementById("login-widget-connect-" + us.uid),
                                                                            "login-widget-connect-on-progress");
                                     us.to_watch.try_login({ user: document.getElementById("login-widget-login-" + us.uid).value,
                                                             pass: document.getElementById("login-widget-password-" + us.uid).value,
                                                             keep_alive: false,
                                                             state: 'available' }, us.widget.login.on_connect);
                           }}, attr: { id: "login-widget-connect-" + us.uid,
                                       'class': "login-widget-connect" + " widget-button",
                                       type: "button", value: _xivoc_i18n("connection")
                  }}},
                  { div: { attr: { id: "login-widget-loading-" + us.uid,
                                   'class': "login-widget-loading" }
                  }}
                ]}};

            
            for (var lang in xivoc_i18n_lang_possible) {
              if (lang === xivoc_i18n_lang) {
                awidget.div.child[0].div.child[3].select.child.push({ option: { attr: { value: lang, selected: "selected" },
                                                                                text: _xivoc_i18n(lang) }});
              } else {
                awidget.div.child[0].div.child[3].select.child.push({ option: { attr: { value: lang },
                                                                                text: _xivoc_i18n(lang) }});
              }
            }
                      
  return XiVOH.build_from_array(awidget);
};


// display the common info
XiVOCWidget.prototype.widget.common_info = {};
// @private method
XiVOCWidget.prototype.widget.common_info.start = function (us) {
  us.limit = 1;
  us.resizable = false;
  us.refresh = true;
  us.size = { width: "600px", height: "100px" };
  us.widget_dom = us.widget.common_info.build(us);
  us.interval = setInterval(function () {
                  if (us.refresh) { 
                    var refresh = us.widget.common_info.build(us);
                    us.widget_dom.parentNode.replaceChild(refresh,us.widget_dom);
                    us.widget_dom = refresh;
                  }
                },1000);
};

// @private method
XiVOCWidget.prototype.widget.common_info.build = function (us) {
  us.refresh = true;
  us.title = _xivoc_i18n("your status");

  var client = us.to_watch;

  var user_detail = client.get_user_detail();
  var fullname = user_detail.fullname;
  var phonenum = user_detail.phonenum;
  var phone_list =  client.get_phone_list();

  var s = client.get_allowed_state();
  var color = s.state.color;
  var state_long_name = s.state.color;

  var select_state = { select: { attr: { 'class': "widget-field" },
                                 ev: { change: function () {
                                   client.change_state(this.value);
                                 },    focus: function () {
                                       us.refresh = false;
                                 }}, child: [
                     ]}}

  var awidget = { div: { attr: { id: us.id, 'class': "common-info-widget" }, child: [
                  { ul: { child: [
                    { li: { child: [
                      { div: { attr: { 'class': "common-info-widget-name" },
                               text: fullname }},
                      { div: { attr: { 'class': "common-info-widget-phonenumber" },
                               text: phonenum }},
                      { div: { attr: { 'class': "common-info-widget-state", style: "color:" + color }, child: [
                        select_state
                      ]}}
                    ]}},
                    { li: { child: [
                      { div: { attr: { 'class': "common-info-widget-phone" }}}
                    ]}},
                    { li: { child: [
                      { div: { attr: { 'class': "common-info-widget-phonestatus" },
                               text: phone_list.xivo[user_detail.techlist[0]].hintstatus.longname }}
                    ]}},
                  ]}}
                ]}};


  if (user_detail.voicemailnum !== "") {
    var ifnew = (parseInt(user_detail.mwi[2])> 0 ) ? "-new" : "" ;
    var li_icon = { li: { child: [
                    { div: { attr: { 'class': "common-info-widget-voicemail-icon" + ifnew }}}
                  ]}};

    awidget.div.child[0].ul.child.push(li_icon);
    var li_info = { li: { child: [
                    { div: { attr: { 'class': "common-info-widget-voicemailnum" },
                             text: _xivoc_i18n("VoiceMailBox") + user_detail.voicemailnum }},
                    { div: { attr: { 'class': "common-info-widget-newmessage" },
                             text: _xivoc_i18n("new message") + user_detail.mwi[2] }},
                    { div: { attr: { 'class': "common-info-widget-oldmessage" },
                             text: _xivoc_i18n("old message") + user_detail.mwi[1] }},
                  ]}}
    awidget.div.child[0].ul.child.push(li_info);
  } else
    us.size = { width: "350px", height: "100px" };



  for (var state_name in s.allowed) {
    if (state_name == client.get_state())
      select_state.select.child.push(
        { option: { attr: { value: state_name, style: "background-color:" + s.names[state_name].color,
          selected: 'selected' },
          text: s.names[state_name].longname }});
    else
      select_state.select.child.push(
        { option: { attr: { value: state_name, style: "background-color:" + s.names[state_name].color },
          text: s.names[state_name].longname }});
  }

  return XiVOH.build_from_array(awidget);
};

// show the users around us
XiVOCWidget.prototype.widget.user_around = {};
// @private method
XiVOCWidget.prototype.widget.user_around.start = function (us) {

  us.title = _xivoc_i18n('user around');
  us.match = "";

  us.widget.user_around.build(us);
  us.widget_dom = us.widget.user_around.build(us);


  us.interval = setInterval(function () {
                              us.widget.user_around.rebuild(us);
                            },333);
};

XiVOCWidget.prototype.widget.user_around.rebuild = function (us) {
  var client = us.to_watch;
  var user_list = client.get_user_list();
  var phone_list = client.get_phone_list();

  for (var i in user_list) {
    if (user_list[i].phonenum !== null) {
      var old = document.getElementById("user-around-widget-user-" + us.uid + "-" + i);
      var recent = XiVOH.build_from_array(us.widget.user_around.build_user(us,i,user_list,phone_list));

      old.parentNode.replaceChild(recent,old);

      if (!((user_list[i].fullname + user_list[i].phonenum).toLowerCase().indexOf(us.match) !== -1 )) {
        recent.setAttribute("class","user-around-widget-user-unmatched");
      }
    }
  }

};

// @private method
XiVOCWidget.prototype.widget.user_around.build_user = function (us,i,user_list,phone_list) {
  var client = us.to_watch;

  var user =  { li: { attr: { 'class': 'user-around-widget-user', id: "user-around-widget-user-" + us.uid + "-" + i }, child: [
                { div: { attr: { 'class': "user-around-widget-fullname" }, text: user_list[i].fullname + " <" + user_list[i].phonenum + "> " }},
                { ul: { attr: { 'class': "user-around-widget-status-list" }, child: [] }}
              ]}};
  
  if (user_list[i].user !== "") 
    user.li.child[1].ul.child.push({ li: { attr: { 'class': "user-around-widget-icons" + " user-around-widget-state",
                                                    style: "background-color:" + user_list[i].statedetails.color,
                                                    title: user_list[i].statedetails.longname }}});
  
  
  if ((typeof user_list[i].techlist[0] === "string") &&
      (typeof phone_list.xivo[user_list[i].techlist[0]] === "object") &&
      (typeof phone_list.xivo[user_list[i].techlist[0]].hintstatus === "object"))
    user.li.child[1].ul.child.push({ li: { attr: { 'class': "user-around-widget-icons" + " user-around-widget-phone-status",
                                                   xivo_uid: user_list[i].xivo_userid,
                                                   style: "background-color:" + phone_list.xivo[user_list[i].techlist[0]].hintstatus.color,
                                                   title: phone_list.xivo[user_list[i].techlist[0]].hintstatus.longname },
                                           ev: { click: function () { client.make_call('user:xivo/' + this.getAttribute("xivo_uid")); }}
                                                   
                                                   }});
  else 
    user.li.attr.style = "display:none"; // don't display user without phone.
  
  if ((typeof user_list[i].mobilenum === "string") && (user_list[i].mobilenum !== "")) 
    user.li.child[1].ul.child.push({ li: { attr: { 'class': "user-around-widget-icons" + " user-around-widget-mobilephone",
                                                    num: user_list[i].mobilenum, title: "call " + user_list[i].mobilenum }, 
                                           ev: { click: function () { client.make_call('ext:' + this.getAttribute("num")); }}
                                   }});

  return user;
  
};

// @private method
XiVOCWidget.prototype.widget.user_around.build = function (us) {
  var client = us.to_watch;
  var user_list = client.get_user_list();
  var phone_list = client.get_phone_list();

  var ulist = { ul: { attr: { 'class': "user-around-widget-userlist" }, child: [] }}

  var ulist_filter = 
                { table: { child: [
                  { tr: { child: [
                    { td: { child: [
                       { span: { attr: { type: 'text', 'class': "user-around-widget-display-pattern-label" },
                         text: _xivoc_i18n('name or number to search') }}
                    ]}},
                    { td: { child: [
                       { input: { attr: { type: 'text', 'class': "user-around-widget-display-pattern" + " widget-field",
                                          id: "user-around-widget-display-pattern" + us.uid },
                                  ev: { change: function () {
                                    us.match = document.getElementById(this.getAttribute("id")).value.toLowerCase();
                                  }, keyup: function () {
                                    us.match = document.getElementById(this.getAttribute("id")).value.toLowerCase();
                                  }}
                       }},
                    ]}}
                  ]}}
                ]}};


  var awidget = { div: { attr: { id: us.id, 'class': "user-around-widget" }, child: [
                      ulist_filter,
                      ulist
                ]}};

  for (var i in user_list) {
    if ((user_list[i].context === "default") && (user_list[i].phonenum !== null)) {
      ulist.ul.child.push(us.widget.user_around.build_user(us,i,user_list,phone_list));
    }
  }


  return XiVOH.build_from_array(awidget);
};


XiVOCWidget.prototype.widget.respawner = {}

// @private method
XiVOCWidget.prototype.widget.respawner.start = function (us) {

  us.widget_list = [];
  us.spawned_widget_list = [];
  us.title = _xivoc_i18n('respawner');
  us.resizable = false;
  us.closable = false;
  us.decoration = false;
  us.refresh = true;

  us.widget_dom = us.widget.respawner.build(us);

  XiVOH.call_me_asap(function () { return document.getElementById(us.id); },
                     function () {
                       us.widget.respawner.update(us);
                     },1000);

  (document.getElementsByTagName("body"))[0].appendChild(us.get_dom());
};

// @private method
XiVOCWidget.prototype.widget.respawner.make_widget_list = function (us) {
  var widget_list = { ul: { attr: { id: "respawner-widget-list-" + us.uid }, child: [
                    ]}};

  var i,e;
  for (i=0,e=us.widget_list.length;i<e;i++) {
      var widget_name = us.widget_list[i][1];
      widget_list.ul.child.push({ li: { attr: { 'class': "respawner-widget-spawn"}, child: [
                                  { a: { attr: { href: "#", widget: widget_name }, 
                                         text: _xivoc_i18n(widget_name),
                                         ev: { click: function () {
                                          var i,e,widget_name=this.getAttribute("widget"),dontspawn=false;
                                          //@weird if we stop to use boxy we will have to rewrite this part
                                          for (i=0,e=us.spawned_widget_list.length;i<e;i++) {
                                            if ((us.spawned_widget_list[i] !== undefined ) &&
                                                (us.spawned_widget_list[i][0].win.visible === true) &&
                                                (us.spawned_widget_list[i][1] === widget_name) &&
                                                (us.spawned_widget_list[i][0].limit !== undefined)) {
                                              dontspawn = true;
                                              break;
                                            } else if ((us.spawned_widget_list[i] !== undefined) &&
                                                       (us.spawned_widget_list[i][0].win.visible === false))
                                              delete us.spawned_widget_list[i];

                                          }
                                          
                                          // to keep the array size small
                                          us.spawned_widget_list = us.spawned_widget_list.reduce( function (a,b,c) {
                                                                                                    if (b !== undefined) {
                                                                                                      return a.concat([b]);
                                                                                                    } else
                                                                                                      return a;
                                                                                                  },[]);


                                          if (!dontspawn)
                                            us.to_watch.spawn_widget(widget_name, us.to_watch.decorator,us.to_watch.hideout);
                                        }}
                                  }}
                                ]}});
  }

  return widget_list;
};

// @private method
XiVOCWidget.prototype.widget.respawner.update = function (us) {
  var old_one = document.getElementById('respawner-widget-list-' + us.uid);
  var refresh = us.widget.respawner.make_widget_list(us);

  old_one.parentNode.replaceChild(XiVOH.build_from_array(refresh),old_one);
};

// @private method
XiVOCWidget.prototype.widget.respawner.build = function (us) {
  var widget_list = us.widget.respawner.make_widget_list(us);

  var awidget = { div: { attr: { id: us.id, 'class': "respawner-widget" },
                          child: [
                        { span: { attr: { 'class': "respawner-widget-root-menu-left" }}},
                        { ul: { attr: { 'class': "respawner-widget-root-menu" }, child: [
                          { li: { attr: { 'class': "respawner-widget-menu-spawn" }, child: [
                            { a: { attr: { href: "#"}, text: _xivoc_i18n("existing widget") }},
                            widget_list,
                          ]}},
                          { li: { attr: { 'class': "respawner-widget-menu-separator" }}},
                          { li: { attr: { 'class': "respawner-widget-menu-label" },
                                  text: us.to_watch.get_user_detail().fullname }},
                        ]}},
                        { a: { attr: { 'class': "respawner-widget-logout", href: "#" },
                               ev: { click: function () { window.location.reload(); } }}}
                ]}};

  return XiVOH.build_from_array(awidget);
};

// @public method
XiVOCWidget.prototype.get_dom = function () { return  this.widget_dom; };

XiVOH.addOnloadEvent(function () {
  XiVOH.load_script('js/xivoc-i18n-' + xivoc_i18n_lang + '.js',function() { 
    XiVOCWidget.prototype.loaded = true;
  });
},false);


// display the directory widget
XiVOCWidget.prototype.widget.directory = {};
// @private method
XiVOCWidget.prototype.widget.directory.start = function (us) {

  us.title = _xivoc_i18n('directory search');

  us.widget_dom = us.widget.directory.build(us);

};

// @private method
XiVOCWidget.prototype.widget.directory.render_result = function (id,pattern,client) {
  var result = client.get_directory_search(pattern);
  var headers = result.headers;
  var list = result.list;
  var i,i2,e,e2,column_count;

  var awidget = { table: { attr: { 'class': "directory-search-widget-table" + " widget-table",
                                   border: 0, cellpadding: 0, cellspacing: 0 }, child: [
                  { thead: { child: [
                    { tr: { child: [
                    ]}}
                  ]}},
                  { tbody: { child: [
                  ]}}
                ]}};


  for (i=0,column_count=headers.length;i < column_count;i++) {
    awidget.table.child[0].thead.child[0].tr.child.push({ td: { text: _xivoc_i18n(headers[i]) }});
  }

  for (i=0, e=list.length;i < e;i++) {
    var row;

    if (i%2)
      row = { tr: { attr: { 'class': "widget-odd-row" }, child: [] }};
    else
      row = { tr: { child: [] }};

    for (i2=0, e2=headers.length;i2 < e2;i2++) {
      if ((list[i][i2].length !== 0) && (headers[i2] === "Numéro")) {
        row.tr.child.push({ td: { child: [
                { a: { attr: { href: "#", num: XiVOH.parse_num(list[i][i2]), 'class': "click-to-phone-name" }, text: "",
                       ev: { click: function () { client.make_call('ext:' + this.getAttribute("num")); } }}},
                { span: { attr: { 'class': "phone-name" }, text: list[i][i2] }}
                ]}});
      } else if ((list[i][i2].length !== 0) && (headers[i2] === "E-mail")) {
        row.tr.child.push({ td: { child: [
                { a: { attr: { href: "mailto:" + list[i][i2]}, text: list[i][i2] }}
                ]}});
      } else {
        row.tr.child.push({ td: { text: list[i][i2] }});
      }
    }
    awidget.table.child[1].tbody.child.push(row);
  }

  var result_rendered = XiVOH.build_from_array(awidget);

  var render_on = document.getElementById('directory-search-widget-renderon-' + id);
  if ( render_on.firstChild  !== null )
    render_on.replaceChild(result_rendered,render_on.firstChild);
  else 
    document.getElementById('directory-search-widget-renderon-' + id).appendChild(result_rendered);
};

// @private method
XiVOCWidget.prototype.widget.directory.build = function (us) {
  var client = us.to_watch;

  var awidget = { div: { attr: { id: us.id, 'class': "directory-search-widget" }, child: [
                  { table: { child: [
                    { tr: { child: [
                      { td: { child: [
                        { input: { attr: { id: "directory-search-widget-pattern-" + us.uid,
                                           'class': "directory-search-widget-pattern" + " widget-field" },
                                   ev: { change: function () {
                                     var pattern = document.getElementById('directory-search-widget-pattern-' + us.uid).value;
                                     if (pattern !== "") {
                                       client.request_directory_search(pattern,true,function (usc) {
                                                                        us.widget.directory.render_result(us.uid,pattern,usc);
                                                                       });
                                     }
                                   }
                        }}},
                      ]}},
                      { td: { attr: { style: "width: 80px;padding-left: 10px" }, child: [
                        { input: { attr: { 'class': "directory-search-widget-button" + " widget-button", type: "button", value: _xivoc_i18n("search") },
                                   ev: { click: function () {
                                     var pattern = document.getElementById('directory-search-widget-pattern-' + us.uid).value;
                                     if (pattern !== "") {
                                       client.request_directory_search(pattern,true,function (usc) {
                                                                        us.widget.directory.render_result(us.uid,pattern,usc);
                                                                       });
                                     }
                                   }}
                        }}
                      ]}}
                    ]}},
                    { tr: { child: [
                      { td: { attr: { colspan: 2 }, child: [
                        { div: { attr: { 'class': "directory-search-widget-renderon", id: "directory-search-widget-renderon-" + us.uid }}}
                      ]}}
                    ]}}
                  ]}}
                ]}};

  return XiVOH.build_from_array(awidget);
};

// show the service widget
XiVOCWidget.prototype.widget.service = {};
// @private method
XiVOCWidget.prototype.widget.service.start = function (us) {

  us.limit = 1; // spawn only one widget of this kind

  us.size = { width: "270px", height: "222px" };
  us.title = _xivoc_i18n('services');
  us.resizable = false;

  us.widget.service.build(us);
  us.widget_dom = us.widget.service.build(us);

  XiVOH.call_me_asap(function () { return document.getElementById(us.id); },
                     function () {
                       us.widget.service.update(us);
                       us.interval = setInterval(function () { if (us.refresh) us.widget.service.update(us); },250);
                     },500);
};

// @private method
XiVOCWidget.prototype.widget.service.update = function (us) {
  var client = us.to_watch;
  document.getElementById("service-widget-call-voicemail-" + us.uid).checked = client.get_feature('enablevoicemail').enabled;
  document.getElementById("service-widget-call-recording-" + us.uid).checked = client.get_feature('callrecord').enabled;
  document.getElementById("service-widget-call-filtering-" + us.uid).checked = client.get_feature('incallfilter').enabled;
  document.getElementById("service-widget-do-not-disturb-" + us.uid).checked = client.get_feature('enablednd').enabled;

  document.getElementById("service-widget-transfert-when-no-reply-" + us.uid).checked = client.get_feature('enablerna').enabled;
  document.getElementById("service-widget-transfert-when-no-reply-field-" + us.uid).value = client.get_feature('enablerna')['number'];
  if (document.getElementById("service-widget-transfert-when-no-reply-field-" + us.uid).value !== "") 
    document.getElementById("service-widget-transfert-when-no-reply-" + us.uid).removeAttribute('disabled');
    

  document.getElementById("service-widget-transfert-on-busy-" + us.uid).checked = client.get_feature('enablebusy').enabled;
  document.getElementById("service-widget-transfert-on-busy-field-" + us.uid).value = client.get_feature('enablebusy')['number'];
  if (document.getElementById("service-widget-transfert-on-busy-field-" + us.uid).value !== "")
    document.getElementById("service-widget-transfert-on-busy-" + us.uid).removeAttribute('disabled');


  document.getElementById("service-widget-transfert-unconditional-" + us.uid).checked = client.get_feature('enableunc').enabled;
  document.getElementById("service-widget-transfert-unconditional-field-" + us.uid).value = client.get_feature('enableunc')['number'];
  if (document.getElementById("service-widget-transfert-unconditional-field-" + us.uid).value !== "")
    document.getElementById("service-widget-transfert-unconditional-" + us.uid).removeAttribute('disabled');
};

XiVOCWidget.prototype.widget.service.build = function (us) {
  var client = us.to_watch;

  var awidget = { div: { attr: { id: us.id, 'class': "service-widget" },
                         ev: { mouseover: function () { us.refresh = false; },
                               mouseout: function () { us.refresh = true;
                         }}, child: [
                  { ul: {  attr: { id: us.id, 'class': "service-widget" }, child: [
                    { li: { child: [
                      { input: { attr: { type: "checkbox", id: "service-widget-call-voicemail-" + us.uid,
                                        'class': "service-widget-call-voicemail" + " widget-checkbox" },
                                 ev: { change: function () {
                                        client.request_featuresput({ 'function': "enablevoicemail",
                                                                      value: this.checked ? '1' : '0',
                                                                      userid: 'xivo/' + client.get_user_id()});
                                    }}
                                }},
                      { label: { attr: {'for': "service-widget-call-voicemail-" + us.uid },
                                text: _xivoc_i18n("voice mail") }}
                      ]}},
                    { li: { child: [
                      { input: { attr: { type: "checkbox", id: "service-widget-call-recording-" + us.uid,
                                        'class': "service-widget-call-recording" + " widget-checkbox" },
                                 ev: { change: function () {
                                        client.request_featuresput({ 'function': "callrecord",
                                                                      value: this.checked ? '1' : '0',
                                                                      userid: 'xivo/' + client.get_user_id()});
                                    }}
                                }},
                      { label: { attr: {'for': "service-widget-call-recording-" + us.uid},
                                 text: _xivoc_i18n("call recording") }}
                      ]}},
                    { li: { child: [
                      { input: { attr: { type: "checkbox", id: "service-widget-call-filtering-" + us.uid,
                                        'class': "service-widget-call-filtering" + " widget-checkbox" },
                                 ev: { change: function () {
                                        client.request_featuresput({ 'function': "incallfilter",
                                                                      value: this.checked ? '1' : '0',
                                                                      userid: 'xivo/' + client.get_user_id()});
                                    }}
                                  }},
                      { label: { attr: {'for': "service-widget-call-filtering-" + us.uid },
                                 text: _xivoc_i18n("call filtering") }}
                    ]}},
                    { li: { child: [
                      { input: { attr: { type: "checkbox", id: "service-widget-do-not-disturb-" + us.uid,
                                        'class': "service-widget-do-not-disturb" + " widget-checkbox" },
                                 ev: { change: function () {
                                        client.request_featuresput({ 'function': "enablednd",
                                                                      value: this.checked ? '1' : '0',
                                                                      userid: 'xivo/' + client.get_user_id()});
                                    }}
                                  }},
                      { label: { attr: {'for': "service-widget-do-not-disturb-" + us.uid },
                                 text: _xivoc_i18n("do not disturb") }}
                    ]}},
                  ]}},
                ]}};

  var forward_on_no_reply = { ul: { 'class': "service-widget-transfert-when-no-reply" ,child: [
        { li: { child: [
          { input: { attr: { type: "checkbox", id: "service-widget-transfert-when-no-reply-" + us.uid,
                            'class': "service-widget-transfert-when-no-reply" + " widget-checkbox",
                            disabled: "disabled" },
                     ev: { change: function () {
                            client.request_featuresput({ 'function': "enablerna",
                                                          value: this.checked ? "1" : "0",
                                                          userid: 'xivo/' + client.get_user_id(),
                                                          destination: document.getElementById("service-widget-transfert-when-no-reply-field-" + us.uid).value });
                        }}
          }},
          { label: { attr: {'for': "service-widget-transfert-when-no-reply-" + us.uid },
                     text: _xivoc_i18n("forward on no reply") }}
        ]}},
        { li: { child: [
          { span: { text: _xivoc_i18n("destination:") }},
          { input: { attr: { type: "text", id: "service-widget-transfert-when-no-reply-field-" + us.uid,
                      'class': "widget-field" },
                     ev: { keyup: function () {
                         var servicet = document.getElementById("service-widget-transfert-when-no-reply-" + us.uid);
                         var service = document.getElementById("service-widget-transfert-when-no-reply-field-" + us.uid);

                         if (service.value === "")
                           servicet.setAttribute('disabled','disabled');
                         else
                           servicet.removeAttribute('disabled');

                         }, change: function () {
                         document.getElementById("service-widget-transfert-when-no-reply-" + us.uid).checked = false;
                     }}
          }}
        ]}}
      ]}};

  awidget.div.child.push(forward_on_no_reply);

  var forward_on_busy = { ul: { 'class': "service-widget-transfert-on-busy" ,child: [
        { li: { child: [
          { input: { attr: { type: "checkbox", id: "service-widget-transfert-on-busy-" + us.uid,
                            'class': "service-widget-transfert-on-busy" + " widget-checkbox",
                            disabled: "disabled" },
                     ev: { change: function () {
                            client.request_featuresput({ 'function': "enablebusy",
                                                          value: this.checked ? "1" : "0",
                                                          userid: 'xivo/' + client.get_user_id(),
                                                          destination: document.getElementById("service-widget-transfert-on-busy-field-" + us.uid).value });
                        }}
          }},
          { label: { attr: {'for': "service-widget-transfert-on-busy-" + us.uid },
                     text: _xivoc_i18n("forward on busy") }}
        ]}},
        { li: { child: [
          { span: { text: _xivoc_i18n("destination:") }},
          { input: { attr: { type: "text", id: "service-widget-transfert-on-busy-field-" + us.uid,
                      'class': "widget-field" },
                     ev: { keyup: function () {
                         var servicet = document.getElementById("service-widget-transfert-on-busy-" + us.uid);
                         var service = document.getElementById("service-widget-transfert-on-busy-field-" + us.uid);

                         if (service.value === "")
                           servicet.setAttribute('disabled','disabled');
                         else
                           servicet.removeAttribute('disabled');

                         }, change: function () {
                         document.getElementById("service-widget-transfert-on-busy-" + us.uid).checked = false;
                     }}
          }}
        ]}}
      ]}};

  awidget.div.child.push(forward_on_busy);

  var forward_unconditional = { ul: { 'class': "service-widget-transfert-unconditional" ,child: [
        { li: { child: [
          { input: { attr: { type: "checkbox", id: "service-widget-transfert-unconditional-" + us.uid,
                            'class': "service-widget-transfert-unconditional" + " widget-checkbox",
                            disabled: "disabled" },
                     ev: { change: function () {
                            client.request_featuresput({ 'function': "enableunc",
                                                          value: this.checked ? "1" : "0",
                                                          userid: 'xivo/' + client.get_user_id(),
                                                          destination: document.getElementById("service-widget-transfert-unconditional-field-" + us.uid).value });
                        }}
          }},
          { label: { attr: {'for': "service-widget-transfert-unconditional-" + us.uid },
                     text: _xivoc_i18n("unconditional forward") }}
        ]}},
        { li: { child: [
          { span: { text: _xivoc_i18n("destination:") }},
          { input: { attr: { type: "text", id: "service-widget-transfert-unconditional-field-" + us.uid,
                      'class': "widget-field" },
                     ev: { keyup: function () {
                         var servicet = document.getElementById("service-widget-transfert-unconditional-" + us.uid);
                         var service = document.getElementById("service-widget-transfert-unconditional-field-" + us.uid);
                         
                         if (service.value === "")
                           servicet.setAttribute('disabled','disabled');
                         else 
                           servicet.removeAttribute('disabled');

                         }, change: function () {
                         document.getElementById("service-widget-transfert-unconditional-" + us.uid).checked = false;
                     }}
          }}
        ]}}
      ]}};

  awidget.div.child.push(forward_unconditional);

  return XiVOH.build_from_array(awidget);
};

XiVOCWidget.prototype.widget.dial = {}

XiVOCWidget.prototype.widget.dial.start = function (us) {
  us.size = { width: "270px", height: "55px" };
  us.title = _xivoc_i18n('dial');
  us.resizable = false;

  us.widget_dom = us.widget.dial.build(us);
};

XiVOCWidget.prototype.widget.dial.build = function (us) {
  var client = us.to_watch;

  var awidget = { div: { attr: { id: us.id, 'class': "dial-widget" }, child: [
                  { input: { attr: { type: "text", id: "dial-widget-field-" + us.uid, 'class': "widget-field" },
                             ev: { keydown: function (event) {
                               if (event.keyCode === 13) {
                                 client.make_call('ext:' + document.getElementById("dial-widget-field-" + us.uid ).value ); 
                               }
                             }}
                  }},
                  { input: { attr: { type: "button", value: _xivoc_i18n("dial"), 'class': "widget-button" },
                    ev: { click: function () {
                      client.make_call('ext:' + document.getElementById("dial-widget-field-" + us.uid ).value ); 
                    }}}}
                ]}};

  return XiVOH.build_from_array(awidget);
};

XiVOCWidget.prototype.widget.history = {}

XiVOCWidget.prototype.widget.history.start = function (us) {
  us.size = { width: "420px", height: "55px" };
  us.title = _xivoc_i18n('history');
  us.resizable = true;

  us.widget_dom = us.widget.history.build(us);

};

XiVOCWidget.prototype.widget.history.format_call_length = function (sec) {
  var s = sec % 60;
  var m = ( ( sec - s ) / 60 ) % 60 ;
  var h = ( sec - ( m  * 60 ) - s  ) / ( 60 * 60 ) ;

  var t = "";
  if ( h != 0) t += h + _xivoc_i18n("hou.");
  if ( m != 0) t += m + _xivoc_i18n("min.");
  if ( s != 0) t += s + _xivoc_i18n("sec.");

  if (t === "") t = '-';

  return t;
};

XiVOCWidget.prototype.widget.history.render = function (mode,us) {
  var call_list = us.to_watch.get_history(mode);
  var i,e;
  var client = us.to_watch;


  var render_on = document.getElementById('history-widget-renderon-' + us.uid);
  var render = { table: { attr: { 'class': "widget-table" + " history-widget-table" }, child: [
                 { thead: { child: [
                   { tr: { child: [
                     { td: { text: _xivoc_i18n('timestamp') }},
                     { td: { text: _xivoc_i18n('caller') }},
                     { td: { text: _xivoc_i18n('length of call') }},
                   ]}}
                 ]}},
                 { tbody: { child: [
                 ]}}
               ]}};

  for (i=0,e=call_list.length;i<e;i++) {
    var row;

    if (i%2)
      row = { tr: { attr: { 'class': "widget-odd-row" }, child: [] }};
    else
      row = { tr: { child: [] }};

    if ( XiVOH.parse_num(call_list[i].fullname) !== "" )
      row.tr.child.push({ td: { attr: {'class': "history-widget-ts" },
                                text: call_list[i].ts }},
                        { td: { attr: {'class': "history-widget-fullname" }, child: [
                          { a: { attr: { href: "#", num: XiVOH.parse_num(call_list[i].fullname),
                                        'class': "click-to-phone-name" },
                            ev: { click: function () { client.make_call('ext:' + this.getAttribute("num")); } }}},
                          { span: { attr: { 'class': "phone-name" }, text: call_list[i].fullname }}
                        ]}},
                        { td: { attr: { 'class': "history-widget-duration" },
                                text: us.widget.history.format_call_length(call_list[i].duration) }});
    else
      row.tr.child.push({ td: { attr: {'class': "history-widget-ts" },
                                text: call_list[i].ts }},
                        { td: { attr: {'class': "history-widget-fullname" }, child: [
                          { span: { attr: { 'class': "phone-name" }, text: call_list[i].fullname }}
                        ]}},
                        { td: { attr: { 'class': "history-widget-duration" },
                                text: us.widget.history.format_call_length(call_list[i].duration) }});

    render.table.child[1].tbody.child.push(row);
  }

  if (render_on.firstChild !== null )
    render_on.replaceChild(XiVOH.build_from_array(render),render_on.firstChild);
  else
    render_on.appendChild(XiVOH.build_from_array(render));

};

XiVOCWidget.prototype.widget.history.build = function (us) {
  var client = us.to_watch;

  var awidget = { div: { attr: { id: us.id, 'class': "history-widget" }, child: [
                  { ul: { child: [
                    { li: { attr: { 'class': "history-widget-command-container" }, child: [
                      { ul: { child: [
                        { li: { child: [
                          { input: { attr: { type: 'radio',
                                             name: "history-widget-radiobutton",
                                             'class': "history-widget-radiobutton",
                                             id: "history-widget-radiobutton-made-" + us.uid },
                                     ev: { change: function () {
                                        us.to_watch.request_history(true,0,function (_Xivoc) {
                                          us.widget.history.render(0,us);
                                        });
                                     }}
                          }},
                          { label: { attr: { 'for': "history-widget-radiobutton-made-" + us.uid,
                                             title: _xivoc_i18n("made"),
                                             'class': "history-widget-radiobutton-made-label" },
                                     text: _xivoc_i18n("made") }}
                        ]}},
                        { li: { child: [
                          { input: { attr: { type: 'radio',
                                             name: "history-widget-radiobutton",
                                             'class': "history-widget-radiobutton",
                                             id: "history-widget-radiobutton-received-" + us.uid },
                                     ev: { change: function () {
                                        us.to_watch.request_history(true,1,function (_Xivoc) {
                                          us.widget.history.render(1,us);
                                        });
                                     }}
                          }},
                          { label: { attr: { 'for': "history-widget-radiobutton-received-" + us.uid ,
                                             title: _xivoc_i18n("received"),
                                             'class': "history-widget-radiobutton-received-label" },
                                     text: _xivoc_i18n("received") }}
                        ]}},
                        { li: { child: [
                          { input: { attr: { type: 'radio',
                                             name: "history-widget-radiobutton",
                                             'class': "history-widget-radiobutton",
                                             id: "history-widget-radiobutton-missed-" + us.uid },
                                     ev: { change: function () {
                                        us.to_watch.request_history(true,2,function (_Xivoc) {
                                          us.widget.history.render(2,us);
                                        });
                                     }}
                          }},
                          { label: { attr: { 'for': "history-widget-radiobutton-missed-" + us.uid,
                                             title: _xivoc_i18n("missed"),
                                             'class': "history-widget-radiobutton-missed-label" },
                                     text: _xivoc_i18n("missed") }}
                        ]}},
                      ]}},
                    ]}},
                    { li: { attr: { id: 'history-widget-renderon-' + us.uid,
                            'class': 'history-widget-renderon' }, child: [
                    ]}},
                  ]}},
                ]}};

  return XiVOH.build_from_array(awidget);
}

XiVOCWidget.prototype.widget.footer = {}

XiVOCWidget.prototype.widget.footer.start = function (us) {
  us.resizable = false;
  us.closable = false;
  us.decoration = false;

  us.widget_dom = us.widget.footer.build(us);

  old = document.getElementById("version-copyright");

  if (old !== null) {
    old.parentNode.replaceChild(us.widget_dom,old);
  } else
    (document.getElementsByTagName("body"))[0].appendChild(us.widget_dom);
};

XiVOCWidget.prototype.widget.footer.build = function (us) {
  var awidget = { h6: { attr: { id: "version-copyright" },
                        child: [
                  { span: { text: _xivoc_i18n("FOR_VERSION_1") }},
                  { a: { attr: { href: _xivoc_i18n("XIVO_URI"), title: "XIVO", target: "_blank" }, text: _xivoc_i18n("XIVO_URI_TEXT") }},
                  { span: { text: _xivoc_i18n("FOR_VERSION_2") }},
                  { a: { attr: { href: _xivoc_i18n("COMPANY_URI"), title: "XIVO", target: "_blank" }, text: _xivoc_i18n("COMPANY_URI_TEXT") }}
                ]}};

  return XiVOH.build_from_array(awidget);
};

XiVOClient.prototype.loaded = false;

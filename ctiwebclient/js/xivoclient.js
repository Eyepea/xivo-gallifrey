//
// XiVO webclient
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
//
// @class
function XiVOHelper() {}

var XiVOH = new XiVOHelper();


// @public method
// @argument ev: event
// @argument fnc: function
// @argument on: elem on wich we add the event listener
XiVOHelper.prototype.addEventListener = function (ev,fnc,on) {
  if (typeof window.addEventListener !== "undefined") {
    on.addEventListener(ev,fnc,false);
  } else if (typeof window.attachEvent !== "undefined") {
    on.attachEvent("on" + ev,fnc);
  } else {
    return 1;
  }

  return 0;
};

// @public method
// @argument fnc: function
// chunck found there http://www.tek-tips.com/faqs.cfm?fid=4862
XiVOHelper.prototype.addOnloadEvent = function (fnc) {
  if (XiVOH.addEventListener("load",fnc,window)) {
    if (window.onload !== undefined) {
      var oldOnload = window.onload;
      window.onload = function (e) {
        oldOnload(e);
        window[fnc]();
      };
    } else {
      window.onload = fnc;
    }
  }
};


// @public method
// @argument elem: string
// @argument attr: object @default={}
// @argument text: string
// create a dom element from 
XiVOHelper.prototype.build = function (elem,attr,text) {
  var e = document.createElement(elem);
  var i;

  if (attr !== undefined) {
    for (i in attr) {
      if (attr.hasOwnProperty(i)) {
        e.setAttribute(i,attr[i]);
      }
    }
  }

  if (text !== undefined) {
     e.appendChild(document.createTextNode(text));
  }

  return e;
};

// @public method
// @argument on: dom element 
// @argument className: string
XiVOHelper.prototype.addClass = function (on,className) {
  var all_class = on.getAttribute("class");
  on.setAttribute("class",all_class + " " + className);
};

// @public method
// @argument on: dom element 
// @argument className: string
XiVOHelper.prototype.delClass = function (on,className) {
  var all_class = on.getAttribute("class").replace(className,"");
  on.setAttribute("class",all_class);
};

// @public method
XiVOHelper.prototype.load_script = function (uri,func) {
  var script;
  if (func !== undefined) {
    script = XiVOH.build("script",{ src: uri, type: "text/javascript", charset: "utf-8"});

    XiVOH.addEventListener("load",func,script);
  } else {
    script = XiVOH.build("script",{ src: uri, type: "text/javascript", charset: "utf-8" });
  }

  document.getElementsByTagName("head")[0].appendChild(script);
};

// @public method
// @argument ob: object
// create a dom element from an array
XiVOHelper.prototype.build_from_array = function (ob) {
  var elem;
  var on;

  for (elem in ob) {
    if (ob.hasOwnProperty(elem)) {
      break;
    }
  }

  var wid = XiVOH.build(elem,
                        (ob[elem].attr)?ob[elem].attr:{},
                        (ob[elem].text)?ob[elem].text:undefined);
  
  if (typeof ob[elem].ev === "object") {
    for (on in ob[elem].ev) {
      if (ob[elem].ev.hasOwnProperty(on)) {
        XiVOH.addEventListener(on,ob[elem].ev[on],wid);
      }
    }
  }

  var i;
  if (typeof ob[elem].child === "object") {
    for (i=0 ; i<ob[elem].child.length ; i++) {
      wid.appendChild(this.build_from_array(ob[elem].child[i]));
    }
  }

  return wid;
};

// @public method
XiVOHelper.prototype.call_me_asap = function (test,fun,time) {
  if (test()) {
    fun();
  } else {
    var myself = this;
    setTimeout(function() {
      myself.call_me_asap(test,fun);
    },(time !== undefined)?time:1000);
  }
};

// @class
function XiVOClient()
{
  this.state = undefined;
  this.user = undefined;
  this.history = {};
  this.last_history_request = undefined;
  this.user_list = undefined;
  this.phone_list = undefined;
  this.queue_list = undefined;
  this.on_sheet = undefined;
  this.user_id = undefined;
  this.capaxlets = undefined;
  this.capafuncs = undefined;
  this.feature_list = { incallfilter: { enabled: false }, callrecord: { enabled: false },
                        enablednd: { enabled: false }, enablevoicemail: { enabled: false },
                        enablerna: { enabled: false }, enablebusy: { enabled: false },
                        enableunc: { enabled: false }};
  this.directory_search = {};
  this.c = undefined;
  this.next_received_line = '';
}

// @private method
// some features are named with two name..
XiVOClient.prototype._features_mapname = function (feature_name) {
  var feature_map = {
    enablevoicemail: 'enablevoicemail',
    callfilter: 'incallfilter',
    callrecord: 'callrecord',
    enablednd: 'enablednd',
    enablerna: 'enablerna',
    rna: 'enablerna',
    enablebusy: 'enablebusy',
    busy: 'enablebusy',
    enableunc: 'enableunc',
    unc: 'enableunc'
  };

  return feature_map[feature_name];
};

// @private method
XiVOClient.prototype._base_command = function() {
  return { 'direction': "xivoserver" };
};

// @private method
// render a flat array to json
XiVOClient.prototype._render_command= function(cmd) {
  var buff = "{";
  var i;

  for (i in cmd) {
    if (cmd.hasOwnProperty(i)) {
      buff +=  '"' + i + '": "' + cmd[i] + '",';
    }
  }

  return buff.substr(0,buff.length-1) + "}\n";
};


// @public method
// @argument orbited_server_hostname: string
// @argument orbited_server_port: string or number
// this method is called to load some, mandatory js file (sha1, Orbited) from the orbited server
// and do some global configuration
XiVOClient.prototype.init_lib = function (config) {
  var nocache = '?' + (new Date()).getTime();

  this.xivo_cti_ip = config.XiVO_CTI_Server.ip;
  this.xivo_cti_port = config.XiVO_CTI_Server.port;

  var head = document.getElementsByTagName("head");

  if (head === null) {
    head = XiVOH.build('head',{});
    document.getElementsByTagName("html")[0].appendChild(head,document.getElementsByTagName("html")[0].firstChild);
  } else {
    head = head[0];
  }

  XiVOH.load_script(config.orbited.proto + '://' + config.orbited.host + ':' + config.orbited.port + "/orbited/static/Orbited.js" + nocache);
  XiVOH.load_script("js/sha1.js" + nocache);

  var call_me_when_Orbited_loaded = function () {
  // if we aren't in the case of doing some cross sub-domain comet then we mustn't set any 
  // of these orbited setting to avoid the browser cross-scripting restrictions errors
  /*
    Orbited.settings.protocol = config.orbited.proto;
    Orbited.settings.log = true;
    Orbited.settings.hostname = config.orbited.host;
    Orbited.settings.port = config.orbited.port;
  */
    TCPSocket = Orbited.TCPSocket;
  };

  XiVOH.call_me_asap(function () { return (typeof Orbited === "object") ; },
                    call_me_when_Orbited_loaded);
};


// @private method
// @argument us: XiVOClient instance
// send the login to start the authentication process
XiVOClient.prototype._send_login = function (us) {
  var login_command = this._base_command();
  
  login_command["class"] = 'login_id';
  login_command["company"] = 'default';
  login_command["userid"] = us.user;
  login_command["ident"] = 'undef@X11-LE';
  login_command["xivoversion"] = '1.1';
  login_command["version"] = '9999';
  
  us.conn.send(this._render_command(login_command));
};

// @private method
// @argument us: XiVOClient instance
// @argument sid: 'session id'
// once we sent our login to the server, he must provide us a session id that we use 
// in password hash
XiVOClient.prototype._send_password = function (us,sid) {
  us.hpass = new SHA1(sid + ':' + us.pass).hexdigest();

  var pass_command = this._base_command();

  pass_command["class"] = 'login_pass';
  pass_command["hashedpassword"] = us.hpass;

  us.conn.send(this._render_command(pass_command));
};

// @private method
XiVOClient.prototype._send_keep_alive = function () {
  var keep_alive_command = this._base_command();

  keep_alive_command["class"] = 'keepalive';
  keep_alive_command["rate-bytes"] = '0';
  keep_alive_command["rate-msec"] = '0';
  keep_alive_command["rate-samples"] = '0';

  this.conn.send(this._render_command(keep_alive_command));
};

// @public method
// @argument state: string
// @argument c: callback(XiVOClient instance)
// it is the last step of autentication, the server answer is our credential and some info
// about the state we were allowed to set
// (1) the callback function is called once the server has answered all it should
XiVOClient.prototype.send_login_capas = function (state,c) {
  var capas_command = this._base_command();

  capas_command["class"] = 'login_capas';
  capas_command["state"] = state;
  capas_command["capaid"] = this.capa;
  capas_command["lastconnwins"] = 'true';
  capas_command["loginkind"] = 'user';

  this.conn.send(this._render_command(capas_command));
  this.c = c;
};

// @public method
// @argument nocache: boolean
// @argument c: callback(XiVOClient instance)
// request a phone list and call the callback function once we have it
// if you want to avoid caching, you should set the nocache argument to something that evaluate to true
XiVOClient.prototype.request_phone_list = function (nocache,c) {
  if ((this.phone_list === undefined) || (nocache)) {
    var get_phone_command = this._base_command();

    get_phone_command["class"] = 'phones';
    get_phone_command["function"] = 'getlist';
 
    this.conn.send(this._render_command(get_phone_command));
    this.c = c;
  } else {
    c(this);
  }
};

// @public method
// @argument nocache: boolean
// @argument c: callback(XiVOClient instance)
// request a queue list and call the callback function once we have it
// if you want to avoid caching, you should set the nocache argument to something that evaluate to true
XiVOClient.prototype.request_queue_list = function (nocache,c) {
  if ((this.queue_list === undefined) || (nocache)) {
    var get_queue_command = this._base_command();

    get_queue_command["class"] = 'queues';
    get_queue_command["function"] = 'getlist';
 
    this.conn.send(this._render_command(get_queue_command));
    this.c = c;
  } else {
    c(this);
  }
};

// @public method
// @argument nocache: boolean
// @argument user_id string @default=this.get_user_id()
XiVOClient.prototype.request_featuresget = function (nocache,user_id) {
  var get_feature_command = this._base_command();
  user_id = (user_id === undefined) ? "xivo/" + this.get_user_id() : user_id;

  get_feature_command["class"] = 'featuresget';
  get_feature_command["userid"] = user_id;

  this.conn.send(this._render_command(get_feature_command));
};

// @public method
// @argument nocache: boolean
// @argument mode number
// @argument c: callback(XiVOClient instance)
// @argument user_id string @default=this.get_user_id()
// @argument size number @default=8
// mode : 
//        0 -- the call you made
//        1 -- the call you answered
//        2 -- the call you missed
XiVOClient.prototype.request_history = function (nocache,mode,c,user_id,size) {

  var get_history_command = this._base_command();

  size = (size !== undefined) ? size : 8;
  mode = (mode !== undefined) ? mode : 8;
  user_id = (user_id === undefined) ? "xivo/" + this.get_user_id() : user_id;

  get_history_command["class"] = 'history';
  get_history_command["peer"] = user_id;
  get_history_command["mode"] = mode;
  get_history_command["size"] = size;


  this.last_history_request = String(mode);
  this.c = c;

  this.conn.send(this._render_command(get_history_command));
};

// @public method
// @argument crap: object
// @argument nocache: boolean
// @argument c: callback(XiVOClient instance)
// @todo some cache
// 3 
// crap = { 'function': "callrecord", value: "0", userid:"xivo/56" }
// 'function' can be 'callrecord' 'enablednd' 'callfilter' 'enablevoicemail'
XiVOClient.prototype.request_featuresput = function (crap,nocache,c) {
  //if ((this.queue_list === undefined) || (nocache)) {
    var featureput_command = this._base_command();
    var i;

    featureput_command["class"] = 'featuresput';

    for (i in crap) {
      if (crap.hasOwnProperty(i)) {
        featureput_command[i]= crap[i];
      }
    }
 
    this.conn.send(this._render_command(featureput_command));
    //this.c = c;
  //} else 
    if (typeof c === "function") {
      c(this);
    }
};

// @public method
// @argument nocache: boolean
// @argument c: callback(XiVOClient instance)
// request a phone list and call the callback function once we have it
// if you want to avoid caching, you should set the nocache argument to something that evaluate to true
XiVOClient.prototype.request_user_list = function (nocache,c) {

  if ((this.user_list === undefined) || (nocache)) {
    var get_user_command = this._base_command();

    get_user_command["class"] = 'users';
    get_user_command["function"] = 'getlist';
 
    this.conn.send(this._render_command(get_user_command));
    this.c = c;
  } else {
    c(this);
  }
};

// @public method
// @argument pattern: string
// @argument nocache: boolean
// @argument c: callback(XiVOClient instance)
// request a list of contact
XiVOClient.prototype.request_directory_search = function (pattern,nocache,c) {

  if ((this.directory_search[pattern] === undefined) || (nocache)) {
    var directory_search_command = this._base_command();

    directory_search_command["class"] = 'directory-search';
    directory_search_command["pattern"] = pattern;
 
    this.conn.send(this._render_command(directory_search_command));
    this.last_directory_search = pattern;
    this.c = c;
  } else {
    c(this);
  }
};

// @public method
// @argument dest: string
// @argument source: string @default='user:special:me'
// initiate a call 
XiVOClient.prototype.make_call = function (dest,source) {
  var call_command = this._base_command();
  
  source = (source !== undefined)?source:"user:special:me";

  call_command["class"] = 'originate';
  call_command["source"] = source;
  call_command["destination"] = dest;
  
  this.conn.send(this._render_command(call_command));
};

// @public method
// @argument dest: string
// @argument source: string
// hangup both side
XiVOClient.prototype.hangup = function (source) {
  var hangup_command = this._base_command();
  
  hangup_command["class"] = 'hangup';
  hangup_command["source"] = source;
  
  this.conn.send(this._render_command(hangup_command));
};

// @public method
// @argument dest: string
// @argument source: string
// do a call transfert
XiVOClient.prototype.transfert = function (dest,source) {
  var transfert_command = this._base_command();
  
  transfert_command["class"] = 'transfert';
  transfert_command["source"] = source;
  transfert_command["destination"] = dest;
  
  this.conn.send(this._render_command(transfert_command));
};

// @public method
// @argument state: string
// @argument c: callback(XiVOClient instance)
XiVOClient.prototype.change_state = function (state,c) {
  if (this.state !== state) {
    var availstate_command = this._base_command();
    
    availstate_command["class"] = 'availstate';
    availstate_command["availstate"] = state;
 
    this.c = c;
    this.conn.send(this._render_command(availstate_command));
  } else {
    this._async_call();
  }
};

// @public method
// @argument mode number
XiVOClient.prototype.get_history = function (mode) { return this.history[String(mode)]; };
// @public method
XiVOClient.prototype.get_state = function () { return this.state; };
// @public method
XiVOClient.prototype.get_user = function () { return this.user; };
// @public method
XiVOClient.prototype.get_pass = function () { return this.pass; };
// @public method
XiVOClient.prototype.get_capaxlets = function () { return this.capaxlets; };
// @public method
XiVOClient.prototype.get_capafuncs = function () { return this.capafuncs; };
// @public method
XiVOClient.prototype.get_directory_search = function (pattern) { return this.directory_search[pattern]; };
// @public method
XiVOClient.prototype.get_user_id = function () { return this.user_id; };
// @public method
XiVOClient.prototype.get_user_detail = function () { 
  var user_list = this.get_user_list();
  var our_id = this.get_user_id();
  var user;
  for(user in user_list) {
    if (user_list.hasOwnProperty(user)) {
      if (user_list[user].xivo_userid === our_id) {
        return user_list[user];
      }
    }
  }
};
// @public method
XiVOClient.prototype.get_queue_list = function () { return this.queue_list; };
// @public method
XiVOClient.prototype.get_feature = function (feature) { return this.feature_list[feature]; };
// @public method
XiVOClient.prototype.get_user_list = function () { return this.user_list; };
// @public method
XiVOClient.prototype.get_phone_list = function () { return this.phone_list; };
// @public method
XiVOClient.prototype.get_allowed_state = function () { return this.allowed; };

// @public method
XiVOClient.prototype.get_allowed_state = function () { return this.allowed; };

// @private method
// @argument us: XiVOClient instance
// @argument s: string
// this method is called everytime we receive a line from the server
XiVOClient.prototype._parse_incoming_message = function (us,s,full_line) {

  /* reunite partial lines */
  s = us.next_received_line + s;
  pos = s.lastIndexOf("\n");
  if (pos == -1 ) {
    us.next_received_line = s;
    return ;
  }
  us.next_received_line = s.substr(pos+1,s.length);
  s = s.substr(0,pos);

  if (us.firstline === undefined) {
    us.firstline = 'eated';
    return ;
  }
  
  var sp = s.split("\n");
  var si; /* var to be used as an iterator inside the switch */
  var se; /* '' */
  var i;

  for(i in sp) {
    if (!sp.hasOwnProperty(i)) {
      continue;
    }

    s = sp[i];

    try {
      eval('var a =' + s +';');
    } catch(err) {
      /* we received a full line not in json */
      // alert(err);
      // alert(s);
      return;
    }


    us.last_packet = a;

    switch(a["class"]) {
      case 'login_id_ok':
        us._send_password(us,a.sessionid);
        break;
      case 'login_pass_ok':
        us.capa = a.capalist[0];
        us._async_call(1);
        if (us.keep_alive) {
          us.keep_alive = setInterval( function () {
            us._send_keep_alive();
          },1000);
        }
        break;
      case 'login_capas_ok':
        us.capaxlets = a.capaxlets;
        us.capafuncs = a.capafuncs;
        us.capaservice = a.capaservices;
        us.allowed = a.capapresence;
        us.user_id = a.xivo_userid;
        break;
      case 'presence':
        if (a.xivo_userid === us.get_user_id()) {
          us.allowed.allowed = a.capapresence.allowed;
          us.state = a.capapresence.state.stateid;
          us._async_call();
        }

        for (si in us.user_list) {
          if (us.user_list.hasOwnProperty(si)) {
            if (us.user_list[si].xivo_userid ===  a.xivo_userid ) {
              us.user_list[si].statedetails = a.capapresence.state;
              break;
            }
          }
        }
        break;
      case 'disconn':
        if (us.keep_alive) {
          clearInterval(us.keep_alive);
        }

        if (typeof us.ondc === "function") {
          us.ondc(us,-1);
        }
        break;
      case 'loginko':
        us._async_call(0);
        break;
      case 'queues':
          us.queue_list = a.payload;
        break;
      case 'sheet':
        if (typeof us.on_sheet === "function") {
          us.on_sheet(us);
        }
        break;
      case 'phones':
        if (a["function"] === 'sendlist') {
          us.phone_list = a.payload;
          us._async_call();
        } else if (a["function"] === 'update') {
          us.phone_list.xivo[a.phoneid].hintstatus = a['status'].hintstatus;
          us.phone_list.xivo[a.phoneid].comms = a['status'].comms;
        }
        break;
      case 'directory':
        us.directory_search[us.last_directory_search] = { headers: a.headers };
        us.directory_search[us.last_directory_search]['list'] = [];
        for (si=0,se=a.resultlist.length;si<se;si++) {
          us.directory_search[us.last_directory_search]['list'][si] = a.resultlist[si].split(';');
        }
        us._async_call();
        break;
      case 'users':
        if ((typeof a.subclass === "string") && (a.subclass === "mwi")) {
          for (si in us.user_list) {
            if (us.user_list.hasOwnProperty(si)) {
              if (us.user_list[si].xivo_userid ===  a.user[1] ) {
                us.user_list[si].mwi = a.payload;
                break;
              }
            }
          }
          break;
        }
        us.user_list = a.payload;
        us._async_call();
        break;
      case 'features':
        for (si in a.payload) {
          if (a.payload.hasOwnProperty(si)) {
            us.feature_list[us._features_mapname(si)] = a.payload[si];
          }
        }
        break;
      case 'history':
        us.history[us.last_history_request] = a.payload;
        us._async_call();
        break;
      default:
        break;
    }
  }
};

// @public method
// @argument on: string
// @argument c: callback(XiVOClient instance)
// this function is called to register a function to an action we are interested in
// c can only be 'sheet' atm
XiVOClient.prototype.notify = function (on,c) {
  this["on_" + on] = c;
};

// @public method
// @argument on: string
// @argument c: callback(code,XiVOClient instance)
XiVOClient.prototype.register_disconnect_function = function (c) {
  this.ondc = c;
};

// @private method
XiVOClient.prototype._async_call = function (ok) {
  if (typeof this.c === "function") {
    var calle = this.c;
    this.c = undefined;
    calle(this,ok);
  }
};

// @public method
XiVOClient.prototype.request_everything = function (c) {
  this.request_user_list(true,function(us) {
    us.request_phone_list(true,function(us) {
      us.request_featuresget(true);
      c(us);
    });
  });
};

// @public method
// @argument info: { user: "login", pass: "password" (, state: "available", keep_alive: true)}
// @argument c: callback(XiVOClient instance,boolean connection_succeeded)
// login your XiVOClient instance on the server. 
// If the state key isn't set you will have to do yourself the last step of the authentication
// (aka calling yourself send_login_capas)
// keep_alive is there to make your XiVOClient instance send periodically some keep_alive request to your
// XiVO CTI server
XiVOClient.prototype.try_login = function (info,c) {
  var us = this;

  this.user = info.user;
  this.pass = info.pass;
  if (info.state !== undefined) {
    this.c = function (us,connected) {
      if (connected) {
        us.send_login_capas(info.state,function (us) { c(us,connected); });
      } else {
        c(us,connected);
      }
    };
  } else {
    this.c = c;
  }

  this.state = '"state not available"';
  this.keep_alive = (info.keep_alive !== undefined) ? info.keep_alive : false;

  var call_me_when_Orbited_loaded = function () {
    us.conn = new TCPSocket();

    us.conn.onopen = function () {
      us._send_login(us);
    };
    us.conn.onread = function (s) {
      us._parse_incoming_message(us,s);
    };
    us.conn.onclose = function (code) {
      if (typeof us.ondc === "function") {
        us.ondc(code,us);
      }
    };

    us.conn.open(us.xivo_cti_ip,us.xivo_cti_port);
  };

  XiVOH.call_me_asap(function () { return (typeof TCPSocket !== "undefined") ; },call_me_when_Orbited_loaded);
};

// @var global
// the default XiVOClient instance 
var XiVOC = new XiVOClient();




// register on firefox and maybe on ie
XiVOH.addOnloadEvent(function () {
  var XiVO_client_used_config = null;
  /* retrieve the orbited server ip&port from the script 
     if the XiVO_client_used_config var hasn't be defined */
  if (typeof XiVO_client_config === "undefined") {
    script_list = document.getElementsByTagName("script");
    var re = new RegExp('(http[s]?)://([^:/]*)(:[0-9]*)?.*/js/(xivoclient).js$');
    var i = 0, e = script_list.length;
    for (; i < e ; i++) {
      var arr = re.exec(script_list[i].src);
      if ((arr !== null) && (arr[4] === "xivoclient")) {
        var on_port;
        if (typeof arr[3] !== "undefined") {
          on_port = arr[3].substr(1);
        } else {
          if (arr[1] === "http") {
            on_port = "80";
          } else {
            on_port = "443";
          }
        }
        XiVO_client_used_config = { orbited: { proto: arr[1], host: arr[2], port: on_port },
                                    XiVO_CTI_Server: { ip: "127.0.0.1", port: "5003" }};
      }
    }

  } else {
    XiVO_client_used_config = XiVO_client_config;
  }

  if (XiVO_client_used_config === null) {
    XiVO_client_used_config = { orbited: { proto: "https", host: window.location.host, port: "443" },
                                XiVO_CTI_Server: { ip: "127.0.0.1", port: "5003" }};
  }

  XiVOC.init_lib(XiVO_client_used_config);
},false);

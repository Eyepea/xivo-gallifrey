if (typeof JSON !== "undefined") {
  JSON = {};
  JSON.parse = function(str) {
    var ret;
    eval('ret = ' + str + ';');
    return ret;
  };
  JSON.stringify = function (ob) {
    var i, e;
    var s;
    var h = 0;
    
    if (ob instanceof Array) {
      i = 0; e = ob.length;
      s = "[";
      if (e) {
        for(;i<e;i++) {
          if (typeof ob[i] === "string") {
            s += '"' + ob[i] + '"';
          } else if (typeof ob[i] === "number") {
            s += ob[i];
          } else {
            s += JSON.stringify(ob[i]);
          }
        s += ",";
        }
        s = s.substring(0, s.length - 1);
      }
      s += ']';
    } else if (ob instanceof Object) {
      s = "{";
      for(i in ob) {
        if (ob.hasOwnProperty(i)) {
          h = 1;
          if (typeof ob[i] === "string") {
            s += '"' + i + '":"' + ob[i] + '"';
          } else if (typeof ob[i] === "number") {
            s += '"' + i + '":' + ob[i];
          } else {
            s += '"' + i + '":' + JSON.stringify(ob[i]);
          }
          s += ",";
        }
      }
    
      if (h) {
        s = s.substring(0, s.length - 1);
      }
      s += '}';
    }
    
    return s;
  };
}

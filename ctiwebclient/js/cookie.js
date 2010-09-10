function create_cookie(name,value,days)
{
  var expires;
  if (days) {
    var date = new Date();
    date.setTime(date.getTime() + (days * 24 * 60 * 60 * 1000));
    expires = "; expires=" + date.toGMTString();
  } else {
    expires = "";
  }
  document.cookie = name + "=" + value + expires + "; path=/";
}

function read_cookie(name)
{
  var nameEQ = name + "=";
  var ca = document.cookie.split(';');
  var c, i;

  for(i=0;i<ca.length;i++) {
    c = ca[i];
    while (c.charAt(0)===' ') {
      c = c.substring(1,c.length);
    }
    if (c.indexOf(nameEQ) === 0) {
      return c.substring(nameEQ.length,c.length);
    }
  }
  return null;
}

function erase_cookie(name)
{
  create_cookie(name,"",-1);
}

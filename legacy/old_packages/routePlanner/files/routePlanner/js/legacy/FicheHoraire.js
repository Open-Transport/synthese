var ns4 = (document.layers);
var ie4 = (document.all && !document.getElementById);
var ie5 = (document.all && document.getElementById);
var ns6 = (!document.all && document.getElementById);

function s(id){
  // Netscape 4
  if(ns4)
  {
    document.layers[id].visibility = "show";
    document.layers['dfh'].style.visibility = "hide";
  }
  // Explorer 4
  else if(ie4)
  {
    document.all[id].style.visibility = "visible";
    document.all['dfh'].style.visibility = "hidden";
  }
  // W3C - Explorer 5+ and Netscape 6+
  else if(ie5 || ns6)
  {
    document.getElementById(id).style.display = "block";
    document.getElementById('dfh').style.display = "none";
//      document.getElementById(id).style.visibility = "visible";
//      document.getElementById('dfh').style.visibility = "hidden";
  }
}
function h(id){
  // Netscape 4
  if(ns4)
  {
    document.layers[id].visibility = "hide";
    document.layers['dfh'].style.visibility = "show";
  }
  // Explorer 4
  else if(ie4)
  {
    document.all[id].style.visibility = "hidden";
    document.all['dfh'].style.visibility = "visible";
  }
  // W3C - Explorer 5+ and Netscape 6+
  else if(ie5 || ns6)
  {
    document.getElementById(id).style.display = "none";
    document.getElementById('dfh').style.display = "block";
//      document.getElementById(id).style.visibility = "hidden";
//      document.getElementById('dfh').style.visibility = "visible";
  }
}

function sh(ids, idh)
{
  h(ids);
  s(idh);
}

window.onunload = function() {
  if (window.app)
    app.destroy();
}

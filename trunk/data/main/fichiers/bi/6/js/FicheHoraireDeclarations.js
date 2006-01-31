var HTMLLigne = new Array();
var HTMLQuaiDepart = new Array();
var HTMLQuaiArrivee = new Array();
var HTMLDestination = new Array();
var HTMLModeTransport = new Array();
var HTMLPointArret = new Array();
var HTMLHorairesDepart = new Array();
var HTMLHorairesArrivee = new Array();
var HTMLDuree = new Array();
var SC = new Array();

function Resize()
{
 document.all.TableauHoraire.style.width = document.body.clientWidth-15;
 if (document.all.TB.offsetWidth < document.body.clientWidth-15)
 {
  document.all.TableauHoraire.style.overflow='hidden';
  document.all.CacheDefilementDroit.style.visibility='hidden';
 }
 else
 {
  document.all.TableauHoraire.style.overflow='scroll';
  document.all.CacheDefilementDroit.style.left = document.body.clientWidth - 16;
  document.all.CacheDefilementDroit.style.height = document.all.TableauHoraire.offsetHeight; 
  document.all.CacheDefilementDroit.style.visibility='visible';
 }
 document.all.Commentaires.style.position = 'absolute';
 document.all.Commentaires.style.pixelTop = document.all.TableauHoraire.offsetHeight + 100;
}

function openWindow(url, name, w, h, r, s, m, left, top) 
{
 popupWin = window.open(url, name, 'width=' + w + ',height=' + h + ',resizable=' + r + s + ',menubar=' + m + ',left=' + left + ',top=' + top);
 popupWin.focus();
}

	var ns4 = (document.layers);
	var ie4 = (document.all && !document.getElementById);
	var ie5 = (document.all && document.getElementById);
	var ns6 = (!document.all && document.getElementById);
	
	function show(id){
		// Netscape 4
		if(ns4){
			document.layers[id].visibility = "show";
		}
		// Explorer 4
		else if(ie4){
			document.all[id].style.visibility = "visible";
		}
		// W3C - Explorer 5+ and Netscape 6+
		else if(ie5 || ns6){
			document.getElementById(id).style.visibility = "visible";
		}
	}
	function hide(id){
		// Netscape 4
		if(ns4){
			document.layers[id].visibility = "hide";
		}
		// Explorer 4
		else if(ie4){
			document.all[id].style.visibility = "hidden";
		}
		// W3C - Explorer 5+ and Netscape 6+
		else if(ie5 || ns6){
			document.getElementById(id).style.visibility = "hidden";
		}
	}

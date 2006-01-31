
 rub_horaires_out = newImage("/bi/6/img/tisseo-horaires-normal.gif");
 rub_horaires_over = newImage("/bi/6/img/tisseo-horaires-flash.gif");
 rub_tarifs_out = newImage("/bi/6/img/tisseo-tarifs-normal.gif");
 rub_tarifs_over = newImage("/bi/6/img/tisseo-tarifs-flash.gif");


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
//			document.getElementById(id).style.visibility = "visible";
//			document.getElementById('dfh').style.visibility = "hidden";
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
//			document.getElementById(id).style.visibility = "hidden";
//			document.getElementById('dfh').style.visibility = "visible";
		}
	}

	function sh(ids, idh)
	{
		h(ids);
		s(idh);
	}

	function FenetreQuiFaitQuoi()
	{
	  	var wobj = window.open('http://www.tisseo.fr/ffTransports.asp', 'Transport','status=no,width=650,height=630,scrollbars=no,left=50,top=50');
		wobj.focus();
	
	}

// A METTRE DANS LE PER
function Resa(codeLigne, numService, codeResa, codePADepart, codePAArrivee, dateDepart)
{
	var str = URL+"fonction=fres&site="+ document.saisie.site.value;
	str += "&lig="+escape(codeLigne);
	str += "&serv="+numService;
	str += "&res="+codeResa;
	str += "&npad="+codePADepart;
	str += "&npaa="+codePAArrivee;
	str += "&date="+dateDepart;
	
	var wobj = window.open(str, 'FRRESA', 'left=200, top=50, toolbar=no, menubar=no, height='+(screen.availHeight-100));
	wobj.focus();
}

function annul()
{
	var str = "/bi/4/html/annulation.html";
	var wobj = window.open(str, 'FRRESA', 'left=200, top=50, toolbar=no, menubar=no, height='+(screen.availHeight-100));
	wobj.focus();
}
function FenetreAide(rubrique)
{
  	var wobj = window.open('/bi/6/html/aide' + rubrique + '.html', 'Aide','status=no,width=350,height=500,scrollbars=no,left=50,top=50');
	wobj.focus();

}

function FenetreResa(url)
{
	var wobj = window.open(url, 'FRRESA', 'left=200, top=50, toolbar=no, menubar=no, height='+(screen.availHeight-100));
	wobj.focus();
}

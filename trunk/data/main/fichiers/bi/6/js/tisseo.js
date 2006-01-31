// Includes
// document.write('<scr'+'ipt language="javascript" type="text/javascript" src="/js/funcs-detection-flash.js"></scri'+'pt>');
// document.write('<scr'+'ipt language="javascript" type="text/javascript" src="/js/clientsniffer.js"></scri'+'pt>');
// document.write('<scr'+'ipt language="javascript" type="text/javascript" src="/js/CMS_functions.js"></scri'+'pt>');

/** Default main window name
*/
window.name = "tisseo";

/** Default string to appear in status bar
*/
var strDefaultStatus = "Tisseo" ;
window.status = strDefaultStatus;

/**
  * Swaps a picture between given statuses
  * Picture must be preloaded
  * 
  * @param String imgName, image "name" attribute value in HTML page
  * @param String status, current status to show
  * @return void
  */
function swap(imgName, status) {
    if (document.images) {
    	document[imgName].src = eval(imgName + '_' + status + '.src');
    }
    window.status = strDefaultStatus;
}


/**
  * Instanciates an image with its src
  *
  * @param String src, the image source
  * @return Image object created
  */
function newImage(arg) {
	if (document.images && arg!="") {
		rslt = new Image();
		rslt.src = arg;
		return rslt;
	}
}

/**
  * Emptying and filling search field
  *
  * @param Field field, the field object itself
  * @return void
  */
function clearField(field) {
	if (field.value == field.defaultValue) {
		field.value = "";
	}
}

/**
  * Setting default value to search field
  *
  * @param Field field, the field object itself
  * @return void
  */
function checkField(field) {
	if (field.value == "") {
		field.value = field.defaultValue;
	}
}


/* ouverture de fenêtre */
function openWindow(url, name, w, h, r, s, m, left, top) {
	popupWin = window.open(url, name, 'width=' + w + ',height=' + h + ',resizable=' + r + s + ',menubar=' + m + ',left=' + left + ',top=' + top);
}
// parameters available:
//      resizable (r),  location (l), directories (d), menubar (m)
//      status (st), toolbar (t), scrollbars (s), copyhistory (c)



// Loading main nav images
	

// Links in bottom right of page


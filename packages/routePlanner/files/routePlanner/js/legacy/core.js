// Configuration object that can be updated during initialization for site
// specific settings.
var legacyRoutePlannerConfig = {
  // How many characters need to be typed before an autocomplete request is made
  // NOTE: the request is made if the number of typed characters is greater
  // than this value.
  minCharsToTriggerAutocomplete: 2,
  // Name of the route planner form
  routePlannerFormName: "ri",
  // Type of autocomplete form
  // false -> city and place input fields
  // true -> only one input field
  routePlannerFormOneField: false
};

function swapValues(element1, element2) {
  var echange = element1.value;
  element1.value = element2.value;
  element2.value = echange;
}

function globalKeyPressHandler(event) {
  if (!event)
    event = window.event;

  if (event.keyCode == 13 && _enterIsForbidden || event.keyCode == 27) {
    event.returnValue = false;

    event.cancel = true;
  }
}
document.onkeypress = globalKeyPressHandler;

// Blinking class ---

var clignoteState = 'visible';
var clignoteElements;
var totalElements = -1;

function clignote() {
  var allElements = document.getElementsByTagName('*');

  // Cache elements to blink, since walking all elements is expensive,
  // especially on IE <= 7.
  if (!clignoteElements || totalElements != allElements.length) {
    clignoteElements = [];
    totalElements = allElements.length;

    for (var i = 0; i < allElements.length; ++i)
    {
      if (allElements.item(i).className == 'clignote') {
        clignoteElements.push(allElements.item(i));
      }
    }
  }

  clignoteState = (clignoteState == 'visible') ? 'hidden' : 'visible';

  for (i = 0; i < clignoteElements.length; ++i) {
    clignoteElements[i].style.visibility = clignoteState;
  }
  setTimeout("clignote()", 500);
}
clignote();

// Completion handler --------------------------------------------------------------------------------------------------------------------

var _urlFunction = [];
var _currentInputField;
var _enterIsForbidden = false;
var _documentForm = null; // le formulaire contenant notre champ texte
var _inputField = []; // le champ texte lui-même
var _secondaryField = []; // le champ texte lui-même
var _submitButton = null; // le bouton submit de notre formulaire
var _oldInputFieldValue = []; // valeur précédente du champ texte
var _currentInputFieldValue = []; // valeur actuelle du champ texte
var _resultCache = []; // mécanisme de cache des requetes
var _fieldToCleanAtUpdate = [];
var _normalObject = [];
var _waitObject = [];
var _timeOut;

// retourne un objet xmlHttpRequest.
// méthode compatible entre tous les navigateurs (IE/Firefox/Opera)
function getXMLHTTP() {
  var xhr = null;
  if (window.XMLHttpRequest) { // Firefox et autres
    xhr = new XMLHttpRequest();
  } else if (window.ActiveXObject) { // Internet Explorer
    try {
      xhr = new ActiveXObject("Msxml2.XMLHTTP");
    } catch (e) {
      try {
        xhr = new ActiveXObject("Microsoft.XMLHTTP");
      } catch (e1) {
        xhr = null;
      }
    }
  } else { // XMLHttpRequest non supporté par le navigateur
    alert("Votre navigateur ne supporte pas les objets XMLHTTPRequest...");
  }
  return xhr;
}

function XMLParse(text) {
  if (window.ActiveXObject) {
    var docXML = new ActiveXObject("Microsoft.XMLDOM")
    docXML.loadXML(text)
    return docXML;
  } else {
    var parser = new DOMParser();
    return parser.parseFromString(text, "text/xml");
  }
}

function initAutoCompleteForm(form, submit) {
  _documentForm = form;
  _submitButton = submit;
  createAutocompletionDiv();
  cacheResults("", []);
  window.onresize = onResizeHandler;
}

function initAutoCompleteField(i, field, otherField, urlFunction, fieldToClean, normalObject, waitObject) {
  _inputField[i] = field;
  _currentInputFieldValue[i] = _inputField[i].value;
  _oldInputFieldValue[i] = _currentInputFieldValue[i];
  _inputField[i].onblur = onBlurHandler;
  _inputField[i].onkeyup = autoCompleteKeyUp;
  _inputField[i].onkeydown = function (event) {
    if (!event) event = window.event;
    if (event.keyCode == 27) return false;
    if (event.keyCode == 38) return false;
    if (event.keyCode == 40) return false;
    if (event.keyCode == 13) return !_enterIsForbidden;
  };
  _inputField[i].onfocus = function () {
    this.select();
  };
  _urlFunction[i] = urlFunction;
  _secondaryField[i] = otherField;
  _fieldToCleanAtUpdate[i] = fieldToClean;
  _normalObject[i] = normalObject;
  _waitObject[i] = waitObject;
}

// echappe les caractère spéciaux
function escapeURI(La) {
  if (encodeURIComponent) {
    return encodeURIComponent(La);
  }
  if (escape) {
    return escape(La);
  }
}

var _xmlHttp = null; // l'objet xmlHttpRequest utilisé pour contacter le serveur

function callSuggestions(i, valeur) {
  // appel à l'url distante
  var city = "";
  var place = "";
  if (_secondaryField[i] && (!legacyRoutePlannerConfig.routePlannerFormOneField)) {
    city = _secondaryField[i].value;
    place = valeur;
  } else {
    city = valeur;
  }
  if (getFromCache(city, place)) {
      drawList(i, valeur, getFromCache(city, place));
      return;
  }

  if (_xmlHttp && _xmlHttp.readyState != 0) {
    _xmlHttp.abort();
  }
  if (_waitObject[i] && _normalObject[i]) {
    _waitObject[i].style.display = 'inline';
    _normalObject[i].style.display = 'none';
  }

  _xmlHttp = getXMLHTTP();
  if (_xmlHttp) {
    _xmlHttp.open("GET", _urlFunction[i](city, place), true);
    _xmlHttp.onreadystatechange = function () {
      if (_xmlHttp.readyState == 4 && _xmlHttp.responseText) {
        var docXML
        if (window.ActiveXObject) {
          docXML = new ActiveXObject("Microsoft.XMLDOM")
          docXML.loadXML(_xmlHttp.responseText)
        } else {
          var parser = new DOMParser();
          var docXML = parser.parseFromString(_xmlHttp.responseText, "text/xml");
        }
        var liste = traiteXmlSuggestions(docXML)
        cacheResults(city, place, liste)
        drawList(i, valeur, liste)
        if (_waitObject[i] && _normalObject[i]) {
          _waitObject[i].style.display = 'none';
          _normalObject[i].style.display = 'inline';
        }
      }
    };
    // envoi de la requete
    _xmlHttp.send(null)
  }
}

// Mecanisme de caching des réponses
function cacheResults(value1, value2, suggestions) {
  _resultCache[value1 + "$$$" + value2] = suggestions;
}

function getFromCache(value1, value2) {
  return _resultCache[value1 + "$$$" + value2];
}


// Transformation XML en tableau
function traiteXmlSuggestions(xmlDoc) {
  var optionsListe = [];
  if (legacyRoutePlannerConfig.routePlannerFormOneField) {
    // Best place
    var bestPlaceNode = xmlDoc.getElementsByTagName('best_place')[0];
    optionsListe.push(['Meilleure proposition : ', '', '', null]);
    var cities = bestPlaceNode.getElementsByTagName('city');
    var stops = bestPlaceNode.getElementsByTagName('stop');
    var roads = bestPlaceNode.getElementsByTagName('road');
    var addresses = bestPlaceNode.getElementsByTagName('address');
    var publicPlaces = bestPlaceNode.getElementsByTagName('public_place');
    if (cities.length > 0) {
      optionsListe.push(traiteXmlCity(cities[0]));
    }
    if (stops.length > 0) {
      optionsListe.push(traiteXmlStop(stops[0]));
    }
    if (roads.length > 0) {
      optionsListe.push(traiteXmlRoad(roads[0]));
    }
    if (addresses.length > 0) {
        optionsListe.push(traiteXmlAddress(addresses[0]));
    }
    if (publicPlaces.length > 0) {
        optionsListe.push(traiteXmlPublicPlace(publicPlaces[0]));
    }

    // Cities
    var citiesNode = xmlDoc.getElementsByTagName('cities')[0];
    if (citiesNode) {
        var cities = citiesNode.getElementsByTagName('city');
        if (cities.length > 0) {
            optionsListe.push(['Communes : ', '', '', null]);
        }
        for (var i = 0; i < cities.length; ++i) {
            optionsListe.push(traiteXmlCity(cities[i]));
        }
    }

    // Stops
    var stopsNode = xmlDoc.getElementsByTagName('stops')[0];
    if (stopsNode) {
        var stops = stopsNode.getElementsByTagName('stop');
        if (stops.length > 0) {
            optionsListe.push(['Arrêts : ', '', '', null]);
        }
        for (var i = 0; i < stops.length; ++i) {
            optionsListe.push(traiteXmlStop(stops[i]));
        }
    }

    // Roads
    var roadsNode = xmlDoc.getElementsByTagName('roads')[0];
    if (roadsNode) {
        var roads = roadsNode.getElementsByTagName('road');
        if (roads.length > 0) {
            optionsListe.push(['Rues : ', '', '', null]);
        }
        for (var i = 0; i < roads.length; ++i) {
            optionsListe.push(traiteXmlRoad(roads[i]));
        }
    }

    // Addresses
    var addressesNode = xmlDoc.getElementsByTagName('addresses')[0];
    if (addressesNode) {
        var addresses = addressesNode.getElementsByTagName('address');
        if (addresses.length > 0) {
            optionsListe.push(['Adresses : ', '', '', null]);
        }
        for (var i = 0; i < addresses.length; ++i) {
            optionsListe.push(traiteXmlAddress(addresses[i]));
        }
    }

    // Public places
    var ppNode = xmlDoc.getElementsByTagName('public_places')[0];
    if (ppNode) {
        var pps = ppNode.getElementsByTagName('public_place');
        if (pps.length > 0) {
            optionsListe.push(['Lieux publics : ', '', '', null]);
        }
        for (var i = 0; i < pps.length; ++i) {
            optionsListe.push(traiteXmlPublicPlace(pps[i]));
        }
    }


  } else {
    var options = xmlDoc.getElementsByTagName('option');
    for (var i = 0; i < options.length; ++i) {
      optionsListe.push([
        options[i].firstChild.data,
        options[i].getAttribute('cityName'),
        options[i].getAttribute('name')]);
    }
  }
  return optionsListe;
}

function traiteXmlCity(city) {
  return [
    city.getAttribute('key'),
    city.getAttribute('city_name'),
    '',
    'city'];
}

function traiteXmlStop(stop) {
  return [
    stop.getAttribute('key'),
    stop.getAttribute('cityName'),
    stop.getAttribute('stop_name'),
    'stop'];
}

function traiteXmlRoad(road) {
  return [
    road.getAttribute('key'),
    road.getAttribute('city_name'),
    road.getAttribute('name'),
    'road'];
}

function traiteXmlAddress(address) {
  return [
    address.getAttribute('number') + ' ' + address.getAttribute('road_name') + ' ' + address.getAttribute('road_city_name'),
    address.getAttribute('road_city_name'),
    address.getAttribute('number') + ' ' + address.getAttribute('road_name'),
    'road'];
}

function traiteXmlPublicPlace(pp) {
  return [
    pp.getAttribute('key'),
    pp.getAttribute('city_name'),
    pp.getAttribute('name'),
    'public_place'];
}

function setClassForElement(c, name) {
  c.className = name;
}

function getClassForElement(c) {
  return c.className;
}

// calcule le décalage à gauche
function calculateOffsetLeft(r) {
  return calculateOffset(r, "offsetLeft");
}

// calcule le décalage vertical
function calculateOffsetTop(r) {
  return calculateOffset(r, "offsetTop");
}

function calculateOffset(r, attr) {
  var kb = 0;
  while (r) {
    kb += r[attr];
    r = r.offsetParent;
  }
  return kb;
}

// calcule la largeur du champ
function calculateWidth(i) {
  return _inputField[i].offsetWidth - 2 * 1;
}

function setCompleteDivSize(i) {
  if (_completeDiv) {
    _completeDiv.style.left = calculateOffsetLeft(_inputField[i]) + "px";
    _completeDiv.style.top = calculateOffsetTop(_inputField[i]) + _inputField[i].offsetHeight - 1 + "px";
    _completeDiv.style.width = calculateWidth(i) + "px"
  }
}

function createAutocompletionDiv() {
  _completeDiv = document.createElement("DIV");
  _completeDiv.id = "completeDiv";

  document.body.appendChild(_completeDiv);
  setClassForElement(_completeDiv, "autoCompleteDivListeStyle");
}

var currentList;

function drawList(i, valeur, liste) {
  // Clean the old list
  while(_completeDiv.childNodes.length > 0) {
    _completeDiv.removeChild(_completeDiv.childNodes[0]);
  }
  setCompleteDivSize(i);
  // mise en place des suggestions
  _currentList = liste;
  for (var f = 0; f < liste.length; ++f) {
    var nouveauDiv = document.createElement("DIV");
    if (!legacyRoutePlannerConfig.routePlannerFormOneField || (liste[f][3] != null)) {
      nouveauDiv.onmousedown = divOnMouseDown;
      nouveauDiv.onmouseover = divOnMouseOver;
      nouveauDiv.onmouseout = divOnMouseOut;
      setClassForElement(nouveauDiv, "autoCompleteDiv");
    } else {
       setClassForElement(nouveauDiv, "autoCompleteDivTitle");
    }
    var nouveauSpan = document.createElement("SPAN");
    nouveauSpan.innerHTML = liste[f][0]; // le texte de la suggestion
    nouveauDiv.appendChild(nouveauSpan);
    _completeDiv.appendChild(nouveauDiv)
  }
  PressAction(i);
  if (_completeDivRows > 0) {
    _completeDiv.height = 16 * _completeDivRows+4;
    _enterIsForbidden = true;
  } else {
    hideList();
  }
  _currentInputField = i;
}

function hideList() {
  _enterIsForbidden = false;
  hideCompleteDiv();
}

function isListVisible() {
  return _completeDiv && _completeDiv.style && _completeDiv.style.display == 'block';
}

var _eventKeycode = null;


function autoCompleteKeyUp(event) {
  // accès evenement compatible IE/Firefox
  if (!event && window.event) {
    event = window.event;
  }
  if (_timeOut) {
    clearTimeout(_timeOut);
    _timeOut = null;
  }

  inputField = this; //event.target?event.target:event.srcElement;
  var i;
  for (i = 0; i < _inputField.length; ++i)
    if (_inputField[i] == inputField)
      break;

  var command = "handleAutoComplete(" + event.keyCode + ", " + i + ")";
  if (event.keyCode == 40 || event.keyCode == 38 || event.keyCode == 13 || event.keyCode == 27)
    eval(command);
  else
    _timeOut = setTimeout(command, 500);
}

function handleAutoComplete(keyCode, i) {
  _timeOut = null;
  var inputField = _inputField[i];
  var cityId = null;
  if (_secondaryField[i] && !legacyRoutePlannerConfig.routePlannerFormOneField) {
    for (cityId = 0; cityId < _inputField.length; ++cityId)
      if (_inputField[cityId] == _secondaryField[i])
        break;
  }

  if (legacyRoutePlannerConfig.routePlannerFormOneField) {
    _secondaryField[i].value = '';
  }

  // On input value change
  if ((inputField.value != _currentInputFieldValue[i] || (keyCode == 40 && !isListVisible())) &&
      inputField.value.length > legacyRoutePlannerConfig.minCharsToTriggerAutocomplete) {
    var valeur = escapeURI(inputField.value);
    callSuggestions(i, valeur); // appel distant
    _currentInputFieldValue[i] = inputField.value;
  }
  // Key down
  if (keyCode == 40 && isListVisible()) {
    if (_highlightedSuggestionIndex == -1 ||
        (_highlightedSuggestionIndex == _completeDiv.getElementsByTagName("div").length - 1))
      highlight(0);
    else
      highlight(_highlightedSuggestionIndex + 1);
  }

  // Key up
  if (keyCode == 38 && isListVisible()) {
    if (_highlightedSuggestionIndex <= 0)
      highlight(_completeDiv.getElementsByTagName("div").length - 1);
    else
      highlight(_highlightedSuggestionIndex - 1);
  }

  // Enter
  if (keyCode == 13 && isListVisible()) {
    if (cityId != null && _inputField[cityId].value == '') {
      setAndJump(i, _currentList[_highlightedSuggestionIndex][2], cityId, _currentList[_highlightedSuggestionIndex][1]);
    } else {
      setAndJump(i, getSuggestion(_highlightedSuggestionDiv));
    }
  }

  // Esc
  if (keyCode == 27 && isListVisible()) {
    hideList();
  }
}

function setAndJump(id, value, idCity, cityValue) {
  if (idCity != null) {
    _inputField[idCity].value = cityValue;
  }
  _inputField[id].value = value;

  if (legacyRoutePlannerConfig.routePlannerFormOneField) {
    _secondaryField[id].value = _currentList[_highlightedSuggestionIndex][3];
  }

  hideList();

  var next;
  for (next = 0; _documentForm.elements[next] != _inputField[id]; ++next);
  if (_fieldToCleanAtUpdate[id])
    _fieldToCleanAtUpdate[id].value = "";

  for (++next; next < _documentForm.elements.length && _documentForm.elements[next].type != "text"; ++next);
  if (next < _documentForm.elements.length) {
    _documentForm.elements[next].focus();
    _documentForm.elements[next].select();
  }
}

function highlight(n) {
  _highlightedSuggestionIndex = n;
  var suggestionList = _completeDiv.getElementsByTagName("div");
  _highlightedSuggestionDiv = suggestionList.item(_highlightedSuggestionIndex);
  var suggestionLongueur = suggestionList.length;
  for (var l = 0; l < suggestionLongueur; ++l) {
    if (getClassForElement(suggestionList.item(l)) != "autoCompleteDivTitle")
      setClassForElement(suggestionList.item(l), "autoCompleteDiv");
  }
  if ((_highlightedSuggestionDiv) && (getClassForElement(_highlightedSuggestionDiv) != "autoCompleteDivTitle"))
    setClassForElement(_highlightedSuggestionDiv, "autoCompleteDivAct");
}


var _completeDivRows = 0;
var _completeDivDivList = null;
var _highlightedSuggestionIndex = -1;
var _highlightedSuggestionDiv = null;

// gère une touche pressée autre que haut/bas/enter
function PressAction(i) {
  _highlightedSuggestionIndex = -1;
  var suggestionList = _completeDiv.getElementsByTagName("div");
  var suggestionLongueur = suggestionList.length;
  // on stocke les valeurs précédentes
  // nombre de possibilités de complétion
  _completeDivRows = suggestionLongueur;
  // possiblités de complétion
  _completeDivDivList = suggestionList;
  // si le champ est vide, on cache les propositions de complétion
  if (_currentInputFieldValue[i] == "" || suggestionLongueur == 0) {
    hideCompleteDiv()
  } else {
    showCompleteDiv(i)
  }
  var trouve = false;
  if (legacyRoutePlannerConfig.routePlannerFormOneField) {
    if (suggestionLongueur > 1)
      highlight(1);
  } else {
    // si on a du texte sur lequel travailler
    if (_currentInputFieldValue[i].length > 0) {
      var indice;
      // T vaut true si on a dans la liste de suggestions un mot commencant comme l'entrée utilisateur
      for (indice = 0; indice < suggestionLongueur; indice++) {
        if (getSuggestion(suggestionList.item(indice)).toUpperCase().indexOf(_currentInputFieldValue[i].toUpperCase()) == 0) {
          trouve = true;
          break
        }
      }
    }
    highlight(0);
  }
}

// taille de la selection dans le champ input
function rangeSize(n) {
  var N = -1;
  if (n.createTextRange) {
    var fa = document.selection.createRange().duplicate();
    N = fa.text.length
  } else if (n.setSelectionRange) {
    N = n.selectionEnd - n.selectionStart
  }
  return N;
}

// taille du champ input non selectionne
function beforeRangeSize(n) {
  var v = 0;
  if (n.createTextRange) {
    var fa = document.selection.createRange().duplicate();
    fa.moveEnd("textedit", 1);
    v = n.value.length-fa.text.length
  } else if (n.setSelectionRange) {
    v = n.selectionStart
  } else {
    v = -1
  }
  return v;
}


// Retourne la valeur de la possibilite (texte) contenu dans une div de possibilite
function getSuggestion(uneDiv) {
  if (!uneDiv) {
    return null;
  }
  return trimCR(uneDiv.getElementsByTagName('span')[0].firstChild.data);
}

// supprime les caractères retour chariot et line feed d'une chaine de caractères
function trimCR(chaine) {
  for (var f = 0, nChaine = "", zb = "\n\r"; f < chaine.length; f++) {
    if (zb.indexOf(chaine.charAt(f)) == -1) {
      nChaine += chaine.charAt(f);
    }
  }
  return nChaine;
}

// Cache completement les choix de completion
function hideCompleteDiv() {
  _completeDiv.style.display = "none"
}

// Rends les choix de completion visibles
function showCompleteDiv(i) {
  _completeDiv.style.display = "block";
  setCompleteDivSize(i)
}


// Handler de resize de la fenetre
function onResizeHandler(event) {
  // recalcule la taille des suggestions
  //for (i = 0; i < _inputField.length; ++i)
  //  setCompleteDivSize(i);
}

// Handler de blur sur le champ texte
function onBlurHandler(event) {
  hideList();
};

// declenchee quand on clique sur une div contenant une possibilite
function divOnMouseDown(i) {
  _highlightedSuggestionDiv = this;
  handleAutoComplete(13, _currentInputField);
};

// declenchee quand on passe sur une div de possibilite. La div précédente est passee en style normal
function divOnMouseOver() {
  var highlightedIndex = 0, e = this;
  while (e = e.previousSibling)
    highlightedIndex++;

  highlight(highlightedIndex);
};

// declenchee quand la sourie quitte une div de possiblite. La div repasse a l'etat normal
function divOnMouseOut() {
  setClassForElement(this, "autoCompleteDiv");
};



// Table column highlight and click ------------------------------------------------------------------------------------------------------

function mouse_click_handler(e) {

  e = e || window.event;
  var cell = e.srcElement || e.target;

  var tname = (cell.nodeType == 1) ? cell.tagName.toLowerCase() : '';

  while(tname != "table" && tname !="td" && tname != "th") {
    cell = cell.parentNode || cell.parentElement;
    tname = cell.tagName.toLowerCase();
  }

  if (tname == "td" || tname == "th") {
    var newClass;
    var cellIdx = _getCellIndex(cell);
    var row = cell.parentNode || cell.parentElement;
    var rowIdx = _getRowIndex(row);

    if (cellIdx > 0 && cell.className != 'completion') {
      showJourneyBoard(cellIdx);
    }
  }
}

function mouse_event_handler(e) {

  e = e ||  window.event;
  var cell = e.srcElement || e.target;

  var tname = (cell.nodeType == 1) ? cell.tagName.toLowerCase() : '';

  while (tname != "table" && tname != "td" && tname != "th") {
    cell = cell.parentNode || cell.parentElement;
    tname = cell.tagName.toLowerCase();
  }

  if (tname == "td" || tname == "th") {
    var newClass;

    var cellIdx = _getCellIndex(cell);
    var row = cell.parentNode || cell.parentElement;
    var rowIdx = _getRowIndex(row);

    if (cellIdx > 0 && cell.className != 'completion') {
      _setCol();
    }
  }

  function _getTable() {
    var tbleObj;

    if (mouse_event_handler.previous.table) {
      return;
    } else {
      tbleObj = row.parentNode || row.parentElement; // tbody
      var tn = tbleObj.tagName.toLowerCase();
      while (tn != "table" && tn != "html") {
        tbleObj = tbleObj.parentNode || tbleObj.parentElement;
        tn = tbleObj.tagName.toLowerCase();
      }
      mouse_event_handler.previous.table = tbleObj;
    }
  }

  function _clearHighlight() {
    _clearRow();
    _clearCol();

    mouse_event_handler.previous.row = null;
    mouse_event_handler.previous.cellIdx = null;
  }

  function _clearRow() {
    if (mouse_event_handler.previous.row) {
      mouse_event_handler.previous.row.className = "";
      mouse_event_handler.previous.row.cells[0].className = "";
    }
  }

  function _setRow() {
    _clearRow();
    if (tname == 'td' || mouse_event_handler.previous.row != row) {
      row.className = 'hlt';
      row.cells[0].className = 'hlt';
      mouse_event_handler.previous.row = row;
    } else {
      mouse_event_handler.previous.row = null;
    }
  }

  function _clearCol() {
    _getTable();
    if (mouse_event_handler.previous.cellIdx != null) {
      var table = mouse_event_handler.previous.table;
      var cell = mouse_event_handler.previous.cellIdx;
      for (var i = 0; i < table.rows.length; i++) {
        table.rows[i].cells[cell].className = '';
      }
    }
  }

  function _setCol () {
    _clearCol();
    if (tname == 'td' || mouse_event_handler.previous.cellIdx != cellIdx) {
      mouse_event_handler.previous.table.rows[0].cells[cellIdx].className = 'hlt';
      var trs = mouse_event_handler.previous.table.rows;
      for (var i = 0; i < trs.length; i++) {
        trs[i].cells[cellIdx].className = 'hlt-col';
      }
      mouse_event_handler.previous.cellIdx = cellIdx;
    } else {
      mouse_event_handler.previous.cellIdx = null;
    }
  }

}

function _getRowIndex(row) {
  var rtrn = row.rowIndex || 0;

  if (rtrn == 0) {
    do {
        if (row.nodeType == 1)
          rtrn++;
        row = row.previousSibling;
    } while (row);
    --rtrn;
  }
  return rtrn;
}

function _getCellIndex(cell) {
  var rtrn = cell.cellIndex || 0;

  if (rtrn == 0) {
    do {
      if (cell.nodeType == 1) rtrn++;
      cell = cell.previousSibling;
    } while (cell);
    --rtrn;
  }
  return rtrn;
}

mouse_event_handler.previous = {cellIdx: null, row: null, table: null};

function addEvent(obj, event_name, fnc) {
  if (typeof obj == "undefined")
    return;
  else if (obj.attachEvent)
    obj.attachEvent("on" + event_name, fnc);
  else if (obj.addEventListener)
    obj.addEventListener(event_name, fnc, false);
  else
    obj["on" + event_name] = fnc;

}


window.onunload = function () {
  mouse_event_handler.previous = null;
}


// Ajax generic interactivity ----------------------------------------------------------------------------------------------------------------------------

function ajaxFullfill(divObject, url) {
  if (_xmlHttp && _xmlHttp.readyState != 0) {
    _xmlHttp.abort()
  }
  _xmlHttp = getXMLHTTP();
  if (_xmlHttp) {
    _xmlHttp.open("GET", url, true);
    _xmlHttp.onreadystatechange = function() {
      if (_xmlHttp.readyState == 4) {
        eval("divObject.innerHTML ='" + _xmlHttp.responseText + "';");
      }
    };
    _xmlHttp.send(null)
  }
}

function getURLFromForm(form) {
  var url = form.action + "?";
  var first = true;
  for (var i = 0; i < form.elements.length; ++i)
    if (form.elements[i].name) {
      url += (!first ? "&" : "") + form.elements[i].name + "=" + form.elements[i].value;
      first = false;
    }
  return url;
}

function ajaxRunUrl(url, resultAnalyzeFunction, functionToRunIfOk, functionToRunIfKo) {
  if (_xmlHttp && _xmlHttp.readyState != 0) {
    _xmlHttp.abort()
  }
  _xmlHttp = getXMLHTTP();
  if (_xmlHttp) {
    _xmlHttp.open("GET", url, true);
    _xmlHttp.onreadystatechange = function() {
      if (_xmlHttp.readyState == 4) {
        if (resultAnalyzeFunction(_xmlHttp.responseText))
          functionToRunIfOk(_xmlHttp.responseText);
        else
          functionToRunIfKo();
      }
    };

    // envoi de la requete
    _xmlHttp.send(null);
  }
  return false;
}

function ajaxSendFormAndRun(form, resultAnalyzeFunction, functionToRunIfOk, functionToRunIfKo) {
  var url = getURLFromForm(form);
  return ajaxRunUrl(url, resultAnalyzeFunction, functionToRunIfOk, functionToRunIfKo);
}

function ajaxSendForm(divObject, form) {
  var url = getURLFromForm(form);
  ajaxFulFill(divObject, url);
  return false;
}

// Route planning parameters backup ----------------------------------------------------------------------------------------------------------------------------

var _originCityName;
var _originPlaceName;
var _destinationCityName;
var _destinationPlaceName;
var _accessibility;
var _site;

function initRoutePlanningParametersBackup(originCityName, originPlaceName, destinationCityName, destinationPlaceName, accessibility, site) {
  _originCityName = originCityName;
  _originPlaceName = originPlaceName;
  _destinationCityName = destinationCityName;
  _destinationPlaceName = destinationPlaceName;
  _accessibility = accessibility;
  _site = site;
}

// Ajax login ----------------------------------------------------------------------------------------------------------------------------

var _userName;
var _userPhone;
var _userEMail;
var _userId;
var _sessionId;

function initSession(sessionId, userName, userPhone, userEMail, userId) {
  _userName = userName;
  _userPhone = userPhone;
  _sessionId = sessionId;
  _userId = userId;
  _userEMail = userEMail;
}

function ajaxLoginSuccess(xmlSession) {
  var docXML = XMLParse(xmlSession);
  var acceptedElements = docXML.getElementsByTagName('session');
  if (!acceptedElements.length || !acceptedElements[0].firstChild || !acceptedElements[0].firstChild.data)
  {  // login failed
    return false;
  } else {
    var userNameElements = docXML.getElementsByTagName('name');
    var userPhoneElements = docXML.getElementsByTagName('phone');
    var userEMailElements = docXML.getElementsByTagName('email');
    var userIdElements = docXML.getElementsByTagName('user_id');
    initSession(
      acceptedElements[0].firstChild.data
      , userNameElements[0].firstChild.data
      , userPhoneElements[0].firstChild ? userPhoneElements[0].firstChild.data : ""
      , userEMailElements[0].firstChild ? userEMailElements[0].firstChild.data : ""
      , userIdElements[0].firstChild.data
    );
    return true;
  }
}

function ajaxLogout(functionToRun) {
  initSession("", "", "", "");
  return true;
}



// Fake popup ----------------------------------------------------------------------------------------------------------------------------

var fadeDiv;

function initLightBox() {
  // FIXME: with IE7 the lightBox div stands above the popup (it works with IE7 elsewhere).
  if (document.body.className.indexOf("ie7OrLess") != -1)
    return;

  if (fadeDiv)
    return;

  fadeDiv = document.createElement("DIV");
  fadeStyle = "display: block; visibility: hidden; position: fixed; top: 0%; left: 0%; width: 100%; height: 100%; background-color: black; z-index: 10001; -moz-opacity: 0.8; -khtml-opacity:0.7; opacity: .80; filter: alpha(opacity=80);";
  fadeDiv.id = 'fade';
  fadeDiv.setAttribute("style", fadeStyle);
  fadeDiv.style.cssText = fadeStyle;
  document.body.appendChild(fadeDiv);
}

function showPopup(id) {
  hidePopup();
  document.getElementById(id).style.display = 'block';
  if (fadeDiv)
    fadeDiv.style.visibility = 'visible';
  document.getElementById('routesMap').style.display = 'block';
}

function hidePopup() {
  initLightBox();
  var aElm = document.body.getElementsByTagName('div');
  for (var i = 0; i < aElm.length; i++)
    if (aElm[i].className == 'popup_content')
      aElm[i].style.display = 'none';
  if (fadeDiv)
    fadeDiv.style.visibility = 'hidden';
  document.getElementById('routesMap').style.display = 'none';
}

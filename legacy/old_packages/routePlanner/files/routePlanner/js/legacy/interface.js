// Return -------------------------------------------------------------------------------------------------

function retour()
{
  var opt = document.getElementById("origin_place_txt").value;
  var dpt = document.getElementById("destination_place_txt").value;
  document.getElementById("origin_place_txt").value = dpt;
  document.getElementById("destination_place_txt").value = opt;
  
  if(legacyRoutePlannerConfig.routePlannerFormOneField) {
    var oct = document.getElementById("origin_class_txt").value;
    var dct = document.getElementById("destination_class_txt").value;
    document.getElementById("origin_class_txt").value = dct;
    document.getElementById("destination_class_txt").value = oct;
  }
  else {
    var oct = document.getElementById("origin_city_txt").value;
    var dct = document.getElementById("destination_city_txt").value;
    document.getElementById("origin_city_txt").value = dct;
    document.getElementById("destination_city_txt").value = oct;
  }
  
  document.getElementById("origin_city_txt").form.submit();
  return 0;
}

// Journey boards -----------------------------------------------------------------------------------------

var _departureTimes = new Array();
var curJourneyBoard = 0;

function showJourneyBoard(n)
{
  curJourneyBoard = n;
  showPopup('jb' + n);

  if (document.getElementById('reservationFormDiv' + n)) {
    document.getElementById('reservationFormDiv' + n).innerHTML = _sessionId
      ? getReservationForm()
      : "Vous devez vous identifier pour réserver.<br />" + getHorizontalLoginForm("displayReservationForm","displayReservationLoginError");
    if (document.getElementById('reservationFormPassword'))
      document.getElementById('reservationFormPassword').focus();
  }
  app.displaySolution(n - 1);
}


// Reservation form -----------------------------------------------------------------------------------------

function ajaxResa(text)
{
  var docXML = XMLParse(text);
  var acceptedElements = docXML.getElementsByTagName('reservation');
  if (!acceptedElements.length || !acceptedElements[0].firstChild || acceptedElements[0].firstChild.data <= 0)
  {
    // Reservation failed
    return false;
  } else {
    return true;
  }
}

function getReservationFormDiv()
{
  return document.getElementById('reservationFormDiv' + curJourneyBoard);
}


function reservationOK()
{
  getReservationFormDiv().innerHTML = "<b>Réservation effectuée.</b><br />";
}

function reservationFail()
{
  getReservationFormDiv().innerHTML = '<span class="alertMessage">Erreur, réservation refusée.</span><br />'
    + getReservationForm();
  document.getElementById('reservationFormPassword').focus();
}

function displayReservationForm()
{
  getReservationFormDiv().innerHTML = getReservationForm();
  mainLoginSuccess();
}

function displayReservationLoginError()
{
  getReservationFormDiv().innerHTML = "Utilisateur ou mot de passe incorrect.<br />"
    + getHorizontalLoginForm("displayReservationForm","displayReservationLoginError");

}

function getReservationForm()
{
  var html =
    '<form action="synthese3" onsubmit="return ajaxSendFormAndRun(this,ajaxResa,reservationOK,reservationFail);">'
    + '<input type="hidden" name="a" value="bra" />'
    + '<input type="hidden" name="fonction" value="XMLReservationFunction" />'
    + '<input type="hidden" name="sid" value="'+ _sessionId +'" />'
    + '<input type="hidden" name="nr" value="1" />'
    + '<input type="hidden" name="actionParamcuid" value="' + _userId + '" />'
    + '<input type="hidden" name="actionParamsit" value="'+ _site +'" />'
    + '<input type="hidden" name="actionParamacc" value="'+ _accessibility +'" />'
    + '<input type="hidden" name="actionParamdct" value="'+ _originCityName +'" />'
    + '<input type="hidden" name="actionParamdpt" value="'+ _originPlaceName +'" />'
    + '<input type="hidden" name="actionParamact" value="'+ _destinationCityName +'" />'
    + '<input type="hidden" name="actionParamapt" value="'+ _destinationPlaceName +'" />'
    + '<input type="hidden" name="actionParamda" value="' + _departureTimes[curJourneyBoard] + '" />'
    + '<b>Réservation au nom de ' + _userName + ':</b><br />'
    + 'Contact : '+ _userPhone +' / '+ _userEMail + '<br />'
    + 'Places : <input type="text" name="actionParamsenu" class="resaSeats" value="1" /> '
    + '<input type="submit" value="Réserver" /></form>';

  return html;
}



// Horizontal Login / Logout Forms -----------------------------------------------------------------------------------------

function getHorizontalLoginForm(successFunctionName,failFunctionName)
{
  var html =
    '<form action="synthese3" onsubmit="return ajaxSendFormAndRun(this,ajaxLoginSuccess,'+ successFunctionName +','+ failFunctionName +');">'
    + '<input type="hidden" name="nr" value="1" />'
    + '<input type="hidden" name="a" value="login" />'
    + '<input type="hidden" name="fonction" value="XMLSessionFunction" />'
    + 'Utilisateur : <input type="text" name="actionParamlogin" /> Mot de passe : <input type="password" name="actionParampwd" />'
    + '<input type="submit" value="OK" /></form>';
  return html;
}

function getHorizontalLogoutForm(functionName)
{
  var html =
    '<form action="synthese3" onsubmit="return ajaxSendFormAndRun(this,ajaxLogout,'+ functionName +','+ functionName +');">'
    + '<input type="hidden" name="nr" value="1" />'
    + '<input type="hidden" name="a" value="logout" />'
    + '<input type="hidden" name="fonction" value="XMLSessionFunction" />'
    + '<input type="hidden" name="sid" value="' + _sessionId + '" />'
    + 'Utilisateur connecté : '+ _userName +' <input type="submit" value="Déconnecter" /></form>'
    ;
  return html;
}


function mainLogout()
{
  document.getElementById("mainLoginP").innerHTML = getHorizontalLoginForm("mainLoginSuccess","mainLoginFailure")
    + ' <span id="mainLoginFailure"></span>';
  // displayFavoriteBar();
}

function mainLoginSuccess()
{
  document.getElementById("mainLoginP").innerHTML = getHorizontalLogoutForm("mainLogout");
  // displayFavoriteBar();
}

function displayFavoriteBar()
{
  if (_sessionId > 0) {
    var url = "synthese3?fonction=page&page=favorites_row&i=2";
    ajaxFullfill(document.getElementById("favorites"), url);
    document.getElementById("favorites").style.display = 'block';
  } else {
    document.getElementById("favorites").innerHTML = "";
    document.getElementById("favorites").style.display = 'none';
  }
}

function addToFavorites()
{
  var url = "synthese3?fonction=page&page=ajax_login_response&a=add_favorite_journey&uid" + _userId
    + "&ocn=" + document.getElementByid("origin_city_txt").value
    + "&opn=" + document.getElementByid("origin_place_txt").value
    + "&dcn=" + document.getElementByid("destination_city_txt").value
    + "&dpn=" + document.getElementByid("destination_place_txt").value;
  ajaxRunUrl(url, function(){return true;}, displayFavoriteBar, displayFavoriteBar);
}

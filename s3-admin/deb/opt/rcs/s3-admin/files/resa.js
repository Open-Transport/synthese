
var customerUpdateTimer = 0;

function programCustomerUpdate(formName, newCustomerFieldName, divId, fieldName, url)
{
    clearTimeout(customerUpdateTimer);
    customerUpdateTimer = setTimeout("customerUpdate('"+ formName +"','"+ newCustomerFieldName +"','"+ divId +"','"+ fieldName +"','"+ url +"');", 500);
}

function customerUpdate(formName,newCustomerFieldName,divId,fieldName, url)
{
    if (window.XMLHttpRequest)                 //  Objet de la fenêtre courant
    { 
        xhr = new XMLHttpRequest();     //  Firefox, Safari, ...
    } 
    else 
    if (window.ActiveXObject)                    //  Version Active
    {
        xhr = new ActiveXObject("Microsoft.XMLHTTP");   // Internet Explorer 
    }
    
    xhr.onreadystatechange = function()
    {
        if (xhr.readyState == 4)
        {
            radios = document.forms[formName][newCustomerFieldName];
            if (xhr.responseText == '')
            {
				document.getElementById(divId).innerHTML = "";
				for (i = 0; i < radios.length; ++i)
				{
                    if (radios[i].value == 1)
						radios[i].checked = true;
					if (radios[i].value == 0)
						radios[i].disabled = true;
				}
			}
			else
			{
	            document.getElementById(divId).innerHTML = 
					' : <select name="'+ fieldName +'">'
					+ xhr.responseText
					+ '</select>'
					;
				var n = document.forms[formName][fieldName].options.length;
				document.getElementById(divId).innerHTML 
					+= " (" + n +" client"+ (n>1?"s":"") +" trouvé"+ (n>1?"s":"") +")";
				
				for (i = 0; i < radios.length; ++i)
                {
                    if (radios[i].value == 0)
						radios[i].checked = true;
                    radios[i].disabled = false;
		        }
            }
        } else { 
            // Attendre... 
        }
    };
    xhr.open('GET', url, true);                  
    xhr.send(null);
}


var clientPageLoadDate = new Date();
var durationSinceCallStart = 0;

function updateCallStopwatch()
{
	var curDate = new Date();
	var stampBetweenNowAndLoad =
		curDate.getHours()*3600 + curDate.getMinutes()*60 + curDate.getSeconds()
		 - clientPageLoadDate.getHours()*3600
		 - clientPageLoadDate.getMinutes()*60 - clientPageLoadDate.getSeconds() + durationSinceCallStart;
						
	var minutes = Math.floor(stampBetweenNowAndLoad / 60);
	var secondes = stampBetweenNowAndLoad - 60 * Math.floor(stampBetweenNowAndLoad / 60);
	document.getElementById("calldurationspan").innerHTML = minutes + ":" + (secondes < 10 ? "0" : "") + secondes;
	setTimeout("updateCallStopwatch();", 100);
}

var customerUpdateTimer = 0;

function programCustomerUpdate(form, selectID, url)
{
    clearTimeout(customerUpdateTimer);
    customerUpdateTimer = setTimeout("customerUpdate('"+ form +"','"+ selectID +"','"+ url +"');", 500);
}

function customerUpdate(form,selectID,url)
{
    if (window.XMLHttpRequest)
    { 
        xhr = new XMLHttpRequest();
    } 
    else if (window.ActiveXObject)
    {
        xhr = new ActiveXObject("Microsoft.XMLHTTP");
    }
    
    xhr.onreadystatechange = function()
    {
        if (xhr.readyState == 4)
        {
            if (xhr.responseText == '')
            {
		$("#"+selectID).html("");
		$(".result").html("Aucun résultat.");
	    }
	    else
	    {
	        $("#"+selectID).html(xhr.responseText);
		var n = $("#"+selectID).children().length;
		$(".result").html(n+" client"+ (n>1?"s":"") +" trouvé"+ (n>1?"s":""));
            	}
        } else { 
            // Attendre... 
        }
    };
    xhr.open('GET', url, true);                  
    xhr.send(null);
}
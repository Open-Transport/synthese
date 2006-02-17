var URL = '/synthese/mainCGIPreFork.fcp?';

function reservation()
{
	// controle de saisie des champs obligatoires
	// si la variable hidden check[Champ] est présente, on doit rentrer un valeur pour [Champ]
	if (document.saisie.checkNom !=null && document.saisie.Nom.value == '')
		alert("La saisie d'un nom est obligatoire!");
	else if (document.saisie.checkPrenom !=null && document.saisie.Prenom.value == '')
		alert("La saisie d'un prénom est obligatoire!");
	else if (document.saisie.checkAdresse !=null && document.saisie.Adresse.value == '')
		alert("La saisie d'une adresse postale est obligatoire!");
	else if (document.saisie.checkEmail !=null && document.saisie.Email.value == '')
		alert("La saisie d'une adresse email est obligatoire!");
	else if (document.saisie.checkTelephone !=null && document.saisie.Telephone.value == '')
		alert("La saisie d'un numéro de téléphone est obligatoire!");		
	else if (document.saisie.checkNumAbonne !=null && document.saisie.NumAbonne.value == '')
		alert("La saisie d'un numéro d'abonné est obligatoire!");
	else if (document.saisie.checkNbPlaces !=null && document.saisie.NbPlaces.value == '')
		alert("La saisie du nombre de places réservées est obligatoire!");
	else if (document.saisie.checkAdresseDepart !=null && document.saisie.AdresseDepart.value == '')
		alert("La saisie de l'adresse de départ est obligatoire!");
	else if (document.saisie.checkAdresseArrivee !=null && document.saisie.AdresseArrivee.value == '')
		alert("La saisie de l'adresse d'arrivée est obligatoire!");	
	else			
		document.saisie.submit();							
}

function annulation()
{
	// controle de saisie des champs obligatoires
	if (document.saisie.Nom.value == '')
		alert("La saisie d'un nom est obligatoire!");
	else if (document.saisie.NumResa  == '')
		alert("La saisie d'un numéro de réservation est obligatoire!");	
	else			
		document.saisie.submit();							
}


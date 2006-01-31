var URL = '/synthese/mainCGIPreFork.fcp?';

function reservation()
{
	// controle de saisie des champs obligatoires
	// si la variable hidden check[Champ] est pr�sente, on doit rentrer un valeur pour [Champ]
	if (document.saisie.checkNom !=null && document.saisie.Nom.value == '')
		alert("La saisie d'un nom est obligatoire!");
	else if (document.saisie.checkPrenom !=null && document.saisie.Prenom.value == '')
		alert("La saisie d'un pr�nom est obligatoire!");
	else if (document.saisie.checkAdresse !=null && document.saisie.Adresse.value == '')
		alert("La saisie d'une adresse postale est obligatoire!");
	else if (document.saisie.checkEmail !=null && document.saisie.Email.value == '')
		alert("La saisie d'une adresse email est obligatoire!");
	else if (document.saisie.checkTelephone !=null && document.saisie.Telephone.value == '')
		alert("La saisie d'un num�ro de t�l�phone est obligatoire!");		
	else if (document.saisie.checkNumAbonne !=null && document.saisie.NumAbonne.value == '')
		alert("La saisie d'un num�ro d'abonn� est obligatoire!");
	else if (document.saisie.checkNbPlaces !=null && document.saisie.NbPlaces.value == '')
		alert("La saisie du nombre de places r�serv�es est obligatoire!");
	else if (document.saisie.checkAdresseDepart !=null && document.saisie.AdresseDepart.value == '')
		alert("La saisie de l'adresse de d�part est obligatoire!");
	else if (document.saisie.checkAdresseArrivee !=null && document.saisie.AdresseArrivee.value == '')
		alert("La saisie de l'adresse d'arriv�e est obligatoire!");	
	else			
		document.saisie.submit();							
}

function annulation()
{
	// controle de saisie des champs obligatoires
	if (document.saisie.Nom.value == '')
		alert("La saisie d'un nom est obligatoire!");
	else if (document.saisie.NumResa  == '')
		alert("La saisie d'un num�ro de r�servation est obligatoire!");	
	else			
		document.saisie.submit();							
}


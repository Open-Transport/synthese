#include "Functions.h"
/*
	get the menu text with category, step, language given
	@parameters:
		int: language choiced
			1, french
			
		int: category,
			1, login
			
			
		int: step number
		
	@return:
		string: menu text
*/
string Functions::getMenu(int _category, int _step)
{
	language=1;
	switch(Functions::getLanguage())
	{
		case 1:	// french
		
			switch(_category)
			{
				case 0:	// main menu
					switch(_step)
					{
						case 0:
							return "dial 1 for english, 2 pour le francais, 3 fuer Deutsch, 4 per italien, 5 para espanol.";
						case 1:
							return "merci de votre utilisation, au revoir";
					}

				case 1:	// login
					switch(_step)
					{
						case 1: // welcome
							return "Bienvenu au système de reservation";
						case 2: // input usr name
							return "Veuillez introduire 9 chiffres du numero d\'utilisateur, ou presser # pour la reception manuelle.";
						case 3:	// input psw
							return "Veuillez entrer 6 chiffres du mots de passe";
						case 4: // driver menu
							return "pressez 8 pour consulter la liste de reservation et pressez 9 pour reserver un trajet entant qu\'un client";
						case 5: // say how much reservation
							return "le total de reservations enregistrées est ";
						case 6: // ask key input
							return "veuillez entrer 7 pour consulter la liste des reservations effectuées et 9 pour commencer une nouvelle recherche";
						case 7: // usr or psw incrroct
							return "Le nom d'utilisateur ou mots de passe incorrects, veuilez le verifier. Pressez 0 pour la reception manuelle.";
						case 8: // close session
							return "L'entrée invalide, la session sera fermée. Merci de votre visite, au revoir.";
						default:
							return NULL;
					}
					break;
					
				case 2:	// search
					switch(_step)
					{
						case 1:	// favoris list
							return "veuillez introduire un numero des favoris de trajets suivants:";
						case 2:	// favoris list
							return "veuillez tapez directment le numero de trajet pour une reservation:";
					}
					break;
				case 3:	// reservation
					switch(_step)
					{
						case 1:	// confirmation
							return "pressez 1 pour oui et 3 pour non";
						case 2:	// favoris list
							return "Nous vous informons que votre reservation est effectuee avec succes.";
						case 3: // menukey 4,5,6,9,0
							return "4 pour le precedent, 6 pour le prochain et 5 pour repeter le trajet, pressez 9 pour les 3 prochaines reservations enregistrees et 0 pour sortir";
						case 4: // without delete
							return "aucune reservation effectuee, veuillez contacter operator";
						case 5: // driver no delete right
							return "vous n'avez pas le droit d'effacement";
						case 6: // resa deleted
							return "reservation est effacee";
			
					}
					break;
			
			}
			break;
	}
	return NULL;


}

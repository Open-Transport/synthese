
#include "cProfil.h"

/*

*/
class cSession
{
	cTexte		_ID;
	tAdresseIP	_IP;
	cProfil*	_Proprietaire;
	cCalculateur*	_CalculateurOccupe;
	bool		_CouperCalcul;
	cSite*		_Site;

public:
	const cProfil*	getProprietaire();

	//!	Injecter ce test à chaque recursion de calculitineraire
	bool	CalculPeutContinuer();

	/*!	Controle si un calculateur est occupe si oui coupe le calcul */
	bool	ExecuteRequete(const cTexteRequete&);


	//!	\name Actions
	bool	ModifieProfil(cProfil*, cTexte __User, cTexte __Password);

	bool	AnnuleReservation(cReservation&);

};

bool cSession::ExecuteRequete(const cTexteRequete& __Requete, tAdresseIP __IP)
{
	if (__IP != _IP)
		return false;

	_Site->ExecuteRequete(__Requete);
}
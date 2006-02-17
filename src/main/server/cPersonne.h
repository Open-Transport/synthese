
#include "cTexte.h"

class cPersonne
{
		//!	\name Coordonnées de la personne
	//@{
	cTexte							
_Prenom;				//!< Prénom du client
	cTexte							_Nom;	
				//!< Nom du client
	cTexte							_Adresse;	
			//!< Adresse du client
	cTexte							_NumeroAbonne;		
	//!< Numéro de l'abonnement du client
	cTexte							_Login;			
		//!< Login du client
	cTexte							_Telephone;			
	//!< Numéro de téléphone du client
	cTexte							_EMail;			
		//!< Adresse e-mail
	//@}
	


public:

//!	\name Accesseurs
	//@{
	const cTexte&	GetCode();
	
const cLigne*	GetLigne();
	const cTexte&	GetCodeService();
	const cTexte&	GetPrenom();
	const cTexte&	
GetNom();
	const cTexte&	GetAdresse();
	const cTexte&	GetNumeroAbonne();
	const cTexte&	GetTelephone();
	
const cTexte&	GetEMail();
};


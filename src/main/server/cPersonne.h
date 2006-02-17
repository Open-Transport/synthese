
#include "cTexte.h"

class cPersonne
{
		//!	\name Coordonn�es de la personne
	//@{
	cTexte							
_Prenom;				//!< Pr�nom du client
	cTexte							_Nom;	
				//!< Nom du client
	cTexte							_Adresse;	
			//!< Adresse du client
	cTexte							_NumeroAbonne;		
	//!< Num�ro de l'abonnement du client
	cTexte							_Login;			
		//!< Login du client
	cTexte							_Telephone;			
	//!< Num�ro de t�l�phone du client
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


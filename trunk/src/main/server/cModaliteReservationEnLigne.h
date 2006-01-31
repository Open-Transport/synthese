/** En-t�te modalit� de r�servation avec possibilit� de r�server en ligne
	@file cModaliteReservationEnLigne.h
*/


#ifndef SYNTHESE_CMODALITERESERVATIONENLIGNE_H
#define SYNTHESE_CMODALITERESERVATIONENLIGNE_H


class cModaliteReservationEnLigne;


#include "cModaliteReservation.h"


/**	Modalit� de r�servation permettant la r�servation en ligne
	\author Hugues Romain
	\date 2005
*/
class cModaliteReservationEnLigne : public cModaliteReservation
{
	//!	\name Connexion Mnesia
	//@{
	cTexte	_Noeud;		//!< Noeud mnesia � contacter en local (serveur)
	cTexte	_Biscuit;	//!< Passphrase commune � tous les noeuds du scope
	//@}
	
	//! \name Adresses d'envoi des �tats
	//@{
	cTexte	vEMail;		//!< Adresse e-mail du destinataire des mails d'�tats de r�servations (ex: le transporteur)
	cTexte	vEMailCopie;	//!< Adresse e-mail de copie des mails d'�tats de r�servations (ex: l'organisme autoritaire)
	//@}

	//! \name Champs optionnels affich�s dans le formulaire
	//@{
	tBool3	vRELPrenom;			//!< Pr�nom du client (Indifferent = champ affich�, remplissage facultatif)
	tBool3	vRELAdresse;			//!< Adresse du client (Indifferent = champ affich�, remplissage facultatif)
	tBool3	vRELTelephone;			//!< Num�ro de t�l�phone du client (Indifferent = champ affich�, remplissage facultatif)
	tBool3	vRELNumeroAbonne;		//!< Num�ro d'abonn� du client (Indifferent = champ affich�, remplissage facultatif)
	tBool3	vRELEMail;			//!< Adresse e-mail du client (Indifferent = champ affich�, remplissage facultatif)
	//@}
		
	//! \name champs de gestion des places
	//@{
	int		vMax;				//!< Nombre maximal de r�servations par service
	int*		vSeuils;				//!< Paliers de nombre de r�servations g�n�rant un envoi de mail d'alerte
	//@}

public:
	//! \name Accesseurs
	//@{
	const cTexte&	GetMail()				const;
	const cTexte&	GetMailCopie()			const;
	tBool3			GetRELPrenom()			const;
	tBool3			GetRELAdresse()			const;
	tBool3			GetRELEMail()			const;
	tBool3			GetRELTel()				const;
	tBool3			GetRELNumeroAbo()		const;
	bool 			ReservationEnLigne()	const;
	//@}
	
	//! \name Calculateurs
	//@{
/*	size_t		Disponibilite(const cTrain* Circulation, const cMoment&, cDatabaseManager* tBaseManager)	const;
	tSeuilResa	getSeuil(const int ancienneValeur, const int nouvelleValeur) 						const;
	bool		Reserver(const cTrain* Service	, const cArretLogique* tPADepart, const cArretLogique* tPAArrivee
						, const cMoment& DateDepart, const cTexteSQL& tNom, const cTexteSQL& tNomBrut
						, const cTexteSQL& tPrenom, const cTexteSQL& tAdresse, const cTexteSQL& tEmail
						, const cTexteSQL& tTelephone, const cTexteSQL& tNumAbonne
						, const cTexteSQL& tAdressePAArrivee, const cTexteSQL& tAdressePADepart
						, const size_t iNombrePlaces, cDatabaseManager* baseManager)				const;
	bool		Annuler(const cTexte& CodeReservation, cDatabaseManager* tBaseManager)				const;
*/	//@}
	
	//! \name Modificateurs
	//@{
	bool	addSeuil		(const tSeuilResa valeur, const size_t NombreSeuils=0);
	bool	setMax			(const int valeur);
	bool	SetRELAdresse	(const tBool3);
	bool	SetRELEMail		(const tBool3);
	bool	SetRELNumeroAbo	(const tBool3);
	bool	SetRELPrenom	(const tBool3);
	bool	SetRELTel		(const tBool3);
	bool	SetMail			(const cTexte&	newVal);
	bool	SetMailCopie	(const cTexte&	newVal);
	//@}

	//! \name Constructeurs et destructeurs
	//@{
	cModaliteReservationEnLigne();
	//@}
	
};

#endif
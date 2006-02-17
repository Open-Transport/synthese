/*!	\brief cHandicape.h
	\brief En-t�te classe modalit� de prise en charge des handicap�s
*/



#ifndef SYNTHESE_CHANDICAPE_H
#define SYNTHESE_CHANDICAPE_H

class cHandicape;

#include "cTexte.h"
#include "cModaliteReservation.h"
#include "Parametres.h"



/*!	\brief Types de prise en charge des handicap�s
	@ingroup m05
	\author Hugues Romain
	\date 2005
*/
class cHandicape
{
	tBool3					_Valeur;		//!< Type de prise en charge (Accept�/Interdit/Inconnu)
	cModaliteReservation*	_Resa;		//!< Modalit� de r�servation de la prise en charge si applicable (NULL sinon)
	tContenance				_Contenance;	//!< Nombre de places offertes aux handicap�s
	cTexte					_Doc;		//!< Texte de description de la modalit�
	tPrix					_Prix;		//!< Prix de l'utilisation du service par un handicap�

public:
	//!	\name Modificateurs
	//@{
	void setPrix(tPrix);
	void setDoc(const cTexte&);
	void setContenance(tContenance);
	void setResa(cModaliteReservation*);
	void setTypeHandicape(tBool3);
	//@}
	
	//!	\name Accesseurs
	//@{
	tContenance getContenance();
	tBool3 getTypeHandicape();
	//@}
	
	//!	\name Constructeur et destructeur
	//@{
	cHandicape();
	//@}
};

inline void cHandicape::setTypeHandicape(tBool3 __Valeur)
{
	_Valeur = __Valeur;
}

inline tBool3 cHandicape::getTypeHandicape()
{
	return(_Valeur);
}

inline void cHandicape::setResa(cModaliteReservation* __Resa)
{
	_Resa = __Resa;
}

inline void cHandicape::setContenance(tContenance __Contenance)
{
	_Contenance = __Contenance;
}

inline tContenance cHandicape::getContenance()
{
	return _Contenance;
}

inline void cHandicape::setDoc(const cTexte& __Doc)
{
	_Doc = __Doc;
}


inline void cHandicape::setPrix(tPrix __Prix)
{
	_Prix = __Prix;
}

#endif

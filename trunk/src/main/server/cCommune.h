/*!	\file cCommune.h
	\brief En-t�te classe commune
	\date 2000-2005
*/



#ifndef SYNTHESE_CCOMMUNE_H
#define SYNTHESE_CCOMMUNE_H

class cCommune;

#include "Point.h"
#include "cTexte.h"
#include "cAccesPADe.h"

#define COMMUNES_NOMBRE_DESIGNATIONS_DEFAUT 10



/** Commune
	@ingroup m05
	\author Hugues Romain
	\version 2.0
*/
class cCommune
{
	cPoint							vPoint;			//!< Coordonn�es de la commune
	cTexte							vNom;			//!< Nom officiel de la commune
	tIndex							_Index;			//!< Index de la commune (non conserv� en m�moire morte)
	cAccesPADe*						_AccesPADeToutLieu;	//!< Arr�t tout lieu s'il existe
	cTableauDynamique<cAccesPADe*>	_AccesPADe;		//!< Tableau des d�signations d'arr�t de la commune
	
public:
	
	//!	\name Accesseurs
	//@{
	const cTexte&	GetNom()			const;
	cAccesPADe*	GetPADePrincipale()	const;
	cAccesPADe*	GetPADeToutLieu()	const;
	tIndex		Index()			const;
	//@}
	
	//!	\name Constructeurs
	//@{
	explicit cCommune(const cTexte& newNom);
	//@}
	
	//!	\name Modificateurs
	//@{
	void SetNumero(tIndex newNumero);
	bool addDesignation(cAccesPADe* newAccesPADe, tTypeAccesPADe);
	//@}

	//!	\name Calculateurs
	//@{
	cAccesPADe** textToPADe(const cTexte& Entree, size_t n=0) const;
	//@}
	
	/*template <class T> T& toXML(T& Objet) const
	{
		Objet << "\n<commune>" << GetNom() << "</commune>";
		return(Objet);
	}*/
};

#define SYNTHESE_CCOMMUNE_CLASSE

inline const cTexte& cCommune::GetNom() const
{ 
	return vNom;
}

inline void cCommune::SetNumero(tIndex newNumero)
{
	_Index = newNumero;
}

inline tIndex cCommune::Index() const
{
	return _Index;
}



/** Accesseur d�signation de l'arr�t principal de commune.
	@return L'arr�t principal de la commune s'il existe, le premier arr�t dans l'ordre alphab�tique sinon
	@author Hugues Romain
*/
inline cAccesPADe* cCommune::GetPADePrincipale() const
{
	return _AccesPADe[0] ? _AccesPADe[0] : _AccesPADe[1];
}

inline cAccesPADe* cCommune::GetPADeToutLieu() const
{
	return _AccesPADeToutLieu;
}



/*!	\brief Affichage standard d'une commune
	\author Hugues Romain
	\date 2005
*/
template <class T>
inline T& operator<<(T& flux, const cCommune& Obj)
{
	flux << Obj.GetNom();
	return flux;
}

#include "cCommuneAccesPADe.inline.h"

#endif

/*!	\file cTarif.h
	\brief En-tête classe de gestion des tarifications
*/

#ifndef SYNTHESE_CTARIF_H
#define SYNTHESE_CTARIF_H

#include "cTexte.h"

// Modalités de tarification
typedef int tNumeroTarif;
class cTarif;

/**
	@ingroup m05
*/
enum tTypeTarif
{
	Zonage = 'Z',
	Section = 'S',
	Distance = 'D'
};


 /** Classes de gestion des tarifications
 	@ingroup m05
*/
class cTarif
{
	
	tNumeroTarif	vIndex;		//!< Index du tarif
	tTypeTarif	vTypeTarif;	//!< Type de tarif
	cTexte		vLibelle;	//!< Nom du tarif
	

public:
	//! \name constructeur
	//@{
	cTarif();
	//@}

	//! \name Modificateurs
	//@{
	void setTypeTarif(const tTypeTarif newVal);
	void setLibelle(const cTexte& libelle);
	void setNumeroTarif(const tNumeroTarif newVal);
	//@}
	
	//! \name Accesseurs
	//@{
	tTypeTarif 	getTypeTarif()		const;
	const cTexte&	getLibelle()		const;
	tNumeroTarif	getNumeroTarif()	const;
	//@}
};

inline void cTarif::setTypeTarif(const tTypeTarif newVal)
{
	vTypeTarif = newVal;
}

inline tTypeTarif cTarif::getTypeTarif() const
{
	return(vTypeTarif);
}

inline void cTarif::setNumeroTarif(const tNumeroTarif newVal)
{
	vIndex = newVal;
}

inline void cTarif::setLibelle(const cTexte& libelle)
{
	vLibelle.Vide();
	vLibelle << libelle;
}

inline const cTexte& cTarif::getLibelle() const
{
	return (vLibelle);
}

inline tNumeroTarif cTarif::getNumeroTarif() const
{
	return(vIndex);
}

#endif

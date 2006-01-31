/*---------------------------------------*
 |                                       |
 |  APDOS / APISAPI - SYNTHESE v0.5      |
 |  � Hugues Romain 2000/2001            |
 |  cDescriptionPassage.h                |
 |  Header Classes Description passages  |
 |                                       |
 *---------------------------------------*/

#ifndef SYNTHESE_CDESCRIPTIONPASSAGE_H
#define SYNTHESE_CDESCRIPTIONPASSAGE_H

class cDescriptionPassage;

#include "cGareLigne.h"
#include "Temps.h"
#include "cSitPert.h"
#include "cTrain.h"
 
/** R�sultat de tableau d'affichage
	@ingroup m34
*/
class cDescriptionPassage
{
	friend class cTableauAffichage;

private:
	// Variables
	cMoment							vMomentPrevu;
	cMoment							vMomentReel;
	cMoment							_ArriveeTerminus;	//!< Moment d'arriv�e au terminus de la ligne
	cSitPert*						vSitPert;
	tNumeroService					vNumArret;
	cGareLigne*						_GareLigne;
	cTableauDynamique<cArretLogique*>		_Gare;
	
	cDescriptionPassage*	vSuivant;
	cDescriptionPassage*	vPrecedent;
	
public:
	//!	\name Accesseurs
	//@{
	cArretLogique*					GetGare(tIndex __i=0)		const;
	tIndex					NombreGares()				const;
	cGareLigne*				getGareLigne()				const;
	const cMoment&			getMomentPrevu()			const;
	const cMoment&			getMomentReel()				const;
	cSitPert*				getSitPert()				const;
	const cMoment&			MomentFin()					const;
	tNumeroService			NumArret()					const;
	cDescriptionPassage*	Suivant()					const;
	//@}
	
	//!	\name Calculateurs
	//@{
	cMoment					MomentArriveeDestination()	const;
	const cArretLogique*			Destination()				;
	//@}

	// Constructeur et fonctions de construction
	cDescriptionPassage();
	void					Remplit(cGareLigne* GareLigne, const cMoment& tempMomentDepart, const tNumeroService& iNumeroService);
	cDescriptionPassage*	GetDernierEtLibere();
	cDescriptionPassage*	Insere(cDescriptionPassage*); // Retourne le premier de la liste apr�s avoir ins�r� le newDP
	

};

inline cDescriptionPassage* cDescriptionPassage::GetDernierEtLibere()
{
	cDescriptionPassage* newDP = vPrecedent;
	vPrecedent = newDP->vPrecedent;
	vPrecedent->vSuivant = NULL;
	_Gare.Vide();
	return(newDP);
}

inline const cMoment& cDescriptionPassage::MomentFin() const
{	
	const cDescriptionPassage* curDP=this;
	for (; curDP->vSuivant != NULL; curDP = curDP->vPrecedent);
	return(curDP->vMomentPrevu);
}

inline const cMoment& cDescriptionPassage::getMomentPrevu() const
{
	return(vMomentPrevu);
}

inline cDescriptionPassage* cDescriptionPassage::Suivant() const
{
	return(vSuivant);
}

inline cArretLogique* cDescriptionPassage::GetGare(tIndex __i) const
{
	return _Gare.IndexValide(__i) ? _Gare[__i] : NULL;
}

inline const cMoment& cDescriptionPassage::getMomentReel() const
{
	return(vMomentReel);
}

inline cSitPert* cDescriptionPassage::getSitPert() const
{
	return(vSitPert);
}

inline tNumeroService cDescriptionPassage::NumArret() const
{
	return(vNumArret);
}

#endif

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
class LogicalPlace;
class cTrain;
class cGareLigne;
class cSitPert;

#include "cMoment.h"
 
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
	size_t					vNumArret;
	cGareLigne*						_GareLigne;
	cTableauDynamique<LogicalPlace*>		_Gare;
	
	cDescriptionPassage*	vSuivant;
	cDescriptionPassage*	vPrecedent;
	
public:
	//!	\name Accesseurs
	//@{
	LogicalPlace*					GetGare(tIndex __i=0)		const;
	tIndex					NombreGares()				const;
	cGareLigne*				getGareLigne()				const;
	const cMoment&			getMomentPrevu()			const;
	const cMoment&			getMomentReel()				const;
	cSitPert*				getSitPert()				const;
	const cMoment&			MomentFin()					const;
	const size_t&			NumArret()					const;
	cDescriptionPassage*	Suivant()					const;
	//@}
	
	//!	\name Calculateurs
	//@{
	cMoment					MomentArriveeDestination()	const;
	const LogicalPlace*			Destination()				;
	//@}

	// Constructeur et fonctions de construction
	cDescriptionPassage();
	void					Remplit(cGareLigne* GareLigne, const cMoment& tempMomentDepart, const size_t& iNumeroService);
	cDescriptionPassage*	GetDernierEtLibere();
	cDescriptionPassage*	Insere(cDescriptionPassage*); // Retourne le premier de la liste apr�s avoir ins�r� le newDP
	

};

#endif

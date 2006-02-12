/*! \file cTrain.h
	\brief En-t�te classe cTrain
*/

#ifndef SYNTHESE_CTRAIN_H
#define SYNTHESE_CTRAIN_H

class cTrain;
class cLigne;
typedef short int tNumeroService;

#include "cTexte.h"
#include "cJourCirculation.h"
#include "cSitPert.h"
#include "cModaliteReservation.h"

/** Impl�mentation de la notion de @ref defCirculation
	@ingroup m05
*/
class cTrain
{
	cDureeEnMinutes		vAttente;			//!< Attente maximale (service continu)
	cDureeEnMinutes		vEtalementCadence;	//!< Dur�e de la plage de service continu
	cJC*				vCirculation;		//!< Calendrier de circulation
	cTexte				vNumero;			//!< Num�ro officiel de la circulation
	bool				vEstCadence;		//!< Indique si la circulation est un service continu
	cSitPert*			vSitPert;			//!< Lien vers la situation perturb�e applicable � la circulation (NULL = pas de situation perturb�e)
	cLigne*				vLigne;				//!< Ligne � laquelle appartient la circulation
	tNumeroService		vIndex;				//!< Index de la circulation dans la ligne
	cHoraire*			_HoraireDepart;		//!< Horaire de d�part de l'origine

public:
	//! \name Accesseurs
	//@{
	const cDureeEnMinutes&	Attente()					const;
	bool					EstCadence()				const;
	const cDureeEnMinutes&	EtalementCadence()			const;
	cJC*					getJC()						const;
	const cTexte&			getNumero()					const;
	cLigne*					getLigne()					const;
	const cHoraire*			getHoraireDepartPremier() 	const;
	//@}

	//! \name Constructeur et destructeur
	//@{
	cTrain();
	~cTrain();
	//@}
	
	//! \name Modificateurs
	//@{
	void	setAmplitudeServiceContinu	(const cDureeEnMinutes& newVal);
	void	setAttente					(const cDureeEnMinutes& newVal);
	void	setNumero					(const cTexte&);
	void	setServiceContinu			();
//	cTrain* operator =  (const cTrain&);
	cJC*	setJC					(cJC*);
	void	setLigne					(cLigne*);
	void	setHoraireDepart			(cHoraire*);
	//@}
	
	//!	\name Calculateurs
	//@{
	bool	ReservationOK(const cMoment& __MomentDepart, const cMoment& __MomentCalcul)	const;
	bool	Circule(const cDate& __DateDepart, tDureeEnJours __JPlus)					const;
	//@}
};

#endif

/*! \file cTrain.h
	\brief En-t�te classe cTrain
*/

#ifndef SYNTHESE_CTRAIN_H
#define SYNTHESE_CTRAIN_H

class cTrain;
class cLigne;
class cSitPert;
class cJC;

#include "cTexte.h"
#include "temps.h"

/** Impl�mentation de la notion de @ref defCirculation
	@ingroup m05
*/
class cTrain
{
	tDureeEnMinutes		vAttente;			//!< Attente maximale (service continu)
	tDureeEnMinutes		vEtalementCadence;	//!< Dur�e de la plage de service continu
	cJC*				vCirculation;		//!< Calendrier de circulation
	cTexte				vNumero;			//!< Num�ro officiel de la circulation
	bool				vEstCadence;		//!< Indique si la circulation est un service continu
	cSitPert*			vSitPert;			//!< Lien vers la situation perturb�e applicable � la circulation (NULL = pas de situation perturb�e)
	cLigne* const				vLigne;				//!< Ligne � laquelle appartient la circulation
	size_t				vIndex;				//!< Index de la circulation dans la ligne
	cHoraire*			_HoraireDepart;		//!< Horaire de d�part de l'origine

public:
	//! \name Accesseurs
	//@{
	const tDureeEnMinutes&	Attente()					const;
	bool					EstCadence()				const;
	const tDureeEnMinutes&	EtalementCadence()			const;
	cJC*					getJC()						const;
	const cTexte&			getNumero()					const;
	cLigne*					getLigne()					const;
	const cHoraire*			getHoraireDepartPremier() 	const;
	//@}

	//! \name Constructeur et destructeur
	//@{
	cTrain(cLigne* const);
	~cTrain();
	//@}
	
	//! \name Modificateurs
	//@{
	void	setAmplitudeServiceContinu	(const tDureeEnMinutes& newVal);
	void	setAttente					(const tDureeEnMinutes& newVal);
	void	setNumero					(const cTexte&);
	void	setServiceContinu			();
//	cTrain* operator =  (const cTrain&);
	cJC*	setJC					(cJC*);
	void	setHoraireDepart			(cHoraire*);
	//@}
	
	//!	\name Calculateurs
	//@{
	bool	ReservationOK(const cMoment& __MomentDepart, const cMoment& __MomentCalcul)	const;
	bool	Circule(const cDate& __DateDepart, tDureeEnJours __JPlus)					const;
	//@}
};

#endif

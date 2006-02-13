/*!	\file cResa.h
	\brief En-tête classes Modalités de réservation
*/

#ifndef SYNTHESE_CRESA_H
#define SYNTHESE_CRESA_H

// Modalités de réservation
class cModaliteReservation;
typedef int tSeuilResa;
enum tResa
{
	Impossible = 'N',
	Facultative = 'F',
	Obligatoire = 'O',
	ObligatoireCollectivement = 'C'
};
enum tChampsResa
{
	eChampAbsent = 0,
	eChampFacultatif = 1,
	eChampObligatoire = 2
};

#include "Parametres.h"
#include "cTexte.h"
#include "Temps.h"
#include "cTrain.h"


/*!	\brief Modalités de réservation
	\author Hugues Romain
	\date 2001-2005
*/
class cModaliteReservation
{
protected:
	//! \name Caractéristiques
	//@{
	tResa			vTypeResa;	 			//!< Type de réservation (possible, obligatoire, impossible)
	bool			vReservationEnLigne; 	//!< Réservation via SYNTHESE active
	tPrix			vPrix;					//!< Coût de la réservation (inutilisé pour l'instant)
	//@}
	
	//! \name Chainage et indexation
	//@{
	tIndex			vIndex;				//!< Numéro de la modalité de réservation dans l'environnement
	//@}
	
	//! \name Délai de réservation
	//@{
	bool			vReferenceEstLOrigine;	//!< Indique si le moment de départ de référence est celle du départ de la circulation à son origine (true) ou bien s'il s'agit du moment de départ du voyageur (false)
	tDureeEnMinutes	vDelaiMinMinutes;		//!< Délai minimal en minutes entre la réservation et le moment de référence
	tDureeEnJours	vDelaiMinJours;			//!< Délai minimal en jours entre la réservation et le moment de référence
	cHeure			vDelaiMinHeureMax;		//!< Heure maximale de réservation le dernier jour de la période d'ouverture à la réservation
	tDureeEnJours	vDelaiMaxJours;		//!< Nombre maximal de jours entre la réservation et le départ du service
	//@}
	
	//! \name Coordonnées de contact
	//@{
	cTexte			vNumeroTelephone;		//!< Numéro de téléphone de la centrale de réservation
	cTexte			vHorairesTelephone;		//!< Description des horaires d'ouverture de la centrale de réservation
	cTexte			vDescription;			//!< Informations complémentaires sur la centrale de réservation ou la modalité de réservation
	cTexte			vSiteWeb;				//!< URL d'un site web permettant de réserver le service en ligne (incompatible avec la REL)
	//@}
		
public:

	//! \name Accesseurs
	//@{
	tResa		TypeResa()									const;
	const cHeure&	GetDelaiMinHeureMax() 							const;
	const cTexte&	GetHorairesTelephone()							const;
	const cTexte&	GetSiteWeb()									const;
	const cTexte&	GetTelephone()									const;
	virtual bool 	ReservationEnLigne()							const;
	tIndex		Index()										const;
	/*template <class T> 
	T& 			toXML(T& Tampon, const cTrain*, cMoment momentDepart)	const;*/
	//@}
		
	//! \name Calculateurs
	//@{
	cMoment		momentLimiteReservation	(const cTrain*, const cMoment& MomentDepart)							const;
	cMoment		momentDebutReservation	(const cMoment&)										const;
	bool 		circulationPossible		(const cTrain*, const cMoment& MomentResa, const cMoment& MomentDepart) 	const;
	bool 		reservationPossible		(const cTrain*, const cMoment& MomentResa, const cMoment& MomentDepart) 	const;
	//@}
	
	//! \name Modificateurs
	//@[
	bool	SetDelaiMaxJours		(const tDureeEnJours);
	void	setDelaiMaxJours		(const tDureeEnJours);
	bool	SetDelaiMinHeureMax 	(const cHeure&);
	void	setDelaiMinHeureMax		(const cHeure&);
	bool	SetDelaiMinJours		(const int);
	void	setDelaiMinJours		(const tDureeEnJours);
	bool	SetDelaiMinMinutes		(const int);
	void	setDelaiMinMinutes		(const tDureeEnMinutes&);
	bool	SetDoc				(const cTexte&);
	bool	SetHorairesTel			(const cTexte&);
	bool setIndex				(tIndex);
	bool	SetPrix				(const float);
	void	setPrix				(const tPrix);
	bool	SetReservationEnLigne 	(const bool);
	bool	SetSiteWeb			(const cTexte&);
	bool	SetTel				(const cTexte&);
	bool	SetTypeResa			(const char);
	void	setTypeResa			(const tResa);
	bool	setReferenceEstLOrigine	(const bool);
	//@}

	//! \name Constructeurs et destructeurs
	//@{
	cModaliteReservation();
	virtual ~cModaliteReservation() {}
	//@}	
};

#endif

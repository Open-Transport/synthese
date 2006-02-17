/*!	\file cResa.h
	\brief En-t�te classes Modalit�s de r�servation
*/

#ifndef SYNTHESE_CRESA_H
#define SYNTHESE_CRESA_H

// Modalit�s de r�servation
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


/*!	\brief Modalit�s de r�servation
	\author Hugues Romain
	\date 2001-2005
*/
class cModaliteReservation
{
protected:
	//! \name Caract�ristiques
	//@{
	tResa			vTypeResa;	 			//!< Type de r�servation (possible, obligatoire, impossible)
	bool			vReservationEnLigne; 	//!< R�servation via SYNTHESE active
	tPrix			vPrix;					//!< Co�t de la r�servation (inutilis� pour l'instant)
	//@}
	
	//! \name Chainage et indexation
	//@{
	tIndex			vIndex;				//!< Num�ro de la modalit� de r�servation dans l'environnement
	//@}
	
	//! \name D�lai de r�servation
	//@{
	bool			vReferenceEstLOrigine;	//!< Indique si le moment de d�part de r�f�rence est celle du d�part de la circulation � son origine (true) ou bien s'il s'agit du moment de d�part du voyageur (false)
	tDureeEnMinutes	vDelaiMinMinutes;		//!< D�lai minimal en minutes entre la r�servation et le moment de r�f�rence
	tDureeEnJours	vDelaiMinJours;			//!< D�lai minimal en jours entre la r�servation et le moment de r�f�rence
	cHeure			vDelaiMinHeureMax;		//!< Heure maximale de r�servation le dernier jour de la p�riode d'ouverture � la r�servation
	tDureeEnJours	vDelaiMaxJours;		//!< Nombre maximal de jours entre la r�servation et le d�part du service
	//@}
	
	//! \name Coordonn�es de contact
	//@{
	cTexte			vNumeroTelephone;		//!< Num�ro de t�l�phone de la centrale de r�servation
	cTexte			vHorairesTelephone;		//!< Description des horaires d'ouverture de la centrale de r�servation
	cTexte			vDescription;			//!< Informations compl�mentaires sur la centrale de r�servation ou la modalit� de r�servation
	cTexte			vSiteWeb;				//!< URL d'un site web permettant de r�server le service en ligne (incompatible avec la REL)
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

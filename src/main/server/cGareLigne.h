/*! \file cGareLigne.h
	\brief En-t�te Classe GareLigne
*/

#ifndef SYNTHESE_CGARELIGNE_H
#define SYNTHESE_CGARELIGNE_H

class cArretPhysique;
class cLigne;
class cVelo;
class cHandicape;

#include "Temps.h"
#include "cDistanceCarree.h"


/** Liaison entre points d'arr�t et lignes
	@ingroup m15
	\author Hugues Romain
	\date 2000-2003
	\version 2.0
*/
class cGareLigne
{
public:
	enum tTypeGareLigneDA
	{
		Depart = 'D',
		Arrivee = 'A',
		Passage = 'P'
	};

private:

	//! \name Data
	//@{
		cArretPhysique* const			_physicalStop;			//!< Arrêt physique desservi par la gare ligne
		cLigne* const				vLigne;						//!< Ligne desservant l'arr�t
		tDistanceM const			vPM;						//!< Point m�trique de l'arr�t sur la ligne
		const bool			 	vHorairesSaisis;				//!< Horaires avec ou sans saisie
		tTypeGareLigneDA 	vTypeDA;						//!< Gare de d�part, d'arriv�e, ou de passage
	//@}
	
	//! \name Variables Ligne
	//@{
		cGareLigne*			vDepartPrecedent;				//!< Arr�t de passage au d�part pr�c�dent sur la ligne
		cGareLigne*			vDepartCorrespondancePrecedent;	//!< Arr�t de correspondance au d�part pr�c�dent sur la ligne
		cGareLigne*			vArriveeSuivante;				//!< Arr�t de passage � l'arriv�e suivant sur la ligne
		cGareLigne*			vArriveeCorrespondanceSuivante;	//!< Arr�t de passage de destination suivant sur la ligne
		cGareLigne*			_Precedent;						//!< Arr�t pr�c�dent ind�pendemment de sa desserte
		cGareLigne*			vSuivant;						//!< Arr�t suivant ind�pendemment de sa desserte
	//@}

	//! \name Variables Horaires
	//@{
	cHoraire*		vHoraireDepartPremier;		//!< Horaires de d�part th�oriques (par circulation) Service continu : premiers d�parts de la plage
	cHoraire*		vHoraireDepartPremierReel;	//!< Horaires de d�part r�els (par circulation)
	cHoraire*		vHoraireDepartDernier;		//!< Horaires de d�part th�oriques (par circulation) Service continu : derniers d�parts de la plage
	cHoraire*		vHoraireArriveePremier;		//!< Horaires d'arriv�e th�oriques (par circulation) Service continu : permi�res arriv�es de la plage
	cHoraire*		vHoraireArriveePremierReel;	//!< Horaires d'arriv�e r�elles (par circulation)
	cHoraire*		vHoraireArriveeDernier;		//!< Horaires d'arriv�e th�oriques (par circulation) Service continu : derni�res arriv�es de la plage
	tIndex	vIndexDepart[24];			//!< Index des circulations selon l'heure de d�part th�orique
	tIndex	vIndexDepartReel[24];		//!< Index des circulations selon l'heure de d�part r�elle du jour
	tIndex	vIndexArrivee[24];			//!< Index des circulations selon l'heure d'arriv�e th�orique
	tIndex	vIndexArriveeReel[24];		//!< Index des circulations selon l'heure d'arriv�e r�elle du jour
	//@}
	
	//! \name Modificateurs
	//@{
	void AlloueHoraires();	//!< Alloue l'espace n�cessaire au stockage des horaires
	//@}

public:
	
	//! \name Constructeurs et destructeurs
	//@{
		cGareLigne (
			cLigne*, 
			tDistanceM, 
			tTypeGareLigneDA, 
			cArretPhysique* const, 
			bool newHorairesSaisis
		);
		~cGareLigne();
	//@}

	
	//! \name Modificateurs
	//@{
	void ChaineAvecHoraireSuivant(const cGareLigne& AHPrecedent, const cGareLigne& AHSuivant, size_t Position, size_t Nombre, tIndex NumeroServiceATraiter=-1);
	void EcritIndexDepart();
	void EcritIndexArrivee();
	void RealloueHoraires(size_t newService);
	void setArriveeCorrespondanceSuivante(cGareLigne* newVal);
	void setArriveeSuivante(cGareLigne* newVal);
	void setDepartCorrespondancePrecedent(cGareLigne*);
	void setDepartPrecedent(cGareLigne* newVal);
	void setHoraires(const cTexte& Tampon, tIndex Position, tIndex LargeurColonne, bool DepartDifferentPassage);
	void setTypeDA(tTypeGareLigneDA);
	//@}
	
	void ValiditeSolution(size_t, cMoment&);
	
	//int Circule(tNumeroService, tTypeGareLigneDA, cDate&);
	// Calculs d'heures sens depart -> arrivee (pour meilleure arriv�e)
//	bool HeureDepartDA(tNumeroService, cMoment &MomentDepart, cHoraire& HoraireDepart);
//	void HeureArriveeDA(tNumeroService, cMoment& MomentArrivee, cMoment& MomentDepart, cHoraire& HoraireDepart);
	// Calculs d'heures sens arriv�e -> d�part (pour meilleur d�part)
//	bool HeureArriveeAD(tNumeroService, cMoment &MomentDepart, cHoraire& HoraireDepart);
//	void HeureDepartAD(tNumeroService, cMoment& MomentDepart, cMoment& MomentArrivee, cHoraire& HoraireArrivee);

	//! \name Calculateurs
	//@{
	bool			Circule(const cMoment& DateDebut, const cMoment& DateFin)										const;
	void			CalculeArrivee(const cGareLigne& GareLigneDepart, size_t iNumeroService
								, const cMoment& MomentDepart, cMoment& MomentArrivee) 					const;
//	cMoment			CalculeArrivee(const cDescriptionPassage* curDP)										const;
	void			CalculeDepart(const cGareLigne& GareLigneArrivee, size_t iNumeroService
								, const cMoment& MomentArrivee, cMoment& MomentDepart)					const;
	bool			CoherenceGeographique(const cGareLigne& AutreGareLigne)								const;
//	bool			ControleAxe(const cElementTrajet* ET, tBool3 velo, tBool3 handicape, tBool3 taxibus, int codeTarif)	const;
	bool			controleHoraire(const cGareLigne* GareLigneAvecHorairesPrecedente)						const;
	tDureeEnMinutes	MeilleurTempsParcours(const cGareLigne& autreGL)										const;
	tIndex	Prochain(cMoment &MomentDepart, const cMoment& MomentDepartMax
							, tDureeEnMinutes& AmplitudeServiceContinu, tIndex NumProchainMin
							, const cMoment& __MomentCalcul)												const;
	tIndex	Prochain(cMoment &MomentDepart, const cMoment& MomentDepartMax
							, const cMoment& __MomentCalcul
							, tIndex NumProchainMin=INCONNU)												const;
	tIndex	Precedent(cMoment &MomentArrivee, const cMoment& MomentArriveeMin
							, tDureeEnMinutes& AmplitudeServiceContinu)									const;
	tIndex	Precedent(cMoment &MomentArrivee, const cMoment& MomentArriveeMin)					const;
//	void			RemplitDP(cDescriptionPassage& curDP, const cMoment& tempMomentDepart
//							, const tNumeroService& iNumeroService)										const;
	//@}
	
	//! \name Accesseurs
	//@{
	bool					EstArrivee()									const;
	bool					EstDepart()										const;
	cGareLigne*				getArriveeCorrespondanceSuivante()				const;
	cGareLigne*				getArriveeSuivante()							const;
	cGareLigne*				getDepartCorrespondancePrecedent()				const;
	cGareLigne*				getDepartPrecedent()							const;
//	const cGareLigne*		getLiaisonDirecteVers(const cAccesPADe*)		const;
//	const cGareLigne*		getLiaisonDirecteDepuis(const cAccesPADe*)		const;
	const cHoraire&			getHoraireArriveeDernier(size_t) 		const;
	const cHoraire&			getHoraireArriveePremier(size_t) 		const;
	const cHoraire&			getHoraireArriveePremierReel(size_t)	const;
	const cHoraire&			getHoraireDepartDernier(size_t) 		const;
	const cHoraire&			getHoraireDepartPremier(size_t)			const;
	const cHoraire&			getHoraireDepartPremierReel(size_t) 	const;
	bool					HorairesSaisis()								const;
	cLigne*					Ligne()											const;
	tDistanceM				PM()											const;
	cArretPhysique*				ArretPhysique()											const;
	tTypeGareLigneDA		TypeDA()										const;
	//@}
};




#endif

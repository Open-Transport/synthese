/*! \file cGareLigne.h
	\brief En-t�te Classe GareLigne
*/

#ifndef SYNTHESE_CGARELIGNE_H
#define SYNTHESE_CGARELIGNE_H

enum tTypeGareLigneDA
{
	Depart = 'D',
	Arrivee = 'A',
	Passage = 'P'
};

class cArretPhysique;
class cLigne;

#include "Temps.h"
#include "cArretPhysique.h"
#include "cVelo.h"
#include "cHandicape.h"
#include "cDistanceCarree.h"


/** Liaison entre points d'arr�t et lignes
	@ingroup m05
	\author Hugues Romain
	\date 2000-2003
	\version 2.0
*/
class cGareLigne
{
private:

	//! \name Variables Gare
	//@{
	cArretPhysique* const			_physicalStop;			//!< Arrêt physique desservi par la gare ligne
	cGareLigne*		vPADepartSuivant;		//!< Ligne suivante au d�part de l'arr�t
	cGareLigne*		vPAArriveeSuivante;		//!< Ligne suivante � l'arriv�e � l'arr�t
	cGareLigne*		vPADepartAxeSuivant;	//!< Ligne suivante au d�part de l'arr�t, d'un axe diff�rent
	cGareLigne*		vPAArriveeAxeSuivante;	//!< Ligne suivante � l'arriv�e � l'arr�t, d'un axe diff�rent
	//@}
	
	//! \name Variables Ligne
	//@{
	cLigne* const				vLigne;						//!< Ligne desservant l'arr�t
	tDistanceM const			vPM;						//!< Point m�trique de l'arr�t sur la ligne
	tTypeGareLigneDA 	vTypeDA;						//!< Gare de d�part, d'arriv�e, ou de passage
	const bool			 	vHorairesSaisis;				//!< Horaires avec ou sans saisie
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
	tNumeroService	vIndexDepart[24];			//!< Index des circulations selon l'heure de d�part th�orique
	tNumeroService	vIndexDepartReel[24];		//!< Index des circulations selon l'heure de d�part r�elle du jour
	tNumeroService	vIndexArrivee[24];			//!< Index des circulations selon l'heure d'arriv�e th�orique
	tNumeroService	vIndexArriveeReel[24];		//!< Index des circulations selon l'heure d'arriv�e r�elle du jour
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
			cArretPhysique*, 
			bool newHorairesSaisis
		);
		cGareLigne(cLigne*, const cGareLigne&);
	//	cGareLigne(cLigne*);
		~cGareLigne();
	//@}

	
	//! \name Modificateurs
	//@{
	void ChaineAvecHoraireSuivant(const cGareLigne& AHPrecedent, const cGareLigne& AHSuivant, size_t Position, size_t Nombre, tNumeroService NumeroServiceATraiter=-1);
	void EcritIndexDepart();
	void EcritIndexArrivee();
	void RealloueHoraires(tNumeroService newService);
	void setArriveeCorrespondanceSuivante(cGareLigne* newVal);
	void setArriveeSuivante(cGareLigne* newVal);
	void setDepartCorrespondancePrecedent(cGareLigne*);
	void setDepartPrecedent(cGareLigne* newVal);
	void setHoraires(const cTexte& Tampon, tIndex Position, tIndex LargeurColonne, bool DepartDifferentPassage);
	void setHorairesSaisis(bool newVal);
	void setTypeDA(tTypeGareLigneDA);
	void setPADepartSuivant();
	void setPAArriveeSuivant();
	void setPADepartAxeSuivant(cGareLigne*);
	void setPAArriveeAxeSuivant(cGareLigne*);
	void setPM(tDistanceM newVal);
	void setSuivant(cGareLigne* newVal);
	void setPrecedent(cGareLigne*);
	//@}
	
	void ValiditeSolution(tNumeroService, cMoment&);
	
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
	void			CalculeArrivee(const cGareLigne& GareLigneDepart, tNumeroService iNumeroService
								, const cMoment& MomentDepart, cMoment& MomentArrivee) 					const;
//	cMoment			CalculeArrivee(const cDescriptionPassage* curDP)										const;
	void			CalculeDepart(const cGareLigne& GareLigneArrivee, tNumeroService iNumeroService
								, const cMoment& MomentArrivee, cMoment& MomentDepart)					const;
	bool			CoherenceGeographique(const cGareLigne& AutreGareLigne)								const;
//	bool			ControleAxe(const cElementTrajet* ET, tBool3 velo, tBool3 handicape, tBool3 taxibus, int codeTarif)	const;
	bool			controleHoraire(const cGareLigne* GareLigneAvecHorairesPrecedente)						const;
	cDureeEnMinutes	MeilleurTempsParcours(const cGareLigne& autreGL)										const;
	tNumeroService	Prochain(cMoment &MomentDepart, const cMoment& MomentDepartMax
							, cDureeEnMinutes& AmplitudeServiceContinu, tNumeroService NumProchainMin
							, const cMoment& __MomentCalcul)												const;
	tNumeroService	Prochain(cMoment &MomentDepart, const cMoment& MomentDepartMax
							, const cMoment& __MomentCalcul
							, tNumeroService NumProchainMin=INCONNU)												const;
	tNumeroService	Precedent(cMoment &MomentArrivee, const cMoment& MomentArriveeMin
							, cDureeEnMinutes& AmplitudeServiceContinu)									const;
	tNumeroService	Precedent(cMoment &MomentArrivee, const cMoment& MomentArriveeMin)					const;
//	void			RemplitDP(cDescriptionPassage& curDP, const cMoment& tempMomentDepart
//							, const tNumeroService& iNumeroService)										const;
	//@}
	
	//! \name Accesseurs
	//@{
	const cDureeEnMinutes&	Attente(tNumeroService iNumeroService)			const;
	cGareLigne*				Destination()									const;
	bool					EstArrivee()									const;
	bool					EstDepart()										const;
	cGareLigne*				getArriveeCorrespondanceSuivante()				const;
	cGareLigne*				getArriveeSuivante()							const;
	cGareLigne*				getDepartCorrespondancePrecedent()				const;
	cGareLigne*				getDepartPrecedent()							const;
//	const cGareLigne*		getLiaisonDirecteVers(const cAccesPADe*)		const;
//	const cGareLigne*		getLiaisonDirecteDepuis(const cAccesPADe*)		const;
	const cHoraire&			getHoraireArriveeDernier(tNumeroService) 		const;
	const cHoraire&			getHoraireArriveePremier(tNumeroService) 		const;
	const cHoraire&			getHoraireArriveePremierReel(tNumeroService)	const;
	const cHoraire&			getHoraireDepartDernier(tNumeroService) 		const;
	const cHoraire&			getHoraireDepartPremier(tNumeroService)			const;
	const cHoraire&			getHoraireDepartPremierReel(tNumeroService) 	const;
	const cTexte&			getNomArretPhysique()									const;
	cGareLigne*				getPrecedent()									const;
	const cArretPhysique*			getVoie()										const;
	bool					HorairesSaisis()								const;
	cLigne*					Ligne()											const;
	cGareLigne*				Origine()										const;
	cGareLigne*				PAArriveeAxeSuivante()							const;
	cGareLigne*				PAArriveeSuivante()								const;
	cGareLigne*				PADepartAxeSuivant()							const;
	cGareLigne*				PADepartSuivant()								const;
	tDistanceM				PM()											const;
	cArretPhysique*				ArretPhysique()											const;
	cGareLigne*				Suivant()										const;
	tTypeGareLigneDA		TypeDA()										const;
	//@}
};

#define SYNTHESE_CGARELIGNE_CLASSE

#include "cGareLigneAccesPADe.inline.h"

inline cLigne* cGareLigne::Ligne() const
{
	return(vLigne);
}

inline void cGareLigne::setTypeDA(tTypeGareLigneDA newVal)
{
	vTypeDA = newVal;
}

inline bool cGareLigne::EstArrivee() const
{
	return(vTypeDA == Passage || vTypeDA == Arrivee);	
}

inline bool cGareLigne::EstDepart() const
{
	return(vTypeDA == Passage || vTypeDA == Depart);
}


inline cGareLigne*	cGareLigne::PADepartSuivant()									const
{
	return(vPADepartSuivant);
}

inline cGareLigne*	cGareLigne::PAArriveeSuivante()									const
{
	return(vPAArriveeSuivante);
}

inline cGareLigne*	cGareLigne::PADepartAxeSuivant() const
{
	return(vPADepartAxeSuivant);
}

inline cGareLigne*	cGareLigne::PAArriveeAxeSuivante() const
{
	return(vPAArriveeAxeSuivante);
}

inline cArretPhysique* cGareLigne::ArretPhysique() const
{
	return _physicalStop;
}

inline tTypeGareLigneDA cGareLigne::TypeDA() const
{
	return(vTypeDA);
}

inline void cGareLigne::setPADepartAxeSuivant(cGareLigne* newGareLigne)
{
	vPADepartAxeSuivant = newGareLigne;
}

inline void cGareLigne::setPAArriveeAxeSuivant(cGareLigne* newGareLigne)
{
	vPAArriveeAxeSuivante = newGareLigne;
}

inline bool cGareLigne::HorairesSaisis() const
{
	return(vHorairesSaisis);
}

inline cGareLigne* cGareLigne::Suivant() const
{
	return(vSuivant);
}

inline cGareLigne* cGareLigne::getArriveeSuivante() const
{
	return(vArriveeSuivante);
}

inline cGareLigne* cGareLigne::getArriveeCorrespondanceSuivante() const
{
	return(vArriveeCorrespondanceSuivante);
}

inline cGareLigne* cGareLigne::getDepartPrecedent() const
{
	return(vDepartPrecedent);
}

inline cGareLigne* cGareLigne::getDepartCorrespondancePrecedent() const
{
	return(vDepartCorrespondancePrecedent);
}

inline const cHoraire& cGareLigne::getHoraireDepartPremier(tNumeroService iService) const
{
	//_ASSERTE(iService >= 0 && iService < vLigne->NombreServices());
	
	return(vHoraireDepartPremier[iService]);
}


inline const cHoraire& cGareLigne::getHoraireDepartDernier(tNumeroService iNumeroService) const
{
	//_ASSERTE(iNumeroService >= 0 && iNumeroService < vLigne->NombreServices());
	
	return(vHoraireDepartDernier[iNumeroService]);
}

inline const cHoraire& cGareLigne::getHoraireArriveePremier(tNumeroService iNumeroService) const
{
	//_ASSERTE(iNumeroService >= 0 && iNumeroService < vLigne->NombreServices());
	
	return(vHoraireArriveePremier[iNumeroService]);
}


inline const cHoraire& cGareLigne::getHoraireArriveeDernier(tNumeroService iNumeroService) const
{
	//_ASSERTE(iNumeroService >= 0 && iNumeroService < vLigne->NombreServices());
	
	return(vHoraireArriveeDernier[iNumeroService]);
}

inline void cGareLigne::setArriveeSuivante(cGareLigne *newVal)
{
	vArriveeSuivante = newVal;
}

inline void cGareLigne::setArriveeCorrespondanceSuivante(cGareLigne *newVal)
{
	vArriveeCorrespondanceSuivante = newVal;
}

inline void cGareLigne::setSuivant(cGareLigne *newVal)
{
	vSuivant =newVal;
}

inline const cHoraire& cGareLigne::getHoraireDepartPremierReel(tNumeroService iNumeroService) const
{
	return(vHoraireDepartPremierReel[iNumeroService]);
}

inline const cHoraire& cGareLigne::getHoraireArriveePremierReel(tNumeroService iNumeroService) const
{
	return(vHoraireArriveePremierReel[iNumeroService]);
}

inline tDistanceM	cGareLigne::PM() const
{
	return(vPM);
}
	



#endif

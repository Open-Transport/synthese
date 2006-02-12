/*---------------------------------------*
 |                                       |
 |  APDOS / APISAPI - SYNTHESE v0.6      |
 |  � Hugues Romain 2000/2003            |
 |  cElementTrajet.h v2                  |
 |  Header Classe Element Trajet         |
 |                                       |
 *---------------------------------------*/

#ifndef SYNTHESE_CELEMENTTRAJET_H
#define SYNTHESE_CELEMENTTRAJET_H

// Elements de trajets
class cElementTrajet;

/** @ingroup m33
*/
enum tTypeElementTrajet
{
	eTrajetAvecVoyageurs	= 0,
	eTrajetAVide			= 1,
	eTrajetVoirie			= 2		// Trajetà pied sur voirie
};

#include "Temps.h"
#include "cArretLogique.h"
#include "cLigne.h"
#include "cDistanceCarree.h"
#include "cArretPhysique.h"
#include "cAxe.h"
 
/** El�ment de trajet (pour r�sultats de calculs d'itin�raires)
	@ingroup m33
	\author Hugues Romain
	
Le pointeur suivant n'est utilis� que si l'�l�ment est int�gr� � un trajet. Dans le cas contraire, il peut �tre utilis� pour tout autre but, et sa valeur sera ignor�e lors de l'int�gration au trajet.
 */
class cElementTrajet
{
friend class cTrajet;

protected:
	//! \name Donn�es
	//@{
	const cArretLogique*			vGareDepart;				//!< Point d'arr�t de d�part @todo le remplacer par l'ID de l'arret
	tNumeroVoie				vVoieDepart;				//!< ArretPhysique de d�part au niveau du point d'arr�t
	cMoment					vMomentDepart;				//!< Moment du d�part (premier si service continu)
	cDureeEnMinutes			vAmplitudeServiceContinu;	//!< Amplitude du service continu (0 si service unique)
	const cArretLogique*			vGareArrivee;				//!< Point d'arr�t d'arriv�e @todo le remplacer par l'ID de l'arret
	tNumeroVoie				vVoieArrivee;				//!< ArretPhysique d'arriv�e au niveau du point d'arr�t
	cMoment					vMomentArrivee;				//!< Moment d'arriv�e (premier si service continu)
	tNumeroService			vNumeroService;				//!< Index du service utilis� au sein de la ligne
	const cLigne*			vLigne;						//!< Ligne utilis�e
	tTypeElementTrajet		vType;
	//@}

	//! \name Chainages
	//@{
	cElementTrajet*			vSuivant;					//!< Element de trajet suivant en correspondance
	cElementTrajet*			_Precedent;					//!< Element de trajet precedent
	//@}
	
	//! \name Donn�es pr�calcul�es
	//@{
	cDureeEnMinutes			vDureeEnMinutesRoulee;		//!< Temps pass� dans les circulations
	cDistanceCarree			vDistanceCarreeObjectif;	//!< Carr� de la distance entre la destination de l'�l�ment et la destination finale du trajet (avec l'incertitude habituelle de ce type de donn�es)
	tDistanceM				vDistanceParcourue;			//!< Distance lin�aire parcourue aucours du trajet
	//@}

public :
	
	//! \name Accesseurs
	//@{
	const cDureeEnMinutes&	AmplitudeServiceContinu()			const;
	const cAxe*				Axe()								const;
	const cDistanceCarree&	DistanceCarreeObjectif()			const;
	tDistanceM				DistanceParcourue()					const;
	const cDureeEnMinutes&	DureeEnMinutesRoulee()				const;
	const cArretLogique*			getGareArrivee()					const;
	const cArretLogique*			getGareDepart()						const;
	const cArretPhysique*			getArretPhysiqueArrivee()					const;
	const cArretPhysique*			getArretPhysiqueDepart()						const;
	tNumeroService 			getService()						const;
	const cLigne*			getLigne()							const;
	const cMoment&			MomentArrivee()						const;
	const cMoment&			MomentDepart()						const;
	const cElementTrajet*	Precedent()							const;
	const cElementTrajet*	Suivant()							const;
	tTypeElementTrajet		Type()								const;
	tNumeroVoie				VoieArrivee()						const;
	tNumeroVoie				VoieDepart()						const;
	//@}
	
	//! \name Accesseurs d'objet permettant la modification
	//@{
	cDistanceCarree&		getDistanceCarreeObjectif();
	cElementTrajet*			getSuivant();
	//@}
	
	//! \name Operateurs
	//@{
	cElementTrajet* operator += (cElementTrajet*);
	cElementTrajet* operator =  (const cElementTrajet&);
	//@}

	//! \name Calculateurs
	//@{
	//cElementTrajet*	Imite(const cMoment& MomentDepart, const cMoment& ArriveeMax)	const;
	//@}

	//! \name Constructeur et destructeur
	//@{
	cElementTrajet();
	~cElementTrajet();
	//@}

	//! \name Modificateurs
	//@{
	void			CalculeDureeEnMinutesRoulee();
	cElementTrajet*	deleteChainageColonne();
	void			setAmplitudeServiceContinu(const cDureeEnMinutes& newVal);
	void			setDistanceCarreeObjectif(const cDistanceCarree& newVal);
	void			setDureeEnMinutesRoulee(const cDureeEnMinutes& newVal);
	void			setInformations(const cGareLigne* GLA, const cGareLigne* GLD, const cMoment& MomentDepart
										, const cMoment& MomentArrivee);
	void			setInformations(const cGareLigne* GLA, const cGareLigne* GLD, const cDate& DateDepart
										, tNumeroService iNumeroService);
	void			setLigne(const cLigne* newVal);
	void			setMomentArrivee(const cMoment& newVal);
	void			setMomentDepart(const cMoment& newVal);
	void			setArretLogiqueArrivee(const cArretLogique* newVal);
	void			setArretLogiqueDepart(const cArretLogique* newVal);
	void			setArretPhysiqueArrivee(tNumeroVoie newVal);
	void			setArretPhysiqueDepart(tNumeroVoie newVal);
	void			setService(const tNumeroService newVal);
  	void			setSuivant(cElementTrajet* newVal);
	void			setType(tTypeElementTrajet newVal);
	//@}
	
  	//! \name Fonctions d'affichage	
	//@{
	template <class T> T& toString(T& Objet) const
	{
		Objet << this->getGareArrivee()
			<< " Q" << TXT(this->VoieArrivee()) << " ("
			<< this->MomentArrivee()
			<< ") \n";

		return(Objet);
	}
	
	template <class T> T& toTrace(T& Objet) const
	{
		Objet << this->getLigne()->getCode()
			<< " : " << this->getGareDepart()
			<< "(" << this->MomentDepart()
			<< ") � " << this->getGareArrivee()
			<< "(" << this->MomentArrivee()
			<< ")\n";
		
		if (vSuivant != NULL)
			vSuivant->toTrace(Objet);

		return(Objet);
	}	

	/*template <class T> T& toXML(T& Objet, cMoment momentDepart, const cDureeEnMinutes& amplitudeServiceContinu) const
	{
		cMoment debutArret, finArret;

		Objet << "\n<elementtrajet>";
		Objet << "\n<depart>";

		debutArret= this->MomentDepart();
		finArret = debutArret;
		if (amplitudeServiceContinu.Valeur())
		{
			finArret += amplitudeServiceContinu;
		}

		this->getGareDepart()->toXML(Objet, debutArret, finArret);
		this->MomentDepart().toXML(Objet,"date");
		Objet << "\n</depart>";
		Objet << "\n<arrivee>";
		
		debutArret = this->MomentArrivee();
		finArret = debutArret;
		if (amplitudeServiceContinu.Valeur())
		{
			finArret += amplitudeServiceContinu;
		}	
		this->getGareArrivee()->toXML(Objet, debutArret, finArret);
		
		this->MomentArrivee().toXML(Objet,"date");
		Objet << "\n</arrivee>";
		
		//SET & FMA : FE1.0.026 : Rajout du calcul de la fin de l'intervalle (ligne)		
		cMoment debutLigne, finLigne;
		debutLigne = this->MomentDepart();
		finLigne = this->MomentArrivee();
		if (amplitudeServiceContinu.Valeur())
		{
			finLigne += amplitudeServiceContinu;
		}
		
		this->getLigne()->toXML(Objet, this->getService(), momentDepart, debutLigne, finLigne);

		Objet << "<servicecontinu actif=\"";
		if (AmplitudeServiceContinu().Valeur())
		{
			Objet << "1\">";
			cMoment tempMoment = MomentDepart();
			tempMoment += AmplitudeServiceContinu();
			tempMoment.toXML(Objet,"datedernier");
		}
		else
			Objet << "0\">";

		Objet << "</servicecontinu>";
		Objet << "\n</elementtrajet>";

		return(Objet);
	}*/
	//@}

	
};



//!	\name Fonctions de comparaisons entre ElementTrajet au format accept� par QSort
//@{
int CompareUtiliteETPourMeilleurDepart(const void* ET1, const void* ET2);
int CompareUtiliteETPourMeilleureArrivee(const void* ET1, const void* ET2);
//@}s
	

#include "cElementTrajet.inline.h"

#endif

/*!	\file cLigne.h
	\brief En-t�te classe cLigne
*/

#ifndef SYNTHESE_CLIGNE_H
#define SYNTHESE_CLIGNE_H

class cLieuLogique;

#include "cTexte.h"
#include "cTrain.h"
#include "cReseau.h"
#include "cAxe.h"
#include "cGareLigne.h"
#include "cMateriel.h"
#include "cModaliteReservation.h"
#include "cVelo.h"
#include "cHandicape.h"
#include "cTarif.h"
#include "cAlerte.h"
#include "cArretPhysique.h"
#include "cDistanceCarree.h"

/** Impl�mentation de la notion de @ref defLigne
	@ingroup m05
	@author Hugues Romain
	@date 2000-2003
*/
class cLigne
{
protected:
	//! @name Donn�es et chainages
	//@{
	cTrain*					vTrain;				//!< Circulations
	cReseau*				vReseau;			//!< R�seau de transport
	cAxe*					vAxe;				//!< Axe
	cGareLigne*				vPremiereGareLigne;	//!< Premier arr�t de la ligne
	cGareLigne* 			vDerniereGareLigne;	//!< Derni�r arr�t de la ligne
	tNumeroService			vNombreServices;	//!< Nombre de services
	cMateriel*				vMateriel;			//!< Mat�riel roulant
	cModaliteReservation*	vResa;				//!< Modalit� de r�servation
	cVelo*					vVelo;				//!< Modalit� de prise en charge des v�los
	cHandicape*				vHandicape;			//!< Modalit� d'acceptation des handicap�s
	cTarif*					vTarif;				//!< Tarification
	cAlerte					vAlerte;			//!< Message d'alerte � afficher
	cLigne*					vSuivant;			//!< Chainage des lignes
	//@}

	//! \name Identification de la ligne
	//@{
	cTexte				vLibelleSimple;		//!< Libell� mentionn� dans le cartouche
	cTexte				vLibelleComplet;	//!< Libell� mentionn� dans la feuille de route
	cTexte				vStyle;				//!< Style CSS du cartouche
	cTexte				vImage;				//!< Image � afficher en tant que cartouche
	cTexte				vCode;				//!< Code d'identification de la ligne
	cTexteSansAccent	vNomPourIndicateur;	//!< Lib�ll� mentionn� dans les tableaux d'indicateurs horaires
	cTexte				vGirouette;			//!< Destination affich�e sur les v�hicules
	//@}
	
	//! \name Parametres
	//@{
	bool			vAAfficherSurTableauDeparts;	//!< Indique si la ligne doit figurer sur les tableaux de d�part
	bool			vAAfficherSurIndicateurs;		//!< Indique si la ligne doit figurer sur les tableaux d'indicateurs horaires
	bool			_AUtiliserDansCalculateur;		//!< Indique si la ligne doit �tre utilis�e dans les recherches d'itin�raire
	cJC				vCirculation;					//!< Calendrier de circulation de la ligne
	//@}
	
public:
	//! \name Constructeurs et destructeur
	//@{
//	cLigne(cLigne* LigneACopier, cEnvironnement* Environnement);
	cLigne(const cTexte& newCode, cAxe* newAxe=NULL);
	~cLigne();
	//@}
	
	//! \name Modificateurs
	//@{
	cGareLigne*			addGareLigne(const cGareLigne* GLigneACopier=NULL, tDistanceM newPM=0, cArretPhysique* newArretPhysique=NULL
								, tTypeGareLigneDA newType=Passage, bool newHorairesSaisis=true
								, bool Route=false);
	void				Ferme();
	void				MajCirculation();
	tNumeroService		NombreServices(int);
	cLigne*				operator =(const cLigne& AutreLigne);
	void				setAAfficherSurTableauDeparts(bool newVal);
	void				setAAfficherSurIndicateurs(bool newVal);
	bool				SetAUtiliserDansCalculateur(bool __Valeur);
	void				setAmplitudeServiceContinu(tNumeroService iNumeroService, const tDureeEnMinutes& newVal);
	void				setAttente(tNumeroService iService, const tDureeEnMinutes& newVal);
	void				setGirouette(const cTexte& newGirouette);
	void				setHoraireDepart(tNumeroService, cHoraire*);
	void				setImage(const cTexte& newImage);
	void				setLibelleComplet(const cTexte& newLibelleComplet);
	void				setLibelleSimple(const cTexte& newNom);
	void				setMateriel(cMateriel*);
	void				setNomPourIndicateur(const cTexte& newNom);
	void				setResa(cModaliteReservation*);
	void				setReseau(cReseau* newReseau);
	void				setJC(tNumeroService i, cJC* newVal);
	void				setServiceContinu(tNumeroService iService);
//	void				setServices(const cTexte& TamponJC, const cTexte& TamponAtt, const cTexte& TamponDernier
//								, size_t LargeurColonne, cEnvironnement* curEnv, ofstream& FichierLOG);
	void				setStyle(const cTexte& newStyle);
	void				setSuivant(cLigne*);
	void				setVelo(cVelo*);
	void				setHandicape(cHandicape*);
	void				setTarif(cTarif*);
	void				setAlerteMessage(cTexte& message);
	void				setAlerteDebut(cMoment& momentDebut);
	void				setAlerteFin(cMoment& momentFin);
	void				setAnneesCirculation(tAnnee, tAnnee);
	bool				setNumeroService(tNumeroService, const cTexte&);
	//@}
	
	// Copie de la ligne sans les services
	cLigne* Copie();
	// Nom libre pour une copie
	char* NomLibrePourCopie();
	
	//! \name Fonctions de modification de la base � chaud
	//@{
//	tNumeroService	addServiceHoraire(const cHoraire* tbHoraires, const cTexte& newNumero, cJC* newCIS, tCodeBaseTrains newCodeBaseTrains=0, tNumeroService NombreNouveauxServices=1);
	bool			allowAddServiceHoraire(const cHoraire*)											const;
	bool			Identifie(const cLieuLogique**, const cMateriel* testMateriel=NULL)					const; // Identification d'un service � la ligne, en fonction des arrets (tous et eux seuls doivent y appartenir) et du materiel (pour importation)
	bool			Sauvegarde()																	const;
//	void				updateServiceHoraire(tNumeroService curNumeroService, const cHoraire* tbHoraires, const cTexte& newNumero, cJC* newCIS, tCodeBaseTrains newCodeBaseTrains=0);
	//@}
	
	//! \name Accesseurs
	//@{
	bool					AAfficherSurTableauDeparts()		const;
	bool					AAfficherSurIndicateurs()			const;
	const tDureeEnMinutes&	Attente(tNumeroService i)			const;
	cAxe*					Axe()								const;
	cGareLigne*				DerniereGareLigne()					const;
	bool					EstCadence(tNumeroService)			const;
	bool					EstUneRoute()						const;
	bool					EstUneLigneAPied()					const;
	const tDureeEnMinutes&	EtalementCadence(tNumeroService)	const;
	const cAlerte*   		getAlerte()							const;
	const cTexte&			getCode()							const;
	const cTexte&			getGirouette()						const;
	cHandicape*				getHandicape()						const;
	const cTexte&			getImage()							const;
	const cTexte&			getLibelleComplet()					const;
	const cTexte&			getLibelleSimple()					const;
	const cTexte&			getNomPourIndicateur()				const;
	cModaliteReservation*	GetResa()							const;
	cReseau*				getReseau()							const;
	const cTexte&			getStyle()							const;
	const cTarif*   		getTarif()							const;
	const cTrain*			GetTrain(tNumeroService)			const;
	const cTrain*			getTrain(tNumeroService)			const;
	cVelo*					getVelo()							const;
	cMateriel*				Materiel()							const;
	tNumeroService			NombreServices()						const;
	cGareLigne*				PremiereGareLigne()					const;
	cLigne*					Suivant()							const;
//	cTexte					Code()								const;
	//@}

	//!	\name Calculateurs
	//@{
	bool					PeutCirculer(const cDate&)			const;
	//@}
	
/*	template <class T> 
	T& toXML(T& Tampon, tNumeroService iNumeroService, cMoment momentDepart, cMoment debutLigne, cMoment finLigne) const
	{
		Tampon << "<ligne>";
		
		//ajout libelle de la ligne
		Tampon << "<libelle>";
		LibelleComplet(Tampon, false);
		Tampon <<"</libelle>";
		
		//ajout destination ligne
		Tampon <<"<destination>";
		cMoment pNull;
		this->PremiereGareLigne()->Destination()->ArretLogique()->toXML(Tampon, pNull, pNull, false);
		Tampon <<"</destination>";
		
		// ajout mat�riel
		vMateriel->toXML(Tampon);
		
		//ajout messages
		bool showMessage = getAlerte()->showMessage(debutLigne, finLigne);
		bool showResa = GetResa()->TypeResa() == Obligatoire;
		if	(showMessage || showResa)
		{
			Tampon << "<warnings>";
			
			if (showMessage)
				getAlerte()->toXML(Tampon);

			if (showResa)
				GetResa()->toXML(Tampon, GetTrain(iNumeroService), momentDepart);
			
			Tampon << "</warnings>";	
		}
		
		Tampon << "</ligne>";
		return(Tampon);
	}
	
*/

};


#include "cLigne.inline.h"


#endif

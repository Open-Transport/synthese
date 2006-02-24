/*!	\file cLigne.h
	\brief En-t�te classe cLigne
*/

#ifndef SYNTHESE_CLIGNE_H
#define SYNTHESE_CLIGNE_H

class LogicalPlace;
class cAxe;
class cGareLigne;
class cVelo;
class cHandicape;
class cReseau;
class cTrain;
class cMateriel;
class cModaliteReservation;
class cTarif;
class cArretPhysique;
class cEnvironnement;

#include <vector>
#include "cJourCirculation.h"
#include "cTexte.h"
#include "cDistanceCarree.h"
#include "cAlerte.h"

/** Impl�mentation de la notion de @ref defLigne
	@ingroup m15
	@author Hugues Romain
	@date 2000-2003
*/
class cLigne
{
public:

	/** Vector of Line-stops */
	typedef vector<cGareLigne*>	LineStops;

	/** Vector of circulations */
	typedef vector<cTrain*> CirculationsVector;

protected:
	//! @name Donn�es et chainages
	//@{
	CirculationsVector					vTrain;				//!< Circulations
	cReseau* 				vReseau;			//!< R�seau de transport
	cAxe* const					vAxe;				//!< Axe
	LineStops	_lineStops;	//!< arrets de la ligne
	cMateriel*				vMateriel;			//!< Mat�riel roulant
	cModaliteReservation*	vResa;				//!< Modalit� de r�servation
	cVelo*					vVelo;				//!< Modalit� de prise en charge des v�los
	cHandicape*				vHandicape;			//!< Modalit� d'acceptation des handicap�s
	cTarif*					vTarif;				//!< Tarification
	cAlerte					vAlerte;			//!< Message d'alerte � afficher
	//@}

	//! \name Identification de la ligne
	//@{
	cTexte				vLibelleSimple;		//!< Libell� mentionn� dans le cartouche
	cTexte				vLibelleComplet;	//!< Libell� mentionn� dans la feuille de route
	cTexte				vStyle;				//!< Style CSS du cartouche
	cTexte				vImage;				//!< Image � afficher en tant que cartouche
	const std::string				vCode;				//!< Code d'identification de la ligne
	cTexteSansAccent	vNomPourIndicateur;	//!< Lib�ll� mentionn� dans les tableaux d'indicateurs horaires
	cTexte				vGirouette;			//!< Destination affich�e sur les v�hicules
	//@}
	
	//! \name Parametres
	//@{
	bool			vAAfficherSurTableauDeparts;	//!< Indique si la ligne doit figurer sur les tableaux de d�part
	bool			vAAfficherSurIndicateurs;		//!< Indique si la ligne doit figurer sur les tableaux d'indicateurs horaires
	bool			_AUtiliserDansCalculateur;		//!< Indique si la ligne doit �tre utilis�e dans les recherches d'itin�raire
	cJC			vCirculation;					//!< Calendrier de circulation de la ligne
	//@}
	
public:
	//! \name Constructeurs et destructeur
	//@{
	cLigne(const std::string& newCode, cAxe* const newAxe, cEnvironnement*);
	~cLigne();
	//@}
	
	//! \name Modificateurs
	//@{
		void				addGareLigne(cGareLigne*);
		void				addService(cTrain* const);
		void				Ferme();
		void				MajCirculation();
		cLigne*				operator =(const cLigne& AutreLigne);
		void				setAAfficherSurTableauDeparts(bool newVal);
		void				setAAfficherSurIndicateurs(bool newVal);
		bool				SetAUtiliserDansCalculateur(bool __Valeur);
		void				setGirouette(const cTexte& newGirouette);
		void				setImage(const cTexte& newImage);
		void				setLibelleComplet(const cTexte& newLibelleComplet);
		void				setLibelleSimple(const cTexte& newNom);
		void				setMateriel(cMateriel*);
		void				setNomPourIndicateur(const cTexte& newNom);
		void				setResa(cModaliteReservation*);
		void				setReseau(cReseau* newReseau);
	//	void				setServices(const cTexte& TamponJC, const cTexte& TamponAtt, const cTexte& TamponDernier
	//								, size_t LargeurColonne, cEnvironnement* curEnv, ofstream& FichierLOG);
		void				setStyle(const cTexte& newStyle);
		void				setVelo(cVelo*);
		void				setHandicape(cHandicape*);
		void				setTarif(cTarif*);
		cAlerte&			getAlerteForModification();
	//@}
	
	// Copie de la ligne sans les services
	cLigne* Copie();
	// Nom libre pour une copie
	char* NomLibrePourCopie();
	
	//! \name Fonctions de modification de la base � chaud
	//@{
	//	tNumeroService	addServiceHoraire(const cHoraire* tbHoraires, const cTexte& newNumero, cJC* newCIS, tCodeBaseTrains newCodeBaseTrains=0, tNumeroService NombreNouveauxServices=1);
		bool			allowAddServiceHoraire(const cHoraire*)											const;
		bool			Sauvegarde()																	const;
	//	void				updateServiceHoraire(tNumeroService curNumeroService, const cHoraire* tbHoraires, const cTexte& newNumero, cJC* newCIS, tCodeBaseTrains newCodeBaseTrains=0);
	//@}
	
	//! \name Accesseurs
	//@{
		bool					AAfficherSurTableauDeparts()		const;
		bool					AAfficherSurIndicateurs()			const;
		cAxe*					Axe()								const;
		cGareLigne*				DerniereGareLigne()					const;
		bool					EstUneLigneAPied()					const;
		const cAlerte&   		getAlerte()							const;
		const std::string&			getCode()							const;
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
		const vector<cTrain*>	getServices()		const { return vTrain; }
		cTrain*					getTrain(size_t)			const;
		cVelo*					getVelo()							const;
		cMateriel*				Materiel()							const;
		const LineStops&		getLineStops()						const { return _lineStops; }
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
		bool showMessage = getAlerte().showMessage(debutLigne, finLigne);
		bool showResa = GetResa()->TypeResa() == Obligatoire;
		if	(showMessage || showResa)
		{
			Tampon << "<warnings>";
			
			if (showMessage)
				getAlerte().toXML(Tampon);

			if (showResa)
				GetResa()->toXML(Tampon, GetTrain(iNumeroService), momentDepart);
			
			Tampon << "</warnings>";	
		}
		
		Tampon << "</ligne>";
		return(Tampon);
	}
	
*/

};


#endif

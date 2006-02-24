/*! \file Environnement.h
	\brief En-t�te classe Environnement
*/

#ifndef SYNTHESE_ENVIRONNEMENT_H
#define SYNTHESE_ENVIRONNEMENT_H

class cEnvironnement;
class cFormatFichier;
class cIndicateurs;
class LogicalPlace;
class cCommune;
class cLigne;
class cDocument;
class cReseau;
class cVelo;
class cHandicape;
class cTarif;

#include "cModaliteReservation.h"
#include <iostream>
#include <map>
#include <vector>
#include <string>
#include "cTexte.h"
#include "Interpretor.h"
#include "cMateriel.h"
#include "Temps.h"
#include "Parametres.h"
#include "cFichierPourEnvironnement.h"
#include "map/Topography.h"
#include "LogicalPlace.h"
#include "cJourCirculation.h"



//! @name Constantes de commande de temps relatives � l'environnement
//@{
#define TEMPS_MIN_ENVIRONNEMENT	'e'
#define TEMPS_MAX_ENVIRONNEMENT	'E'
#define TEMPS_MIN_CIRCULATIONS	'r'
#define TEMPS_MAX_CIRCULATIONS	'R'
//@}


/** Contient toutes les variables partag�es de l'environnement.
	\date 2000-2004
	\author Hugues Romain
	\version 1.0
	@ingroup m15
	
Un environnement est un groupement d'objets d�crivant un espace de calcul autonome. Il contient tout type d'objet. Tout calcul d'itin�raire ou autre recherche se faisant sur un environnement ne fait intervenir que des objets lui appartenant.

Les donn�es sont index�es � l'environnement par un tableau de pointeurs par classe d'objets. Ces tableaux se nomment comme la classe des objets point�s en rempla�ant le c par un v. Les index permettant d'acc�der aux objets sont stock�s sous le type vIndex (qui n'est autre que int). L'index maximal existant dans l'environnement est stock� dans les variables vNombre(classe), chacune de type tIndex �galement. Quelques classes ne sont pas index�es par des tableaux de pointeurs :
 - la classe cLigne assure elle m�me un cha�nage en liste (seule la premi�re ligne est point�e par vPremiereLigne) pour des raisons d'inutilit�
 - la class cAccesPADe est cha�n�e au niveau des communes et des points d'arr�t

Les m�thodes de l'environnement sont class�es en plusieurs groupes
 - Son constructeur charge toutes les donn�es en m�moire � partir des fichiers.
 - Les m�thodes de chargement batissent l'environnement � partir de fichiers de description (.per)
 - Les m�thodes d'enregistrement enregistrent un objet orphelin dans l'environnement. Elles assurent l'ensemble des cha�nages tels que d�finis dans la structure de donn�es de l'environnement. L'ensemble de ces m�thodes se nomme Enregistre. Une m�thode est d�finie par classe � enregistrer. Elles retournent le num�ro d'index attribu� � l'objet, INCONNU si l'enregistrement n'a pu �tre men� � bien.
 - Les modificateurs donnent un acc�s en �criture aux variables param�tres de l'environnement
 - Les accesseurs donnent un acc�s en lecture aux variables param�tres de l'environnement
 - Les calculateurs produisent des r�sultats de calculs simples faisant intervenir les objets appartenant � l'environnement (Les proc�dures de calculs complexes sont effectu�es par la classe cCalculItineraire qui contient un espace de calcul adapt�)
*/
class cEnvironnement
{
public:

	/** lines map */
	typedef std::map<std::string, cLigne*>	LinesMap;

	/** logical places map */
	typedef std::map<size_t, LogicalPlace*> LogicalPlacesMap;

	/** towns map */
	typedef interpretor::Interpretor<cCommune*> TownsMap;

	/** calendars map */
	typedef std::map<size_t, cJC*>	CalendarsMap;

	/** documents map */
	typedef std::map<size_t, cDocument*> DocumentsMap;

	/** fares map */
	typedef std::map<size_t, cTarif*> FaresMap;

	/** reservation rules map */
	typedef std::map<size_t, cModaliteReservation*> ReservationRulesMap;

	/** rolling stock map */
	typedef std::map<size_t, cMateriel*> RollingStockMap;

	/** networks map */
	typedef std::map<size_t, cReseau*> NetworksMap;

	/** handicapped compliances map */
	typedef std::map<size_t, cHandicape*> HandicappedCompliancesMap;

	/** bikes compliances map */
	typedef std::map<size_t, cVelo*> BikeCompliancesMap;

	/** timetables map */
	typedef std::map<size_t, cIndicateurs*> TimeTablesMap;

private:
	//! \name SYNTHESE environment database
	//@{
		LogicalPlacesMap				_logicalPlaces;		//!< Arrêts logiques enregistrés
		TownsMap							_towns;	//!< Communes
		CalendarsMap				_JC;				//!< Calendriers de circulations
		DocumentsMap			_Documents;			//!< Documents
		ReservationRulesMap				vResa;			//!< Modalit�s de r�servation
		NetworksMap							vReseau;		//!< R�seaux de transport
		BikeCompliancesMap					vVelo;			//!< Modalit�s de prise en charge des v�los
		HandicappedCompliancesMap			vHandicape;		//!< Modalit�s d'acceptation des handicap�s
		FaresMap				vTarif;			//!< Tarifications
		RollingStockMap				vMateriel;		//!< Mat�riels roulants
		TimeTablesMap						vIndicateurs;	//!< Indicateurs horaires @todo to be moved outside of the environment like cSite
		LinesMap								_lines;	//!< Transport lines
	//@}
	
	//! \name Parametres
	//@{
		tAnnee			vPremiereAnnee; 	//!< Premi�re ann�e des calendriers de circulation en m�moire
		tAnnee			vDerniereAnnee; 	//!< Derni�re ann�e des calendriers de circulation en m�moire
		const size_t			Code;				//!< Code de l'environnement
	//@}
	
	//! \name Variables d'analyse
	//@{
		cDate			vDateMin;			//!< Premier jour de circulation effective dans la base de donn�es
		cDate			vDateMax;			//!< Dernier jour de circulation effective dans la base de donn�es
	//@}

	//! \name Noms des fichiers de donn�es (� remplacer par des cFichier)
	//@{
		cTexte			vNomFichier; 			//!< Partie commune des noms de fichiers
		const std::string	_path;			//!< Partie commune des envs
		cTexte			vNomFichierPhotos;
		cTexte			vNomFichierMateriel;
		cTexte			vNomFichierReseaux;
		cTexte			vNomFichierVelo;
		cTexte			vNomFichierHandicape;
		cTexte			vNomFichierTarif;
		cTexte			vNomFichierResa;
		cTexte          vNomRepertoireHoraires;
		std::string          vNomRepertoireCarto;
	//@}

	//! \name Formats de fichiers (� remplacer par des cFichier)
	//@{
		cFormatFichier*		_FormatHoraire;
		cFormatFichier*		_FormatResa;
		cFormatFichier*		_FormatPhoto;
		cFormatFichier* 	_FormatVelo;
		cFormatFichier* 	_FormatHandicape;
		cFormatFichier* 	_FormatTarif;
		cFormatFichier* 	_FormatReseaux;
		cFormatFichier* 	_FormatMateriel;

		synmap::Topography      _topography;

	//@}

	
	//! \name Fonctions de Chargement (� int�grer � un h�ritage de cFichier avec une m�thode virtuelle Charge)
	//@{
		bool		ChargeFichierHoraires(const cTexte& NomFichier);
		bool		ChargeFichierMateriel();
		bool		ChargeFichierReseaux();
		bool		ChargeFichierVelo();
		bool		ChargeFichierHandicape();
		bool		ChargeFichierTarif();
		bool		ChargeFichierResa();
		bool		ChargeFichierPhotos();
		bool            ChargeFichiersRoutes ();
	//@}

	
public:
	//!	\name M�thodes d'enregistrement
	//@{
		bool		Enregistre(cLigne* const);
		bool		Enregistre(cJC* const);
		bool		Enregistre(cModaliteReservation* const);
		bool		Enregistre(cTarif* const);
		bool		Enregistre(cMateriel* const);
		bool		Enregistre(cHandicape* const);
		bool		Enregistre(cVelo* const);
		bool		Enregistre(cReseau* const);
		bool		Enregistre(LogicalPlace* const);
	//@}

	bool envOk;

	bool		ChargeFichierIndicateurs();


	
	//! \name Modificateurs
	//@{
		void			addTown(cCommune* const);
	//	cSauvegarde*	JCSauvegardeModifier(tNumeroJC NumeroNewJC, const cTexte& newIntitule);
		void			JCSupprimerInutiles(bool Supprimer=false);
		void			SetDateMinReelle(const cDate&);
		void			SetDateMaxReelle(const cDate&);
		void			SetDatesService(tAnnee, tAnnee);
	//@}

	//! \name Calculateurs
	//@{
		vector<cCommune*>	searchTown(const std::string&, size_t n=0)										const;
		cCommune*			getTown(const std::string&)	const;
		void				NomLigneUnique(cTexte& NomBase)														const;
		bool				ControleNumerosArretCommuneDesignation(int nA, int nC, const cTexte& txtA)	const;
		bool				ControleNumeroTexteCommune(int nC, const cTexte& txtC)								const;
		size_t			ProchainNumeroJC()																	const;
		cDate				dateInterpretee(const cTexte& Texte)												const;
		bool				ControleDate(const cDate&)															const;
	//@}

	//! \name Accesseurs
	//@{
		cCommune*				getTown(size_t)												const;
		LogicalPlace*			getLogicalPlace(size_t)										const;
		cDate					DateMaxPossible()											const;
		cDate					DateMinPossible()											const;
		const cDate&			DateMaxReelle()												const;
		const cDate&			DateMinReelle()												const;
		tAnnee					DerniereAnnee()												const;
		cJC*					GetJC(size_t)												const;
		cJC*					GetJC(const cJC::Calendar& MasqueAIdentifer, const cJC& JCBase)	const;
		cLigne*					GetLigne(const std::string&)										const;
		cMateriel*				GetMateriel(size_t)										const;
		const cTexte&			getNomRepertoireHoraires()									const;
		cDocument*				GetDocument(size_t)											const;
		cModaliteReservation*	getResa(size_t)												const;
		cVelo*					getVelo(size_t)											const;
		cReseau*				getReseau(size_t)											const;
		cHandicape*				getHandicape(size_t)										const;
		cTarif*					getTarif(size_t n)									const;
		const size_t&	Index()														const;
		tAnnee					NombreAnnees(tAnnee)										const;
		tAnnee					NombreAnnees()												const;
		size_t					NombreLignes(const cTexte& MasqueCode)						const; 
		tAnnee					PremiereAnnee()												const;
		
		synmap::Topography&                             getTopography () { return _topography; }
	//@}
	
//	bool				AfficheListeGaresAlpha() const;

	
	// Chargement horaires
	void RemplitDistances(LogicalPlace*, LogicalPlace*, int);
	

	// Fonctions Indicateurs
	bool ConstruitIndicateur();
	void ConstruitColonnesIndicateur(cIndicateurs*);


	//! \name Constructeurs et destructeurs
	//@{	
	cEnvironnement(std::string directory, std::string pathToFormat, size_t id);
	~cEnvironnement();
	//@}
};

/** @} */


#endif

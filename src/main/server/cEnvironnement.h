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



/** @defgroup m05 05 Classes m�tier
	@{
*/


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

private:
	//! \name Attributs priv� d�crivant les tailles des tableaux d'index 
	//@{ 
		tIndex	vNombreResa;			//!< Index maximal des modalit�s de r�servation
		tIndex	vNombreIndicateurs;		//!< Index maximal des paragraphes d'indicateur horaire
		tIndex	vNombreMateriels;		//!< Index maximal des mat�riels roulants
		tIndex	vNombreReseaux;			//!< Index maximal des r�seaux de transport
		tIndex	vNombreVelo;			//!< Index maximal des modalit�s de prise en charge des v�los
		tIndex	vNombreHandicape;		//!< Index maximal des modalit�s d'acceptation des handicap�s
		tIndex 	vNombreTarif;			//!< Index maximal des tarifications
	//@}
	
	//! \name Tableaux priv�s indexant les donn�es
	//@{
		LogicalPlacesMap				_logicalPlaces;		//!< Arrêts logiques enregistrés
		TownsMap							_towns;	//!< Communes
		CalendarsMap				_JC;				//!< Calendriers de circulations
		DocumentsMap			_Documents;			//!< Documents
		std::vector<cModaliteReservation*>				vResa;			//!< Modalit�s de r�servation
		std::vector<cReseau*>							vReseau;		//!< R�seaux de transport
		std::vector<cVelo*>					vVelo;			//!< Modalit�s de prise en charge des v�los
		std::vector<cHandicape*>			vHandicape;		//!< Modalit�s d'acceptation des handicap�s
		FaresMap				vTarif;			//!< Tarifications
		std::vector<cMateriel*>				vMateriel;		//!< Mat�riels roulants
		std::map<size_t, cIndicateurs*>						vIndicateurs;	//!< Indicateurs horaires
		LinesMap								_lines;	//!< Transport lines
	//@}
	
	//! \name Parametres
	//@{
		tAnnee			vPremiereAnnee; 	//!< Premi�re ann�e des calendriers de circulation en m�moire
		tAnnee			vDerniereAnnee; 	//!< Derni�re ann�e des calendriers de circulation en m�moire
		const tIndex			Code;				//!< Code de l'environnement
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
		cFormatFichier* 	_FormatIndicateurs;
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
		bool		Enregistre(cLigne*);
		tIndex		Enregistre(cJC*, tIndex Index=INCONNU);
		tIndex		Enregistre(cModaliteReservation*, tIndex);
		bool		Enregistre(cTarif* const);
		tIndex		Enregistre(cMateriel*, tIndex);
		tIndex		Enregistre(cHandicape*, tIndex);
		tIndex		Enregistre(cVelo*, tIndex);
		tIndex		Enregistre(cReseau*, tIndex);
	//@}

	bool envOk;

	bool		ChargeFichierIndicateurs();


	
	//! \name Modificateurs
	//@{
		void			addLogicalPlace(LogicalPlace*);
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
		cJC*					GetJC(const tMasque* MasqueAIdentifer, const cJC& JCBase)	const;
		cLigne*					GetLigne(const std::string&)										const;
		cMateriel*				GetMateriel(tIndex n)										const;
		const cTexte&			getNomRepertoireHoraires()									const;
		cDocument*				GetDocument(size_t)											const;
		cModaliteReservation*	getResa(tIndex)												const;
		cVelo*					getVelo(tIndex n)											const;
		cReseau*				getReseau(tIndex n)											const;
		cHandicape*				getHandicape(tIndex)										const;
		cTarif*					getTarif(size_t n)									const;
		tIndex					Index()														const;
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

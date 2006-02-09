/*! \file Environnement.h
	\brief En-t�te classe Environnement
*/

#ifndef SYNTHESE_ENVIRONNEMENT_H
#define SYNTHESE_ENVIRONNEMENT_H

class cEnvironnement;
class cFormatFichier;


#include "cModaliteReservation.h"
#include <iostream>
#include <vector>
#include <string>
#include "cTexte.h"
#include "cIndicateurs.h"
#include "cMateriel.h"
#include "cJourCirculation.h"
#include "Temps.h"
#include "cArretLogique.h"
#include "cPhoto.h"
#include "cReseau.h"
#include "cVelo.h"
#include "cHandicape.h"
#include "cTarif.h"
#include "cCalculItineraire.h"
#include "cDescriptionPassage.h"
#include "Parametres.h"
#include "cFichierPourEnvironnement.h"
#include "cDocument.h"
#include "map/Topography.h"


using namespace synmap;


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
	cTableauDynamiquePointeurs<cArretLogique*>				_ArretLogique;		//!< Points d'arr�t logiques
	cTableauDynamiquePointeurs<cCommune*>			_Commune;			//!< Communes
	cTableauDynamiquePointeurs<cJC*>				_JC;				//!< Calendriers de circulations
	cTableauDynamiquePointeurs<cDocument*>			_Documents;			//!< Documents
	std::vector<cModaliteReservation*>				vResa;			//!< Modalit�s de r�servation
	std::vector<cReseau*>							vReseau;		//!< R�seaux de transport
	std::vector<cVelo*>					vVelo;			//!< Modalit�s de prise en charge des v�los
	std::vector<cHandicape*>			vHandicape;		//!< Modalit�s d'acceptation des handicap�s
	std::vector<cTarif*>				vTarif;			//!< Tarifications
	std::vector<cMateriel*>				vMateriel;		//!< Mat�riels roulants
	cIndicateurs**						vIndicateurs;	//!< Indicateurs horaires
	cLigne*								vPremiereLigne;	//!< Acc�s � la premi�re ligne de transport
	//@}
	
	//! \name Parametres
	//@{
	tAnnee			vPremiereAnnee; 	//!< Premi�re ann�e des calendriers de circulation en m�moire
	tAnnee			vDerniereAnnee; 	//!< Derni�re ann�e des calendriers de circulation en m�moire
	tIndex			Code;				//!< Code de l'environnement
	//@}
	
	//! \name Variables d'analyse
	//@{
	cDate			vDateMin;			//!< Premier jour de circulation effective dans la base de donn�es
	cDate			vDateMax;			//!< Dernier jour de circulation effective dans la base de donn�es
	//@}

	//! \name Noms des fichiers de donn�es (� remplacer par des cFichier)
	//@{
	cTexte			vNomFichier; 			//!< Partie commune des noms de fichiers
	cTexte			vNomRepEnv;			//!< Partie commune des envs
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

	Topography      _topography;

	//@}

	//!	\name Espaces de calcul pour threads
	//@{
	tIndex			_NombreCalculateurs;	//!< Nombre d'espaces m�moires disponibles pour les calculs
	cCalculateur*	_Calculateur;			//!< Espaces
	//m�moires d�di�s aux threads
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
	bool		Enregistre(cAccesPADe*, tTypeAccesPADe);
	tIndex		Enregistre(cJC*, tIndex Index=INCONNU);
	tIndex		Enregistre(cModaliteReservation*, tIndex);
	tIndex		Enregistre(cTarif*, tIndex);
	tIndex		Enregistre(cCommune*);
	tIndex		Enregistre(cMateriel*, tIndex);
	tIndex		Enregistre(cHandicape*, tIndex);
	tIndex		Enregistre(cVelo*, tIndex);
	tIndex		Enregistre(cReseau*, tIndex);
	//@}

	bool envOk;

	bool		ChargeFichierIndicateurs();

	
	// Chargement points d'arr�t
	void TrieAlphabetiquePointsArret();
	cTableauDynamique<tIndex>	ArretLogiqueAlpha;

	
	//! \name Modificateurs
	//@{
	cCommune*		GetCommuneAvecCreation(const cTexte& Entree);
	cArretLogique*			GetGareAvecCreation(tNiveauCorrespondance newNiveauCorrespondance, tIndex newNumeroArretLogique=0);
//	cSauvegarde*	JCSauvegardeModifier(tNumeroJC NumeroNewJC, const cTexte& newIntitule);
	void			JCSupprimerInutiles(bool Supprimer=false);
	void			SetDateMinReelle(const cDate&);
	void			SetDateMaxReelle(const cDate&);
	void			SetDatesService(tAnnee, tAnnee);
	bool			Charge(const cTexte& __Chemin, const cTexte& __CheminFormats);
	bool			SetIndex(tIndex);
	
	//@}

	//! \name Calculateurs
	//@{
	cCalculateur*		CalculateurLibre();
	cCommune**			TextToCommune(const cTexte& Entree, size_t n=0)										const;
	void				NomLigneUnique(cTexte& NomBase)														const;
	bool				ControleNumerosArretCommuneDesignation(int nA, int nC, int nD, const cTexte& txtA)	const;
	bool				ControleNumeroTexteCommune(int nC, const cTexte& txtC)								const;
	tIndex				ProchainNumeroJC()																	const;
	cDate				dateInterpretee(const cTexte& Texte)												const;
	bool				ControleDate(const cDate&)															const;
	//@}

	//!	\name Accesseurs tableaux avec droit de modification
	//@{
	cTableauDynamique<cJC*>&				TableauJC();
	cTableauDynamique<cDocument*>&			TableauDocuments();
	cTableauDynamique<cArretLogique*>&				TableauPointsArret();
	//@}
	
	//! \name Accesseurs
	//@{
	cDate					DateMaxPossible()											const;
	cDate					DateMinPossible()											const;
	const cDate&			DateMaxReelle()												const;
	const cDate&			DateMinReelle()												const;
	tAnnee					DerniereAnnee()												const;
	cCalculateur&			getCalculateur(size_t n);
	cCommune*				getCommune(tIndex NumeroCommune)							const;
	cCommune*				GetCommune(int NumeroCommune)								const;
	cJC*					GetJC(int n)												const;
	cJC*					getJC(tIndex n)												const;
	cJC*					GetJC(const tMasque* MasqueAIdentifer, const cJC& JCBase)	const;
	cLigne*					GetLigne(const cTexte&)										const;
	cMateriel*				GetMateriel(tIndex n)										const;
	const cTexte&			getNomRepertoireHoraires()									const;
	cDocument*				GetDocument(tIndex)											const;
	cArretLogique*					getArretLogique(tIndex)										const;
	cArretLogique*					GetArretLogique(int n)										const;
	cModaliteReservation*	getResa(tIndex)												const;
	cVelo*					getVelo(tIndex n)											const;
	cReseau*				getReseau(tIndex n)											const;
	cHandicape*				getHandicape(tIndex)										const;
	cTarif*					getTarif(tNumeroTarif n)									const;
	tIndex					Index()														const;
	bool					isTarif(tNumeroTarif n)										const;
	tAnnee					NombreAnnees(tAnnee)										const;
	tAnnee					NombreAnnees()												const;
	size_t					NombreLignes(const cTexte& MasqueCode)						const; 
	tIndex					NombrePointsArret(bool Reel = false)						const;
	tAnnee					PremiereAnnee()												const;
	cLigne*					PremiereLigne()												const;
	tIndex	  				getNombreTarif()											const;
	
	Topography&                             getTopography () { return _topography; }
	//@}
	
//	bool				AfficheListeGaresAlpha() const;

	
	// Chargement horaires
	void RemplitDistances(cArretLogique*, cArretLogique*, int);
	void RemplitProchainAxe();
	void RemplitCIL();


	// Fonctions Indicateurs
	bool ConstruitIndicateur();
	void ConstruitColonnesIndicateur(cIndicateurs*);


	//! \name Constructeurs et destructeurs
	//@{	
	cEnvironnement(int __NombreCalculateurs);
	~cEnvironnement();
	//@}
};

/** @} */

#include "cEnvironnement.inline.h"

#endif

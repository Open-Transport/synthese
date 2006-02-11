/*!	\file SYNTHESE.h
	\brief En-t�te classe principale de l'application SYNTHESE
*/

#ifndef SYNTHESE_SYNTHESE_H
#define SYNTHESE_SYNTHESE_H

class SYNTHESE;

#include "cTexte.h"
#include "Temps.h"
#include "cSite.h"
#include "cEnvironnement.h"
#include "cInterface.h"
#include "cCalculItineraire.h"
#include "cFichiersPourSYNTHESE.h"
#include "cTexteRequeteSYNTHESE.h"
#include "cLog.h"
#include "cTableauAffichage.h"
#include "cAssociator.h"

/**	@defgroup m00 00 Architecture : serveur TCP et main


	@section m00principale Architecture principale

	Pour pouvoir �tre appel� par plusieurs applications, le serveur SYNTHESE est un serveur TCP autonome, tout comme MySQL par exemple.

	Pour l'interroger, des modules clients TCP sont d�velopp�s dans les applications clientes :
		- Les clients autonomes : @ref m70, @ref m71, @ref m72
		- Un client TCP int�gr� au r�f�rentiel r�seau (voir projet R�f�rentiel r�seau), permettant l'affichage cartographiques de donn�es inconnues de SYNTHESE
	
	@section m00carto Dispositions particuli�res fonctionnalit�s cartographiques

	L'utilisation des fonctionnalit�s cartographiques permettent de fournir au sein de la requ�te des donn�es compl�mentaires � afficher. A titre indicatif, l'utilisation pr�vue des clients pour le trac� de carte est la suivante :

	<table class="tableau">
	<tr><th>Client</th><th>Fournisseur de donn�es</th><th>Utilisateur de donn�es globales</th><th>Type de s�lection</th></tr>
	<tr><th>R�f�rentiel r�seau</th><td>Objets</td><td>Fonds	X, Y, �chelle</td></tr>
	<tr><th>Client CGI</th><td>NON</td><td>Fonds + Objets</td><td>X,Y, �chelle</td></tr>
	</table>

	@subsection archicarte Architecture interne serveur cartographique modules 3 Trac� de carte

	Chaque carte est obtenue par le passage d'une requ�te XML, compos�e de trois groupes d'�l�ments :
	 - Des donn�es dont la dur�e de vie est propre � la requ�te
	 - Des instructions de s�lection des donn�es � afficher
	 - Des param�tres d'affichage de la carte dont le format de sortie

	L'interpr�tation d'une requ�te donne lieu, en fonction du format de sortie, � la fabrication d'une carte vectorielle et/ou � la fabrication d'une table de liens, en fonction des crit�res de s�lection de donn�es sp�cifi�es dans la requ�te, et � partir des donn�es globales charg�es dans le serveur, et des donn�es sp�cifiques fournies dans la requ�te.

	La carte vectorielle est initialement cod�e selon le standard PostScript, puis peut �tre export�e sous divers formats gr�ce � la biblioth�que GhostScript (PDF, JPEG, etc�), ou bien peut �tre directement enregistr�e au format PS pour impression.

	La table de liens est stock�e en m�moire sur des structure de donn�es simples (tableaux dynamiques) et est export�e selon divers formats (PDF, HTML, etc.)

	Le sch�ma d'architecture interne concernant les fonctionnalit�s cartographiques est le suivant :

	@image html m3.png


	@{
*/


/**	Application SYNTHESE
	@author Hugues Romain
	@date 2005
*/
class SYNTHESE
{
	//!	\name Tableaux de donn�es
	//@{
	cTableauDynamiquePointeurs<cEnvironnement*>		_Environnement;		//!< Environnements de donn�es
	cTableauDynamiquePointeurs<cInterface*>			_Interface;			//!< Interfaces d'affichage
	cTableauDynamiquePointeurs<cSite*>				_Site;				//!< Sites d'acc�s
	cTableauDynamiquePointeurs<cTableauAffichage*>	_TableauxAffichage;	//Tableaux d'affichage
    cAssociator *_Associator; // Dictionnaire des arrets et lieux
	//@}
	
	//!	\name Fichier de log
	//@{
	tNiveauLog		_NiveauLOG;				//!< Niveau de log
	cTexte			_CheminLOG;				//!< R�pertoire ou poser les fichiers de trace (vide = pas de trace)
	cLog			_FichierLogBoot;		//!< Fichier de log du boot
	cLog			_FichierLogAcces;		//!< Fichier de log des acc�s
//	cLog			_FichierLogResa;		//!< Fichier de log des r�servations
	cTableauDynamiqueObjets<cTexte> 	_MessageStandard;		//!< Messages standard
	cTableauDynamiqueObjets<cTexte>		_CodesMessageStandard;	//!< Codes Messages standard
	cTableauDynamique<tNiveauLog>		_NiveauMessageStandard;	//!< Messages standard
    map<long,cCalculateur*> _ThreadCalculateur; //!< Lien thread->calculateur
	friend void cLog::Ecrit(tNumeroMessageStandard, const cTexte&, const cTexte&);
	//@}
	
	//!	\name Fonctions de gestion des logs
	//@{
	void			OuvrirLogs();
	void			ChargeMessagesStandard();
	//@}
	
	//!	\name Fonctions pouvant �tre appel�es dans une requ�te
	//@{
	bool FicheHoraire(ostream &pCtxt, ostream& pCerr, const cSite* __Site
					, tIndex NumeroGareOrigine, tIndex NumeroGareDestination
					, const cDate& DateDepart, tIndex codePeriode
					, tBool3 velo, tBool3 handicape, tBool3 taxibus, tIndex tarif
					, tIndex NumeroDesignationOrigine, tIndex NumeroDesignationDestination
					, long vThreadId);
	bool ListeCommunes(ostream &pCtxt, ostream& pCerr, const cSite* __Site
					, bool depart, const cTexte& Entree)												const;
	bool ListeArrets(ostream &pCtxt, ostream& pCerr, const cSite* __Site
					, bool depart, const cTexte& Commune, tIndex NumeroCommune, const cTexte& Arret)	const;
	bool Accueil(ostream &pCtxt, ostream& pCerr, const cSite* __Site)									const;
	bool ValidFH(ostream &pCtxt, ostream& pCerr, const cSite* __Site
					, const cTexte& txtCD, tIndex nCD, tIndex nAD, tIndex nDD
					, const cTexte& txtCA, tIndex nCA, tIndex nAA, tIndex nDA
					, const cTexte& txtAD, const cTexte& txtAA
					, const cDate& DateDepart, tIndex codePeriode
					, tBool3 velo, tBool3 handicape, tBool3 taxibus, tNumeroTarif tarif)				const;
	bool FormulaireReservation(ostream &pCtxt, ostream& pCerr, const cSite* __Site
					, const cTexte& tCodeLigne, tIndex iNumeroService
					, tIndex iNumeroPADepart, tIndex iNumeroPAArrivee, const cDate& tDateDepart)		const;
	bool ValidationReservation(ostream &pCtxt, ostream& pCerr, const cSite* __Site
					, const cTexte& CodeLigne
					, tIndex NumeroService, tIndex iNumeroPADepart, tIndex iNumeroPAArrivee
					, const cDate& tDateDepart, const cTexte& tNom
					, const cTexte& tPrenom, const cTexte& tAdresse, const cTexte& tEmail
					, const cTexte& tTelephone, const cTexte& tNumAbonne
					, const cTexte& tAdressePADepart, const cTexte& tAdressePAArrivee
					, int iNombrePlaces)																const;
	bool AnnulationReservation(ostream &pCtxt, ostream& pCerr, const cSite* __Site
					, const cTexte& CodeReservation, const cTexte& Nom)									const;
	bool TableauDepartsGare(ostream& pCtxt, ostream& pCerr, const cTableauAffichage*
					, const cMoment&)																	const;
	//@}

public:
	//!	\name Ex�cution de requ�te
	//@{
	bool ExecuteRequete(ostream& pCtxt, ostream& pCerr, cTexteRequeteSYNTHESE&, long vThreadId);
	//@}
	//!	\name Terminaison forc�e d'un calculateur
	//@{
	bool TermineCalculateur(long vThreadId);
	//@}

	//!	\name M�thodes d'enregistrement
	//@{
	tIndex	Enregistre(cSite*);
	tIndex	Enregistre(cTableauAffichage*);
	tIndex	Enregistre(cEnvironnement*, tIndex Index = INCONNU);
	tIndex	Enregistre(cInterface*, tIndex Index = INCONNU);
	//@}

	//!	\name Modificateurs
	//@{
	bool	InitAssociateur(const cTexte& NomAssociateur);
	bool	Charge(const cTexte& NomFichier, int __NombreCalculateursParEnvironnement);
	void	SetNiveauLog(tNiveauLog);
	void	SetCheminLog(const cTexte&);
	//@}
	
	//!	\name Accesseurs tableaux avec droit de modification
	//@{
	cTableauDynamique<cEnvironnement*>&		TableauEnvironnements();
	cTableauDynamique<cSite*>&				TableauSites();
	cTableauDynamique<cInterface*>&			TableauInterfaces();
	cTableauDynamique<cTableauAffichage*>&	TableauTableauxAffichage();
	//@}
	
	//!	\name Accesseurs logs avec droit de modification
	//@{
	cLog&								FichierLogBoot();
	cLog&								FichierLogAcces();
	//@}
	
	//!	\name Accesseurs
	//@{
	const cEnvironnement*		GetEnvironnement(tIndex)	const;
	const cInterface*			GetInterface(tIndex)		const;
	const cSite*				GetSite(tIndex)				const;
	const cSite*				GetSite(const cTexte&)		const;
	const cTableauAffichage*	GetTbDep(const cTexte&)		const;
	tNiveauLog					getNiveauLog()				const;
	const cTexte&				getCheminLog()				const;
	//@}
	
	//!	\name Constructeur destructeur
	//@{
	SYNTHESE();
	~SYNTHESE();
	//@}
	
};

/** @} */

#include "SYNTHESE.inline.h"

#endif

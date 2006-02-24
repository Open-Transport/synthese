/*!	\file SYNTHESE.h
	\brief En-tï¿½te classe principale de l'application SYNTHESE
*/

#ifndef SYNTHESE_SYNTHESE_H
#define SYNTHESE_SYNTHESE_H

class SYNTHESE;
class cInterface;
class cEnvironnement;
class cSite;
class cTableauAffichage;
class cCalculateur;
class TimeTables;

#include "cTexte.h"
#include "Temps.h"
#include "cFichiersPourSYNTHESE.h"
#include "cTexteRequeteSYNTHESE.h"
#include "cLog.h"

/**	Application SYNTHESE
	@author Hugues Romain
	@date 2005
	@ingroup m10
*/
class SYNTHESE
{
public:
	/** Environments map */
	typedef std::map<size_t, cEnvironnement*> EnvironmentsMap;

	/** Interfaces map */
	typedef std::map<size_t, cInterface*> InterfacesMap;

	/** Sites map */
	typedef std::map<std::string, cSite*> SitesMap;

	/** Departures tables map */
	typedef std::map<std::string, cTableauAffichage*> DeparturesTablesMap;

	/** TimeTables map */
	typedef std::map<std::string, TimeTables*> TimeTablesMap;

private:

	//!	\name Tableaux de donnï¿½es
	//@{
	EnvironmentsMap		_Environnement;		//!< Environnements de donnï¿½es
	InterfacesMap			_Interface;			//!< Interfaces d'affichage
	SitesMap				_Site;				//!< Sites d'accï¿½s
	DeparturesTablesMap	_TableauxAffichage;	//!< Tableaux d'affichage
	TimeTablesMap		_timeTables;		//!< Time tables collections
	//@}
	
	//!	\name Fichier de log
	//@{
	tNiveauLog		_NiveauLOG;				//!< Niveau de log
	cTexte			_CheminLOG;				//!< Rï¿½pertoire ou poser les fichiers de trace (vide = pas de trace)
	cLog			_FichierLogBoot;		//!< Fichier de log du boot
	cLog			_FichierLogAcces;		//!< Fichier de log des accï¿½s
//	cLog			_FichierLogResa;		//!< Fichier de log des rï¿½servations
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
	
	//!	\name Fonctions pouvant ï¿½tre appelï¿½es dans une requï¿½te
	//@{
	bool FicheHoraire(ostream &pCtxt, ostream& pCerr, const cSite* __Site
					, tIndex NumeroGareOrigine, tIndex NumeroGareDestination
					, const cDate& DateDepart, tIndex codePeriode
					, tBool3 velo, tBool3 handicape, tBool3 taxibus, tIndex tarif
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
					, tBool3 velo, tBool3 handicape, tBool3 taxibus, tIndex tarif)				const;
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
	//!	\name Exï¿½cution de requï¿½te
	//@{
	bool ExecuteRequete(ostream& pCtxt, ostream& pCerr, cTexteRequeteSYNTHESE&, long vThreadId);
	//@}
	//!	\name Terminaison forcée d'un calculateur
	//@{
	bool TermineCalculateur(long vThreadId);
	//@}

	//!	\name Mï¿½thodes d'enregistrement
	//@{
	bool	Enregistre(cSite*);
	bool	Enregistre(cTableauAffichage*);
	bool	Enregistre(cEnvironnement*);
	bool	Enregistre(cInterface*);
	bool	Enregistre(TimeTables*);
	//@}

	//!	\name Modificateurs
	//@{
//	bool	InitAssociateur(const cTexte& NomAssociateur);
	bool	Charge(const cTexte& NomFichier);
	void	SetNiveauLog(tNiveauLog);
	void	SetCheminLog(const cTexte&);
	//@}
	
	//!	\name Accesseurs logs avec droit de modification
	//@{
	cLog&								FichierLogBoot();
	cLog&								FichierLogAcces();
	//@}
	
	//!	\name Accesseurs
	//@{
	cEnvironnement*		GetEnvironnement(size_t)	const;
	cInterface*			GetInterface(size_t)		const;
	cSite*				GetSite(const std::string&)		const;
	cTableauAffichage*	GetTbDep(const std::string&)		const;
	TimeTables*			getTimeTables(const std::string&)	const;
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

#endif

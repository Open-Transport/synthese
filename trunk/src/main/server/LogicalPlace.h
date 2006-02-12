/** En-tête classe Lieu logique.
	@file cLieuLogique.h
*/

#ifndef SYNTHESE_LOGICALPLACE_H
#define SYNTHESE_LOGICALPLACE_H

#include "Point.h"
#include "Temps.h"
#include "cTexte.h"
#include "cAlerte.h"
#include <vector>
#include <map>
#include "map/Address.h"

class cCommune;
class NetworkAccessPoint;
class cGareLigne;
class cArretPhysique;
class cLigne;

/** Classe lieu logique.
	@ingroup m03

	Un lieu logique est un regrouppement de lieux physiques ponctuels permettant l'entrée dans les réseaux de transport et de voirie.

	Un lieu logique est donc "composé" par :
		- des arrêts physiques, points d'entrée sur le réseau de transport
		- des adresses, points d'entrée sur le réseau de voirie

	Le lieu logique porte les propriétés suivantes :
		- chaque point associé au lieu logique est considéré comme équivalent, d'un point de vue de l'accès au lieu logique (ex : si le lieu logique est un 
		  batiment, alors chaque porte d'entrée est considérée comme équivalente pour atteindre le batiment. Dans le cas contraire, le lieu doit être divisé.
		- le lieu logique permet le passage d'un point d'entrée à un autre de manière "indolore" : le temps pour réaliser le transfert n'est pas décompté comme
		  du temps de marche dans le cadre du paramètre de limitation de la marche à pied.
		- le passage d'un point d'entrée à un autre permet la notion de "correspondance", soumise à autorisation (attributs booléens dédiés). Lorsque disponible,
		  la corresponsance est soumise à un délai fourni dans les données, à calculer le plus rationnellement possible. Ce délai peut intégrer une marge de 
		  sécurité liée au "risque" que constitue le transfert d'un élément d'un réseau à un autre. Ex : une correspondance en gare ferroviaire inclue généralement
		  quelques minutes de marge de sécurité pour ne pas être mise en péril par les trains de certaines sociétés ferroviaires fréquemment en retard.
		- un lieu logique peut en "inclure" un autre, permettant de traiter la problématique des arrets "tout lieu", et permettant également de constituer des alias
		- un lieu logique est toujours inscrit sur une commune. Si un lieu se trouve à cheval sur deux communes, le lieu doit être créé sur une commune, et un 
		  alias doitêtre créé sur l'autre
	    
	Les utiliations courantes du lieu logique sont les suivantes :
	<table class="Tableau">
	<tr><th>Type</th><th>Arrêt(s)</th><th>Adresses(s)</th><th>Route</th><th>Alias</th><th>Source</th></tr>
	<tr><th>Interface voirie/réseau de transport</th><td>les arrêts physiques</td><td>les adresses proches d'arrêts situés sur les rues, les entrées de gares, de stations, etc.</td><td>les n arrêts physiques</td><td>néant</td><td>Arret tout lieu de la commune</td><td>Données : fichier de gares puis fichier de voies</td></tr>
	<tr><th>Alias</th><td>néant</td><td>néant</td><td>néant</td><td>lieu aliasé</td><td>Données : fichier de gares</td></tr>
	<tr><th>Lieu public</th><td>néant</td><td>les "portes d'entrée" du lieu</td><td>néant</td><td>arrêt tout lieu de la commune</td><td>Données : fichier de lieux publics</td></tr>
	<tr><th>Route entière</th><td>Les arrêts situés sur la rue elle même</td><td>Toutes les extrémités de segment de route donnant sur une autre route</td><td>la route désignée</td><td>Arrêt tout lieu de la commune</td><td>Généré au chargement du fichier des voies</td></tr>
	<tr><th>Adresse précise</th><td>néant</td><td>L'adresse désignée</td><td>néant</td><td>Arrêts tout lieux de la commune</td><td>Entrée utilisateur</td></tr>
	<tr><th>Arrêts principaux de commune</th><td>néant</td><td>néant</td><td>néant</td><td>Lieux principaux de la commune, et arrêt tout lieux</td><td>Données : fichier des gares</td></tr>
	</table>

	L'entrée dans la recherche d'itinéraires depuis le lieu fonctionne selon le modèle suivant (voir méthode cLieuLogique::ConstruitAccesReseau :
		- chaque arrêt et arrêt tout lieu est considéré comme accessible immédiatement
		- chaque adresse donne une liste d'arrêts accessibles moyennant une marche à pied (méthode cAdresse::RechercheArrets)

	En résumé, l'entrée utilisateur peut donner lieu à la recherche d'un lieu, selon les possibilités suivantes :
		- nom seul : recherche privilégiée dans les arrêts et lieux publics
		- nom préfixé par "rue/avenue/etc." : recherche dans les rues entières
		- nom préfixé par <numéro> "rue/avenue/etc." : recherche dans les rues entières, puis tentative de précision de l'adresse si informations disponibles dans la route
*/
class LogicalPlace : public cPoint
{
public:

	/** Durée de correspondance factice indiquant que le délai minimum pour la correspondance entre deux points d'accès est inconnu. */
	static const tDureeEnMinutes UNKNOWN_TRANSFER_DELAY;

	/** Durée de correspondance factice indiquant que la correspondance entre deux points d'accès est interdite */
	static const tDureeEnMinutes FORBIDDEN_TRANSFER_DELAY;

	/** Règles de correspondance */
	enum tNiveauCorrespondance
	{
		CorrInterdite = 0,
		CorrAutorisee = 1,
		CorrRecommandeeCourt = 2,
		CorrRecommandee = 3
	};

	/** Map vers les arrêts physiques */
	typedef map<size_t, cArretPhysique*> PhysicalStopsMap;

	/** Map vers les adresses routières */
	typedef map<size_t, synmap::Address*>	AddressesMap;


protected:

	//!	@name Localisation
	//@{
		cCommune*		_town;	//!< Commune dans laquelle se trouve le lieu
		synmap::Road* const		_road;		//!< Route si lieu route entière
    //@}

	//!	@name Composition
	//@{
		vector<NetworkAccessPoint*>	_networkAccessPoints;	// Points d'entrée dans les réseaux
		vector<LogicalPlace*>		_aliasedLogicalPlaces;	//!< Lieux logiques inclus
	//@}

	//! @name Désignation
	//@{
		cTexte		_name;	//!< Désignation du lieu
		cTextePostScript		_nameAsDestinationForTimetable;		//!< Désignation pour affichage en tant que destination ou origine d'un service (indicateurs horaires)
		cTexte		_name13;		//!< Désignation de 13 caractères de long (téléaffichage)
		cTexte		_name26;		//!< Désignation de 26 caractères de long (téléaffichage)
    	
	//@}

	//!	@name Paramètres
	//@{
		const bool		_volatile;	//!< Indique si l'objet est destiné à une seule utilisation immédiate ou s'il appartient à la base de données générale.
	//@}


	//! \name Documentation
	//@{
		cAlerte					_alert;			//!< Alerte en cours de validité
	//@}
	
	//! \name Gestion des correspondances
	//@{
		cDureeEnMinutes			_minTransferDelay;			//!< Délai minimal de correspondance entre point d'entrée réseaux le plus faible du lieu
		const tNiveauCorrespondance	_transferRules;			//!< Type d'autorisation de correspondance
		vector< cDureeEnMinutes >		_maxTransferDelay;				//!< Tableau des plus longs délais minimaux de correspondance au départ de chaque point d'entrée de réseau
		vector< vector< cDureeEnMinutes > >		_transferDelay;			//!< Tableau des délais minimaux de correspondance entre quais
	//@}
	
	
	//! \name Donnes complmentaires
	//@{
	//tVitesseKMH			vVitesseMax[NOMBREVMAX];	//!< Tableau des vitesses maximales par tranche de distance (temporairement inutilis)
	//@}

	//! \name Chainage et indexation
	//@{
		const tIndex	_id;				//!< Index du lieu logique dans l'environnement
		cGareLigne*		_firstDepartureLineStop;	//!< Pointeur vers le premier départ de ligne (ordre aléatoire) \todo trier par orientation ?
		cGareLigne*		_firstArrivalLineStop;	//!< Pointeur vers la première arrivée de ligne (ordre aléatoire) \todo trier par orientation ?
	//@}


public:

	//!	@name Accesseurs
	//@{
		cDureeEnMinutes			AttenteCorrespondance(tIndex Dep, tIndex Arr)			const;
		const cDureeEnMinutes&	AttenteMinimale()										const;
		tNiveauCorrespondance	CorrespondanceAutorisee()								const;
		const cAlerte*			getAlerte() 											const;
		const cTexte&			getDesignationOD()										const;
		cArretPhysique*			GetArretPhysique(int)									const;
		NetworkAccessPoint*		getNetworkAccessPoint(tIndex id)	const { return _networkAccessPoints[id]; }
		const cTexte&           getDesignation13()                                      const;
		const cTexte&           getDesignation26()                                      const;
		const cTexte&			getName() const { return _name; }
		cCommune*				getTown() const { return _town; }
		tIndex					getId() const {return _id; }
	    
		tIndex					Index()													const;
		const cDureeEnMinutes&	PireAttente(tIndex i)									const;
		cGareLigne*				PremiereGareLigneArr()									const;
		cGareLigne*				PremiereGareLigneDep()									const;
		bool	getVolatile()	const { return _volatile; }
		vector<LogicalPlace*>	getAliasedLogicalPlaces()	const { return _aliasedLogicalPlaces; }
		PhysicalStopsMap	getPysicalStops()										const;
		AddressesMap	getAddresses()										const;
	//	tVitesseKMH				vitesseMax(size_t Categorie)							const;
	//@}
	
	//! \name Calculateurs
	//@{	
	//	cElementTrajet*			ProchainDirect(LogicalPlace* Destination, cMoment& MomentDepart, const cMoment& ArriveeMax
	//								, tIndex ArretPhysiqueArriveePrecedente)										const;
		cMoment					MomentArriveePrecedente (const cMoment& MomentArrivee, const cMoment& MomentArriveeMin)		const;
		cMoment					MomentDepartSuivant(const cMoment& MomentDepart, const cMoment& MomentDepartMax
													, const cMoment& __MomentCalcul)										const;
		cGareLigne*				DessertAuDepart	(const cLigne*)																const;
		cGareLigne*				DessertALArrivee				(const cLigne*)												const;
		tNiveauCorrespondance	NiveauCorrespondance			(const cDistanceCarree& D)									const;
		LogicalPlace*			accurateAddressLogicalPlace(synmap::Address::AddressNumber addressNumber)							;
	//@}

	//! \name Modificateurs
	//@{
		tIndex	addNetworkAccessPoint(NetworkAccessPoint* networkAccessPoint, tIndex id=INCONNU);
		void	addAliasedLogicalPlace(LogicalPlace*);
		void	setAlerteDebut(cMoment& momentDebut);
		void	setAlerteFin(cMoment& momentFin);
		void	setAlerteMessage(cTexte& message);
		void	setPremiereGareLigneDep(cGareLigne*);
		void	setPremiereGareLigneArr(cGareLigne*);
		bool	setDelaiCorrespondance(tIndex __VoieDepart, tIndex __VoieArrivee, tDureeEnMinutes);
		void	setDesignationOD(const cTexte&);
	//	bool	setVMax(tCategorieDistance, tVitesseKMH);
		bool    setDesignation13(const cTexte&);
		bool    setDesignation26(const cTexte&);
		void	setDesignation(cCommune*, std::string);
	//@}

	//! \name Constructeurs et destructeur
	//@{
		LogicalPlace(tIndex, tNiveauCorrespondance);
		LogicalPlace(synmap::Road*);
		LogicalPlace(synmap::Address*);
		LogicalPlace(cCommune*, std::string);
		LogicalPlace(tIndex, cCommune*);
		LogicalPlace(cCommune*);
		~LogicalPlace();
	//@}

};

#endif
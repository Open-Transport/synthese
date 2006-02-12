/** En-t�te classe Lieu logique.
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

	Un lieu logique est un regrouppement de lieux physiques ponctuels permettant l'entr�e dans les r�seaux de transport et de voirie.

	Un lieu logique est donc "compos�" par :
		- des arr�ts physiques, points d'entr�e sur le r�seau de transport
		- des adresses, points d'entr�e sur le r�seau de voirie

	Le lieu logique porte les propri�t�s suivantes :
		- chaque point associ� au lieu logique est consid�r� comme �quivalent, d'un point de vue de l'acc�s au lieu logique (ex : si le lieu logique est un 
		  batiment, alors chaque porte d'entr�e est consid�r�e comme �quivalente pour atteindre le batiment. Dans le cas contraire, le lieu doit �tre divis�.
		- le lieu logique permet le passage d'un point d'entr�e � un autre de mani�re "indolore" : le temps pour r�aliser le transfert n'est pas d�compt� comme
		  du temps de marche dans le cadre du param�tre de limitation de la marche � pied.
		- le passage d'un point d'entr�e � un autre permet la notion de "correspondance", soumise � autorisation (attributs bool�ens d�di�s). Lorsque disponible,
		  la corresponsance est soumise � un d�lai fourni dans les donn�es, � calculer le plus rationnellement possible. Ce d�lai peut int�grer une marge de 
		  s�curit� li�e au "risque" que constitue le transfert d'un �l�ment d'un r�seau � un autre. Ex : une correspondance en gare ferroviaire inclue g�n�ralement
		  quelques minutes de marge de s�curit� pour ne pas �tre mise en p�ril par les trains de certaines soci�t�s ferroviaires fr�quemment en retard.
		- un lieu logique peut en "inclure" un autre, permettant de traiter la probl�matique des arrets "tout lieu", et permettant �galement de constituer des alias
		- un lieu logique est toujours inscrit sur une commune. Si un lieu se trouve � cheval sur deux communes, le lieu doit �tre cr�� sur une commune, et un 
		  alias doit�tre cr�� sur l'autre
	    
	Les utiliations courantes du lieu logique sont les suivantes :
	<table class="Tableau">
	<tr><th>Type</th><th>Arr�t(s)</th><th>Adresses(s)</th><th>Route</th><th>Alias</th><th>Source</th></tr>
	<tr><th>Interface voirie/r�seau de transport</th><td>les arr�ts physiques</td><td>les adresses proches d'arr�ts situ�s sur les rues, les entr�es de gares, de stations, etc.</td><td>les n arr�ts physiques</td><td>n�ant</td><td>Arret tout lieu de la commune</td><td>Donn�es : fichier de gares puis fichier de voies</td></tr>
	<tr><th>Alias</th><td>n�ant</td><td>n�ant</td><td>n�ant</td><td>lieu alias�</td><td>Donn�es : fichier de gares</td></tr>
	<tr><th>Lieu public</th><td>n�ant</td><td>les "portes d'entr�e" du lieu</td><td>n�ant</td><td>arr�t tout lieu de la commune</td><td>Donn�es : fichier de lieux publics</td></tr>
	<tr><th>Route enti�re</th><td>Les arr�ts situ�s sur la rue elle m�me</td><td>Toutes les extr�mit�s de segment de route donnant sur une autre route</td><td>la route d�sign�e</td><td>Arr�t tout lieu de la commune</td><td>G�n�r� au chargement du fichier des voies</td></tr>
	<tr><th>Adresse pr�cise</th><td>n�ant</td><td>L'adresse d�sign�e</td><td>n�ant</td><td>Arr�ts tout lieux de la commune</td><td>Entr�e utilisateur</td></tr>
	<tr><th>Arr�ts principaux de commune</th><td>n�ant</td><td>n�ant</td><td>n�ant</td><td>Lieux principaux de la commune, et arr�t tout lieux</td><td>Donn�es : fichier des gares</td></tr>
	</table>

	L'entr�e dans la recherche d'itin�raires depuis le lieu fonctionne selon le mod�le suivant (voir m�thode cLieuLogique::ConstruitAccesReseau :
		- chaque arr�t et arr�t tout lieu est consid�r� comme accessible imm�diatement
		- chaque adresse donne une liste d'arr�ts accessibles moyennant une marche � pied (m�thode cAdresse::RechercheArrets)

	En r�sum�, l'entr�e utilisateur peut donner lieu � la recherche d'un lieu, selon les possibilit�s suivantes :
		- nom seul : recherche privil�gi�e dans les arr�ts et lieux publics
		- nom pr�fix� par "rue/avenue/etc." : recherche dans les rues enti�res
		- nom pr�fix� par <num�ro> "rue/avenue/etc." : recherche dans les rues enti�res, puis tentative de pr�cision de l'adresse si informations disponibles dans la route
*/
class LogicalPlace : public cPoint
{
public:

	/** Dur�e de correspondance factice indiquant que le d�lai minimum pour la correspondance entre deux points d'acc�s est inconnu. */
	static const tDureeEnMinutes UNKNOWN_TRANSFER_DELAY;

	/** Dur�e de correspondance factice indiquant que la correspondance entre deux points d'acc�s est interdite */
	static const tDureeEnMinutes FORBIDDEN_TRANSFER_DELAY;

	/** R�gles de correspondance */
	enum tNiveauCorrespondance
	{
		CorrInterdite = 0,
		CorrAutorisee = 1,
		CorrRecommandeeCourt = 2,
		CorrRecommandee = 3
	};

	/** Map vers les arr�ts physiques */
	typedef map<size_t, cArretPhysique*> PhysicalStopsMap;

	/** Map vers les adresses routi�res */
	typedef map<size_t, synmap::Address*>	AddressesMap;


protected:

	//!	@name Localisation
	//@{
		cCommune*		_town;	//!< Commune dans laquelle se trouve le lieu
		synmap::Road* const		_road;		//!< Route si lieu route enti�re
    //@}

	//!	@name Composition
	//@{
		vector<NetworkAccessPoint*>	_networkAccessPoints;	// Points d'entr�e dans les r�seaux
		vector<LogicalPlace*>		_aliasedLogicalPlaces;	//!< Lieux logiques inclus
	//@}

	//! @name D�signation
	//@{
		cTexte		_name;	//!< D�signation du lieu
		cTextePostScript		_nameAsDestinationForTimetable;		//!< D�signation pour affichage en tant que destination ou origine d'un service (indicateurs horaires)
		cTexte		_name13;		//!< D�signation de 13 caract�res de long (t�l�affichage)
		cTexte		_name26;		//!< D�signation de 26 caract�res de long (t�l�affichage)
    	
	//@}

	//!	@name Param�tres
	//@{
		const bool		_volatile;	//!< Indique si l'objet est destin� � une seule utilisation imm�diate ou s'il appartient � la base de donn�es g�n�rale.
	//@}


	//! \name Documentation
	//@{
		cAlerte					_alert;			//!< Alerte en cours de validit�
	//@}
	
	//! \name Gestion des correspondances
	//@{
		cDureeEnMinutes			_minTransferDelay;			//!< D�lai minimal de correspondance entre point d'entr�e r�seaux le plus faible du lieu
		const tNiveauCorrespondance	_transferRules;			//!< Type d'autorisation de correspondance
		vector< cDureeEnMinutes >		_maxTransferDelay;				//!< Tableau des plus longs d�lais minimaux de correspondance au d�part de chaque point d'entr�e de r�seau
		vector< vector< cDureeEnMinutes > >		_transferDelay;			//!< Tableau des d�lais minimaux de correspondance entre quais
	//@}
	
	
	//! \name Donnes complmentaires
	//@{
	//tVitesseKMH			vVitesseMax[NOMBREVMAX];	//!< Tableau des vitesses maximales par tranche de distance (temporairement inutilis)
	//@}

	//! \name Chainage et indexation
	//@{
		const tIndex	_id;				//!< Index du lieu logique dans l'environnement
		cGareLigne*		_firstDepartureLineStop;	//!< Pointeur vers le premier d�part de ligne (ordre al�atoire) \todo trier par orientation ?
		cGareLigne*		_firstArrivalLineStop;	//!< Pointeur vers la premi�re arriv�e de ligne (ordre al�atoire) \todo trier par orientation ?
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
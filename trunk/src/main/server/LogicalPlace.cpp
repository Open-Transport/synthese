/** Impl�mentation lieu logique
	@file LogicalPlace.cpp
*/

#include "LogicalPlace.h"


const tDureeEnMinutes LogicalPlace::UNKNOWN_TRANSFER_DELAY = -1;
const tDureeEnMinutes LogicalPlace::FORBIDDEN_TRANSFER_DELAY = 99;

/** Constructeur de lieu logique de type standard
	@param __volatile Indique si l'objet est destin� � une seule utilisation imm�diate ou s'il appartient � la base de donn�es g�n�rale.
	@author Hugues Romain
	@date 2001-2006
*/
LogicalPlace::LogicalPlace(tIndex __id, tNiveauCorrespondance __transferRules)
	: cPoint()
	, _volatile(false)
	, _transferRules(__transferRules)
	, _id(id)
	, _road(NULL)
{
	// Aucune desserte par des lignes
	_firstArrivalLineStop = NULL;
	_firstDepartureLineStop = NULL;
	
	// Valeurs initialis�es
	_minTransferDelay = CorrespondanceAutorisee() != CorrInterdite ? FORBIDDEN_TRANSFER_DELAY : 0;
}


/** Constructeur de lieu logique de type route enti�re.
	@param id ID du lieu logique
	@param road Route support du lieu logique

	@todo MJ mettre ici le remplissage du lieu logique avec toutes les intersections de la route
*/
LogicalPlace::LogicalPlace(Road* road)
	: cPoint()
	, _volatile(false)
	, _transferRules(CorrInterdite)
	, _id(0)
	, _road(road)
{
	// Aucune desserte par des lignes
	_firstArrivalLineStop = NULL;
	_firstDepartureLineStop = NULL;

	// Valeurs initialis�es
	_minTransferDelay = FORBIDDEN_TRANSFER_DELAY;

	setDesignation(road->getTown(), road->getName());
}


/** Constructeur de lieu logique de type adresse pr�cise.
	@param entireRoadLogicalPlace Lieu de type route enti�re � copier
	@param address Adresse
*/
LogicalPlace::LogicalPlace(Address* address)
	: cPoint(entireRoadLogicalPlace)
	, _volatile(true)
	, _transferRules(CorrInterdite)
	, _id(0)
	, _road(NULL)
{
	// Aucune desserte par des lignes
	_firstArrivalLineStop = NULL;
	_firstDepartureLineStop = NULL;

	std::string name;
	name = address->getAddressNumber() + " " + address->getRoad()->getName();
	setDesignation(address->getRoad()->getTown(), name);
}


/** Constructeur de lieu logique de type principal de commune
	@param town Commune
*/
LogicalPlace::LogicalPlace(cCommune* town)
	: cPoint(town)
	, _volatile(false)
	, _transferRules(CorrInterdite)
	, _id(0)
	, road(NULL)
{
	// Aucune desserte par des lignes
	_firstArrivalLineStop = NULL;
	_firstDepartureLineStop = NULL;

	setDesignation(town, town->GetNom().Texte());
}


/** Constructeur de lieu logique de type lieu public
	@param town Commune
*/
LogicalPlace::LogicalPlace(cCommune* town, std::string name)
	: cPoint()
	, _volatile(false)
	, _transferRules(CorrInterdite)
	, _id(0)
	, road(NULL)
{
	// Aucune desserte par des lignes
	_firstArrivalLineStop = NULL;
	_firstDepartureLineStop = NULL;

	setDesignation(town, name);
}



/** Constructeur de lieu logique de type tout lieu de commune
	@param town Commune
*/
LogicalPlace::LogicalPlace(tIndex id, cCommune* town)
	: cPoint(town)
	, _volatile(false)
	, _transferRules(CorrInterdite)
	, _id(id)
	, road(NULL)
{
	// Aucune desserte par des lignes
	_firstArrivalLineStop = NULL;
	_firstDepartureLineStop = NULL;

}



/*!	Destructeur.
*/
LogicalPlace::~LogicalPlace()
{
	for (size_t i=0; i<_networkAccesPoints.size(); i++)
		delete _networkAccesPoints[i];
}


tNiveauCorrespondance LogicalPlace::NiveauCorrespondance(const cDistanceCarree& D) const
{
	if (_transferRules == CorrRecommandeeCourt)
		return (D.DistanceCarree() > O2COURTLONG) ? CorrAutorisee : CorrRecommandee);

	return _transferRules;
}



/**	Modificateur d'un d�lai minimal de correspondance entre deux quais.
	@param NumeroArretPhysiqueDepart ArretPhysique de d�part (indique la ligne dans la matrice de d�lais de correspondance)
	@param NumeroArretPhysiqueArrivee ArretPhysique d'arriv�e (indique la colonne dans la matrice de d�lais de correspondance)
	@param delay le d�lai minimal de correspondance entre les deux quais, indiqu� en minutes (format num�rique).

	Voir
		- LogicalPlace::UNKNOWN_TRANSFER_DELAY
		- LogicalPlace::FORBIDDEN_TRANSFER_DELAY

	@return true si l'op�ration a �t� effectu�e avec succ�s
	@author Hugues Romain
	@date 2005-2006
*/
bool LogicalPlace::setDelaiCorrespondance(tIndex NumeroArretPhysiqueDepart, tIndex NumeroArretPhysiqueArrivee, tDureeEnMinutes delay)
{
	// Ecriture de la donn�e
	try
	{
		_transferDelay[NumeroArretPhysiqueDepart][NumeroArretPhysiqueArrivee] = delay;

		if (delay != UNKNOWN_TRANSFER_DELAY && delay != FORBIDDEN_TRANSFER_DELAY)
		{
			// Ecriture du d�lai minimal le plus important attente au d�part du quai de d�part sp�cifi�
			if (_transferDelay[NumeroArretPhysiqueDepart][NumeroArretPhysiqueArrivee] > _maxTransferDelay[NumeroArretPhysiqueDepart])
				_maxTransferDelay[NumeroArretPhysiqueDepart] = _transferDelay[NumeroArretPhysiqueDepart][NumeroArretPhysiqueArrivee];
			
			// Ecriture du d�lai minimal le plus faible du point d'arr�t
			if (_transferDelay[NumeroArretPhysiqueDepart][NumeroArretPhysiqueArrivee] < _minTransferDelay)
				_minTransferDelay = _transferDelay[NumeroArretPhysiqueDepart][NumeroArretPhysiqueArrivee];
		}
	}
	catch(...)
	{
		return false;
	}

	// Succ�s
	return true;
}


cMoment LogicalPlace::MomentArriveePrecedente(const cMoment& MomentArrivee, const cMoment& MomentArriveeMin) const
{
	cMoment tempMomentArrivee;
	cMoment tempMomentArriveePrecedente = MomentArriveeMin;
	for (cGareLigne* curGLA = _firstArrivalLineStop; curGLA != NULL; curGLA = curGLA->PAArriveeSuivante())
	{
		tempMomentArrivee = MomentArrivee;
		tempMomentArrivee -= cDureeEnMinutes(1);
		if (curGLA->Precedent(tempMomentArrivee, MomentArriveeMin) != -1)
		{
			if (tempMomentArrivee > tempMomentArriveePrecedente)
				tempMomentArriveePrecedente = tempMomentArrivee;
		}
	}
	return (tempMomentArriveePrecedente);
}
	
cMoment LogicalPlace::MomentDepartSuivant(const cMoment& MomentDepart, const cMoment& MomentDepartMax, const cMoment& __MomentCalcul) const
{
	cMoment tempMomentDepart;
	cMoment tempMomentDepartSuivant = MomentDepartMax;
	for (cGareLigne* curGLD= _firstDepartureLineStop; curGLD!=NULL; curGLD=curGLD->PADepartSuivant())
	{
		tempMomentDepart = MomentDepart;
		tempMomentDepart += cDureeEnMinutes(1);
		if (curGLD->Prochain(tempMomentDepart, MomentDepartMax, __MomentCalcul) != INCONNU)
		{
			if (tempMomentDepart < tempMomentDepartSuivant)
				tempMomentDepartSuivant = tempMomentDepart;
		}
	}
	return (tempMomentDepartSuivant);
}



/** Recherche de la solution sans changement vers un autre PA.
	\param ArretPhysiqueArriveePrecedente 0 si aucun.
	\author Hugues Romain
	\date 2001
*/
/*cElementTrajet* cArretLogique::ProchainDirect(cArretLogique* Destination, cMoment& MomentDepart, const cMoment& ArriveeMax, tNumeroVoie ArretPhysiqueArriveePrecedente) const
{
	cMoment tempMomentDepart;
	cMoment tempMomentArrivee;
	tNumeroService tempNumeroService;
	cGareLigne* curGLA;
	cElementTrajet* newET=NULL;
	
	for (cGareLigne* curGLD = vPremiereGareLigneDep; curGLD != NULL; curGLD = curGLD->PADepartSuivant())
		if (curGLD->Circule(MomentDepart, ArriveeMax) && (curGLA = Destination->DessertALArrivee(curGLD->Ligne())) && curGLD->getHoraireDepartPremier(0) < curGLA->getHoraireArriveePremier(0))
		{
			// Calcul de l'horaire ??
			tempMomentDepart = MomentDepart;
			if (vCorrespondanceAutorisee && ArretPhysiqueArriveePrecedente)
				tempMomentDepart += vTableauAttente[ArretPhysiqueArriveePrecedente][curGLD->ArretPhysique()];
			
			tempNumeroService = curGLD->Prochain(tempMomentDepart, ArriveeMax);
			if (tempNumeroService != -1)
			{
				tempMomentArrivee = tempMomentDepart;
				curGLA->CalculeArrivee(*curGLD, tempNumeroService, tempMomentDepart, tempMomentArrivee);

				// Stockage
				if (newET==NULL || tempMomentArrivee < newET->MomentArrivee())
				{
					if (newET == NULL)
						newET = new cElementTrajet;
					newET->setInformations(curGLA, curGLD, tempMomentDepart, tempMomentArrivee);
					newET->setService(tempNumeroService);
				}
			}
		}
	
	return(newET);
}*/



/* SET Enlever pour resa car non utilis
cElementTrajet*	cArretLogique::AcheminementDepuisDepot(cArretLogique* Destination, const cMoment& MomentDepart, cReseau* Reseau) const
{
	for (cGareLigne* curGareLigne = PremiereGareLigneDep(); curGareLigne != NULL; curGareLigne = curGareLigne->PADepartSuivant())
		if (!curGareLigne->Ligne()->Axe()->Autorise() && (Reseau == NULL || curGareLigne->Ligne()->getReseau() == Reseau) && curGareLigne->Destination()->ArretLogique() == Destination)
		{
			cElementTrajet* curET = new cElementTrajet;
			cMoment MomentDepartNew;
			MomentDepartNew = MomentDepart;
			curGareLigne->CalculeDepart(*curGareLigne->Destination(), curGareLigne->Destination()->Precedent(MomentDepartNew, MomentDepart), MomentDepart, MomentDepartNew);
			curET->setInformations(curGareLigne->Destination(), curGareLigne, MomentDepartNew, MomentDepart);
			return(curET);
		}
	return(NULL);
}


cElementTrajet*	cArretLogique::AcheminementVersDepot(cArretLogique* Origine, const cMoment& MomentArrivee, cReseau* Reseau) const
{
	for (cGareLigne* curGareLigne = PremiereGareLigneArr(); curGareLigne != NULL; curGareLigne = curGareLigne->PAArriveeSuivante())
		if (!curGareLigne->Ligne()->Axe()->Autorise() && (Reseau == NULL || curGareLigne->Ligne()->getReseau() == Reseau) && curGareLigne->Origine()->ArretLogique() == Origine)
		{
			cElementTrajet* curET = new cElementTrajet;
			cMoment MomentArriveeNew;
			MomentArriveeNew = MomentArrivee;
			curGareLigne->CalculeArrivee(*curGareLigne->Origine(), curGareLigne->Origine()->Prochain(MomentArriveeNew, MomentArrivee), MomentArrivee, MomentArriveeNew);
			curET->setInformations(curGareLigne, curGareLigne->Origine(), MomentArrivee, MomentArriveeNew);
			return(curET);
		}
	return(NULL);
}
*/	

cGareLigne* LogicalPlace::DessertAuDepart(const cLigne* Ligne) const
{
	// Recherche peut etre amelioree en utilisant Reseau puis Axe
	for (cGareLigne* curGLD = _firstDepartureLineStop; curGLD != NULL; curGLD = curGLD->PADepartSuivant())
		if (curGLD->Ligne() == Ligne)
			return(curGLD);
	return(NULL);
}

cGareLigne* LogicalPlace::DessertALArrivee(const cLigne* Ligne) const
{
	// Recherche peut etre amelioree en utilisant Reseau puis Axe
	for (cGareLigne* curGLA = _firstArrivalLineStop; curGLA != NULL; curGLA = curGLA->PAArriveeSuivante())
		if (curGLA->Ligne() == Ligne)
			return(curGLA);
	return(NULL);
}

const cTexte& LogicalPlace::getNom() const
{
	return _name;
}

cCommune* LogicalPlace::getCommune() const
{
	return _town;
}

/** Edition de VMax.
	@author Hugues Romain
	@date 2001
*/
bool LogicalPlace::setVMax(tCategorieDistance CategorieDistance, tVitesseKMH newVitesseKMH)
{
	//vVitesseMax[CategorieDistance] = newVitesseKMH+1; 
	return(true);
}


/*!	Modificateur d�signation courte pour indicateur papier.
	\param newDesignationOD Designation courte de l'arret
	\author Hugues Romain
	\date 2001-2005
*/
void LogicalPlace::setDesignationOD(const cTexte& newDesignationOD)
{
	_nameAsDestinationForTimetable.Vide();
	_nameAsDestinationForTimetable << newDesignationOD;
}


bool LogicalPlace::addService(char newType, cPhoto* newPhoto, const cTexte& newDesignation)
{
/*	// SET PORTAGE LINUX
	size_t i;
	for (i=0; vService[i]!=NULL; i++)
	{ }
	//END PORTAGE LINUX
	vService[i] = new cServiceEnGare(newType, newPhoto, newDesignation);
	return(true);
*/
}


bool LogicalPlace::SetPoint(int X, int Y)
{
	bool Succes = true;
	
	if (X < 0)
	{
		if (X != INCONNU)
			Succes = false;
	}
	else
		_point.setX(X);
	
	if (Y < 0)
	{
		if (Y != INCONNU)
			Succes = false;
	}
	else
		_point.setY(Y);
	
	return Succes;
}



/**	Nombre de quais dans la gare.
	\return Le nombre de quais dans la gare
*/
tIndex LogicalPlace::NombreArretPhysiques() const
{
	return _physicalStops.Taille() - 1;
}

void LogicalPlace::setAlerteMessage(cTexte& message)
{
	_alert.setMessage(message);
}

void LogicalPlace::setAlerteDebut(cMoment& momentDebut)
{
	_alert.setMomentDebut(momentDebut);
}

void LogicalPlace::setAlerteFin(cMoment& momentFin)
{
	_alert.setMomentFin(momentFin);
}




/** Modificateur d�signation de 13 caracteres.
    @param __Designation13 Valeur a donner a la designation de 13 caracteres.En cas de chaine de longueur trop importante, la valeur est tronquee.
    return true si la copie a ete effectuee integralement, false si elle a ete tronquee.
*/
bool LogicalPlace::setDesignation13(const cTexte& __Designation13)
{
	_name13 = __Designation13.Extrait(0, 13);
    return _name13.Compare(__Designation13);
}



/** Modificateur d�signation de 26 caracteres.
    @param __Designation26 Valeur a donner a la designation de 26 caracteres.En cas de chaine de longueur trop importante, la valeur est tronquee.
    return true si la copie a ete effectuee integralement, false si elle a ete tronquee.
*/
bool LogicalPlace::setDesignation26(const cTexte& __Designation26)
{
    _name26 = __Designation26.Extrait(0, 26);
	return _name26.Compare(__Designation26);
}



/** Accesseur d�signation 13 caract�res.
    @return D�signation de 13 caract�res du point d'arr�t.
*/
const cTexte& 
LogicalPlace::getDesignation13() const
{
    return _name13;
}



/** Accesseur d�signation 26 caract�res.
    @return D�signation de 26 caract�res du point d'arr�t.
*/
const cTexte& 
LogicalPlace::getDesignation26() const
{
    return _name26;
}


const cAlerte* LogicalPlace::getAlerte() const
{
	return(&_alert);
}



const cTexte& LogicalPlace::GetNom(int i) const
{
//	if (!_AccesPADe[i])
//		i=0;
	return(getNom());
}

cCommune* LogicalPlace::GetCommune(int i) const
{
//	if (!_AccesPADe[i])
//		i=0;
	return(getCommune());
}


cArretPhysique* LogicalPlace::GetArretPhysique(int n) const
{
	if (n < 1 || n > NombreArretPhysiques())
		return NULL;
	else
		return getArretPhysique(n);
}



cArretPhysique* LogicalPlace::getArretPhysique(tIndex n) const
{
	return _physicalStops[n];
}


cGareLigne* LogicalPlace::PremiereGareLigneDep() const
{
	return _firstDepartureLineStop;
}

cGareLigne* LogicalPlace::PremiereGareLigneArr() const
{
	return _firstArrivalLineStop;
}

tIndex LogicalStop::Index() const
{
	return _id;
}


void LogicalPlace::setPremiereGareLigneDep(cGareLigne* curGareLigne)
{
	_firstDepartureLineStop = curGareLigne;
}

void LogicalPlace::setPremiereGareLigneArr(cGareLigne* curGareLigne)
{
	_firstArrivalLineStop = curGareLigne;
}
	
const cTexte& LogicalPlace::getDesignationOD() const
{
	return _nameAsDestinationForTimetable;
}



tNiveauCorrespondance LogicalPlace::CorrespondanceAutorisee() const
{
	return _transferRules;
}

cDureeEnMinutes LogicalPlace::AttenteCorrespondance(tIndex Dep, tIndex Arr) const
{
	return _transferRules ? _transferDelay[Dep][Arr] : cDureeEnMinutes(0);
}

const cDureeEnMinutes& LogicalPlace::PireAttente(tIndex i) const
{
	if (i<1 || i> NombreArretPhysiques())
		i=0;
	return _maxTransferDelay[i];
}

/*
bool LogicalPlace::declServices(size_t newNombreServices)
{
	vService = (cServiceEnGare**) calloc(newNombreServices+1, sizeof(cPhoto));
	return(true);
}
*/

/*
inline bool cArretLogique::addPhoto(cPhoto* Photo)
{
	// SET PORTAGE LINUX
	size_t i;
	for (i=0; vPhoto[i]!=NULL; i++)
	{ }
	//END PORTAGE LINUX
	vPhoto[i] = Photo;
	return(true);

}
*/

const cDureeEnMinutes& LogicalPlace::AttenteMinimale() const
{
	return _minTransferDelay;
}

/*
tVitesseKMH cArretLogique::vitesseMax(size_t Categorie) const
{
	return(vVitesseMax[Categorie]);
}
*/
/*
inline tIndex	cArretLogique::NombrePhotos() const
{
	return(vNombrePhotos);
}
*/
/*
inline const cPhoto* cArretLogique::getPhoto(tIndex iNumeroPhoto) const
{
	return(vPhoto[iNumeroPhoto]);
}

inline cServiceEnGare* cArretLogique::GetService(int i) const
{
	if (vService)
		return(vService[i]);
	else
		return(NULL);
}

inline cPhoto* cServiceEnGare::getPhoto() const
{
	return(vPhoto);
}

inline const cTexte& cServiceEnGare::Designation() const
{
	return(vDesignation);
}
*/
template <class T>
T& operator<<(T& flux, const LogicalPlace& Obj)
{
//	flux << *Obj.getAccesPADe();
	flux << Obj.getCommune()->GetNom() << " " << _name;
	return flux;
}


/** Cr�ation d'un lieu sp�cifique pr�cis sur un lieu route enti�re � partir d'un num�ro d'adresse.
	@param addressNumber Num�ro d'adresse (vide/INCONNU = pas de num�ro fourni)
	@return Pointeur vers le lieu le plus pr�cis possible :
		- rue entiere si impossible de pr�ciser (this non jetable)
		- pointeur vers un lieu cr�� et contenant l'adresse pr�cise, avec l'attribut _Jetable � true.
		- NULL si num�ro d'adresse identifi� comme invalide par les donn�es de la rues si disponibles

	@warning V�rifier la valeur _volatile du lieu retourn�. Si elle est � true, alors il faut d�truire l'objet apr�s utilisation sous peine de fuites m�moires.

	La cr�ation d'un lieu sp�cifique est effectu�e si et seulement si :
		- un num�ro d'adresse non inconnu est fourni
		- la route du lieu courant a les capacit�s de d�terminer la position d�sign�e par le num�ro d'adresse
		- le num�ro d'adresse appartient bel et bien � la plage d'adresses de la rue

	@todo MJ Impl�mentation � terminer
*/
LogicalPlace* LogicalPlace::accurateAddressLogicalPlace(RoadChunk::AddressNumber addressNumber)
{
	if (false) // Clause Calcul d'une adresse pr�cise possible � impl�menter 
	{
		Address accurateAddress = new Address(_road, addressNumber);
		LogicalPlace accurateAddressLogicalPlace = new LogicalPlace(INCONNU, CorrInterdite, true);
		accurateAddressLogicalPlace.addNetworkAccessPoint(accurateAddress);
		return accurateAddressLogicalPlace;			
	}

	// Retour par d�faut
	return this;
}


/** Ajout d'acc�s � r�seau.
	@param networkAccessPoint Adresse � ajouter
	@return le rang de l'�l�ment ajout� (INCONNU si erreur)

	Cette m�thode effectue les actions suivantes :
*/
tIndex LogicalPlace::addNetworkAccessPoint(NetworkAccessPoint* networkAccessPoint, tIndex id)
{
	/** - Insertion de l'�l�ment */
	try
	{
		if (id == INCONNU)
		{
			_networkAccesPoints.push_back(networkAccessPoint);
			id = _networkAccesPoints.size() - 1;
		}
		else
			_networkAccesPoints[id] = networkAccessPoint;
	}
	catch(...)
	{
		return INCONNU;
	}

	/** - Initialisation de la matrice de correspondances pour le nouvel arriv� � la valeur inconnu */
	if (CorrespondanceAutorisee() != CorrInterdite)
	{
		_maxTransferDelay[id] = 0;
		for (tIndex i=0; i<_networkAccesPoints.size(); i++)
		{
			setDelaiCorrespondance(i,id,UNKNOWN_TRANSFER_DELAY);
			setDelaiCorrespondance(id,i,UNKNOWN_TRANSFER_DELAY);
		}
		setDelaiCorrespondance(id,id,FORBIDDEN_TRANSFER_DELAY);
	}

	/** - Gestion des coordonn�es g�ographiques */
	if (!_networkAccesPoints[id]->unknownLocation())
	{
		int avgX = 0;
		int avgY = 0;
		int nbPoints = 0;
		for (tIndex i=0; i <= _networkAccesPoints.size(); i++)
		{
			if (_networkAccesPoints[i] != NULL && !_networkAccesPoints[i]->unknownLocation())
			{
				avgX += curPoint->XKMM();
				avgY += curPoint->YKMM();
				nbPoints++;
			}
		}
		_point.setX((CoordonneeKMM) (avgX / nbPoints));
		_point.setY((CoordonneeKMM) (avgY / nbPoints));
	}

	return id;
}


/** Modificateur nom du lieu logique.
	@param town Commune
	@param name Nom
*/
void LogicalPlace::setDesignation(cCommune* town, std::string name)
{
	_town = town;
	_name = name;

	// G�n�ration automatique de Designation OD si besoin
	if (!_nameAsDestinationForTimetable.Taille())
	{
		cTexte newDesignationOD(getNom().Taille() + getCommune()->GetNom().Taille() + 1);
		newDesignationOD << getCommune()->GetNom() << " " << getNom();
		setDesignationOD(newDesignationOD);
	}

	// Enregistrement sur la commune
	_town->addLogicalPlace(this);

	// Lieu logique
	if (_town->getAllPlaces())
		_aliasedLogicalPlaces.push_back(_town->getAllPlaces());

}


/** Ajout de lieu logique inclus/alis�.
	@param logicalPlace Lieu logique � inclure/aliaser
*/
void LogicalPlace::addAliasedLogicalPlace(LogicalPlace* logicalPlace)
{
	_aliasedLogicalPlaces.push_back(logicalPlace);
}
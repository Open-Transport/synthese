#include "cTableauAffichageSelectifDestinations.h"
#include "cLigne.h"
#include "cGareLigne.h"
#include "cArretPhysique.h"
#include "LogicalPlace.h"

cTableauAffichageSelectifDestinations::cTableauAffichageSelectifDestinations(const string& __Code)
: cTableauAffichage(__Code)
{
	_DureePresenceDestinationForcee = 120;
}

cTableauAffichageSelectifDestinations::~cTableauAffichageSelectifDestinations(void)
{
}


// Ajoute les terminus des lignes au d�part de l'arr�t
bool cTableauAffichageSelectifDestinations::AjouteDestinationsDefaut(void)
{
	LogicalPlace::PhysicalStopsMap physicalStops = _ArretLogique->getPhysicalStops();
	for (LogicalPlace::PhysicalStopsMap::const_iterator liter = physicalStops.begin();
		liter != physicalStops.end();
		++liter)

		for (cArretPhysique::LineStopVector::const_iterator piter = liter->second->departureLineStopVector().begin();
			piter != liter->second->departureLineStopVector().end();
			++piter)

		{
			cGareLigne* __GareLigne = *piter;

		if (_ArretPhysiques.find(__GareLigne->ArretPhysique()) != _ArretPhysiques.end())
			if (!AddDestination(__GareLigne->Ligne()->getLineStops().back()->ArretPhysique()->getLogicalPlace()))
				return false;
	}
	return true;
}

// Ajout de destination forc�e
bool cTableauAffichageSelectifDestinations::AddDestination(const LogicalPlace* __Destination)
{
	return _DestinationsSelection.AddElementSiInexistant(__Destination) != INCONNU;
}

// Calcul de la liste des d�parts � afficher
cDescriptionPassage* cTableauAffichageSelectifDestinations::Calcule(const cMoment& __MomentDebut)
{
	return NULL;
}

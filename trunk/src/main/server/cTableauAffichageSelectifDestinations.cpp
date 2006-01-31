#include "cTableauAffichageSelectifDestinations.h"

cTableauAffichageSelectifDestinations::cTableauAffichageSelectifDestinations(const cTexte& __Code)
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
	for (const cGareLigne* __GareLigne = _ArretLogique->PremiereGareLigneDep();
		__GareLigne;
		__GareLigne = __GareLigne->PADepartSuivant()
	){
		if (_ArretPhysiques.Recherche(__GareLigne->getVoie()))
			if (!AddDestination(__GareLigne->Destination()->ArretLogique()))
				return false;
	}
	return true;
}

// Ajout de destination forc�e
bool cTableauAffichageSelectifDestinations::AddDestination(const cArretLogique* __Destination)
{
	return _DestinationsSelection.AddElementSiInexistant(__Destination) != INCONNU;
}

// Calcul de la liste des d�parts � afficher
cDescriptionPassage* cTableauAffichageSelectifDestinations::Calcule(const cMoment& __MomentDebut)
{
	return NULL;
}

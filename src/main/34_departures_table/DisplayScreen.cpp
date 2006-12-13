#include "DisplayScreen.h"

/* THIS CODE COMES FROM SYNTHESE 2. IT MUST BE REFRESHED. */
#ifdef RIEN

namespace synthese
{
namespace departurestable
{


/** Constructor.
	@param __Code Key of the departure table
*/
cTableauAffichage::cTableauAffichage(const cTexte& __Code, GenerationMethod generationMethod)
: cSite(__Code), _generationMethod(generationMethod)
{
	_NombreDeparts		= INCONNU;	// Illimité
	_PointArret			= NULL;
	_OriginesSeulement	= false;
	_destinationForceDelay	= 120;	// default = 2 hours
	_maxDelay			= 24 * 60;	// default = 24 hours
}

cTableauAffichage::~cTableauAffichage(void)
{
}


void cTableauAffichage::setDestinationForceDelay(int delay)
{
	_destinationForceDelay = delay;
}

void cTableauAffichage::SetOriginesSeulement(bool __Valeur)
{
	_OriginesSeulement = __Valeur;
}

void cTableauAffichage::setMaxDelay(int maxDelay)
{
	_maxDelay = maxDelay;
}

void cTableauAffichage::addForbiddenPlace(const cGare* place)
{
	_forbiddenArrivalPlaces.insert(place);
}


/** Modificateur du point d'arrêt.
*/
void cTableauAffichage::SetPointArret(const cGare* __PointArret)
{
	_PointArret = __PointArret;
}


/** Modificateur nombre de départs affichés.
*/
void cTableauAffichage::SetNombreDeparts(tIndex __NombreDeparts)
{
	_NombreDeparts = __NombreDeparts;
}


/** Ajout de ligne ne devant pas être affichée sur le tableau.
*/
void cTableauAffichage::AddLigneInterdte(const cLigne* __Ligne)
{
	_LignesInterdites.insert(__Ligne);
}


/** Ajout de quai à afficher. 

	Si aucun quai ajouté alors tous les quais du point d'arrêt sont affichés.
*/
void cTableauAffichage::AddQuaiAutorise(tIndex __NumeroQuai)
{
	_Quais.insert(_PointArret->GetQuai(__NumeroQuai));
}


void cTableauAffichage::SetTitre(const cTexte&__Titre)
{
	_Titre = __Titre;
}

void cTableauAffichage::AddDestinationAffichee(const cGare* __PointArret)
{
	_displayedPlaces.insert(__PointArret);
}

const cTexte& cTableauAffichage::getTitre() const
{
	return _Titre;
}



/** Modificateur numéro du panneau.
	@param __NumeroPanneau Numéro du panneau devant afficher les résultats
*/
void cTableauAffichage::SetNumeroPanneau(tIndex __NumeroPanneau)
{
	_NumeroPanneau = __NumeroPanneau;
}


void cTableauAffichage::addForcedDestination(const cGare* place)
{
	_forcedDestinations.insert(place);
}


/** Accesseur numéro du panneau.
	@return Numéro du panneau
*/
tIndex cTableauAffichage::getNumeroPanneau() const
{
	return _NumeroPanneau;
}


ArrivalDepartureTableGenerator* cTableauAffichage::getGenerator(const cMoment& startDateTime) const
{
	ArrivalDepartureTableGenerator* result;

	// End time
	cMoment endDateTime = startDateTime;
	endDateTime += cDureeEnMinutes(_maxDelay);

	// Construction of the generator
	switch (_generationMethod)
	{
	case STANDARD_METHOD:
		return (ArrivalDepartureTableGenerator*) new StandardArrivalDepartureTableGenerator(
			_PointArret
			, ArrivalDepartureTableGenerator::DISPLAY_DEPARTURES
			, _OriginesSeulement ? ArrivalDepartureTableGenerator::ENDS_ONLY : ArrivalDepartureTableGenerator::WITH_PASSING
			, _Quais
			, _LignesInterdites
			, _displayedPlaces
			, _forbiddenArrivalPlaces
			, startDateTime
			, endDateTime
			, _NombreDeparts
		);

	case WITH_FORCED_DESTINATIONS_METHOD:
		return (ArrivalDepartureTableGenerator*) new ForcedDestinationsArrivalDepartureTableGenerator(
			_PointArret
			, ArrivalDepartureTableGenerator::DISPLAY_DEPARTURES
			, _OriginesSeulement ? ArrivalDepartureTableGenerator::ENDS_ONLY : ArrivalDepartureTableGenerator::WITH_PASSING
			, _Quais
			, _LignesInterdites
			, _displayedPlaces
			, _forbiddenArrivalPlaces
			, startDateTime
			, endDateTime
			, _NombreDeparts
			, _forcedDestinations
			, _destinationForceDelay
		);
	}
}

}
}

#endif

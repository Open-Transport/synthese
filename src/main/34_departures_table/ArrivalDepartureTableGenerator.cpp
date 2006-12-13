
#include "ArrivalDepartureTableGenerator.h"

/* THIS CODE COMES FROM SYNTHESE 2. IT MUST BE REFRESHED. */
#ifdef RIEN

namespace synthese
{
namespace departurestable
{

ArrivalDepartureTableGenerator::ArrivalDepartureTableGenerator(
		const cGare* place
		, Direction direction
		, EndFilter endfilter
		, const PhysicalStopFilter& physicalStopFilter
		, const LineFilter& lineFilter
		, const DisplayedPlacesList& displayedPlacesList
		, const ForbiddenPlacesList& forbiddenPlaces
		, const cMoment& startDateTime
		, const cMoment& endDateTime
		, size_t maxSize
) :	_place(place), _direction(direction), _endFilter(endfilter), _physicalStopFilter(physicalStopFilter)
, _lineFilter(lineFilter), _displayedPlaces(displayedPlacesList), _forbiddenPlaces(forbiddenPlaces), _startDateTime(startDateTime)
	, _endDateTime(endDateTime), _maxSize(maxSize)
{}


/** Les tests effectués sont les suivants. Ils doivent tous être positifs :
		- Ligne non explicitement interdite d'affichage sur tout tableau de départs
		- Ligne non explicitement interdite d'affichage sur le tableau de départ courant
		- Ligne au départ d'un quai autorisé sur le tableau de départ courant
		- Si tableau des origines seulement, ligne originaire de l'arrêt affiché sur le tableau courant
		- L'arrêt d'arrivée suivant ou la destination doivent être différent de l'arrêt courant
*/
bool ArrivalDepartureTableGenerator::_allowedLineStop(const cGareLigne* linestop) const
{
	/** - If a forbidden place is served, the the line is not allowed */
	if (!_forbiddenPlaces.empty())
		for (const cGareLigne* curLinestop = linestop; curLinestop != NULL; curLinestop = curLinestop->getArriveeSuivante())
			if (_forbiddenPlaces.find(curLinestop->PointArret()) != _forbiddenPlaces.end())
				return false;

	return 	linestop->Ligne()->AAfficherSurTableauDeparts()
		&&	_lineFilter.find(linestop->Ligne()) == _lineFilter.end()
		&&	(_physicalStopFilter.empty() || (_physicalStopFilter.find(linestop->getVoie()) != _physicalStopFilter.end()))
		&&	((_endFilter == WITH_PASSING) || (linestop->getDepartPrecedent() == NULL))
		&&	(((linestop->getArriveeSuivante() != NULL) && (linestop->getArriveeSuivante()->PointArret() != _place))
			|| (linestop->Destination()->PointArret() != _place))
	;
}


ArrivalDepartureTableGenerator::ArrivalDepartureList::iterator
ArrivalDepartureTableGenerator::_insert(
	const cGareLigne* linestop, int serviceNumber, const cMoment& realDepartureTime, UnlimitedSize unlimitedSize)
{
	// Values
	Element element;
	element.linestop = linestop;
	element.serviceNumber = serviceNumber;
	element.realDepartureTime = realDepartureTime;

	ActualDisplayedArrivalsList arrivals;
	set<const cGare*> encounteredPlaces;
	const cGare* destinationPlace = linestop->Destination()->PointArret();
	for (const cGareLigne* curLinestop = linestop; curLinestop != NULL; curLinestop = curLinestop->getArriveeSuivante())
	{
		const cGare* place = curLinestop->PointArret();
		
		if (	_displayedPlaces.find(place) != _displayedPlaces.end()
					&& encounteredPlaces.find(place) == encounteredPlaces.end()	// If the place must be displayed according to the display rules (only once per place)
					&& place != destinationPlace
				|| curLinestop->getArriveeSuivante()==NULL		// or if the place is the terminus
				|| curLinestop == linestop			// or if the place is the origin
		){
			arrivals.push_back(place);
			encounteredPlaces.insert(place);
		}
	}

	/** - Insertion */
	pair<ArrivalDepartureList::iterator, bool> insertResult = _result.insert(pair<Element, ActualDisplayedArrivalsList>(element, arrivals));

	/** - Control of size : if too long, deletion of the last element */
	if (unlimitedSize == SIZE_AS_DEFINED && _maxSize != UNLIMITED_SIZE && _result.size() > _maxSize)
	{
		_result.erase(--_result.end());
	}
	
	return insertResult.first;
}


}
}

#endif

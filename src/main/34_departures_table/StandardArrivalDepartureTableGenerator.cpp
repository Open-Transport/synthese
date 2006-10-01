
/* THIS CODE COMES FROM SYNTHESE 2. IT MUST BE REFRESHED. */
#ifdef RIEN

#include "StandardArrivalDepartureTableGenerator.h"

namespace synthese
{
namespace departurestable
{

StandardArrivalDepartureTableGenerator::StandardArrivalDepartureTableGenerator(
	const cGare* place
	, Direction direction
	, EndFilter endfilter
	, const PhysicalStopFilter& physicalStopFilter
	, const LineFilter& lineFilter
	, const DisplayedPlacesList& displayedPlacesList
	, const ForbiddenPlacesList& forbiddenPlaces
	, const cMoment& startTime
	, const cMoment& endDateTime
	, size_t maxSize
) : ArrivalDepartureTableGenerator(place, direction, endfilter, physicalStopFilter, lineFilter
								   , displayedPlacesList, forbiddenPlaces, startTime, endDateTime, maxSize)
{}

const ArrivalDepartureTableGenerator::ArrivalDepartureList& StandardArrivalDepartureTableGenerator::generate()
{
	// Parcours sur toutes les lignes au départ et sur tous les services
	for (const cGareLigne* departureLineStop = _place->PremiereGareLigneDep(); departureLineStop != NULL; departureLineStop = departureLineStop->PADepartSuivant())
	{
		if (!_allowedLineStop(departureLineStop))
			continue;

		// Loop on services
		cMoment departureDateTime = _startDateTime;
		int serviceNumber = -2;
		while ((serviceNumber = departureLineStop->Prochain(departureDateTime, _endDateTime, _startDateTime, ++serviceNumber)) != INCONNU)
		{
			_insert(departureLineStop, serviceNumber, departureDateTime);
		}		
	}
	return _result;

}

}
}

#endif

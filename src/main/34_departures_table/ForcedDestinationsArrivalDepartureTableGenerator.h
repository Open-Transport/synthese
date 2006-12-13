
/* THIS CODE COMES FROM SYNTHESE 2. IT MUST BE REFRESHED. */
#ifdef RIEN

#ifndef SYNTHESE_ForcedDestinationsArrivalDepartureTableGenerator_H
#define SYNTHESE_ForcedDestinationsArrivalDepartureTableGenerator_H

#include "ArrivalDepartureTableGenerator.h"

namespace synthese
{
namespace departurestable
{

class ForcedDestinationsArrivalDepartureTableGenerator : public ArrivalDepartureTableGenerator
{
public:
	typedef set<const cGare*> ForcedDestinationsSet;

private:

	ForcedDestinationsSet	_forcedDestinations;	//!< Destinations � afficher absolument
	const int					_persistanceDuration;	//!< Dur�e pendant laquelle une destination est forc�e


public:
	ForcedDestinationsArrivalDepartureTableGenerator(
		const cGare* place
		, Direction, EndFilter
		, const PhysicalStopFilter&, const LineFilter&
		, const DisplayedPlacesList&, const ForbiddenPlacesList&
		, const cMoment& startDateTime, const cMoment& endDateTime
		, size_t maxSize, const ForcedDestinationsSet&, int persistanceDuration
		);


	const ArrivalDepartureList& generate();
};

}
}

#endif

#endif


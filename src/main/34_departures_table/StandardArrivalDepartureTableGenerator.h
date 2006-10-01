
/* THIS CODE COMES FROM SYNTHESE 2. IT MUST BE REFRESHED. */
#ifdef RIEN

#ifndef SYNTHESE_StandardArrivalDepartureTableGenerator_H
#define SYNTHESE_StandardArrivalDepartureTableGenerator_H

#include "ArrivalDepartureTableGenerator.h"

namespace synthese
{
namespace departurestable
{

class StandardArrivalDepartureTableGenerator : public ArrivalDepartureTableGenerator
{
private:

public:
	StandardArrivalDepartureTableGenerator(
		const cGare* place
		, Direction, EndFilter
		, const PhysicalStopFilter&, const LineFilter&
		, const DisplayedPlacesList&, const ForbiddenPlacesList&
		, const cMoment& startDateTime, const cMoment& endDateTime
		, size_t maxSize = UNLIMITED_SIZE
		);

	const ArrivalDepartureList& generate();
};

}
}

#endif

#endif
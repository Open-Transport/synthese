
/* THIS CODE COMES FROM SYNTHESE 2. IT MUST BE REFRESHED. */
#ifdef RIEN

#include "ForcedDestinationsArrivalDepartureTableGenerator.h"
#include "StandardArrivalDepartureTableGenerator.h"
#include <map>

namespace synthese
{
namespace departurestable
{

ForcedDestinationsArrivalDepartureTableGenerator::ForcedDestinationsArrivalDepartureTableGenerator(
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
	, const ForcedDestinationsSet& forcedDestinations
	, int persistanceDuration
) : ArrivalDepartureTableGenerator(place, direction, endfilter, physicalStopFilter, lineFilter
								   , displayedPlacesList, forbiddenPlaces, startTime, endDateTime, maxSize)
	, _forcedDestinations(forcedDestinations)
	, _persistanceDuration(persistanceDuration)
{
	// Add terminuses to forced destinations
	for (const cGareLigne* curGLD = _place->PremiereGareLigneDep(); curGLD!= NULL; curGLD=curGLD->PADepartSuivant())
	{
		if (!_allowedLineStop(curGLD))
			continue;

		_forcedDestinations.insert(curGLD->Destination()->PointArret());
		_displayedPlaces.insert(curGLD->Destination()->PointArret());
	}
}




const ArrivalDepartureTableGenerator::ArrivalDepartureList& ForcedDestinationsArrivalDepartureTableGenerator::generate()
{
	/** - Search of best departure for each forced destination */
	typedef map<const cGare*, ArrivalDepartureList::iterator> ReachedDestinationMap;
	ReachedDestinationMap reachedDestination;
	
	for (const cGareLigne* curGLD = _place->PremiereGareLigneDep(); curGLD!= NULL; curGLD=curGLD->PADepartSuivant())
	{
		// Selection of the line
		if (!_allowedLineStop(curGLD))
			continue;

		// Next service
		cMoment tempStartDateTime = _startDateTime;
		
		// Max time for forced destination
		cMoment maxTimeForForcedDestination(_startDateTime);
		maxTimeForForcedDestination += cDureeEnMinutes(_persistanceDuration);

		int serviceNumber = curGLD->Prochain(tempStartDateTime, maxTimeForForcedDestination, _startDateTime);
		
		// No service
		if (serviceNumber == INCONNU)
			continue;

		bool insertionIsDone = false;

		// Exploration of the line
		for (const cGareLigne* curGLA = curGLD->getArriveeSuivante(); curGLA != NULL; curGLA = curGLA->getArriveeSuivante())
		{
			// Attempting to select the destination
			if (_forcedDestinations.find(curGLA->PointArret()) == _forcedDestinations.end())
				continue;

			// If first reach
			if (reachedDestination.find(curGLA->PointArret()) == reachedDestination.end())
			{
				// Allocation
				ArrivalDepartureList::iterator itr = _insert(curGLD, serviceNumber, tempStartDateTime, FORCE_UNLIMITED_SIZE);

				// Links
				reachedDestination[curGLA->PointArret()] = itr;
			}
			// Else optimizing a previously founded ptd
			else if (tempStartDateTime < reachedDestination[curGLA->PointArret()]->first.realDepartureTime)
			{
				// Allocation
				ArrivalDepartureList::iterator itr = _insert(curGLD, serviceNumber, tempStartDateTime, FORCE_UNLIMITED_SIZE);
				ArrivalDepartureList::iterator oldIt = reachedDestination[curGLA->PointArret()];

				reachedDestination[curGLA->PointArret()] = itr;

			        // If the preceding ptd is not used for an other place, deletion
				ReachedDestinationMap::iterator it;
				for (it	= reachedDestination.begin();
					it != reachedDestination.end(); ++it)
					if (it->second == oldIt)
						break;
				if (it == reachedDestination.end())
					_result.erase(oldIt);
			}
		}
	}

	// In case of incomplete departure table, the serie is filled with normal algorithm
	if (_result.size() < _maxSize)
	{
		StandardArrivalDepartureTableGenerator standardTable(_place, _direction, _endFilter, _physicalStopFilter
			, _lineFilter, DisplayedPlacesList(), _forbiddenPlaces, _startDateTime, _endDateTime, _maxSize + _result.size());
		const ArrivalDepartureList& standardTableResult = standardTable.generate();

		for (ArrivalDepartureList::const_iterator itr = standardTableResult.begin();
			_result.size() < _maxSize && itr != standardTableResult.end(); ++itr)
		{
			if (_result.find(itr->first) == _result.end())
				_insert(itr->first.linestop, itr->first.serviceNumber, itr->first.realDepartureTime);
		}
	}

	return _result;

}

}
}

#endif

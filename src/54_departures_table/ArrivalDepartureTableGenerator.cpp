
/** ArrivalDepartureTableGenerator class implementation.
	@file ArrivalDepartureTableGenerator.cpp

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2
	of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include "LineStop.h"
#include "Line.h"
#include "PublicTransportStopZoneConnectionPlace.h"
#include "PhysicalStop.h"

#include "ArrivalDepartureTableGenerator.h"

using namespace std;

namespace synthese
{
	using namespace env;
	using namespace time;
	using namespace graph;

	namespace departurestable
	{

		ArrivalDepartureTableGenerator::ArrivalDepartureTableGenerator(
				const PhysicalStops&		physicalStops
				, const DeparturesTableDirection&			direction
				, const EndFilter&			endfilter
				, const LineFilter&			lineFilter
				, const DisplayedPlacesList&	displayedPlacesList
				, const ForbiddenPlacesList&	forbiddenPlaces
				, const DateTime& startDateTime
				, const DateTime& endDateTime
				, int blinkingDelay
				, size_t maxSize
		) : _physicalStops(physicalStops), _direction(direction), _endFilter(endfilter)
		, _lineFilter(lineFilter), _displayedPlaces(displayedPlacesList), _forbiddenPlaces(forbiddenPlaces), _startDateTime(startDateTime)
			, _endDateTime(endDateTime), _maxSize(maxSize), _blinkingDelay(blinkingDelay)
			, _calculationDateTime(TIME_CURRENT)
		{}


		/** Les tests effectués sont les suivants. Ils doivent tous être positifs :
				- Ligne non explicitement interdite d'affichage sur tout tableau de départs
				- Ligne non explicitement interdite d'affichage sur le tableau de départ courant
				- Si tableau des origines seulement, ligne originaire de l'arrêt affiché sur le tableau courant
				- L'arrêt d'arrivée suivant ou la destination doivent être différent de l'arrêt courant (?? -> removed currently)
		*/
		bool ArrivalDepartureTableGenerator::_allowedLineStop(const LineStop* linestop) const
		{
			/** - If a forbidden place is served, the the line is not allowed */
			if (!_forbiddenPlaces.empty())
				for(const LineStop* curLinestop(linestop);
					curLinestop != NULL;
					curLinestop = static_cast<const LineStop*>(curLinestop->getFollowingArrivalForFineSteppingOnly())
				){
					if(	_forbiddenPlaces.find(curLinestop->getPhysicalStop()->getConnectionPlace()->getKey()) !=
						_forbiddenPlaces.end()
					){
						return false;
					}
				}

			return 	linestop->getLine()->getUseInDepartureBoards()
				&&	_lineFilter.find(linestop->getLine()->getKey()) == _lineFilter.end()
				&&	((_endFilter == WITH_PASSING) || (linestop->getPreviousDepartureForFineSteppingOnly() == NULL))
				//&&	(((linestop->getFollowingArrival() != NULL) && (linestop->getFollowingArrival()->getConnectionPlace() != _place))
				//	|| ( linestop->getLine()->getDestination()->getConnectionPlace() != _place))
			;
		}


		ArrivalDepartureList::iterator
		ArrivalDepartureTableGenerator::_insert(
			const ServicePointer& servicePointer
			, UnlimitedSize unlimitedSize
		){
			// Values
			DeparturesTableElement element(
				servicePointer
				, (servicePointer.getActualDateTime() - _startDateTime) < _blinkingDelay
				);

			ActualDisplayedArrivalsList arrivals;
			set<const PublicTransportStopZoneConnectionPlace*> encounteredPlaces;
			const PublicTransportStopZoneConnectionPlace* destinationPlace(
				static_cast<const Line*>(
					servicePointer.getEdge()->getParentPath()
				)->getDestination()->getConnectionPlace()
			);
			for(const LineStop* curLinestop = static_cast<const LineStop*>(servicePointer.getEdge());
				curLinestop != NULL;
				curLinestop = static_cast<const LineStop*>(curLinestop->getFollowingArrivalForFineSteppingOnly())
			){
				const PublicTransportStopZoneConnectionPlace* place(
					curLinestop->getPhysicalStop()->getConnectionPlace()
				);
				
				if(	(	_displayedPlaces.find(place->getKey()) != _displayedPlaces.end() &&
						// If the place must be displayed according to the display rules (only once per place)
						encounteredPlaces.find(place) == encounteredPlaces.end() &&
						place != destinationPlace
					)||
					// or if the place is the terminus
					curLinestop->getFollowingArrivalForFineSteppingOnly() == NULL ||
					curLinestop == servicePointer.getEdge()			// or if the place is the origin
				){
					arrivals.push_back(place);
					encounteredPlaces.insert(place);
				}
			}

			/** - Insertion */
			pair<ArrivalDepartureList::iterator, bool> insertResult = _result.insert(pair<DeparturesTableElement, ActualDisplayedArrivalsList>(element, arrivals));

			/** - Control of size : if too long, deletion of the last element */
			if (unlimitedSize == SIZE_AS_DEFINED && _maxSize != UNLIMITED_SIZE && _result.size() > _maxSize)
			{
				_result.erase(--_result.end());
			}
			
			return insertResult.first;
		}


	}
}

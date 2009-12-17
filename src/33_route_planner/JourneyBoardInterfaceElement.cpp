
/** JourneyBoardInterfaceElement class implementation.
	@file JourneyBoardInterfaceElement.cpp

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


#include "JourneyBoardInterfaceElement.h"
#include "JourneyBoardStopCellInterfacePage.h"
#include "JourneyBoardServiceCellInterfacePage.h"
#include "JourneyBoardJunctionCellInterfacePage.h"
#include "RoutePlannerModule.h"

#include "Request.h"

#include "Edge.h"
#include "PhysicalStop.h"
#include "AddressablePlace.h"
#include "Road.h"
#include "ServiceUse.h"
#include "Journey.h"
#include "Service.h"

#include "Interface.h"
#include "ValueElementList.h"

#include "InterfacePageException.h"

#include "04_time/module.h"

#include "Conversion.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace interfaces;
	using namespace util;
	using namespace time;
	using namespace env;
	using namespace graph;
	using namespace road;

	template<> const string util::FactorableTemplate<LibraryInterfaceElement,routeplanner::JourneyBoardInterfaceElement>::FACTORY_KEY(
		"journey_board"
	);

	namespace routeplanner
	{

		string JourneyBoardInterfaceElement::display(
			ostream& stream
			, const ParametersVector& parameters
			, VariablesMap& variables
			, const void* object /*= NULL*/
			, const server::Request* request /*= NULL*/ ) const
		{
			// Parameters
			const Journey* journey = static_cast<const Journey*>(object);
			int __FiltreHandicape = (Conversion::ToInt(_handicappedFilter->getValue(parameters, variables, object, request)) > 1);
			int __FiltreVelo = (Conversion::ToInt(_bikeFilter->getValue(parameters, variables, object, request)) > 1 );
			const string pageCode(_pageCode.get() ? _pageCode->getValue(parameters, variables, object, request) : string());

			// Resources
			const JourneyBoardStopCellInterfacePage* stopCellInterfacePage = _page->getInterface()->getPage<JourneyBoardStopCellInterfacePage>(pageCode);
			const JourneyBoardServiceCellInterfacePage* serviceCellInterfacePage = _page->getInterface()->getPage<JourneyBoardServiceCellInterfacePage>(pageCode);
			const JourneyBoardJunctionCellInterfacePage* junctionCellInterfacePage = _page->getInterface()->getPage<JourneyBoardJunctionCellInterfacePage>(pageCode);
			const Hour unknownHour(TIME_UNKNOWN );
			const DateTime unknownDateTime(TIME_UNKNOWN );

			// Loop on lines of the board
			bool __Couleur = false;

			const Hub* lastPlace(journey->getOrigin()->getHub());
			double distance(0);

			const Journey::ServiceUses& services(journey->getServiceUses());
			for (Journey::ServiceUses::const_iterator it = services.begin(); it != services.end(); ++it)
			{
				const ServiceUse& leg(*it);

				const Road* road(dynamic_cast<const Road*> (leg.getService()->getPath ()));
				if (road == NULL)
				{
					distance = 0;
					
					// LIGNE ARRET MONTEE Si premier point d'arrêt et si alerte
					if (leg.getDepartureEdge()->getHub() != lastPlace)
					{
						/*					DateTime debutPrem(leg.getDepartureDateTime());
						DateTime finPrem(debutPrem);
						if (journey->getContinuousServiceRange () )
						finPrem += journey->getContinuousServiceRange ();
						*/

						stopCellInterfacePage->display(
							stream
							, false
							, NULL // leg->getDestination() ->getConnectionPlace()->hasApplicableAlarm ( debutArret, finArret ) ? __ET->getDestination()->getConnectionPlace()->getAlarm() : NULL
							, false
							, *static_cast<const PhysicalStop*>(leg.getDepartureEdge()->getFromVertex())
							, __Couleur
							, leg.getDepartureDateTime()
							, journey->getContinuousServiceRange()
							, request
							);

						lastPlace = leg.getDepartureEdge()->getHub();
						__Couleur = !__Couleur;
					}

					// LIGNE CIRCULATIONS
/*					DateTime debutLigne(leg.getDepartureDateTime());
					DateTime finLigne(leg.getArrivalDateTime());

					if ( journey->getContinuousServiceRange () )
					{
						finLigne = lastArrivalTime;
					}
*/

					serviceCellInterfacePage->display(
						stream 
						, leg
						, journey->getContinuousServiceRange()
						, __FiltreHandicape
						, __FiltreVelo
						, NULL // leg->getService ()->getPath ()->hasApplicableAlarm ( debutLigne, finLigne ) ? __ET->getService()->getPath ()->getAlarm() : NULL
						, __Couleur
						, request
					);
					
					__Couleur = !__Couleur;

					// LIGNE ARRET DE DESCENTE

/*					DateTime debutArret(leg.getArrivalDateTime ());
					DateTime finArret(debutArret);
					if ( (it + 1) < journey->getServiceUses().end())
						finArret = (it + 1)->getDepartureDateTime();
					if ( journey->getContinuousServiceRange () )
						finArret += journey->getContinuousServiceRange ();
*/

					stopCellInterfacePage->display( stream, true
						, NULL // leg->getDestination() ->getConnectionPlace()->hasApplicableAlarm ( debutArret, finArret ) ? __ET->getDestination()->getConnectionPlace()->getAlarm() : NULL
						, leg.getArrivalEdge()->getHub() == leg.getService()->getPath()->getEdges().back()->getHub()
						, *static_cast<const PhysicalStop*>(leg.getArrivalEdge()->getFromVertex())
						, __Couleur
						, leg.getArrivalDateTime()
						, journey->getContinuousServiceRange()
						, request
					);

					lastPlace = leg.getArrivalEdge()->getHub();
					__Couleur = !__Couleur;

				}
				else
				{
					// 1/2 Alerte
/*					DateTime debutArret(leg.getArrivalDateTime ());
					DateTime finArret(debutArret);
					if ((it+1) < journey->getServiceUses().end())
						finArret = (it + 1)->getDepartureDateTime();
					if ( journey->getContinuousServiceRange () )
						finArret += journey->getContinuousServiceRange ();
*/
					distance += leg.getDistance();

					if (it + 1 != services.end())
					{
						const ServiceUse& nextLeg(*(it+1));
						const Road* nextRoad(dynamic_cast<const Road*> (nextLeg.getService()->getPath ()));

						if (nextRoad && nextRoad->getRoadPlace() == road->getRoadPlace())
							continue;
					}

/*					const AddressablePlace* aPlace(
						AddressablePlace::GetPlace(
							leg.getArrivalEdge()->getHub()
					)	);
*/					junctionCellInterfacePage->display(
						stream
						, NULL //aPlace
						, NULL // leg->getDestination()->getConnectionPlace()->hasApplicableAlarm(debutArret, finArret) ? __ET->getDestination()->getConnectionPlace()->getAlarm() : NULL
						, __Couleur
						, road
						, distance
						, request
					);
					
					distance = 0;				
					__Couleur = !__Couleur;
				}
			}
			return string();
		}

		void JourneyBoardInterfaceElement::storeParameters( interfaces::ValueElementList& vel )
		{
			if (vel.size() < 2)
				throw InterfacePageException("Insufficient parameters number");
			_handicappedFilter = vel.front();
			_bikeFilter = vel.front();
			if(!vel.isEmpty())
			{
				_pageCode = vel.front();
			}
		}
	}
}

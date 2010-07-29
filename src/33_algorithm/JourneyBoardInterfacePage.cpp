
/** JourneyBoardInterfacePage class implementation.
	@file JourneyBoardInterfacePage.cpp

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

#include "JourneyBoardInterfacePage.h"
#include "StaticFunctionRequest.h"
#include "OnlineReservationRule.h"
#include "NamedPlace.h"
#include "Journey.h"
#include "Edge.h"
#include "AddressablePlace.h"
#include "ReservationContact.h"
#include "Service.h"
#include "Crossing.h"
#include "JourneyPattern.hpp"
#include "DateTimeInterfacePage.h"
#include "CommercialLine.h"
#include "Env.h"
#include "GeoPoint.h"
#include "Projection.h"
#include "WebPageDisplayFunction.h"
#include "Road.h"
#include "PhysicalStop.h"
#include "SentAlarm.h"
#include "ContinuousService.h"
#include "RollingStock.h"
#include "StopArea.hpp"
#include "RoadPlace.h"

#include <sstream>
#include <set>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>

using namespace std;
using namespace boost;
using namespace boost::posix_time;


namespace synthese
{
	using namespace routeplanner;
	using namespace util;
	using namespace pt;
	using namespace resa;
	using namespace graph;
	using namespace road;
	using namespace geography;
	using namespace server;
	using namespace transportwebsite;
	
	
	
	namespace routeplanner
	{
		const string JourneyBoardInterfacePage::DATA_RANK("rank");
		const string JourneyBoardInterfacePage::DATA_IS_THE_LAST_JOURNEY_BOARD("is_the_last_journey_board");
		const string JourneyBoardInterfacePage::DATA_HANDICAPPED_FILTER("handicapped_filter");
		const string JourneyBoardInterfacePage::DATA_BIKE_FILTER("bike_filter");
		const string JourneyBoardInterfacePage::DATA_DEPARTURE_TIME("departure_time");
		const string JourneyBoardInterfacePage::DATA_DEPARTURE_DATE("departure_date");
		const string JourneyBoardInterfacePage::DATA_DEPARTURE_TIME_INTERNAL_FORMAT("internal_departure_time");
		const string JourneyBoardInterfacePage::DATA_CONTINUOUS_SERVICE_LAST_DEPARTURE_TIME("continuous_service_last_departure_time");
		const string JourneyBoardInterfacePage::DATA_DEPARTURE_PLACE_NAME("departure_place_name");
		const string JourneyBoardInterfacePage::DATA_DEPARTURE_PLACE_LONGITUDE("departure_longitude");
		const string JourneyBoardInterfacePage::DATA_DEPARTURE_PLACE_LATITUDE("departure_latitude");
		const string JourneyBoardInterfacePage::DATA_ARRIVAL_TIME("arrival_time");
		const string JourneyBoardInterfacePage::DATA_CONTINUOUS_SERVICE_LAST_ARRIVAL_TIME("continuous_service_last_arrival_time");
		const string JourneyBoardInterfacePage::DATA_ARRIVAL_PLACE_NAME("arrival_place_name");
		const string JourneyBoardInterfacePage::DATA_ARRIVAL_PLACE_LONGITUDE("arrival_longitude");
		const string JourneyBoardInterfacePage::DATA_ARRIVAL_PLACE_LATITUDE("arrival_latitude");
		const string JourneyBoardInterfacePage::DATA_DURATION("duration");
		const string JourneyBoardInterfacePage::DATA_RESERVATION_AVAILABLE("reservation_available");
		const string JourneyBoardInterfacePage::DATA_RESERVATION_COMPULSORY("reservation_compulsory");
		const string JourneyBoardInterfacePage::DATA_RESERVATION_DELAY("reservation_delay");
		const string JourneyBoardInterfacePage::DATA_RESERVATION_DEADLINE("reservation_deadline");
		const string JourneyBoardInterfacePage::DATA_RESERVATION_PHONE_NUMBER("reservation_phone_number");
		const string JourneyBoardInterfacePage::DATA_ONLINE_RESERVATION("online_reservation");
		const string JourneyBoardInterfacePage::DATA_CONTENT("content");
		const string JourneyBoardInterfacePage::DATA_CONTINUOUS_SERVICE_WAITING("continuous_service_waiting");
		
		// Cells
		const string JourneyBoardInterfacePage::DATA_ODD_ROW("is_odd_row");
		const string JourneyBoardInterfacePage::DATA_ALARM_LEVEL("alarm_level");
		const string JourneyBoardInterfacePage::DATA_ALARM_MESSAGE("alarm_message");

		// Stop cells
		const string JourneyBoardInterfacePage::DATA_IS_ARRIVAL("is_arrival");
		const string JourneyBoardInterfacePage::DATA_IS_TERMINUS("is_terminus");
		const string JourneyBoardInterfacePage::DATA_STOP_NAME("stop_name");
		const string JourneyBoardInterfacePage::DATA_FIRST_TIME("first_time");
		const string JourneyBoardInterfacePage::DATA_LAST_TIME("last_time");
		const string JourneyBoardInterfacePage::DATA_LONGITUDE("longitude");
		const string JourneyBoardInterfacePage::DATA_LATITUDE("latitude");
		const string JourneyBoardInterfacePage::DATA_IS_LAST_LEG("is_last_leg");

		// Junction cells
		const string JourneyBoardInterfacePage::DATA_REACHED_PLACE_IS_NAMED("reached_place_is_named");
		const string JourneyBoardInterfacePage::DATA_ROAD_NAME("road_name");
		const string JourneyBoardInterfacePage::DATA_LENGTH("length");

		// Service cells
		const string JourneyBoardInterfacePage::DATA_FIRST_DEPARTURE_TIME("first_departure_time");
		const string JourneyBoardInterfacePage::DATA_LAST_DEPARTURE_TIME("last_departure_time");
		const string JourneyBoardInterfacePage::DATA_FIRST_ARRIVAL_TIME("first_arrival_time");
		const string JourneyBoardInterfacePage::DATA_LAST_ARRIVAL_TIME("last_arrival_time");
		const string JourneyBoardInterfacePage::DATA_ROLLINGSTOCK_ID("rolling_stock_id");
		const string JourneyBoardInterfacePage::DATA_ROLLINGSTOCK_NAME("rolling_stock_name");
		const string JourneyBoardInterfacePage::DATA_ROLLINGSTOCK_ARTICLE("rolling_stock_article");
		const string JourneyBoardInterfacePage::DATA_DESTINATION_NAME("destination_name");
		const string JourneyBoardInterfacePage::DATA_HANDICAPPED_FILTER_STATUS("handicapped_filter_status");
		const string JourneyBoardInterfacePage::DATA_HANDICAPPED_PLACES_NUMBER("handicapped_places_number");
		const string JourneyBoardInterfacePage::DATA_BIKE_FILTER_STATUS("bike_filter_status");
		const string JourneyBoardInterfacePage::DATA_BIKE_PLACES_NUMBER("bike_places_number");
		const string JourneyBoardInterfacePage::DATA_LINE_SHORT_NAME("line_short_name");
		const string JourneyBoardInterfacePage::DATA_LINE_LONG_NAME("line_long_name");
		const string JourneyBoardInterfacePage::DATA_LINE_IMAGE("line_image");
		const string JourneyBoardInterfacePage::DATA_LINE_ID("line_id");
		const string JourneyBoardInterfacePage::DATA_LINE_STYLE("line_style");


		void JourneyBoardInterfacePage::Display(
			std::ostream& stream,
			boost::shared_ptr<const cms::Webpage> page,
			boost::shared_ptr<const cms::Webpage> durationPage,
			boost::shared_ptr<const cms::Webpage> datePage,
			boost::shared_ptr<const cms::Webpage> stopCellPage,
			boost::shared_ptr<const cms::Webpage> serviceCellPage,
			boost::shared_ptr<const cms::Webpage> junctionPage,
			const server::Request& request,
			size_t n
			, const Journey& journey
			, const Place& departurePlace
			, const Place& arrivalPlace
			, logic::tribool handicappedFilter
			, logic::tribool bikeFilter
			, bool isTheLast
		){
			StaticFunctionRequest<WebPageDisplayFunction> displayRequest(request, false);
			displayRequest.getFunction()->setPage(page);
			displayRequest.getFunction()->setUseTemplate(false);
			ParametersMap pm;

			// Rank
			pm.insert(DATA_RANK, n);
			pm.insert(DATA_IS_THE_LAST_JOURNEY_BOARD, isTheLast);

			// Filters
			pm.insert(DATA_HANDICAPPED_FILTER, handicappedFilter);
			pm.insert(DATA_BIKE_FILTER, bikeFilter);

			// Departure time
			{
				stringstream s;
				s << setw(2) << setfill('0') << journey.getFirstDepartureTime().time_of_day().hours() << ":" << setw(2) << setfill('0') << journey.getFirstDepartureTime().time_of_day().minutes();
				pm.insert(DATA_DEPARTURE_TIME, s.str());
			}
			if(datePage.get())
			{
				stringstream sDate;
				DateTimeInterfacePage::Display(sDate, datePage, request, journey.getFirstDepartureTime());
				pm.insert(DATA_DEPARTURE_DATE, sDate.str());
			}
			else
			{
				pm.insert(DATA_DEPARTURE_DATE, journey.getFirstDepartureTime());
			}
			pm.insert(DATA_DEPARTURE_TIME_INTERNAL_FORMAT, to_iso_extended_string(journey.getFirstDepartureTime()));

			if(journey.getContinuousServiceRange().total_seconds())
			{
				pm.insert(DATA_CONTINUOUS_SERVICE_LAST_DEPARTURE_TIME, to_simple_string(journey.getLastDepartureTime().time_of_day()));
			}

			// Departure place
			string displayedDeparturePlace(
				(	dynamic_cast<const Crossing*>(journey.getOrigin()->getHub()) ?
					dynamic_cast<const NamedPlace&>(departurePlace) :
					dynamic_cast<const NamedPlace&>(*journey.getOrigin()->getHub())
				).getFullName()
			);
			pm.insert(DATA_DEPARTURE_PLACE_NAME, displayedDeparturePlace);
			GeoPoint departurePoint(WGS84FromLambert(departurePlace.getPoint()));
			pm.insert(DATA_DEPARTURE_PLACE_LONGITUDE, departurePoint.getLongitude());
			pm.insert(DATA_DEPARTURE_PLACE_LATITUDE, departurePoint.getLatitude());
			
			// Arrival time
			{
				stringstream s;
				s << setw(2) << setfill('0') << journey.getFirstArrivalTime().time_of_day().hours() << ":" << setw(2) << setfill('0') << journey.getFirstArrivalTime().time_of_day().minutes();
				pm.insert(DATA_ARRIVAL_TIME, s.str());
			}

			if(journey.getContinuousServiceRange().total_seconds())
			{
				pm.insert(DATA_CONTINUOUS_SERVICE_LAST_ARRIVAL_TIME, to_simple_string(journey.getLastArrivalTime().time_of_day()));
			}

			// Arrival place
			string displayedArrivalPlace(
				(	dynamic_cast<const Crossing*>(journey.getDestination()->getHub()) ?
					dynamic_cast<const NamedPlace&>(arrivalPlace) :
					dynamic_cast<const NamedPlace&>(*journey.getDestination()->getHub())
				).getFullName()
			);
			pm.insert(DATA_ARRIVAL_PLACE_NAME, displayedArrivalPlace);
			GeoPoint arrivalPoint(WGS84FromLambert(arrivalPlace.getPoint()));
			pm.insert(DATA_ARRIVAL_PLACE_LONGITUDE, arrivalPoint.getLongitude());
			pm.insert(DATA_ARRIVAL_PLACE_LATITUDE, arrivalPoint.getLatitude());

			// Duration
			if(durationPage.get())
			{
				stringstream sDuration;
				DateTimeInterfacePage::Display(sDuration, durationPage, request, journey.getDuration());
				pm.insert(DATA_DURATION, sDuration.str());
			}
			else
			{
				pm.insert(DATA_DURATION, journey.getDuration());
			}

			// Reservation
			ptime now(second_clock::local_time());
			ptime resaDeadLine(journey.getReservationDeadLine());
			logic::tribool resaCompliance(journey.getReservationCompliance());
			pm.insert(DATA_RESERVATION_AVAILABLE, resaCompliance && resaDeadLine > now);
			pm.insert(DATA_RESERVATION_COMPULSORY, resaCompliance == true);
			pm.insert(DATA_RESERVATION_DELAY, resaDeadLine.is_not_a_date_time() ? 0 : (resaDeadLine - now).total_seconds() / 60);

			if(!journey.getReservationDeadLine().is_not_a_date_time())
			{
				if(datePage.get())
				{
					stringstream sResa;
					DateTimeInterfacePage::Display(sResa, datePage, request, journey.getReservationDeadLine());
					pm.insert(DATA_RESERVATION_DEADLINE, sResa.str());
				}
				else
				{
					pm.insert(DATA_RESERVATION_DEADLINE, journey.getReservationDeadLine());
				}
			}

			// Reservation contact
			set<const ReservationContact*> resaRules;
			BOOST_FOREACH(const ServicePointer& su, journey.getServiceUses())
			{
				const JourneyPattern* line(dynamic_cast<const JourneyPattern*>(su.getService()->getPath()));
				if(line == NULL) continue;

				if(	line->getCommercialLine()->getReservationContact() &&
					UseRule::IsReservationPossible(su.getUseRule().getReservationAvailability(su))
					){
						resaRules.insert(line->getCommercialLine()->getReservationContact());
				}
			}
			stringstream sPhones;
			bool onlineBooking(!resaRules.empty());
			BOOST_FOREACH(const ReservationContact* rc, resaRules)
			{
				sPhones <<
					rc->getPhoneExchangeNumber() <<
					" (" << rc->getPhoneExchangeOpeningHours() << ") "
					;
				if (!OnlineReservationRule::GetOnlineReservationRule(rc))
				{
					onlineBooking = false;
				}
			}
			pm.insert(DATA_RESERVATION_PHONE_NUMBER, sPhones.str());
			pm.insert(DATA_ONLINE_RESERVATION, onlineBooking);

			// Content
			if(stopCellPage.get() && serviceCellPage.get() && junctionPage.get())
			{
				stringstream content;

				// Loop on lines of the board
				bool __Couleur = false;

				const Hub* lastPlace(journey.getOrigin()->getHub());
				double distance(0);

				const Journey::ServiceUses& services(journey.getServiceUses());
				for (Journey::ServiceUses::const_iterator it = services.begin(); it != services.end(); ++it)
				{
					const ServicePointer& leg(*it);

					const Road* road(dynamic_cast<const Road*> (leg.getService()->getPath ()));
					if (road == NULL)
					{
						distance = 0;

						// LIGNE ARRET MONTEE Si premier point d'arrêt et si alerte
						if (leg.getDepartureEdge()->getHub() != lastPlace)
						{
							/*					ptime debutPrem(leg.getDepartureDateTime());
							ptime finPrem(debutPrem);
							if (journey.getContinuousServiceRange () )
							finPrem += journey.getContinuousServiceRange ();
							*/

							DisplayStopCell(
								content,
								stopCellPage,
								request,
								false
								, NULL // leg->getDestination() ->getConnectionPlace()->hasApplicableAlarm ( debutArret, finArret ) ? __ET->getDestination()->getConnectionPlace()->getAlarm() : NULL
								, false
								, *static_cast<const PhysicalStop*>(leg.getDepartureEdge()->getFromVertex())
								, __Couleur
								, leg.getDepartureDateTime()
								, journey.getContinuousServiceRange(),
								false
							);

							lastPlace = leg.getDepartureEdge()->getHub();
							__Couleur = !__Couleur;
						}

						// LIGNE CIRCULATIONS
						/*					ptime debutLigne(leg.getDepartureDateTime());
						ptime finLigne(leg.getArrivalDateTime());

						if ( journey.getContinuousServiceRange () )
						{
						finLigne = lastArrivalTime;
						}
						*/

						DisplayServiceCell(
							content,
							serviceCellPage,
							request,
							leg,
							journey.getContinuousServiceRange(),
							handicappedFilter,
							bikeFilter,
							NULL, // leg->getService ()->getPath ()->hasApplicableAlarm ( debutLigne, finLigne ) ? __ET->getService()->getPath ()->getAlarm() : NULL
							__Couleur
						);

						__Couleur = !__Couleur;

						// LIGNE ARRET DE DESCENTE

						/*					ptime debutArret(leg.getArrivalDateTime ());
						ptime finArret(debutArret);
						if ( (it + 1) < journey.getServiceUses().end())
						finArret = (it + 1)->getDepartureDateTime();
						if ( journey.getContinuousServiceRange () )
						finArret += journey.getContinuousServiceRange ();
						*/

						DisplayStopCell(
							content,
							stopCellPage,
							request,
							true
							, NULL // leg->getDestination() ->getConnectionPlace()->hasApplicableAlarm ( debutArret, finArret ) ? __ET->getDestination()->getConnectionPlace()->getAlarm() : NULL
							, leg.getArrivalEdge()->getHub() == leg.getService()->getPath()->getEdges().back()->getHub()
							, *static_cast<const PhysicalStop*>(leg.getArrivalEdge()->getFromVertex())
							, __Couleur
							, leg.getArrivalDateTime()
							, journey.getContinuousServiceRange(),
							it+1 == services.end()
						);

						lastPlace = leg.getArrivalEdge()->getHub();
						__Couleur = !__Couleur;

					}
					else
					{
						// 1/2 Alerte
						/*					ptime debutArret(leg.getArrivalDateTime ());
						ptime finArret(debutArret);
						if ((it+1) < journey.getServiceUses().end())
						finArret = (it + 1)->getDepartureDateTime();
						if ( journey.getContinuousServiceRange () )
						finArret += journey.getContinuousServiceRange ();
						*/
						distance += leg.getDistance();

						if (it + 1 != services.end())
						{
							const ServicePointer& nextLeg(*(it+1));
							const Road* nextRoad(dynamic_cast<const Road*> (nextLeg.getService()->getPath ()));

							if (nextRoad && nextRoad->getRoadPlace() == road->getRoadPlace())
								continue;
						}

						/*					const AddressablePlace* aPlace(
						AddressablePlace::GetPlace(
						leg.getArrivalEdge()->getHub()
						)	);
						*/
						DisplayJunctionCell(
							content,
							junctionPage,
							request,
							*leg.getArrivalEdge()->getFromVertex()
							, NULL // leg->getDestination()->getConnectionPlace()->hasApplicableAlarm(debutArret, finArret) ? __ET->getDestination()->getConnectionPlace()->getAlarm() : NULL
							, __Couleur
							, road
							, distance
						);

						distance = 0;				
						__Couleur = !__Couleur;
					}
				}
				pm.insert(DATA_CONTENT, content.str());
			}


			displayRequest.getFunction()->setAditionnalParametersMap(pm);
			displayRequest.run(stream);
		}



		void JourneyBoardInterfacePage::DisplayStopCell(
			std::ostream& stream,
			boost::shared_ptr<const cms::Webpage> page,
			const server::Request& request,
			bool isItArrival,
			const messages::SentAlarm* alarm,
			bool isItTerminus,
			const pt::PhysicalStop& physicalStop,
			bool color,
			const boost::posix_time::ptime& time,
			boost::posix_time::time_duration continuousServiceRange,
			bool isLastLeg
		){
			StaticFunctionRequest<WebPageDisplayFunction> displayRequest(request, false);
			displayRequest.getFunction()->setPage(page);
			displayRequest.getFunction()->setUseTemplate(false);
			ParametersMap pm;

			ptime endRangeTime(time);
			if (continuousServiceRange.total_seconds() > 0)
				endRangeTime += continuousServiceRange;

			pm.insert(DATA_IS_ARRIVAL, isItArrival);
			
			// Alarm
			if(alarm)
			{
				pm.insert(DATA_ALARM_LEVEL, alarm->getLongMessage());
				pm.insert(DATA_ALARM_MESSAGE, alarm->getLevel());
			}

			pm.insert(DATA_IS_TERMINUS, isItTerminus);
			pm.insert(DATA_STOP_NAME, dynamic_cast<const NamedPlace&>(*physicalStop.getHub()).getFullName());
			pm.insert(DATA_ODD_ROW, color);
			{
				stringstream s;
				if(!time.is_not_a_date_time())
				{
					s << setw(2) << setfill('0') << time.time_of_day().hours() << ":" << setw(2) << setfill('0') << time.time_of_day().minutes();
				}
				pm.insert(DATA_FIRST_TIME, s.str()); // 6
			}
			{
				stringstream s;
				if(continuousServiceRange.total_seconds() > 0)
				{
					s << setw(2) << setfill('0') << endRangeTime.time_of_day().hours() << ":" << setw(2) << setfill('0') << endRangeTime.time_of_day().minutes();
				}
				pm.insert(DATA_LAST_TIME, s.str()); // 7
			}

			// Point
			GeoPoint point(WGS84FromLambert(physicalStop));
			pm.insert(DATA_LONGITUDE, point.getLongitude());
			pm.insert(DATA_LATITUDE, point.getLatitude());
			
			pm.insert(DATA_IS_LAST_LEG, isLastLeg);

			displayRequest.getFunction()->setAditionnalParametersMap(pm);
			displayRequest.run(stream);
		}



		void JourneyBoardInterfacePage::DisplayJunctionCell(
			std::ostream& stream,
			boost::shared_ptr<const cms::Webpage> page,
			const Request& request,
			const graph::Vertex& vertex,
			const messages::SentAlarm* alarm,
			bool color,
			const road::Road* road,
			double distance
		){
			StaticFunctionRequest<WebPageDisplayFunction> displayRequest(request, false);
			displayRequest.getFunction()->setPage(page);
			displayRequest.getFunction()->setUseTemplate(false);
			ParametersMap pm;

			// Point
			GeoPoint point(WGS84FromLambert(vertex));
			pm.insert(DATA_LONGITUDE, point.getLongitude());
			pm.insert(DATA_LATITUDE, point.getLatitude());
			pm.insert(DATA_REACHED_PLACE_IS_NAMED, dynamic_cast<const NamedPlace*>(vertex.getHub()) != NULL);
			pm.insert(DATA_ODD_ROW, color);
			if(road && road->getRoadPlace())
			{
				pm.insert(DATA_ROAD_NAME, road->getRoadPlace()->getName());
			}
			pm.insert(DATA_LENGTH, distance);

			displayRequest.getFunction()->setAditionnalParametersMap(pm);
			displayRequest.run(stream);
		}



		void JourneyBoardInterfacePage::DisplayServiceCell(
			std::ostream& stream,
			boost::shared_ptr<const cms::Webpage> page,
			const server::Request& request,
			const graph::ServicePointer& serviceUse,
			boost::posix_time::time_duration continuousServiceRange,
			boost::logic::tribool handicappedFilterStatus,
			boost::logic::tribool bikeFilterStatus,
			const messages::SentAlarm* alarm,
			bool color
		){
			StaticFunctionRequest<WebPageDisplayFunction> displayRequest(request, false);
			displayRequest.getFunction()->setPage(page);
			displayRequest.getFunction()->setUseTemplate(false);
			ParametersMap pm;

			// Continuous service
			ptime lastDepartureDateTime(serviceUse.getDepartureDateTime());
			ptime lastArrivalDateTime(serviceUse.getArrivalDateTime());
			if (continuousServiceRange.total_seconds())
			{
				lastArrivalDateTime += continuousServiceRange;
				lastDepartureDateTime += continuousServiceRange;
			}

			// JourneyPattern extraction
			const JourneyPattern* line(static_cast<const JourneyPattern*>(serviceUse.getService()->getPath()));
			const CommercialLine* commercialLine(line->getCommercialLine());
			const ContinuousService* continuousService(dynamic_cast<const ContinuousService*>(serviceUse.getService()));

			// Build of the parameters vector
			{
				stringstream s;
				s << setw(2) << setfill('0') << serviceUse.getDepartureDateTime().time_of_day().hours() << ":" << setw(2) << setfill('0') << serviceUse.getDepartureDateTime().time_of_day().minutes();
				pm.insert(DATA_FIRST_DEPARTURE_TIME, s.str()); // 0
			}
			{
				stringstream s;
				if(continuousServiceRange.total_seconds() > 0)
				{
					s << setw(2) << setfill('0') << lastDepartureDateTime.time_of_day().hours() << ":" << setw(2) << setfill('0') << lastDepartureDateTime.time_of_day().minutes();
				}
				pm.insert(DATA_LAST_DEPARTURE_TIME, s.str()); // 1
			}
			{
				stringstream s;
				s << setw(2) << setfill('0') << serviceUse.getArrivalDateTime().time_of_day().hours() << ":" << setw(2) << setfill('0') << serviceUse.getArrivalDateTime().time_of_day().minutes();
				pm.insert(DATA_FIRST_ARRIVAL_TIME, s.str()); // 2
			}
			{
				stringstream s;
				if(continuousServiceRange.total_seconds() > 0)
				{
					s << setw(2) << setfill('0') << lastArrivalDateTime.time_of_day().hours() << ":" << setw(2) << setfill('0') << lastArrivalDateTime.time_of_day().minutes();
				}
				pm.insert(DATA_LAST_ARRIVAL_TIME, s.str()); // 3
			}
			if(line->getRollingStock())
			{
				pm.insert(DATA_ROLLINGSTOCK_ID, line->getRollingStock()->getKey()); // 4
				pm.insert(DATA_ROLLINGSTOCK_NAME, line->getRollingStock()->getName()); // 5
				pm.insert(DATA_ROLLINGSTOCK_ARTICLE, line->getRollingStock()->getArticle()); // 6
			}
			pm.insert(DATA_DESTINATION_NAME, line->getDirection().empty() ? line->getDestination()->getConnectionPlace()->getFullName() : line->getDirection() ); // 7
			pm.insert(DATA_HANDICAPPED_FILTER_STATUS, handicappedFilterStatus);
			pm.insert(
				DATA_HANDICAPPED_PLACES_NUMBER,
				serviceUse.getUseRule().getAccessCapacity () ?
					lexical_cast<string>(*serviceUse.getUseRule().getAccessCapacity ()) :
					"9999"
			);
			pm.insert(DATA_BIKE_FILTER_STATUS, bikeFilterStatus);
			pm.insert(
				DATA_BIKE_PLACES_NUMBER,
				serviceUse.getUseRule().getAccessCapacity () ?
					lexical_cast<string>(*serviceUse.getUseRule().getAccessCapacity ()) :
				"9999"
			); // 11
			pm.insert(DATA_LINE_SHORT_NAME, commercialLine->getShortName() ); // 12
			pm.insert(DATA_LINE_LONG_NAME, commercialLine->getLongName() ); // 13
			if(continuousService)
			{
				pm.insert(DATA_CONTINUOUS_SERVICE_WAITING, continuousService->getMaxWaitingTime().total_seconds() / 60);
			}
			pm.insert(DATA_LINE_STYLE, commercialLine->getStyle() ); //15
			pm.insert(DATA_LINE_IMAGE, commercialLine->getImage() );
			pm.insert(DATA_LINE_ID, commercialLine->getKey()); // 17
			if(alarm)
			{
				pm.insert(DATA_ALARM_MESSAGE, alarm->getLongMessage());
				pm.insert(DATA_ALARM_LEVEL, alarm->getLevel());
			}
			pm.insert(DATA_ODD_ROW, color); // 21

			displayRequest.getFunction()->setAditionnalParametersMap(pm);
			displayRequest.run(stream);
		}
	}
}

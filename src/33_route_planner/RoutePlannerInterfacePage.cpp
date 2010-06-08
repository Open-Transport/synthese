
/** RoutePlannerInterfacePage class implementation.
	@file RoutePlannerInterfacePage.cpp

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

#include "RoutePlannerInterfacePage.h"
#include "Request.h"
#include "HourPeriod.h"
#include "Site.h"
#include "User.h"
#include "City.h"
#include "Place.h"
#include "Edge.h"
#include "AccessParameters.h"
#include "PTConstants.h"
#include "DateTimeInterfacePage.h"
#include "PTRoutePlannerResult.h"
#include "NamedPlace.h"
#include "WebPage.h"
#include "PTRoutePlannerResult.h"
#include "CommercialLine.h"
#include "Line.h"
#include "StaticFunctionRequest.h"
#include "WebPageDisplayFunction.h"
#include "Crossing.h"
#include "JourneyBoardInterfacePage.h"
#include "ReservationRuleInterfacePage.h"
#include "Service.h"
#include "LineMarkerInterfacePage.h"
#include "Road.h"

#include <sstream>

using namespace std;
using namespace boost;
using namespace boost::gregorian;
using namespace boost::posix_time;

namespace synthese
{
	using namespace pt;
	using namespace util;
	using namespace transportwebsite;
	using namespace geography;
	using namespace graph;
	using namespace ptrouteplanner;
	using namespace server;
	using namespace road;

	namespace routeplanner
	{
		const string RoutePlannerInterfacePage::DATA_SOLUTIONS_NUMBER("solutions_number");
		const string RoutePlannerInterfacePage::DATA_LINES("lines");
		const string RoutePlannerInterfacePage::DATA_SCHEDULES("schedules");
		const string RoutePlannerInterfacePage::DATA_WARNINGS("warnings");
		const string RoutePlannerInterfacePage::DATA_DURATIONS("durations");
		const string RoutePlannerInterfacePage::DATA_RESERVATIONS("reservations");
		const string RoutePlannerInterfacePage::DATA_BOARDS("boards");
		const string RoutePlannerInterfacePage::DATA_MAPS_LINES("maps_lines");
		const string RoutePlannerInterfacePage::DATA_MAPS("maps");

		const string RoutePlannerInterfacePage::DATA_INTERNAL_DATE("internal_date");
		const string RoutePlannerInterfacePage::DATA_ORIGIN_CITY_TEXT("origin_city_text");
		const string RoutePlannerInterfacePage::DATA_HANDICAPPED_FILTER("handicapped_filter");
		const string RoutePlannerInterfacePage::DATA_ORIGIN_PLACE_TEXT("origin_place_text");
		const string RoutePlannerInterfacePage::DATA_BIKE_FILTER("bike_filter");
		const string RoutePlannerInterfacePage::DATA_DESTINATION_CITY_TEXT("destination_city_text");
		const string RoutePlannerInterfacePage::DATA_DESTINATION_PLACE_ID("destination_place_id");
		const string RoutePlannerInterfacePage::DATA_DESTINATION_PLACE_TEXT("destination_place_text");
		const string RoutePlannerInterfacePage::DATA_PERIOD_ID("period_id");
		const string RoutePlannerInterfacePage::DATA_DATE("date");
		const string RoutePlannerInterfacePage::DATA_PERIOD("period");
		const string RoutePlannerInterfacePage::DATA_ACCESSIBILITY_CODE("accessibility_code");
		const string RoutePlannerInterfacePage::DATA_SITE_ID("site_id");
		const string RoutePlannerInterfacePage::DATA_IS_SAME_PLACES("is_same_places");
		const string RoutePlannerInterfacePage::DATA_USER_FULL_NAME("user_full_name");
		const string RoutePlannerInterfacePage::DATA_USER_PHONE("user_phone");
		const string RoutePlannerInterfacePage::DATA_USER_ID("user_id");

		const string RoutePlannerInterfacePage::DATA_CELLS("cells");
		const string RoutePlannerInterfacePage::DATA_IS_ODD_ROW("is_odd_row");
		const string RoutePlannerInterfacePage::DATA_IS_ORIGIN_ROW("is_origin_row");
		const string RoutePlannerInterfacePage::DATA_IS_DESTINATION_ROW("is_destination_row");
		const string RoutePlannerInterfacePage::DATA_PLACE_NAME("place_name");

		const string RoutePlannerInterfacePage::DATA_IS_FIRST_ROW("is_first_row");
		const string RoutePlannerInterfacePage::DATA_IS_LAST_ROW("is_last_row");
		const string RoutePlannerInterfacePage::DATA_COLUMN_NUMBER("column_number");
		const string RoutePlannerInterfacePage::DATA_IS_FOOT("is_foot");
		const string RoutePlannerInterfacePage::DATA_FIRST_TIME("first_time");
		const string RoutePlannerInterfacePage::DATA_LAST_TIME("last_time");
		const string RoutePlannerInterfacePage::DATA_IS_CONTINUOUS_SERVICE("is_continuous_service");
		const string RoutePlannerInterfacePage::DATA_IS_FIRST_WRITING("is_first_writing");
		const string RoutePlannerInterfacePage::DATA_IS_LAST_WRITING("is_last_writing");
		const string RoutePlannerInterfacePage::DATA_IS_FIRST_FOOT("is_first_foot");

		const string RoutePlannerInterfacePage::DATA_CONTENT("content");

		void RoutePlannerInterfacePage::Display(
			std::ostream& stream,
			shared_ptr<const WebPage> page,
			shared_ptr<const WebPage> schedulesRowPage,
			shared_ptr<const WebPage> schedulesCellPage,
			shared_ptr<const WebPage> linesRowPage,
			shared_ptr<const WebPage> lineMarkerPage,
			shared_ptr<const WebPage> boardPage,
			shared_ptr<const WebPage> warningPage,
			shared_ptr<const WebPage> reservationPage,
			shared_ptr<const WebPage> durationPage,
			shared_ptr<const WebPage> textDurationPage,
			shared_ptr<const WebPage> mapPage,
			shared_ptr<const WebPage> mapLinePage,
			shared_ptr<const WebPage> dateTimePage,
			boost::shared_ptr<const transportwebsite::WebPage> stopCellPage,
			boost::shared_ptr<const transportwebsite::WebPage> serviceCellPage,
			boost::shared_ptr<const transportwebsite::WebPage> junctionPage,
			boost::shared_ptr<const transportwebsite::WebPage> mapStopCellPage,
			boost::shared_ptr<const transportwebsite::WebPage> mapServiceCellPage,
			boost::shared_ptr<const transportwebsite::WebPage> mapJunctionPage,
			const server::Request& request,
			const ptrouteplanner::PTRoutePlannerResult& object,
			const boost::gregorian::date& date,
			size_t periodId,
			const geography::Place* originPlace,
			const geography::Place* destinationPlace,
			const transportwebsite::HourPeriod* period,
			const graph::AccessParameters& accessParameters
		){
			StaticFunctionRequest<WebPageDisplayFunction> displayRequest(request, false);
			displayRequest.getFunction()->setPage(page);
			displayRequest.getFunction()->setUseTemplate(false);
			ParametersMap pm;

			const City* originCity(dynamic_cast<const City*>(originPlace));
			string originPlaceName;
			if (originCity == NULL)
			{
				originCity = dynamic_cast<const NamedPlace*>(originPlace)->getCity();
				originPlaceName = dynamic_cast<const NamedPlace*>(originPlace)->getName();
			}
			const City* destinationCity(dynamic_cast<const City*>(destinationPlace));
			string destinationPlaceName;
			if (destinationCity == NULL)
			{
				destinationCity = dynamic_cast<const NamedPlace*>(destinationPlace)->getCity();
				destinationPlaceName = dynamic_cast<const NamedPlace*>(destinationPlace)->getName();
			}
			
			pm.insert(DATA_INTERNAL_DATE, to_iso_extended_string(date));
			pm.insert(DATA_ORIGIN_CITY_TEXT, originCity->getName());
			pm.insert(DATA_HANDICAPPED_FILTER, accessParameters.getUserClass() == USER_HANDICAPPED);
			pm.insert(DATA_ORIGIN_PLACE_TEXT, originPlaceName);
			pm.insert(DATA_BIKE_FILTER, accessParameters.getUserClass() == USER_BIKE);
			pm.insert(DATA_DESTINATION_CITY_TEXT, destinationCity->getName());
			//pm.insert("" /*lexical_cast<string>(destinationPlace->getKey())*/);
			pm.insert(DATA_DESTINATION_PLACE_TEXT, destinationPlaceName);
			pm.insert(DATA_PERIOD_ID, periodId);

			// Text formatted date
			if(dateTimePage.get())
			{
				stringstream sDate;
				DateTimeInterfacePage::Display(sDate, dateTimePage, request, date);
				pm.insert(DATA_DATE, sDate.str());
			}

			if(period)
			{
				pm.insert(DATA_PERIOD, period->getCaption());
			}
			pm.insert(DATA_SOLUTIONS_NUMBER, object.getJourneys().size());
			pm.insert(DATA_ACCESSIBILITY_CODE, static_cast<int>(accessParameters.getUserClass()));
			pm.insert(DATA_SITE_ID, page->getRoot()->getKey());
			pm.insert(DATA_IS_SAME_PLACES, object.getSamePlaces());
			if(request.getUser().get())
			{
				pm.insert(DATA_USER_FULL_NAME, request.getUser()->getFullName());
				pm.insert(DATA_USER_PHONE, request.getUser()->getPhone());
				pm.insert(DATA_USER_ID, request.getUser()->getKey());
			}



			// Schedule rows
			if(schedulesRowPage.get() && schedulesCellPage.get())
			{
				stringstream rows;
				const PTRoutePlannerResult::PlacesList& placesList(
					object.getOrderedPlaces()
				);
				typedef vector<shared_ptr<ostringstream> > PlacesContentVector;
				PlacesContentVector sheetRows(placesList.size());
				BOOST_FOREACH(PlacesContentVector::value_type& stream, sheetRows)
				{
					stream.reset(new ostringstream);
				}

				// Cells

				// Loop on each journey
				int i=1;
				for(PTRoutePlannerResult::Journeys::const_iterator it(object.getJourneys().begin());
					it != object.getJourneys().end();
					++it, ++i
				){
					bool pedestrianMode = false;
					bool lastPedestrianMode = false;

					PlacesContentVector::iterator itSheetRow(sheetRows.begin());
					PTRoutePlannerResult::PlacesList::const_iterator itPlaces(placesList.begin());

					// Loop on each leg
					const Journey::ServiceUses& jl(it->getServiceUses());
					for (Journey::ServiceUses::const_iterator itl(jl.begin()); itl != jl.end(); ++itl)
					{
						const ServiceUse& curET(*itl);

						if(	itl == jl.begin() ||
							!curET.getEdge()->getParentPath()->isPedestrianMode() ||
							lastPedestrianMode != curET.getEdge()->getParentPath()->isPedestrianMode()
						){
							const NamedPlace* placeToSearch(
								(	itl == jl.begin() &&
									dynamic_cast<const Crossing*>(curET.getDepartureEdge()->getHub())
								) ?
								dynamic_cast<const NamedPlace*>(object.getDeparturePlace()) :
								dynamic_cast<const NamedPlace*>(curET.getDepartureEdge()->getHub())
							);
							assert(placeToSearch != NULL);

							ptime lastDateTime(curET.getDepartureDateTime());
							lastDateTime += it->getContinuousServiceRange();

							for (; itPlaces != placesList.end() && itPlaces->place != placeToSearch; ++itPlaces, ++itSheetRow)
							{
								DisplayScheduleCell(
									**itSheetRow,
									schedulesCellPage,
									request,
									itPlaces == object.getOrderedPlaces().begin(),
									(itl + 1) == jl.end(),
									i,
									pedestrianMode,
									time_duration(not_a_date_time),
									time_duration(not_a_date_time),
									false,
									true,
									true,
									false
								);
							}

							pedestrianMode = curET.getEdge()->getParentPath()->isPedestrianMode();

							// Saving of the columns on each lines
							DisplayScheduleCell(
								**itSheetRow,
								schedulesCellPage,
								request,
								itPlaces == object.getOrderedPlaces().begin()
								, true
								, i
								, pedestrianMode
								, curET.getDepartureDateTime().time_of_day()
								, lastDateTime.time_of_day()
								, it->getContinuousServiceRange().total_seconds() > 0
								, itl == jl.begin()
								, true
								, pedestrianMode && !lastPedestrianMode
							);
							++itPlaces; ++itSheetRow;
							lastPedestrianMode = pedestrianMode;
						}

						if(	itl == jl.end()-1
							||	!(itl+1)->getEdge()->getParentPath()->isPedestrianMode()
							||	!curET.getEdge()->getParentPath()->isPedestrianMode()
						){
							const NamedPlace* placeToSearch(
								itl == jl.end()-1 && dynamic_cast<const Crossing*>(curET.getArrivalEdge()->getHub()) ?
								dynamic_cast<const NamedPlace*>(object.getArrivalPlace()) :
								dynamic_cast<const NamedPlace*>(curET.getArrivalEdge()->getHub())
							);
							assert(placeToSearch != NULL);

							for (; itPlaces != placesList.end() && itPlaces->place != placeToSearch; ++itPlaces, ++itSheetRow )
							{
								DisplayScheduleCell(
									**itSheetRow,
									schedulesCellPage,
									request,
									true
									, true
									, i
									, pedestrianMode
									, time_duration(not_a_date_time)
									, time_duration(not_a_date_time)
									, false
									, true
									, true
									, false
								);
							}

							ptime lastDateTime(curET.getArrivalDateTime());
							lastDateTime += it->getContinuousServiceRange();

							DisplayScheduleCell(
								**itSheetRow,
								schedulesCellPage,
								request,
								true
								, (itl + 1) == jl.end()
								, i
								, pedestrianMode
								, curET.getArrivalDateTime().time_of_day()
								, lastDateTime.time_of_day()
								, it->getContinuousServiceRange().total_seconds() > 0
								, true
								, (itl + 1) == jl.end()
								, false
							);
						}
					}

					for (++itPlaces, ++itSheetRow; itPlaces != placesList.end(); ++itPlaces, ++itSheetRow)
					{
						RoutePlannerInterfacePage::DisplayScheduleCell(
							**itSheetRow,
							schedulesCellPage,
							request,
							true
							, true
							, i
							, false
							, time_duration(not_a_date_time)
							, time_duration(not_a_date_time)
							, false
							, true
							, true
							, false
						);
					}
				}

				// Initialization of text lines
				bool color(false);
				PlacesContentVector::const_iterator it(sheetRows.begin());
				BOOST_FOREACH(const PTRoutePlannerResult::PlacesList::value_type& pi, placesList)
				{
					assert(dynamic_cast<const NamedPlace*>(pi.place));

					DisplayRow(
						rows,
						schedulesRowPage,
						request,
						*dynamic_cast<const NamedPlace*>(pi.place),
						(*it)->str(),
						color,
						pi.isOrigin,
						pi.isDestination
					);
					color = !color;
					++it;
				}

				pm.insert(DATA_SCHEDULES, rows.str());
			}

			// Lines row
			if(linesRowPage.get())
			{
				stringstream linesRow;
				size_t n = 1;
				BOOST_FOREACH(const PTRoutePlannerResult::Journeys::value_type& journey, object.getJourneys())
				{
					DisplayLinesCell(
						linesRow,
						linesRowPage,
						lineMarkerPage,
						request,
						n,
						journey
					);
					++n;
				}
				pm.insert(DATA_LINES, linesRow.str());
			}


			// Boards
			if(boardPage.get())
			{
				stringstream boards;

				logic::tribool hFilter(
					false
					//Conversion::ToTribool(_handicappedFilter->getValue(parameters, variables, object, request))
				);
				logic::tribool bFilter(
					false
					//Conversion::ToTribool(_bikeFilter->getValue(parameters, variables, object, request))
				);

				size_t i=1;
				for(PTRoutePlannerResult::Journeys::const_iterator it(object.getJourneys().begin());
					it != object.getJourneys().end();
					++it, ++i
				){
					JourneyBoardInterfacePage::Display(
						boards,
						boardPage,
						textDurationPage,
						dateTimePage,
						stopCellPage,
						serviceCellPage,
						junctionPage,
						request,
						i,
						*it,
						*object.getDeparturePlace(),
						*object.getArrivalPlace(),
						hFilter,
						bFilter,
						it+1 != object.getJourneys().end()
					);
				}

				pm.insert(DATA_BOARDS, boards.str());
			}

			// Warnings row
			if(warningPage.get())
			{
				stringstream warnings;

				/// @todo warnings

				pm.insert(DATA_WARNINGS, warnings.str());
			}


			// Durations row
			if(durationPage.get())
			{
				stringstream durations;
				BOOST_FOREACH(const PTRoutePlannerResult::Journeys::value_type& journey, object.getJourneys())
				{
					DateTimeInterfacePage::Display(
						durations,
						durationPage,
						request,
						journey.getDuration()
					);
				}
				pm.insert(DATA_DURATIONS, durations.str());
			}


			// Reservations row
			if(reservationPage.get())
			{
				stringstream reservations;
				BOOST_FOREACH(PTRoutePlannerResult::Journeys::value_type journey, object.getJourneys())
				{
					ReservationRuleInterfacePage::Display(
						reservations,
						reservationPage,
						dateTimePage,
						request,
						journey
					);
				}
				pm.insert(DATA_RESERVATIONS, reservations.str());
			}


			// Maps lines
			if(mapLinePage.get())
			{
				stringstream mapsLines;

				// Selection of the lines to display
				set<const CommercialLine*> lines;
				BOOST_FOREACH(const PTRoutePlannerResult::Journeys::value_type& journey, object.getJourneys())
				{
					BOOST_FOREACH(const ServiceUse& service, journey.getServiceUses())
					{
						if(dynamic_cast<const Line*>(service.getService()->getPath()))
						{
							lines.insert(static_cast<const Line*>(service.getService()->getPath())->getCommercialLine());
						}
					}
				}

				// Display of each line
				BOOST_FOREACH(const CommercialLine* line, lines)
				{
					LineMarkerInterfacePage::Display(
						mapsLines,
						mapLinePage,
						request,
						*line
					);
				}
				pm.insert(DATA_MAPS_LINES, mapsLines.str());
			}


			// Maps
			if(mapPage.get())
			{
				stringstream maps;

				logic::tribool hFilter(
					false
					//Conversion::ToTribool(_handicappedFilter->getValue(parameters, variables, object, request))
				);
				logic::tribool bFilter(
					false
					//Conversion::ToTribool(_bikeFilter->getValue(parameters, variables, object, request))
				);

				size_t i=1;
				for(PTRoutePlannerResult::Journeys::const_iterator it(object.getJourneys().begin());
					it != object.getJourneys().end();
					++it, ++i
				){
					JourneyBoardInterfacePage::Display(
						maps,
						mapPage,
						textDurationPage,
						dateTimePage,
						mapStopCellPage,
						mapServiceCellPage,
						mapJunctionPage,
						request,
						i,
						*it,
						*object.getDeparturePlace(),
						*object.getArrivalPlace(),
						hFilter,
						bFilter,
						it+1 != object.getJourneys().end()
					);
				}

				pm.insert(DATA_MAPS, maps.str());
			}

			displayRequest.getFunction()->setAditionnalParametersMap(pm);
			displayRequest.run(stream);
		}



		void RoutePlannerInterfacePage::DisplayScheduleCell(
			std::ostream& stream,
			boost::shared_ptr<const transportwebsite::WebPage> page,
			const server::Request& request,
			bool isItFirstRow,
			bool isItLastRow,
			size_t columnNumber,
			bool isItFootLine,
			const boost::posix_time::time_duration& firstTime,
			const boost::posix_time::time_duration& lastTime,
			bool isItContinuousService,
			bool isFirstWriting,
			bool isLastWriting,
			bool isFirstFoot
		){
			StaticFunctionRequest<WebPageDisplayFunction> displayRequest(request, false);
			displayRequest.getFunction()->setPage(page);
			displayRequest.getFunction()->setUseTemplate(false);
			ParametersMap pm;
			pm.insert(DATA_IS_FIRST_ROW, isItFirstRow);
			pm.insert(DATA_IS_LAST_ROW, isItLastRow);
			pm.insert(DATA_COLUMN_NUMBER, columnNumber);
			pm.insert(DATA_IS_FOOT, isItFootLine);
			{
				stringstream s;
				if(!firstTime.is_not_a_date_time())
				{
					s << setfill('0') << setw(2) << firstTime.hours() << ":" << setfill('0') << setw(2) << firstTime.minutes();
				}
				pm.insert(DATA_FIRST_TIME, s.str());
			}{
				stringstream s;
				if(!lastTime.is_not_a_date_time())
				{
					s << setfill('0') << setw(2) << lastTime.hours() << ":" << setfill('0') << setw(2) << lastTime.minutes();
				}
				pm.insert(DATA_LAST_TIME, s.str());
			}
			pm.insert(DATA_IS_CONTINUOUS_SERVICE, isItContinuousService);
			pm.insert(DATA_IS_FIRST_WRITING, isFirstWriting);
			pm.insert(DATA_IS_LAST_WRITING, isLastWriting);
			pm.insert(DATA_IS_FIRST_FOOT, isFirstFoot);
			displayRequest.getFunction()->setAditionnalParametersMap(pm);
			displayRequest.run(stream);
		}



		void RoutePlannerInterfacePage::DisplayLinesCell(
			std::ostream& stream,
			boost::shared_ptr<const transportwebsite::WebPage> page,
			boost::shared_ptr<const transportwebsite::WebPage> lineMarkerPage,
			const server::Request& request,
			std::size_t columnNumber,
			const graph::Journey& journey
		){
			StaticFunctionRequest<WebPageDisplayFunction> displayRequest(request, false);
			displayRequest.getFunction()->setPage(page);
			displayRequest.getFunction()->setUseTemplate(false);
			ParametersMap pm;

			pm.insert(DATA_COLUMN_NUMBER, columnNumber);

			// Content
			if(lineMarkerPage.get())
			{
				stringstream content;
				bool __AfficherLignesPied = false;
				//	Conversion::ToBool(
				//	_displayPedestrianLines->getValue(parameters, variables, object, request)
				//	);

				BOOST_FOREACH(const ServiceUse& leg, journey.getServiceUses())
				{
					if ( __AfficherLignesPied || !dynamic_cast<const Road*> (leg.getService()->getPath ()) )
					{
						LineMarkerInterfacePage::Display(
							content,
							lineMarkerPage,
							request,
							*static_cast<const Line*>(leg.getService()->getPath ())->getCommercialLine()
						);
					}
				}

				pm.insert(DATA_CONTENT, content.str());
			}

			displayRequest.getFunction()->setAditionnalParametersMap(pm);
			displayRequest.run(stream);
		}



		void RoutePlannerInterfacePage::DisplayRow( std::ostream& stream, boost::shared_ptr<const transportwebsite::WebPage> page, const server::Request& request, const geography::NamedPlace& place, const std::string& cells, bool alternateColor, bool isOrigin, bool isDestination )
		{
			StaticFunctionRequest<WebPageDisplayFunction> displayRequest(request, false);
			displayRequest.getFunction()->setPage(page);
			displayRequest.getFunction()->setUseTemplate(false);
			ParametersMap pm;
			pm.insert(Request::PARAMETER_OBJECT_ID, place.getKey());
			pm.insert(DATA_CELLS, cells);
			pm.insert(DATA_IS_DESTINATION_ROW, isDestination);
			pm.insert(DATA_IS_ODD_ROW, alternateColor);
			pm.insert(DATA_IS_ORIGIN_ROW, isOrigin);
			pm.insert(DATA_PLACE_NAME, place.getFullName());
			displayRequest.getFunction()->setAditionnalParametersMap(pm);
			displayRequest.run(stream);
		}
	}
}


/** PTRoutePlannerResult class implementation.
	@file PTRoutePlannerResult.cpp

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>

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

#include "PTRoutePlannerResult.h"

#include "Journey.h"
#include "LinePhysicalStop.hpp"
#include "RoadPath.hpp"
#include "Edge.h"
#include "Crossing.h"
#include "ResultHTMLTable.h"
#include "LineStop.h"
#include "Road.h"
#include "City.h"
#include "StopArea.hpp"
#include "CommercialLine.h"
#include "RoadPlace.h"
#include "JourneyPattern.hpp"
#include "Service.h"
#include "Junction.hpp"
#include "JunctionStop.hpp"
#include "MessagesModule.h"
#include "SentScenario.h"
#include "Alarm.h"

using namespace std;
using namespace boost;
using namespace boost::posix_time;

namespace synthese
{
	using namespace graph;
	using namespace road;
	using namespace geography;
	using namespace algorithm;
	using namespace html;
	using namespace pt;
	using namespace messages;

	namespace pt_journey_planner
	{
		PTRoutePlannerResult::PTRoutePlannerResult(
			const geography::Place* departurePlace,
			const geography::Place* arrivalPlace,
			bool samePlaces,
			const TimeSlotRoutePlanner::Result& journeys
		):	_departurePlace(departurePlace),
			_arrivalPlace(arrivalPlace),
			_samePlaces(samePlaces),
			_filtered(false),
			_journeys(journeys)
		{
			_createOrderedPlaces();
		}



		void PTRoutePlannerResult::operator=( const PTRoutePlannerResult& other )
		{
			assert(_departurePlace == other._departurePlace);
			assert(_arrivalPlace == other._arrivalPlace);
			_journeys = other._journeys;
			_orderedPlaces = other._orderedPlaces;
			_samePlaces = other._samePlaces;
			_filtered = other._filtered;
		}



		void PTRoutePlannerResult::displayHTMLTable(
			ostream& stream,
			optional<HTMLForm&> resaForm,
			const string& resaRadioFieldName,
			bool ignoreReservationDeadline
		) const {

			if(_journeys.empty())
			{
				stream << "Aucun résultat trouvé de " << (
						dynamic_cast<const NamedPlace*>(_departurePlace) ?
						dynamic_cast<const NamedPlace*>(_departurePlace)->getFullName() :
						dynamic_cast<const City*>(_departurePlace)->getName()
					) << " à " << (
						dynamic_cast<const NamedPlace*>(_arrivalPlace) ?
						dynamic_cast<const NamedPlace*>(_arrivalPlace)->getFullName() :
						dynamic_cast<const City*>(_arrivalPlace)->getName()
					);
				return;
			}

			HTMLTable::ColsVector v;
			v.push_back("Départ<br />" + (
					dynamic_cast<const NamedPlace*>(_departurePlace) ?
					dynamic_cast<const NamedPlace*>(_departurePlace)->getFullName() :
					dynamic_cast<const City*>(_departurePlace)->getName()
			)	);
			v.push_back("Ligne");
			v.push_back("Arrivée");
			v.push_back("Correspondance");
			v.push_back("Départ");
			v.push_back("Ligne");
			v.push_back("Arrivée<br />" + (
					dynamic_cast<const NamedPlace*>(_arrivalPlace) ?
					dynamic_cast<const NamedPlace*>(_arrivalPlace)->getFullName() :
					dynamic_cast<const City*>(_arrivalPlace)->getName()
			)	);
			HTMLTable t(v, ResultHTMLTable::CSS_CLASS);

			// Solutions display loop
			int solution(1);
			ptime now(second_clock::local_time());
			stream << t.open();
			bool firstReservableFound(false);
			for (PTRoutePlannerResult::Journeys::const_iterator it(_journeys.begin()); it != _journeys.end(); ++it)
			{
				stream << t.row();
				stream << t.col(7, string(), true);
				if(	resaForm &&
					it->getReservationCompliance(ignoreReservationDeadline) &&
					(ignoreReservationDeadline || it->getReservationDeadLine() > now)
				){
					string resaTime(
						to_simple_string(it->getFirstDepartureTime().date()) +" "+to_simple_string(it->getFirstDepartureTime().time_of_day())
					);
					stream <<
						resaForm->getRadioInput(
							resaRadioFieldName,
							optional<string>(resaTime),
							firstReservableFound ? optional<string>() : resaTime,
							"Solution "+ lexical_cast<string>(solution)
						);
					firstReservableFound = true;
				}
				else
					stream << "Solution " << solution;
				++solution;

				// Departure time
				Journey::ServiceUses::const_iterator its(it->getServiceUses().begin());

				if (it->getContinuousServiceRange().total_seconds() > 60)
				{
					ptime endRange(its->getDepartureDateTime());
					endRange += it->getContinuousServiceRange();
					stream << " - Service continu jusqu'à " << endRange;
				}
				if (it->getReservationCompliance(ignoreReservationDeadline) != false)
				{
					stream << " - ";

					if(it->getReservationCompliance(ignoreReservationDeadline) == true)
					{
						stream << HTMLModule::getHTMLImage("/admin/img/resa_compulsory.png", "Réservation obligatoire") << " Réservation obligatoire";
					}
					else
					{
						stream << HTMLModule::getHTMLImage("/admin/img/resa_optional.png", "Réservation facultative") << " Réservation facultative";
					}

					const ptime deadline(it->getReservationDeadLine());
					stream << " avant le " <<
						deadline.date().day() << "/" << deadline.date().month() << "/" << deadline.date().year() << " à " <<
						deadline.time_of_day().hours() << ":" << deadline.time_of_day().minutes()
					;
				}
				if(dynamic_cast<const City*>(_departurePlace) || dynamic_cast<const City*>(_arrivalPlace))
				{
					stream << " (";
					if(dynamic_cast<const City*>(_departurePlace) && dynamic_cast<const StopArea*>(its->getDepartureEdge()->getHub()))
					{
						stream << "départ de " <<
							static_cast<const StopArea*>(
								its->getDepartureEdge()->getHub()
							)->getFullName()
						;
					}
					if(dynamic_cast<const City*>(_arrivalPlace) && dynamic_cast<const StopArea*>((it->getServiceUses().end() - 1)->getArrivalEdge()->getHub()))
					{
						if(dynamic_cast<const City*>(_departurePlace)) stream << " - ";
						Journey::ServiceUses::const_iterator ite(it->getServiceUses().end() - 1);
						stream << "arrivée à " <<
							static_cast<const StopArea*>(
								ite->getArrivalEdge()->getHub()
							)->getFullName()
						;
					}
					stream << ")";
				}


				stream << t.row();
				stream << t.col() << "<b>" << its->getDepartureDateTime() << "</b>";

				// JourneyPattern
				const LinePhysicalStop* ls(dynamic_cast<const LinePhysicalStop*>(its->getDepartureEdge()));
				const JunctionStop* js(dynamic_cast<const JunctionStop*>(its->getDepartureEdge()));
				stream << t.col(1, ls ? ls->getJourneyPattern()->getCommercialLine()->getStyle() : string());
				if(ls)
				{
					stream << ls->getJourneyPattern()->getCommercialLine()->getShortName();
					if(!ls->getJourneyPattern()->getDirection().empty())
					{
						stream << " / " << ls->getJourneyPattern()->getDirection();
					}
				}
				else if(js)
				{
					stream << "JONCTION";
				}
				else
				{
					const Road* road(static_cast<const RoadPath*>(its->getService()->getPath())->getRoad());
					stream << road->getAnyRoadPlace()->getName();
				}

				// Transfers
				if (its == it->getServiceUses().end() -1)
				{
					stream << t.col(4) << "(trajet direct)";
				}
				else
				{
					while(true)
					{
						// Arrival
						stream << t.col() << its->getArrivalDateTime();

						// Place
						stream << t.col();
						if(dynamic_cast<const StopArea*>(its->getArrivalEdge()->getHub()))
						{
							stream <<
								static_cast<const StopArea*>(
									its->getArrivalEdge()->getHub()
								)->getFullName();
						}

						// Next service use
						++its;

						// Departure
						stream << t.col() << its->getDepartureDateTime();

						// JourneyPattern
						const JourneyPattern* ls(dynamic_cast<const JourneyPattern*>(its->getService()->getPath()));
						const Junction* js(dynamic_cast<const Junction*>(its->getService()->getPath()));
						stream << t.col(1, ls ? ls->getCommercialLine()->getStyle() : string());
						if(ls)
						{
							stream << ls->getCommercialLine()->getShortName();
							if(!ls->getDirection().empty())
							{
								stream << " / " << ls->getDirection();
							}
						}
						else if(js)
						{
							stream << "JONCTION";
						}
						else
						{
							const Road* road(static_cast<const RoadPath*>(its->getService()->getPath())->getRoad());
							stream << road->getAnyRoadPlace()->getName();
						}

						// Exit if last service use
						if (its == it->getServiceUses().end() -1)
							break;

						// Empty final arrival col
						stream << t.col();

						// New row and empty origin departure cols;
						stream << t.row();
						stream << t.col();
						stream << t.col();
					}
				}

				// Final arrival
				stream << t.col() << "<b>" << its->getArrivalDateTime() << "</b>";
			}
			stream << t.close();

		}



		void PTRoutePlannerResult::removeFirstJourneys( size_t value )
		{
			for(size_t i(0); i<value; ++i)
			{
				_journeys.pop_front();
			}
			_createOrderedPlaces();
		}



		const NamedPlace* PTRoutePlannerResult::getNamedPlace(const Place* place)
		{
			const NamedPlace* res = dynamic_cast<const NamedPlace*>(place);
			if(res)
				return res;

			//If NULL this is maybe a city, so we need to return mainCityPlace
			const City * city = dynamic_cast<const City*>(place);
			if(city)
			{
				const NamedPlace* mainCityPlace = NULL;

				BOOST_FOREACH(const Place* cityPlace, city->getIncludedPlaces())
				{
					mainCityPlace = dynamic_cast<const NamedPlace*>(cityPlace);
					//Stop on first place
					if(mainCityPlace)
						break;
				}

				if(mainCityPlace)
					return mainCityPlace;
			}
			return NULL;
		}



		bool PTRoutePlannerResult::HaveToDisplayDepartureStopOnGrid(
			Journey::ServiceUses::const_iterator itl,
			const Journey::ServiceUses& jl,
			bool avoidDoubles
		){
			return
				itl == jl.begin() ||
				(	!avoidDoubles &&
					(	!itl->getService()->getPath()->isPedestrianMode() ||
						!(itl-1)->getService()->getPath()->isPedestrianMode()
				)	)
			;
		}



		bool PTRoutePlannerResult::HaveToDisplayArrivalStopOnGrid(
			Journey::ServiceUses::const_iterator itl,
			const Journey::ServiceUses& jl
		){
			return
				itl+1 == jl.end() ||
				!itl->getService()->getPath()->isPedestrianMode() ||
				!(itl+1)->getService()->getPath()->isPedestrianMode()
			;
		}



		void PTRoutePlannerResult::_createOrderedPlaces()
		{
			_orderedPlaces.clear();
			for(Journeys::const_iterator itj(_journeys.begin()); itj != _journeys.end(); ++itj)
			{
				const Journey::ServiceUses& jl(itj->getServiceUses());
				PlacesListConfiguration::List jlist;

				for (Journey::ServiceUses::const_iterator itl(jl.begin()); itl != jl.end(); ++itl)
				{
					const ServicePointer& leg(*itl);

					if(	HaveToDisplayDepartureStopOnGrid(itl, jl, true)
					){
						PlacesListConfiguration::PlaceInformation item(
							GetNamedPlaceForDeparture(
								leg.getService()->getPath()->isPedestrianMode(),
								itl == jl.begin() ? NULL : &(*(itl-1)),
								leg,
								*getNamedPlace(_departurePlace)
							),
							itl == jl.begin(),
							false
						);
						jlist.push_back(item);
					}

					if(	HaveToDisplayArrivalStopOnGrid(itl, jl)
					){
						PlacesListConfiguration::PlaceInformation item(
							GetNamedPlaceForArrival(
								leg.getService()->getPath()->isPedestrianMode(),
								leg,
								itl+1 == jl.end() ? NULL : &(*(itl+1)),
								*getNamedPlace(_arrivalPlace)
							),
							false,
							itl+1 == jl.end()
						);
						jlist.push_back(item);
					}
				}

				_orderedPlaces.addList(make_pair(itj, jlist));
			}
		}



		void PTRoutePlannerResult::filterOnDurationRatio( double ratio )
		{
			// Min duration
			time_duration minDuration(not_a_date_time);
			BOOST_FOREACH(const Journeys::value_type& journey, getJourneys())
			{
				if(minDuration.is_not_a_date_time() || minDuration > journey.getDuration())
				{
					minDuration = journey.getDuration();
				}
			}

			// Max duration
			time_duration maxDuration(seconds(long(ceil(double(minDuration.total_seconds()) * ratio))));

			// Filter
			Journeys result;
			bool filteredByThisMethod(false);
			vector<Journeys::iterator> toRemove;
			BOOST_FOREACH(const Journeys::value_type& journey, getJourneys())
			{
				if(journey.getDuration() > maxDuration)
				{
					_filtered = true;
					filteredByThisMethod = true;
				}
				else
				{
					result.push_back(journey);
				}
			}
			if(filteredByThisMethod)
			{
				_journeys = result;
				_createOrderedPlaces();
			}
		}



		PTRoutePlannerResult::MaxAlarmLevels::MaxAlarmLevels():
			lineLevel(ALARM_LEVEL_NO_ALARM),
			stopLevel(ALARM_LEVEL_NO_ALARM)
		{}



		PTRoutePlannerResult::MaxAlarmLevels PTRoutePlannerResult::getMaxAlarmLevels() const
		{
			MaxAlarmLevels result;
			BOOST_FOREACH(const PTRoutePlannerResult::Journeys::value_type& journey, getJourneys())
			{
				BOOST_FOREACH(const ServicePointer& leg, journey.getServiceUses())
				{
					// Line
					const JourneyPattern* journeyPattern(
						dynamic_cast<const JourneyPattern*>(leg.getService()->getPath())
					);
					if(	journeyPattern
					){
/*						MessagesModule::MessagesByRecipientId::mapped_type messages(
							MessagesModule::GetMessages(
								journeyPattern->getCommercialLine()->getKey()
						)	);
						BOOST_FOREACH(const MessagesModule::MessagesByRecipientId::mapped_type::value_type& it, messages)
						{
							if(	it->getScenario()->isApplicable(leg.getDepartureDateTime(), leg.getArrivalDateTime()) &&
								it->getLevel() > result.lineLevel
							){
								result.lineLevel = it->getLevel();
							}
						}
*/
						// Departure stop
						const StopArea* departureStopArea(
							dynamic_cast<const StopArea*>(
								leg.getDepartureEdge()->getFromVertex()->getHub()
						)	);
						if(departureStopArea)
						{
/*							MessagesModule::MessagesByRecipientId::mapped_type messages(
								MessagesModule::GetMessages(departureStopArea->getKey())
							);
							BOOST_FOREACH(const MessagesModule::MessagesByRecipientId::mapped_type::value_type& it, messages)
							{
								if(	it->getScenario()->isApplicable(leg.getDepartureDateTime()) &&
									it->getLevel() > result.stopLevel
								){
									result.stopLevel = it->getLevel();
								}
							}
*/						}

						// Arrival stop
						const StopArea* arrivalStopArea(
							dynamic_cast<const StopArea*>(
								leg.getArrivalEdge()->getFromVertex()->getHub()
						)	);
						if(arrivalStopArea)
						{
/*							MessagesModule::MessagesByRecipientId::mapped_type messages(
								MessagesModule::GetMessages(arrivalStopArea->getKey())
							);
							BOOST_FOREACH(const MessagesModule::MessagesByRecipientId::mapped_type::value_type& it, messages)
							{
								if(	it->getScenario()->isApplicable(leg.getArrivalDateTime()) &&
									it->getLevel() > result.stopLevel
								){
									result.stopLevel = it->getLevel();
								}
							}
*/						}
					}
			}	}

			return result;
		}



		void PTRoutePlannerResult::filterOnWaitingTime(
			boost::posix_time::time_duration minWaitingTime
		){
			// Nothing to filter
			if(_journeys.empty())
			{
				return;
			}

			// Declarations
			Journeys result;
			bool filteredByThisMethod(false);

			// Filter
			Journeys::const_iterator itJourney(getJourneys().begin());
			while(
				itJourney != getJourneys().end()
			){
				bool forDeparture(itJourney+1 != getJourneys().end() && (itJourney+1)->getFirstDepartureTime() - itJourney->getFirstDepartureTime() < minWaitingTime);
				bool forArrival(itJourney+1 != getJourneys().end() && (itJourney+1)->getFirstArrivalTime() - itJourney->getFirstArrivalTime() < minWaitingTime);

				Journeys::const_iterator bestJourney(itJourney);
				Journeys::const_iterator itJourney2(itJourney+1);

				for(;
					(	(forDeparture || forArrival) &&
						itJourney2 != getJourneys().end() &&
						!itJourney->getContinuousServiceRange().total_seconds() &&
						!itJourney2->getContinuousServiceRange().total_seconds() &&
						(	(forDeparture && itJourney2->getFirstDepartureTime() - itJourney->getFirstDepartureTime() < minWaitingTime) ||
							(forArrival && itJourney2->getFirstArrivalTime() - itJourney->getFirstArrivalTime() < minWaitingTime)
						)
					);
					++itJourney2
				){
					filteredByThisMethod = true;
					if(itJourney2->getDuration() < bestJourney->getDuration())
					{
						bestJourney = itJourney2;
					}

					if(forDeparture)
					{
						forDeparture = (itJourney2->getFirstDepartureTime() - itJourney->getFirstDepartureTime() < minWaitingTime);
					}

					if(forArrival)
					{
						forArrival = (itJourney2->getFirstArrivalTime() - itJourney->getFirstArrivalTime() < minWaitingTime);
					}
				}

				result.push_back(*bestJourney);
				itJourney = itJourney2;
			}


			if(filteredByThisMethod)
			{
				_filtered = true;
				_journeys = result;
				_createOrderedPlaces();
			}
		}



		const geography::NamedPlace* PTRoutePlannerResult::GetNamedPlaceForDeparture(
			bool isPedestrian,
			const graph::ServicePointer* arrivalLeg,
			const graph::ServicePointer& departureLeg,
			const geography::NamedPlace& defaultValue
		){
			const NamedPlace* placeToSearch(NULL);
			// Case Pedestrian :
			if(isPedestrian)
			{
				// If first element : use of the origin place
				if(arrivalLeg)
				{ // Use of the last arrival place
					placeToSearch = dynamic_cast<const NamedPlace*>(arrivalLeg->getArrivalEdge()->getHub());
				}
			}
			else // Use of the PT departure place
			{
				placeToSearch = dynamic_cast<const NamedPlace*>(departureLeg.getDepartureEdge()->getHub());
			}
			if(!placeToSearch)
			{	// Case of crossing with city as departure place
				// Todo see how the right place can be found
				placeToSearch = &defaultValue;
			}
			return placeToSearch;
		}



		const geography::NamedPlace* PTRoutePlannerResult::GetNamedPlaceForArrival(
			bool isPedestrian,
			const graph::ServicePointer& arrivalLeg,
			const graph::ServicePointer* departureLeg,
			const geography::NamedPlace& defaultValue
		){
			const NamedPlace* placeToSearch(NULL);
			// Case Pedestrian :
			if(isPedestrian)
			{
				// If last element : use of the destination place
				if(departureLeg)
				{ // Use of the next departure place
					placeToSearch = dynamic_cast<const NamedPlace*>(departureLeg->getDepartureEdge()->getHub());
				}
			}
			else // Use of the PT arrival place
			{
				placeToSearch = dynamic_cast<const NamedPlace*>(arrivalLeg.getArrivalEdge()->getHub());
			}
			if(!placeToSearch)
			{	// Case of crossing with city as arrival place
				// Todo see how the right place can be found
				placeToSearch = &defaultValue;
			}
			return placeToSearch;
		}
}	}

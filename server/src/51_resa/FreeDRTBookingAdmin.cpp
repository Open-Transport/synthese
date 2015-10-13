
//////////////////////////////////////////////////////////////////////////
/// FreeDRTBookingAdmin class implementation.
///	@file FreeDRTBookingAdmin.cpp
///	@author Hugues Romain
///	@date 2012
///
///	This file belongs to the SYNTHESE project (public transportation specialized software)
///	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>
///
///	This program is free software; you can redistribute it and/or
///	modify it under the terms of the GNU General Public License
///	as published by the Free Software Foundation; either version 2
///	of the License, or (at your option) any later version.
///
///	This program is distributed in the hope that it will be useful,
///	but WITHOUT ANY WARRANTY; without even the implied warranty of
///	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
///	GNU General Public License for more details.
///
///	You should have received a copy of the GNU General Public License
///	along with this program; if not, write to the Free Software
///	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#include "FreeDRTBookingAdmin.hpp"

#include "AdminActionFunctionRequest.hpp"
#include "AdminFunctionRequest.hpp"
#include "AdminParametersException.h"
#include "BookableCommercialLineAdmin.h"
#include "BookReservationAction.h"
#include "FreeDRTArea.hpp"
#include "FreeDRTTimeSlot.hpp"
#include "Language.hpp"
#include "ParametersMap.h"
#include "PlacesListService.hpp"
#include "Profile.h"
#include "ResaModule.h"
#include "ResaRight.h"
#include "ReservationRoutePlannerAdmin.h"
#include "ResultHTMLTable.h"
#include "RoutePlannerFunction.h"
#include "SearchFormHTMLTable.h"
#include "User.h"

using namespace std;
using namespace boost;
using namespace boost::posix_time;
using namespace boost::gregorian;

namespace synthese
{
	using namespace admin;
	using namespace geography;
	using namespace graph;
	using namespace html;
	using namespace server;
	using namespace util;
	using namespace security;
	using namespace resa;
	using namespace pt;
	using namespace pt_journey_planner;
	using namespace pt_website;

	namespace util
	{
		template<> const string FactorableTemplate<AdminInterfaceElement, FreeDRTBookingAdmin>::FACTORY_KEY = "FreeDRTBooking";
	}

	namespace admin
	{
		template<> const string AdminInterfaceElementTemplate<FreeDRTBookingAdmin>::ICON = "resa_compulsory.png";
		template<> const string AdminInterfaceElementTemplate<FreeDRTBookingAdmin>::DEFAULT_TITLE = "Saisie de réservation";
	}

	namespace resa
	{
		const string FreeDRTBookingAdmin::PARAMETER_AREA_ID = "area_id";
		const string FreeDRTBookingAdmin::PARAMETER_ARRIVAL_PLACE = "arrival_place";
		const string FreeDRTBookingAdmin::PARAMETER_DATE = "date";
		const string FreeDRTBookingAdmin::PARAMETER_DEPARTURE_PLACE = "departure_place";
		const string FreeDRTBookingAdmin::PARAMETER_TIME = "time";



		FreeDRTBookingAdmin::FreeDRTBookingAdmin(
		):	AdminInterfaceElementTemplate<FreeDRTBookingAdmin>(),
			_dateTime(second_clock::local_time())
		{}



		void FreeDRTBookingAdmin::setFromParametersMap(
			const ParametersMap& map
		){
			// Area
			try
			{
				_area = Env::GetOfficialEnv().get<FreeDRTArea>(
					map.get<RegistryKeyType>(PARAMETER_AREA_ID)
				);
			}
			catch (ObjectNotFoundException<FreeDRTTimeSlot>&)
			{
				throw AdminParametersException("No such free DRT time slot");
			}

			// Departure place
			PlacesListService placesListService;
			placesListService.setNumber(1);
			placesListService.setText(map.getDefault<string>(PARAMETER_DEPARTURE_PLACE));
			placesListService.setCoordinatesSystem(&CoordinatesSystem::GetInstanceCoordinatesSystem());
			_departurePlace = dynamic_pointer_cast<NamedPlace, Place>(
				placesListService.getPlaceFromBestResult(
					placesListService.runWithoutOutput()
				).value
			);

			// Arrival place
			placesListService.setText(map.getDefault<string>(PARAMETER_ARRIVAL_PLACE));
			_arrivalPlace = dynamic_pointer_cast<NamedPlace, Place>(
				placesListService.getPlaceFromBestResult(
					placesListService.runWithoutOutput()
				).value
			);

			// Date and time
			_dateTime = ptime(
				from_string(map.get<string>(PARAMETER_DATE)),
				map.getDefault<string>(PARAMETER_TIME).empty() ?
				minutes(0) :
				duration_from_string(map.get<string>(PARAMETER_TIME))
			);

				setBaseReservationFromParametersMap(*_env, map);
		}



		ParametersMap FreeDRTBookingAdmin::getParametersMap() const
		{
			ParametersMap m;

			m.merge(getBaseReservationParametersMap());

			// Area
			if(_area.get())
			{
				m.insert(PARAMETER_AREA_ID, _area->getKey());
			}

			// Departure place
			if(_departurePlace.get())
			{
				if(dynamic_cast<const NamedPlace*>(_departurePlace.get()))
				{
					m.insert(
						PARAMETER_DEPARTURE_PLACE,
						dynamic_cast<const NamedPlace*>(_departurePlace.get())->getFullName()
					);
				}
				else if(dynamic_cast<const City*>(_departurePlace.get()))
				{
					m.insert(
						PARAMETER_DEPARTURE_PLACE,
						dynamic_cast<const City*>(_departurePlace.get())->getName()
					);
				}
			}

			// Arrival place
			if(_arrivalPlace.get())
			{
				if(dynamic_cast<const NamedPlace*>(_arrivalPlace.get()))
				{
					m.insert(
						PARAMETER_ARRIVAL_PLACE,
						dynamic_cast<const NamedPlace*>(_arrivalPlace.get())->getFullName()
					);
				}
				else if(dynamic_cast<const City*>(_arrivalPlace.get()))
				{
					m.insert(
						PARAMETER_ARRIVAL_PLACE,
						dynamic_cast<const City*>(_arrivalPlace.get())->getName()
					);
				}
			}

			// Date time
			if(!_dateTime.is_not_a_date_time())
			{
				m.insert(PARAMETER_DATE, _dateTime.date());
				m.insert(PARAMETER_TIME, _dateTime.time_of_day());
			}

			return m;
		}



		bool FreeDRTBookingAdmin::isAuthorized(
			const security::User& user
		) const	{
			return user.getProfile()->isAuthorized<ResaRight>(READ, UNKNOWN_RIGHT_LEVEL);
		}



		void FreeDRTBookingAdmin::display(
			ostream& stream,
			const server::Request& request
		) const	{

			// Query form
			AdminFunctionRequest<FreeDRTBookingAdmin> queryRequest(request, *this);
			SearchFormHTMLTable t(queryRequest.getHTMLForm("query"));
			stream << t.open();
			stream << t.cell(
				"Départ",
				t.getForm().getTextInput(
					PARAMETER_DEPARTURE_PLACE,
					_departurePlace.get() ?
						(	dynamic_cast<const NamedPlace*>(_departurePlace.get()) ?
							dynamic_cast<const NamedPlace*>(_departurePlace.get())->getFullName() :
							dynamic_cast<const City*>(_departurePlace.get())->getName()
						): string()
				)	)
			;
			stream << t.cell(
				"Arrivée",
				t.getForm().getTextInput(
				PARAMETER_ARRIVAL_PLACE,
				_arrivalPlace.get() ?
					(	dynamic_cast<const NamedPlace*>(_arrivalPlace.get()) ?
						dynamic_cast<const NamedPlace*>(_arrivalPlace.get())->getFullName() :
						dynamic_cast<const City*>(_arrivalPlace.get())->getName()
					): string()
			)	);
			stream << t.row();
			const synthese::Language& language(
				request.getUser()->getLanguage() ?
					*request.getUser()->getLanguage() :
					synthese::Language::GetLanguageFromIso639_1Code("fr")
			);
			vector<pair<optional<string>, string> > dates;
			vector<pair<optional<string>, string> > times;
			{
				date date(day_clock::local_day());
				for(size_t i=0; i<14; ++i)
				{
					dates.push_back(
						make_pair(
							to_iso_extended_string(date),
							language.getWeekDayName(date.day_of_week()) +" "+ to_simple_string(date)
					)	);
					date += days(1);
				}
				for(size_t i=0; i<24; ++i)
				{
					times.push_back(
						make_pair(
							lexical_cast<string>(i) +":00",
							lexical_cast<string>(i) +":00"
					)	);
				}
			}

			stream << t.cell(
				"Date",
				t.getForm().getSelectInput(
					PARAMETER_DATE,
					dates,
					optional<string>(to_iso_extended_string(_dateTime.date()))
			)	);
			stream << t.cell(
				"Heure",
				t.getForm().getSelectInput(
					PARAMETER_TIME,
					times,
					optional<string>(lexical_cast<string>(_dateTime.time_of_day().hours())+":00")
			)	);
			stream << t.close();

			// Result
			if(	_departurePlace.get() &&
				_departurePlace->getPoint().get() &&
				_arrivalPlace.get() &&
				_arrivalPlace->getPoint().get()
			){
				/// TODO check if the places belong to the area

				// Declarations
				typedef map<
					ptime,
					ptime
				> Results;
				Results results;

				// Begin and end times
				ptime beginTime(_dateTime);
				beginTime -= hours(3);
				ptime endTime(_dateTime);
				endTime += hours(3);

				// The form
				AdminActionFunctionRequest<BookReservationAction, FreeDRTBookingAdmin> bookRequest(request, *this);
				bookRequest.setActionWillCreateObject();
				bookRequest.getAction()->getJourneyPlanner().setDeparturePlace(
					const_pointer_cast<Place, const Place>(
						_departurePlace
				)	);
				bookRequest.getAction()->getJourneyPlanner().setArrivalPlace(
					const_pointer_cast<Place, const Place>(
						_arrivalPlace)
				)	;
				HTMLForm f(bookRequest.getHTMLForm("book"));
				stream << f.open();
				f.addHiddenField(BookReservationAction::PARAMETER_SERVICE_ID, string());

				// Time slots
				BOOST_FOREACH(const Service* itServ, _area->getAllServices())
				{
					// Declarations
					const FreeDRTTimeSlot& timeSlot(static_cast<const FreeDRTTimeSlot&>(*itServ));

					// Calendar check
					if(!timeSlot.isActive(_dateTime.date()))
					{
						continue;
					}

					// Title
					stream << "<h1>Service " << timeSlot.getServiceNumber() << " (" <<
						timeSlot.getFirstDeparture() << " à " << timeSlot.getLastArrival() <<
						")</h1>"
					;

					// Best journey time at min speed
					double dst(
						_arrivalPlace->getPoint()->distance(
							_departurePlace->getPoint().get()
					)	);
					time_duration bestCommercialJourneyTime(
						minutes(long(0.06 * dst / timeSlot.getCommercialSpeed()))
					);


					// Time bounds
					ptime lowerBound(_dateTime.date(), timeSlot.getFirstDeparture());
					if(lowerBound < beginTime)
					{
						lowerBound = beginTime;
					}
					ptime upperBound(_dateTime.date(), timeSlot.getLastArrival());
					upperBound -= bestCommercialJourneyTime;
					if(upperBound > endTime)
					{
						upperBound = endTime;
					}

					// Reservations
					/// TODO

					// Time loop
					results.clear();
					for(ptime curTime(lowerBound);
						curTime <= upperBound;
						curTime = curTime + minutes(5)
					){
						// Reservations check
						/// TODO

						// Storage
						results.insert(
							make_pair(
								curTime,
								curTime + bestCommercialJourneyTime
						)	);
					}

					// The table
					HTMLTable::ColsVector c;
					c.push_back(string());
					c.push_back("Départ");
					c.push_back("Arrivée");
					c.push_back("Durée");
					HTMLTable t(c, ResultHTMLTable::CSS_CLASS);
					stream << t.open();
					BOOST_FOREACH(const Results::value_type& result, results)
					{
						// New row
						stream << t.row();

						// Form
						stream << t.col() <<
							f.getRadioInput(
								Action_PARAMETER_PREFIX + RoutePlannerFunction::PARAMETER_LOWEST_DEPARTURE_TIME,
								optional<string>(
									to_iso_extended_string(result.first.date()) + " " +
										to_simple_string(result.first.time_of_day())
								),
								optional<string>(),
								string(),
								false,
								"document.getElementById('" + f.getFieldId(BookReservationAction::PARAMETER_SERVICE_ID) + "').value='" + lexical_cast<string>(timeSlot.getKey()) + "';"
							);

						// Departure time
						stream << t.col() << to_simple_string(result.first.time_of_day());

						// Arrival time
						stream << t.col() << to_simple_string(result.second.time_of_day());

						// Duration
						stream << t.col() << to_simple_string(result.second - result.first);
					}
					stream << t.close();
				}
				stream <<
					f.setFocus(
						Action_PARAMETER_PREFIX + RoutePlannerFunction::PARAMETER_LOWEST_DEPARTURE_TIME,
						0
					)
				;

				// Reservation form
				displayReservationForm(
					stream,
					f,
					request
				);

				// Form is closed too
				stream << f.close();
			}
		}



		FreeDRTBookingAdmin::PageLinks FreeDRTBookingAdmin::_getCurrentTreeBranch() const
		{
			boost::shared_ptr<BookableCommercialLineAdmin> p(
				getNewPage<BookableCommercialLineAdmin>()
			);
			p->setCommercialLine(Env::GetOfficialEnv().getSPtr(_area->getLine()));

			PageLinks links(p->_getCurrentTreeBranch());
			links.push_back(getNewCopiedPage());
			return links;

		}
}	}


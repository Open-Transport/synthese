
/** BookReservationAction class implementation.
	@file BookReservationAction.cpp

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

#include "BookReservationAction.h"

#include "AlgorithmLogger.hpp"
#include "City.h"
#include "ClientException.h"
#include "CommercialLine.h"
#include "FreeDRTArea.hpp"
#include "FreeDRTTimeSlot.hpp"
#include "FreeDRTTimeSlotTableSync.hpp"
#include "GeographyModule.h"
#include "House.hpp"
#include "Journey.h"
#include "JourneyPattern.hpp"
#include "JourneysResult.h"
#include "Language.hpp"
#include "LineStop.h"
#include "NamedPlace.h"
#include "OnlineReservationRule.h"
#include "Place.h"
#include "PlacesListService.hpp"
#include "Profile.h"
#include "PTRoutePlannerResult.h"
#include "PTServiceConfig.hpp"
#include "PTUseRule.h"
#include "Request.h"
#include "RequestException.h"
#include "ResaDBLog.h"
#include "ResaModule.h"
#include "ResaRight.h"
#include "ResaTypes.h"
#include "Reservation.h"
#include "ReservationTableSync.h"
#include "ReservationTransaction.h"
#include "ReservationTransactionTableSync.h"
#include "Road.h"
#include "RoadPath.hpp"
#include "RoadPlace.h"
#include "RoutePlannerFunction.h"
#include "ScheduledService.h"
#include "ScheduledServiceTableSync.h"
#include "Session.h"
#include "StopArea.hpp"
#include "UseRule.h"
#include "User.h"
#include "UserTableSync.h"
#include "Vehicle.hpp"
#include "VehiclePosition.hpp"

#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

using namespace std;
using namespace boost;
using namespace boost::posix_time;
using namespace boost::gregorian;

namespace synthese
{
	using namespace algorithm;
	using namespace server;
	using namespace pt_journey_planner;
	using namespace security;
	using namespace pt;
	using namespace pt_website;
	using namespace util;
	using namespace graph;
	using namespace road;
	using namespace geography;

	namespace util
	{
		template<> const string FactorableTemplate<Action, resa::BookReservationAction>::FACTORY_KEY = "bra";
	}

	namespace resa
	{
		const string BookReservationAction::PARAMETER_CREATE_CUSTOMER = Action_PARAMETER_PREFIX + "cc";
		const string BookReservationAction::PARAMETER_COMMENT = Action_PARAMETER_PREFIX + "comment";
		const string BookReservationAction::PARAMETER_CUSTOMER_ID = Action_PARAMETER_PREFIX + "cuid";
		const string BookReservationAction::PARAMETER_SEARCH_CUSTOMER_BY_EXACT_NAME = Action_PARAMETER_PREFIX + "csn";
		const string BookReservationAction::PARAMETER_CUSTOMER_NAME = Action_PARAMETER_PREFIX + "cuna";
		const string BookReservationAction::PARAMETER_CUSTOMER_SURNAME = Action_PARAMETER_PREFIX + "cs";
		const string BookReservationAction::PARAMETER_CUSTOMER_ALLOW_EMPTY_SURNAME = Action_PARAMETER_PREFIX + "aes";
		const string BookReservationAction::PARAMETER_CUSTOMER_PHONE = Action_PARAMETER_PREFIX + "cuph";
		const string BookReservationAction::PARAMETER_CUSTOMER_EMAIL = Action_PARAMETER_PREFIX + "cupe";
		const string BookReservationAction::PARAMETER_CUSTOMER_ADDRESS = Action_PARAMETER_PREFIX + "cupa";
		const string BookReservationAction::PARAMETER_CUSTOMER_POSTCODE = Action_PARAMETER_PREFIX + "cupp";
		const string BookReservationAction::PARAMETER_CUSTOMER_CITYTEXT = Action_PARAMETER_PREFIX + "cupc";
		const string BookReservationAction::PARAMETER_CUSTOMER_COUNTRY = Action_PARAMETER_PREFIX + "cupco";
		const string BookReservationAction::PARAMETER_CUSTOMER_LANGUAGE = Action_PARAMETER_PREFIX + "cl";

		const string BookReservationAction::PARAMETER_PASSWORD = Action_PARAMETER_PREFIX + "pass";

		const string BookReservationAction::PARAMETER_SEATS_NUMBER = Action_PARAMETER_PREFIX + "senu";

		const string BookReservationAction::PARAMETER_SERVICE_ID = Action_PARAMETER_PREFIX + "se";
		const string BookReservationAction::PARAMETER_DEPARTURE_RANK = Action_PARAMETER_PREFIX + "dr";
		const string BookReservationAction::PARAMETER_ARRIVAL_RANK = Action_PARAMETER_PREFIX + "ar";

		const string BookReservationAction::PARAMETER_IGNORE_RESERVATION_RULES = Action_PARAMETER_PREFIX + "irr";
		const string BookReservationAction::PARAMETER_RESERVATION_DELAY_TYPE = Action_PARAMETER_PREFIX + "rdt";
		const string BookReservationAction::PARAMETER_APPROACH_SPEED = Action_PARAMETER_PREFIX + "apsp";

		const string BookReservationAction::PARAMETER_MULTI_RESERVATIONS_NUMBER = Action_PARAMETER_PREFIX + "mrn";
		const string BookReservationAction::PARAMETER_MULTI_RESERVATIONS_DATES = Action_PARAMETER_PREFIX + "mrd";
		const string BookReservationAction::PARAMETER_MULTI_RESERVATIONS_MODE = Action_PARAMETER_PREFIX + "mrm";


		BookReservationAction::BookReservationAction(
		):	util::FactorableTemplate<server::Action, BookReservationAction>(),
			_createCustomer(false),
			_seatsNumber(1),
			_ignoreReservation(false),
			_userClassCode(USER_PEDESTRIAN - USER_CLASS_CODE_OFFSET)
		{}



		ParametersMap BookReservationAction::getParametersMap() const
		{
			ParametersMap map;

			// Customer creation
			map.insert(PARAMETER_CREATE_CUSTOMER, _createCustomer);

			// Ignore reservation rules
			map.insert(PARAMETER_IGNORE_RESERVATION_RULES, _ignoreReservation);

			// Approach speed
			map.insert(PARAMETER_APPROACH_SPEED, _approachSpeed);

			// Journey planner
			ParametersMap jpMap(_journeyPlanner._getParametersMap());
			map.merge(jpMap, Action_PARAMETER_PREFIX);

			// Reservation by service on a free DRT
			if(_freeDRTTimeSlot.get())
			{
				map.insert(PARAMETER_SERVICE_ID, _freeDRTTimeSlot->getKey());
			}
			// Reservation by service on a virtual line
			else if(_service.get() && _journey.size() == 1)
			{
				map.insert(PARAMETER_SERVICE_ID, _service->getKey());
				const ServicePointer& su(*_journey.getServiceUses().begin());
				map.insert(PARAMETER_DEPARTURE_RANK, su.getDepartureEdge()->getRankInPath());
				map.insert(PARAMETER_ARRIVAL_RANK, su.getArrivalEdge()->getRankInPath());
			}

			// Seats number
			map.insert(PARAMETER_SEATS_NUMBER, _seatsNumber);

			return map;
		}



		void BookReservationAction::_setFromParametersMap(const ParametersMap& map)
		{
			_createCustomer = map.getDefault<bool>(PARAMETER_CREATE_CUSTOMER, false);
			_ignoreReservation = map.getDefault<bool>(PARAMETER_IGNORE_RESERVATION_RULES, false);
			_approachSpeed = map.getDefault<double>(PARAMETER_APPROACH_SPEED, 1.111);
			_comment = map.getDefault<string>(PARAMETER_COMMENT);

			// Reservation Rules Delay type
			if(map.getDefault<int>(PARAMETER_RESERVATION_DELAY_TYPE, 0))
			{
				_reservationRulesDelayType = UseRule::RESERVATION_EXTERNAL_DELAY;
			}
			else
			{
				_reservationRulesDelayType = UseRule::RESERVATION_INTERNAL_DELAY;
			}

			if(map.getDefault<bool>(PARAMETER_SEARCH_CUSTOMER_BY_EXACT_NAME, false))
			{
				UserTableSync::SearchResult customers(UserTableSync::Search(*_env, optional<string>(), map.get<string>(PARAMETER_CUSTOMER_NAME)));
				if(!customers.empty())
				{
					_customer = *customers.begin();
				}
			}

			if(!_customer.get())
			{
				if (_createCustomer)
				{
					_customer.reset(new User);
					_customer->setName(map.get<string>(PARAMETER_CUSTOMER_NAME));
					if (_customer->getName().empty())
					{
						throw RequestException("Le nom du client doit être rempli");
					}

					_customer->setSurname(map.getDefault<string>(PARAMETER_CUSTOMER_SURNAME));
					if (_customer->getSurname().empty() && !map.getDefault<bool>(PARAMETER_CUSTOMER_ALLOW_EMPTY_SURNAME, false))
					{
						throw RequestException("Le prénom du client doit être rempli");
					}

					_customer->setPhone(map.get<string>(PARAMETER_CUSTOMER_PHONE));
					if (_customer->getPhone().empty())
					{
						throw RequestException("Le numéro de téléphone doit être rempli");
					}

					// Integrity test : the key is name + surname + phone
					Env env;
					UserTableSync::Search(
						env,
						optional<string>(),
						_customer->getName(),
						_customer->getSurname(),
						_customer->getPhone(),
						optional<RegistryKeyType>(),
						logic::indeterminate,
						logic::indeterminate,
						optional<RegistryKeyType>(),
						0, 1
					);
					if (!env.getRegistry<User>().empty())
					{
						throw RequestException("Un utilisateur avec les mêmes nom, prénom, téléphone existe déjà.");
					}

					_customer->setEMail(map.getDefault<string>(PARAMETER_CUSTOMER_EMAIL));
					_customer->setAddress(map.getDefault<string>(PARAMETER_CUSTOMER_ADDRESS));
					_customer->setPostCode(map.getDefault<string>(PARAMETER_CUSTOMER_POSTCODE));
					_customer->setCityText(map.getDefault<string>(PARAMETER_CUSTOMER_CITYTEXT));
					_customer->setCountry(map.getDefault<string>(PARAMETER_CUSTOMER_COUNTRY));
					_customer->setProfile(ResaModule::GetBasicResaCustomerProfile().get());

					if(map.getOptional<string>(PARAMETER_CUSTOMER_LANGUAGE))
					{
						try
						{
							_customer->setLanguage(&Language::GetLanguageFromIso639_2Code(map.get<string>(PARAMETER_CUSTOMER_LANGUAGE)));
						}
						catch(Language::LanguageNotFoundException&)
						{
							throw RequestException("Langue incorrecte");
						}
					}
				}
				else
				{
					// Customer ID
					optional<RegistryKeyType> id(map.get<RegistryKeyType>(PARAMETER_CUSTOMER_ID));
					if (id)
					{
						_customer = UserTableSync::GetEditable(*id, *_env);
					}
				}
			}

			if(!_customer.get())
			{
				throw RequestException("Undefined customer.");
			}

			// Deduce naming fields from the customer if already recognized
			if (_customer->getName().empty())
			{
				throw RequestException("Client sans nom. Réservation impossible");
			}

			if (_customer->getPhone().empty())
			{
				throw RequestException("Client sans numéro de téléphone. Veuillez renseigner ce champ dans la fiche client et recommencer la réservation.");
			}

			// Seats number
			_seatsNumber = map.getDefault<size_t>(PARAMETER_SEATS_NUMBER, 1);
			/// TODO check seats number in UseRule ?
			if (_seatsNumber < 1 || _seatsNumber > 499)
			{
				throw RequestException("Invalid seats number");
			}

			// Journey planner initialization
			ParametersMap jpMap(map.getExtract(Action_PARAMETER_PREFIX));
			_journeyPlanner._setFromParametersMap(jpMap);
			_journeyPlanner.setMaxSolutions(1);
			_journeyPlanner.setCoordinatesSystem(&CoordinatesSystem::GetInstanceCoordinatesSystem());
			_journeyPlanner.setReservationRulesDelayType(_reservationRulesDelayType);

			// Extraction of parameters from journey planner for other methods
			_departurePlace = _journeyPlanner.getDeparturePlace().placeResult.value;
			_arrivalPlace = _journeyPlanner.getArrivalPlace().placeResult.value;
			_departureDateTime = _journeyPlanner.getStartDepartureDate();
			_userClassCode = _journeyPlanner.getAccessParameters().getUserClassRank();

			// Check extra reservations number and dates
			_reservationsNumber = map.getDefault<size_t>(PARAMETER_MULTI_RESERVATIONS_NUMBER, 1);

			if (_reservationsNumber > 6)
			{
				throw RequestException("Invalid reservations number (too high)");
			}

			_multiReservationsMode = map.getDefault<bool>(PARAMETER_MULTI_RESERVATIONS_MODE, false);

			if (_reservationsNumber > 1 && !_multiReservationsMode)
				_multiReservationsMode = true;

			_reservationsDates.push_back(_departureDateTime);
			std::string tempDates = map.getDefault<string>(PARAMETER_MULTI_RESERVATIONS_DATES, string());

			// If we have extra reservation dates for same service/hours, save them
			if (!tempDates.empty() && _reservationsNumber > 1)
			{
				std::vector<std::string> dates;
				boost::split(dates, tempDates, boost::is_any_of(","));

				BOOST_FOREACH(const std::string& date, dates)
				{
					try {
						_reservationsDates.push_back(time_from_string(date));
						if (_reservationsDates.front() > _reservationsDates.back() + days(5))
						{
							throw Exception(string());
						}
					}
					catch (Exception e)
					{
						throw RequestException("Reservations dates are malformed here : " + date);
					}
				}
			}
			else if (!tempDates.empty() && _reservationsNumber == 1)
			{
				throw RequestException("The reservations number doesn't correspond to the reservations dates");
			}

			// Reservation on a service
			if(map.getOptional<RegistryKeyType>(PARAMETER_SERVICE_ID))
			{
				RegistryKeyType serviceId(map.get<RegistryKeyType>(PARAMETER_SERVICE_ID));

				// Scheduled service (virtual line)
				if(decodeTableId(serviceId) == ScheduledServiceTableSync::TABLE.ID)
				{
					try
					{
						_service = Env::GetOfficialEnv().get<ScheduledService>(serviceId);
					}
					catch(ObjectNotFoundException<ScheduledService>&)
					{
						throw RequestException("No such service");
					}

					gregorian::date date = from_string(map.get<string>(
						Action_PARAMETER_PREFIX + pt_journey_planner::RoutePlannerFunction::PARAMETER_LOWEST_DEPARTURE_TIME
					));

					size_t departureRank(map.get<size_t>(PARAMETER_DEPARTURE_RANK));
					size_t arrivalRank(map.get<size_t>(PARAMETER_ARRIVAL_RANK));
					if(departureRank >= _service->getPath()->getEdges().size())
					{
						throw RequestException("Invalid departure rank");
					}
					if(	arrivalRank >= _service->getPath()->getEdges().size() ||
						arrivalRank <= departureRank
					){
						throw RequestException("Invalid arrival rank");
					}

					ServicePointer sp(
						true,
						false,
						_userClassCode,
						*_service,
						date,
						*_service->getPath()->getEdge(departureRank),
						*_service->getPath()->getEdge(arrivalRank)
					);

					_journey.append(sp);
				}
				// Free DRT
				else if(decodeTableId(serviceId) == FreeDRTTimeSlotTableSync::TABLE.ID)
				{
					// Service
					try
					{
						_freeDRTTimeSlot = Env::GetOfficialEnv().get<FreeDRTTimeSlot>(serviceId);
					}
					catch(ObjectNotFoundException<FreeDRTTimeSlot>&)
					{
						throw RequestException("No such service");
					}

					/// TODO check if the user class code is allowed on this service


					/// TODO check if the origin belong to the area

					// Location check
					if(	!_departurePlace.get() ||
						!_departurePlace->getPoint().get()
					){
						throw RequestException("The origin place must be located");
					}

					/// TODO check if the destinations belong to the area

					// Location check
					if(	!_arrivalPlace.get() ||
						!_arrivalPlace->getPoint().get()
					){
						throw RequestException("The destination place must be located");
					}

					/// TODO check the availability according to existing reservations

					// Arrival time
					/// TODO compute the travel time according to existing reservations and roads
					double dst(
						_departurePlace->getPoint()->distance(
							_arrivalPlace->getPoint().get()
					)	);
					time_duration bestCommercialJourneyTime(
						minutes(long(0.06 * dst / _freeDRTTimeSlot->getCommercialSpeed()))
					);
					_arrivalDateTime = _departureDateTime + bestCommercialJourneyTime;
				}
			}
			// Journey planning
			else
			{
				if(!_departurePlace.get())
				{
					throw RequestException("Invalid destination place");
				}
				if(!_arrivalPlace.get())
				{
					throw RequestException("Invalid origin place");
				}

				// Check same services/hours journeys exist for all extra dates
				if(_reservationsDates.size() == _reservationsNumber)
				{
					bool flag_first = true;
					boost::posix_time::ptime referenceArrivalTime;
					BOOST_FOREACH(const boost::posix_time::ptime& date, _reservationsDates)
					{
						_journeyPlanner.setStartDepartureDate(date);
						_journeyPlanner.runWithoutOutput();

						if (!_journeyPlanner.getResult().get() || _journeyPlanner.getResult()->getJourneys().empty())
						{
							throw RequestException("The route planning does not find a journey to book");
						}

						_journey = _journeyPlanner.getResult()->getJourneys().front();
						_journeys.push_back(_journey);

						if (flag_first)
						{
							referenceArrivalTime = _journeyPlanner.getResult()->getJourneys().front().getLastArrivalTime();
							flag_first = false;
						}
						else
						{
							if (referenceArrivalTime.time_of_day() !=
									_journeyPlanner.getResult()->getJourneys().front().getLastArrivalTime().time_of_day()
							){
								throw RequestException("The route planning has found a different journey for multiple reservations");
							}
						}
					}
				}
				else
				{
					throw RequestException("The reservations number doesn't correspond to the reservations dates");
				}
			}
		}



		void BookReservationAction::run(Request& request)
		{
			// Save customer if necessary
			if (_createCustomer)
			{
				UserTableSync::Save(_customer.get());

				ResaDBLog::AddCustomerCreationEntry(*request.getSession(), *_customer);
			}

			/* Before creating new reservation, some checks are done here:
			 * For every customer who want to create a new reservation
			 * check the seat number is correct
			 * check the same reservation wasn't alerady sent
			 */
			if ( _customer && request.getSession()->hasProfile() &&
				_customer->getKey() == request.getSession()->getUser()->getKey() &&
				request.getSession()->getUser()->getProfile()->isAuthorized<ResaRight>(UNKNOWN_RIGHT_LEVEL, WRITE) &&
				!request.getSession()->getUser()->getProfile()->isAuthorized<ResaRight>(WRITE))
			{
				ptime minTime(second_clock::local_time());
				minTime -= seconds(10);
				ptime now(second_clock::local_time());

				ReservationTransactionTableSync::SearchResult check(ReservationTransactionTableSync::SearchByUser(
					*_env, _customer->getKey(), minTime, now, false));

				if (check.size() > 0)
					throw ClientException("Reservation already sent");

				if (ResaModule::GetMaxSeats() > 0 && _seatsNumber > ResaModule::GetMaxSeats())
				{
					throw ClientException("Maximum number of seats reached");
				}

				/* Check customer haven't sent a reservation on same service before registering the new one */
				if(!_journeys.empty())
				{
					BOOST_FOREACH(const graph::Journey& journey, _journeys)
					{
						for(Journey::ServiceUses::const_iterator itSu(journey.getServiceUses().begin());
							itSu != journey.getServiceUses().end();
							++itSu
						){
							const ServicePointer& su(*itSu);

							assert(su.getService() != NULL);
							assert(su.getDepartureEdge() != NULL);
							assert(su.getDepartureEdge()->getHub() != NULL);
							assert(su.getArrivalEdge() != NULL);
							assert(su.getArrivalEdge()->getHub() != NULL);

							ReservationTableSync::SearchResult reservations(ReservationTableSync::SearchByService(
								Env::GetOfficialEnv(), su.getService()->getKey(),
								boost::optional<boost::posix_time::ptime>(su.getDepartureDateTime()),
								boost::optional<boost::posix_time::ptime>(su.getArrivalDateTime()), UP_LINKS_LOAD_LEVEL));

							BOOST_FOREACH(ReservationTableSync::SearchResult::value_type& reservation, reservations)
							{
								if (reservation->get<IsReservationPossible>())
								{
									User* customer = UserTableSync::GetEditable(
										reservation->get<Transaction>()->get<Customer>() ? reservation->get<Transaction>()->get<Customer>()->getKey() : util::RegistryKeyType(0),
										Env::GetOfficialEnv()).get();

									if ((reservation->get<ServiceId>() == su.getService()->getKey()) &&
										(customer->getKey() == _customer->getKey()) &&
										(reservation->get<DepartureTime>() == su.getDepartureDateTime() &&
										reservation->get<ArrivalTime>() == su.getArrivalDateTime())
									){
										const ReservationStatus& status(reservation->getStatus());

										if (status != CANCELLED && status != CANCELLATION_TO_ACK &&
											status != CANCELLED_AFTER_DELAY && status != ACKNOWLEDGED_CANCELLED_AFTER_DELAY &&
											status != DONE && status != NO_RESERVATION
										){
											throw ClientException("Reservation on this service already sent.");
										}
									}
								}
							}
						}
					}
				}
			}

			std::vector<ReservationTransaction> transactions;
			for (unsigned int i=0; i < _reservationsNumber; i++)
			{
				// New ReservationTransaction
				ReservationTransaction rt;
				rt.setKey(ReservationTransactionTableSync::getId());
				rt.set<BookingTime>(second_clock::local_time());
				rt.set<BookingUserId>(request.getUser()->getKey());
				rt.set<CustomerName>(_customer->getName() + " " + _customer->getSurname());
				rt.set<CustomerPhone>(_customer->getPhone());
				rt.set<CustomerEmail>(_customer->getEMail());
				rt.set<Seats>(_seatsNumber);
				rt.set<Comment>(_comment);
				boost::shared_ptr<User> customer = UserTableSync::GetEditable(_customer->getKey(), *_env, UP_LINKS_LOAD_LEVEL);
				rt.set<Customer>(*customer);
				ReservationTransactionTableSync::Save(&rt);
				transactions.push_back(rt);
			}

			// Contact center
			const OnlineReservationRule* reservationContact(NULL);

			// Journey mode
			if(!_journeys.empty())
			{
				int transaction_number = 0;
				BOOST_FOREACH(const graph::Journey& journey, _journeys)
				{
					// New reservation for each journey leg
					for(Journey::ServiceUses::const_iterator itSu(journey.getServiceUses().begin());
						itSu != journey.getServiceUses().end();
						++itSu
					){
						const ServicePointer& su(*itSu);

						assert(su.getService() != NULL);
						assert(su.getDepartureEdge() != NULL);
						assert(su.getDepartureEdge()->getHub() != NULL);
						assert(su.getArrivalEdge() != NULL);
						assert(su.getArrivalEdge()->getHub() != NULL);

						boost::shared_ptr<Reservation> r(new Reservation);
						r->setKey(ReservationTableSync::getId());
						_env->getEditableRegistry<Reservation>().add(r);

						if(dynamic_cast<const Registrable*>(su.getDepartureEdge()->getHub()))
						{
							r->set<DeparturePlaceId>(
								dynamic_cast<const Registrable*>(su.getDepartureEdge()->getHub())->getKey()
							);
						}
						else if(
							itSu == _journey.getServiceUses().begin()
						){
							if(dynamic_cast<const Registrable*>(_departurePlace.get()))
							{
								r->set<DeparturePlaceId>(
									dynamic_cast<const Registrable*>(_departurePlace.get())->getKey()
								);
							}
							else if(dynamic_cast<const House*>(_departurePlace.get()))
							{
								r->set<DeparturePlaceId>(
									dynamic_cast<const House*>(_departurePlace.get())->getRoadChunk()->getRoad()->get<RoadPlace>()->getKey()
								);
							}

						}
						if(dynamic_cast<const NamedPlace*>(su.getDepartureEdge()->getHub()))
						{
							r->set<DepartureCityName>(
								dynamic_cast<const NamedPlace*>(su.getDepartureEdge()->getHub())->getCity()->getName()
							);
							r->set<DeparturePlaceNameNoCity>(
								dynamic_cast<const NamedPlace*>(su.getDepartureEdge()->getHub())->getName()
							);
							r->set<DeparturePlaceName>(
								dynamic_cast<const NamedPlace*>(su.getDepartureEdge()->getHub())->getFullName()
							);
						}
						else if (dynamic_cast<const RoadPlace*>(su.getService()->getPath()->getPathGroup()))
						{
							r->set<DepartureCityName>(
								dynamic_cast<const RoadPlace*>(su.getService()->getPath()->getPathGroup())->getCity()->getName()
							);
							r->set<DeparturePlaceNameNoCity>(
								dynamic_cast<const RoadPlace*>(su.getService()->getPath()->getPathGroup())->getName()
							);
							r->set<DeparturePlaceName>(
								dynamic_cast<const RoadPlace*>(su.getService()->getPath()->getPathGroup())->getFullName()
							);
						}
						r->set<DepartureTime>(su.getDepartureDateTime());
						r->set<OriginDateTime>(su.getOriginDateTime());
						if(dynamic_cast<const Registrable*>(su.getArrivalEdge()->getHub()))
						{
							r->set<ArrivalPlaceId>(
								dynamic_cast<const Registrable*>(su.getArrivalEdge()->getHub())->getKey()
							);
						}
						else if(
							itSu + 1 == _journey.getServiceUses().end()
						){
							if(dynamic_cast<const Registrable*>(_arrivalPlace.get()))
							{
								r->set<DeparturePlaceId>(
									dynamic_cast<const Registrable*>(_arrivalPlace.get())->getKey()
								);
							}
							else if(dynamic_cast<const House*>(_arrivalPlace.get()))
							{
								r->set<DeparturePlaceId>(
									dynamic_cast<const House*>(_arrivalPlace.get())->getRoadChunk()->getRoad()->get<RoadPlace>()->getKey()
								);
							}
						}
						if(dynamic_cast<const NamedPlace*>(su.getArrivalEdge()->getHub()))
						{
							r->set<ArrivalCityName>(
								dynamic_cast<const NamedPlace*>(
									su.getArrivalEdge()->getHub()
								)->getCity()->getName()
							);
							r->set<ArrivalPlaceNameNoCity>(
								dynamic_cast<const NamedPlace*>(
									su.getArrivalEdge()->getHub()
								)->getName()
							);
							r->set<ArrivalPlaceName>(
								dynamic_cast<const NamedPlace*>(
									su.getArrivalEdge()->getHub()
								)->getFullName()
							);
						}
						else if (dynamic_cast<const RoadPlace*>(su.getService()->getPath()->getPathGroup()))
						{
							r->set<ArrivalCityName>(
								dynamic_cast<const RoadPlace*>(su.getService()->getPath()->getPathGroup())->getCity()->getName()
							);
							r->set<ArrivalPlaceNameNoCity>(
								dynamic_cast<const RoadPlace*>(su.getService()->getPath()->getPathGroup())->getName()
							);
							r->set<ArrivalPlaceName>(
								dynamic_cast<const RoadPlace*>(su.getService()->getPath()->getPathGroup())->getFullName()
							);
						}
						r->setArrivalTime(su.getArrivalDateTime());

						const JourneyPattern* line(dynamic_cast<const JourneyPattern*>(su.getService()->getPath()));
						if (line)
						{
							assert(line->getCommercialLine() != NULL);

							r->set<LineCode>(line->getCommercialLine()->getShortName());
							r->set<LineId>(line->getCommercialLine()->getKey());
						}
						const RoadPath* road(dynamic_cast<const RoadPath*>(su.getService()->getPath()));
						if (road)
						{
							r->set<LineCode>(road->getRoad()->get<RoadPlace>()->getName());
							r->set<LineId>(road->getRoad()->getKey());
						}

						r->set<IsReservationPossible>(false);
						if(	UseRule::IsReservationPossible(su.getUseRule().getReservationAvailability(su, _ignoreReservation, _reservationRulesDelayType))
						){
							if(	dynamic_cast<const JourneyPattern*>(su.getService()->getPath()) &&
								static_cast<const JourneyPattern*>(su.getService()->getPath())->getCommercialLine()
							){
								const OnlineReservationRule* onlineContact(OnlineReservationRule::GetOnlineReservationRule(
										static_cast<const JourneyPattern*>(su.getService()->getPath())->getCommercialLine()->getReservationContact()
								)	);
								if(onlineContact)
								{
									reservationContact = onlineContact;
								}
							}
							if(dynamic_cast<const PTUseRule*>(&su.getUseRule()))
							{
								r->set<ReservationRuleId>(static_cast<const PTUseRule&>(su.getUseRule()).getKey());
							}
							r->set<ReservationDeadLine>(
								su.getUseRule().getReservationDeadLine(
									su.getOriginDateTime(),
									su.getDepartureDateTime(),
									_reservationRulesDelayType
							)	);
							r->set<IsReservationPossible>(true);
						}
						r->set<ServiceId>(su.getService()->getKey());
						r->set<ServiceCode>(lexical_cast<string>(su.getService()->getServiceNumber()));

						r->setTransaction(&(transactions[transaction_number]));

						ReservationTableSync::Save(r.get());
					}
					transaction_number++;
				}
			}
			else // Free DRT mode
			{
				/// TODO read including place
				if(	!dynamic_cast<const NamedPlace*>(_departurePlace.get()) ||
					!dynamic_cast<const NamedPlace*>(_arrivalPlace.get())
				){
					return;
				}

				boost::shared_ptr<Reservation> r(new Reservation);
				r->setKey(ReservationTableSync::getId());
				_env->getEditableRegistry<Reservation>().add(r);

				r->set<DeparturePlaceId>(
					dynamic_cast<const NamedPlace*>(_departurePlace.get())->getKey()
				);
				r->set<DepartureCityName>(
					dynamic_cast<const NamedPlace*>(_departurePlace.get())->getCity()->getName()
				);
				r->set<DeparturePlaceNameNoCity>(
					dynamic_cast<const NamedPlace*>(_departurePlace.get())->getName()
				);
				r->set<DeparturePlaceName>(
					dynamic_cast<const NamedPlace*>(_departurePlace.get())->getFullName()
				);
				r->set<DepartureTime>(_departureDateTime);
				r->set<OriginDateTime>(_departureDateTime);
				r->set<ArrivalPlaceId>(
					dynamic_cast<const NamedPlace*>(_arrivalPlace.get())->getKey()
				);
				r->set<ArrivalCityName>(
					dynamic_cast<const NamedPlace*>(_arrivalPlace.get())->getCity()->getName()
				);
				r->set<ArrivalPlaceNameNoCity>(
					dynamic_cast<const NamedPlace*>(_arrivalPlace.get())->getName()
				);
				r->set<ArrivalPlaceName>(
					dynamic_cast<const NamedPlace*>(_arrivalPlace.get())->getFullName()
				);
				r->set<ArrivalTime>(_arrivalDateTime);

				r->set<LineCode>(_freeDRTTimeSlot->getArea()->getLine()->getShortName());
				r->set<LineId>(_freeDRTTimeSlot->getArea()->getLine()->getKey());
				r->set<IsReservationPossible>(false);

				reservationContact = OnlineReservationRule::GetOnlineReservationRule(
					_freeDRTTimeSlot->getArea()->getLine()->getReservationContact()
				);

				if(dynamic_cast<const PTUseRule*>(&_freeDRTTimeSlot->getUseRule(_userClassCode)))
				{
					r->set<ReservationRuleId>(
						static_cast<const PTUseRule&>(
							_freeDRTTimeSlot->getUseRule(_userClassCode)
						).getKey()
					);
				}
				r->set<ReservationDeadLine>(
					_freeDRTTimeSlot->getUseRule(_userClassCode).getReservationDeadLine(
						_departureDateTime,
						_departureDateTime,
						_reservationRulesDelayType
				)	);
				r->set<ServiceId>(_freeDRTTimeSlot->getKey());
				r->set<ServiceCode>(_freeDRTTimeSlot->getServiceNumber());
				r->setTransaction(&(transactions[0]));

				ReservationTableSync::Save(r.get());
			}

			// Log
			BOOST_FOREACH(ReservationTransaction& rt, transactions)
			{
				ResaDBLog::AddBookReservationEntry(request.getSession().get(), rt);
			}

			// Mail
			if(!_ignoreReservation && !_customer->getEMail().empty() && reservationContact)
			{
				if (_multiReservationsMode)
				{
					reservationContact->sendCustomerEMail(transactions);
				}
				else
				{
					reservationContact->sendCustomerEMail(transactions.front());
				}

				ResaDBLog::AddEMailEntry(*request.getSession(), *_customer, "Récapitulatif de réservation");
			}

			// Redirect
			if(request.getActionWillCreateObject())
			{
				BOOST_FOREACH(ReservationTransaction& rt, transactions)
				{
					request.setActionCreatedId(rt.getKey());
				}
			}
		}



		bool BookReservationAction::isAuthorized(const Session* session
		) const {
			if( !session || !session->hasProfile())
			{
				return false;
			}
			return
				session->getUser()->getProfile()->isAuthorized<ResaRight>(WRITE) ||
				(!_createCustomer &&
				_customer->getKey() == session->getUser()->getKey() &&
				session->getUser()->getProfile()->isAuthorized<ResaRight>(UNKNOWN_RIGHT_LEVEL, WRITE))
			;
		}



		const geography::NamedPlace* BookReservationAction::GetPlaceFromOrigin( const graph::Journey& journey, boost::shared_ptr<Place> originPlace )
		{
			const NamedPlace* place(dynamic_cast<const NamedPlace*>(journey.getOrigin()->getHub()));
			if(place)
			{
				return place;
			}
			place = dynamic_cast<const NamedPlace*>(originPlace.get());
			if(place)
			{
				return place;
			}
			return NULL;
		}



		const geography::NamedPlace* BookReservationAction::GetPlaceFromDestination( const graph::Journey& journey, boost::shared_ptr<Place> destinationPlace )
		{
			const NamedPlace* place(dynamic_cast<const NamedPlace*>(journey.getDestination()->getHub()));
			if(place)
			{
				return place;
			}
			place = dynamic_cast<const NamedPlace*>(destinationPlace.get());
			if(place)
			{
				return place;
			}
			return NULL;
		}
}	}

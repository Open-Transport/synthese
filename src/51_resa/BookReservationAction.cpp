
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

#include "ActionException.h"
#include "AlgorithmLogger.hpp"
#include "City.h"
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
#include "ResaDBLog.h"
#include "ResaModule.h"
#include "ResaRight.h"
#include "ResaTypes.h"
#include "Reservation.h"
#include "ReservationTableSync.h"
#include "ReservationTransaction.h"
#include "ReservationTransactionTableSync.h"
#include "Road.h"
#include "RoadPlace.h"
#include "RoutePlannerFunction.h"
#include "ScheduledService.h"
#include "ScheduledServiceTableSync.h"
#include "Session.h"
#include "StopArea.hpp"
#include "UseRule.h"
#include "User.h"
#include "UserTableSync.h"

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
		const string BookReservationAction::PARAMETER_APPROACH_SPEED = Action_PARAMETER_PREFIX + "apsp";



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
						throw ActionException("Le nom du client doit être rempli");
					}

					_customer->setSurname(map.getDefault<string>(PARAMETER_CUSTOMER_SURNAME));
					if (_customer->getSurname().empty() && !map.getDefault<bool>(PARAMETER_CUSTOMER_ALLOW_EMPTY_SURNAME, false))
					{
						throw ActionException("Le prénom du client doit être rempli");
					}

					_customer->setPhone(map.get<string>(PARAMETER_CUSTOMER_PHONE));
					if (_customer->getPhone().empty())
					{
						throw ActionException("Le numéro de téléphone doit être rempli");
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
						throw ActionException("Un utilisateur avec les mêmes nom, prénom, téléphone existe déjà.");
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
							throw ActionException("Langue incorrecte");
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
				throw ActionException("Undefined customer.");
			}

			// Deduce naming fields from the customer if already recognized
			if (_customer->getName().empty())
			{
				throw ActionException("Client sans nom. Réservation impossible");
			}

			if (_customer->getPhone().empty())
			{
				throw ActionException("Client sans numéro de téléphone. Veuillez renseigner ce champ dans la fiche client et recommencer la réservation.");
			}

			// Seats number
			_seatsNumber = map.getDefault<size_t>(PARAMETER_SEATS_NUMBER, 1);
			/// TODO check seats number in UseRule ?
			if (_seatsNumber < 1 || _seatsNumber > 499)
			{
				throw ActionException("Invalid seats number");
			}

			// Journey planner initialization
			ParametersMap jpMap(map.getExtract(Action_PARAMETER_PREFIX));
			_journeyPlanner._setFromParametersMap(jpMap);
			_journeyPlanner.setMaxSolutions(1);
			_journeyPlanner.setEndDepartureDate(_journeyPlanner.getStartDepartureDate());
			_journeyPlanner.setCoordinatesSystem(&CoordinatesSystem::GetInstanceCoordinatesSystem());

			// Extraction of parameters from journey planner for other methods
			_departurePlace = _journeyPlanner.getDeparturePlace().placeResult.value;
			_arrivalPlace = _journeyPlanner.getArrivalPlace().placeResult.value;
			_departureDateTime = _journeyPlanner.getStartDepartureDate();
			_userClassCode = _journeyPlanner.getAccessParameters().getUserClassRank();

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
						throw ActionException("No such service");
					}

					gregorian::date date = from_string(map.get<string>(
						Action_PARAMETER_PREFIX + pt_journey_planner::RoutePlannerFunction::PARAMETER_LOWEST_DEPARTURE_TIME
					));

					size_t departureRank(map.get<size_t>(PARAMETER_DEPARTURE_RANK));
					size_t arrivalRank(map.get<size_t>(PARAMETER_ARRIVAL_RANK));
					if(departureRank >= _service->getPath()->getEdges().size())
					{
						throw ActionException("Invalid departure rank");
					}
					if(	arrivalRank >= _service->getPath()->getEdges().size() ||
						arrivalRank <= departureRank
					){
						throw ActionException("Invalid arrival rank");
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
						throw ActionException("No such service");
					}

					/// TODO check if the user class code is allowed on this service


					/// TODO check if the origin belong to the area

					// Location check
					if(	!_departurePlace.get() ||
						!_departurePlace->getPoint().get()
					){
						throw ActionException("The origin place must be located");
					}

					/// TODO check if the destinations belong to the area

					// Location check
					if(	!_arrivalPlace.get() ||
						!_arrivalPlace->getPoint().get()
					){
						throw ActionException("The destination place must be located");
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
					throw ActionException("Invalid destination place");
				}
				if(!_arrivalPlace.get())
				{
					throw ActionException("Invalid origin place");
				}

				_journeyPlanner.runWithoutOutput();

				if(	!_journeyPlanner.getResult().get() ||
					_journeyPlanner.getResult()->getJourneys().empty())
				{
					throw ActionException("The route planning does not find a journey to book");
				}

				_journey = _journeyPlanner.getResult()->getJourneys().front();
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

			// New ReservationTransaction
			ReservationTransaction rt;
			rt.setKey(ReservationTransactionTableSync::getId());
			rt.setBookingTime(second_clock::local_time());
			rt.setBookingUserId(request.getUser()->getKey());
			rt.setCustomerName(_customer->getName() + " " + _customer->getSurname());
			rt.setCustomerPhone(_customer->getPhone());
			rt.setCustomerEMail(_customer->getEMail());
			rt.setCustomerUserId(_customer->getKey());
			rt.setSeats(_seatsNumber);
			rt.setComment(_comment);
			rt.setCustomer(
				UserTableSync::GetEditable(_customer->getKey(), *_env, UP_LINKS_LOAD_LEVEL).get()
			);
			ReservationTransactionTableSync::Save(&rt);

			// Contact center
			const OnlineReservationRule* reservationContact(NULL);

			// Journey mode
			if(!_journey.empty())
			{
				// New reservation for each journey leg
				for(Journey::ServiceUses::const_iterator itSu(_journey.getServiceUses().begin());
					itSu != _journey.getServiceUses().end();
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
						r->setDeparturePlaceId(
							dynamic_cast<const Registrable*>(su.getDepartureEdge()->getHub())->getKey()
						);
					}
					else if(
						itSu == _journey.getServiceUses().begin()
					){
						if(dynamic_cast<const Registrable*>(_departurePlace.get()))
						{
							r->setDeparturePlaceId(
								dynamic_cast<const Registrable*>(_departurePlace.get())->getKey()
							);
						}
						else if(dynamic_cast<const House*>(_departurePlace.get()))
						{
							r->setDeparturePlaceId(
								dynamic_cast<const House*>(_departurePlace.get())->getRoadChunk()->getRoad()->getRoadPlace()->getKey()
							);
						}
					}
					if(dynamic_cast<const NamedPlace*>(su.getDepartureEdge()->getHub()))
					{
						r->setDepartureCityName(
							dynamic_cast<const NamedPlace*>(su.getDepartureEdge()->getHub())->getCity()->getName()
						);
						r->setDeparturePlaceNameNoCity(
							dynamic_cast<const NamedPlace*>(su.getDepartureEdge()->getHub())->getName()
						);
						r->setDeparturePlaceName(
							dynamic_cast<const NamedPlace*>(su.getDepartureEdge()->getHub())->getFullName()
						);
					}
					r->setDepartureTime(su.getDepartureDateTime());
					r->setOriginDateTime(su.getOriginDateTime());
					if(dynamic_cast<const Registrable*>(su.getArrivalEdge()->getHub()))
					{
						r->setArrivalPlaceId(
							dynamic_cast<const Registrable*>(su.getArrivalEdge()->getHub())->getKey()
						);
					}
					else if(
						itSu + 1 == _journey.getServiceUses().end()
					){
						if(dynamic_cast<const Registrable*>(_arrivalPlace.get()))
						{
							r->setDeparturePlaceId(
								dynamic_cast<const Registrable*>(_arrivalPlace.get())->getKey()
							);
						}
						else if(dynamic_cast<const House*>(_arrivalPlace.get()))
						{
							r->setDeparturePlaceId(
								dynamic_cast<const House*>(_arrivalPlace.get())->getRoadChunk()->getRoad()->getRoadPlace()->getKey()
							);
						}
					}
					if(dynamic_cast<const NamedPlace*>(su.getArrivalEdge()->getHub()))
					{
						r->setArrivalCityName(
							dynamic_cast<const NamedPlace*>(
								su.getArrivalEdge()->getHub()
							)->getCity()->getName()
						);
						r->setArrivalPlaceNameNoCity(
							dynamic_cast<const NamedPlace*>(
								su.getArrivalEdge()->getHub()
							)->getName()
						);
						r->setArrivalPlaceName(
							dynamic_cast<const NamedPlace*>(
								su.getArrivalEdge()->getHub()
							)->getFullName()
						);
					}
					r->setArrivalTime(su.getArrivalDateTime());

					const JourneyPattern* line(dynamic_cast<const JourneyPattern*>(su.getService()->getPath()));
					if (line)
					{
						assert(line->getCommercialLine() != NULL);

						r->setLineCode(line->getCommercialLine()->getShortName());
						r->setLineId(line->getCommercialLine()->getKey());
					}
					const Road* road(dynamic_cast<const Road*>(su.getService()->getPath()));
					if (road)
					{
						r->setLineCode(road->getRoadPlace()->getName());
						r->setLineId(road->getKey());
					}

                    r->setReservationPossible(false);
					if(	UseRule::IsReservationPossible(su.getUseRule().getReservationAvailability(su, _ignoreReservation))
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
							r->setReservationRuleId(static_cast<const PTUseRule&>(su.getUseRule()).getKey());
						}
						r->setReservationDeadLine(
							su.getUseRule().getReservationDeadLine(
								su.getOriginDateTime(),
								su.getDepartureDateTime()
						)	);
                        r->setReservationPossible(true);
					}
					r->setServiceId(su.getService()->getKey());
					r->setServiceCode(lexical_cast<string>(su.getService()->getServiceNumber()));
					r->setTransaction(&rt);

					ReservationTableSync::Save(r.get());
			}	}
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

				r->setDeparturePlaceId(
					dynamic_cast<const NamedPlace*>(_departurePlace.get())->getKey()
				);
				r->setDepartureCityName(
					dynamic_cast<const NamedPlace*>(_departurePlace.get())->getCity()->getName()
				);
				r->setDeparturePlaceNameNoCity(
					dynamic_cast<const NamedPlace*>(_departurePlace.get())->getName()
				);
				r->setDeparturePlaceName(
					dynamic_cast<const NamedPlace*>(_departurePlace.get())->getFullName()
				);
				r->setDepartureTime(_departureDateTime);
				r->setOriginDateTime(_departureDateTime);
				r->setArrivalPlaceId(
					dynamic_cast<const NamedPlace*>(_arrivalPlace.get())->getKey()
				);
				r->setArrivalCityName(
					dynamic_cast<const NamedPlace*>(_arrivalPlace.get())->getCity()->getName()
				);
				r->setArrivalPlaceNameNoCity(
					dynamic_cast<const NamedPlace*>(_arrivalPlace.get())->getName()
				);
				r->setArrivalPlaceName(
					dynamic_cast<const NamedPlace*>(_arrivalPlace.get())->getFullName()
				);
				r->setArrivalTime(_arrivalDateTime);

				r->setLineCode(_freeDRTTimeSlot->getArea()->getLine()->getShortName());
				r->setLineId(_freeDRTTimeSlot->getArea()->getLine()->getKey());
                r->setReservationPossible(false);

				reservationContact = OnlineReservationRule::GetOnlineReservationRule(
					_freeDRTTimeSlot->getArea()->getLine()->getReservationContact()
				);

				if(dynamic_cast<const PTUseRule*>(&_freeDRTTimeSlot->getUseRule(_userClassCode)))
				{
					r->setReservationRuleId(
						static_cast<const PTUseRule&>(
							_freeDRTTimeSlot->getUseRule(_userClassCode)
						).getKey()
                    );
				}
				r->setReservationDeadLine(
					_freeDRTTimeSlot->getUseRule(_userClassCode).getReservationDeadLine(
						_departureDateTime,
						_departureDateTime
				)	);
				r->setServiceId(_freeDRTTimeSlot->getKey());
				r->setServiceCode(_freeDRTTimeSlot->getServiceNumber());
				r->setTransaction(&rt);

				ReservationTableSync::Save(r.get());
			}

			// Log
			ResaDBLog::AddBookReservationEntry(request.getSession().get(), rt);

			// Mail
			if(!_ignoreReservation && !_customer->getEMail().empty() && reservationContact)
 			{
				reservationContact->sendCustomerEMail(rt);

				ResaDBLog::AddEMailEntry(*request.getSession(), *_customer, "Récapitulatif de réservation");
 			}

			// Redirect
			if(request.getActionWillCreateObject())
			{
				request.setActionCreatedId(rt.getKey());
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

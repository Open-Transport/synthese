
/** ResaModule class implementation.
	@file ResaModule.cpp

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

#include "ResaModule.h"

#include "HTMLModule.h"
#include "Profile.h"
#include "ProfileTableSync.h"
#include "PTServiceConfigTableSync.hpp"
#include "User.h"
#include "UserTableSync.h"
#include "DBLogEntry.h"
#include "DBLogEntryTableSync.h"
#include "Session.h"
#include "StaticActionFunctionRequest.h"
#include "Reservation.h"
#include "ReservationTableSync.h"
#include "ResaRight.h"
#include "ReservationTransaction.h"
#include "ReservationTransactionTableSync.h"
#include "ResaDBLog.h"
#include "ResaCustomerAdmin.h"
#include "CancelReservationAction.h"
#include "ResaEditLogEntryAdmin.h"
#include "CommercialLine.h"
#include "CallBeginAction.h"
#include "CallEndAction.h"
#include "ReservationRoutePlannerAdmin.h"
#include "ResaEditLogEntryAdmin.h"
#include "AdminActionFunctionRequest.hpp"
#include "OnlineReservationRule.h"
#include "OnlineReservationRuleTableSync.h"
#include "Language.hpp"
#include "ScheduledServiceTableSync.h"
#include "FreeDRTTimeSlotTableSync.hpp"
#include "ServerModule.h"

#include <boost/foreach.hpp>

using namespace std;
using namespace boost;
using namespace boost::posix_time;

namespace synthese
{
	using namespace html;
	using namespace security;
	using namespace pt;
	using namespace server;
	using namespace admin;
	using namespace dblog;
	using namespace util;
	using namespace resa;
	using namespace pt_website;

	template<> const std::string util::FactorableTemplate<ModuleClass,ResaModule>::FACTORY_KEY("51_resa");

	namespace resa
	{
		const string ResaModule::_BASIC_PROFILE_NAME("Basic Resa Customer");	// Never change this or the database will be corrupted
		const string ResaModule::_AUTORESA_PROFILE_NAME("Autoresa Resa Customer");	// Never change this or the database will be corrupted
		const string ResaModule::_ADMIN_PROFILE_NAME("Resa admin user");	// Never change this or the database will be corrupted
		const string ResaModule::_RESERVATION_CONTACT_PARAMETER("reservation_contact");
		const string ResaModule::_JOURNEY_PLANNER_WEBSITE("resa_journey_planner_website");

		const std::string ResaModule::DATA_SWITCH_CALL_URL("switch_call_url");
		const std::string ResaModule::DATA_CURRENT_CALL_ID("current_call_id");
		const std::string ResaModule::DATA_CURRENT_CALL_TIMESTAMP("current_call_timestamp");

		const std::string ResaModule::MODULE_PARAMETER_MAX_SEATS_ALLOWED("max_seats_number_allowed");

		ResaModule::_SessionsCallIdMap ResaModule::_sessionsCallIds;
		boost::shared_ptr<Profile> ResaModule::_basicProfile;
		boost::shared_ptr<Profile> ResaModule::_autoresaProfile;
		boost::shared_ptr<Profile> ResaModule::_adminProfile;
		recursive_mutex ResaModule::_sessionsCallIdsMutex;
		boost::shared_ptr<OnlineReservationRule> ResaModule::_reservationContact;
		boost::shared_ptr<PTServiceConfig> ResaModule::_journeyPlannerConfig;
		ResaModule::ReservationsByService ResaModule::_reservationsByService;
		boost::recursive_mutex ResaModule::_reservationsByServiceMutex;
		size_t ResaModule::_maxSeats;
	}

	namespace server
	{
		template<> const string ModuleClassTemplate<ResaModule>::NAME("TAD Réservation");



		template<> void ModuleClassTemplate<ResaModule>::PreInit()
		{
			RegisterParameter(ResaModule::_RESERVATION_CONTACT_PARAMETER, "0", &ResaModule::ParameterCallback);
			RegisterParameter(ResaModule::_JOURNEY_PLANNER_WEBSITE, "0", &ResaModule::ParameterCallback);
			RegisterParameter(ResaModule::MODULE_PARAMETER_MAX_SEATS_ALLOWED, "0", &ResaModule::ParameterCallback);
		}



		template<> void ModuleClassTemplate<ResaModule>::Init()
		{

			// Basic resa profile
			{
				Env env;
				ProfileTableSync::SearchResult profiles(
					ProfileTableSync::Search(env, ResaModule::_BASIC_PROFILE_NAME)
				);
				if (profiles.empty())
					ResaModule::_basicProfile.reset(new Profile);
				else
					ResaModule::_basicProfile = profiles.front();
				ResaModule::_basicProfile->setName(ResaModule::_BASIC_PROFILE_NAME);
				boost::shared_ptr<Right> r(new GlobalRight);
				r->setPrivateLevel(FORBIDDEN);
				r->setPublicLevel(FORBIDDEN);
				ResaModule::_basicProfile->cleanRights();
				ResaModule::_basicProfile->addRight(r);
				ProfileTableSync::Save(ResaModule::_basicProfile.get());
				Log::GetInstance().debug("Basic resa profile checked");
			}

			// Autoresa profile
			{
				Env env;
				ProfileTableSync::SearchResult profiles(
					ProfileTableSync::Search(env, ResaModule::_AUTORESA_PROFILE_NAME)
				);
				if (profiles.empty())
					ResaModule::_autoresaProfile.reset(new Profile);
				else
					ResaModule::_autoresaProfile = profiles.front();
				ResaModule::_autoresaProfile->setName(ResaModule::_AUTORESA_PROFILE_NAME);
				boost::shared_ptr<Right> r2(new GlobalRight);
				r2->setPrivateLevel(FORBIDDEN);
				r2->setPublicLevel(FORBIDDEN);
				ResaModule::_autoresaProfile->cleanRights();
				ResaModule::_autoresaProfile->addRight(r2);
				boost::shared_ptr<Right> r3(new ResaRight);
				r3->setPrivateLevel(WRITE);
				r3->setPublicLevel(FORBIDDEN);
				ResaModule::_autoresaProfile->addRight(r3);
				ProfileTableSync::Save(ResaModule::_autoresaProfile.get());
				Log::GetInstance().debug("Autoresa resa profile checked");
			}
		}

		template<> void ModuleClassTemplate<ResaModule>::Start()
		{
		}

		template<> void ModuleClassTemplate<ResaModule>::End()
		{
			UnregisterParameter(ResaModule::_RESERVATION_CONTACT_PARAMETER);
			UnregisterParameter(ResaModule::_JOURNEY_PLANNER_WEBSITE);
			UnregisterParameter(ResaModule::MODULE_PARAMETER_MAX_SEATS_ALLOWED);
		}



		template<> void ModuleClassTemplate<ResaModule>::InitThread(
			
			){
		}



		template<> void ModuleClassTemplate<ResaModule>::CloseThread(
			
			){
		}
	}

	namespace resa
	{
		void ResaModule::DisplayReservations(
			std::ostream& stream,
			const ReservationTransaction& tr,
			const Language& language
		){
			stream << tr.get<Seats>() << " place" << ((tr.get<Seats>() > 1) ? "s" : "") << " au nom de " << tr.get<CustomerName>() << " (" << tr.get<CustomerPhone>() << ") sur :";
			stream << "<ul>";
			ReservationTransaction::Reservations rs(tr.getReservations());
			for (ReservationTransaction::Reservations::const_iterator itrs(rs.begin()); itrs != rs.end(); ++itrs)
			{
				const Reservation& resa(**itrs);
				stream << "<li>";
				try
				{
					boost::shared_ptr<const ScheduledService> service(
						Env::GetOfficialEnv().get<ScheduledService>(resa.get<ServiceId>())
					);
					if(!dynamic_cast<const JourneyPattern*>(service->getPath()))
					{
						continue;
					}
					const JourneyPattern& jp(static_cast<const JourneyPattern&>(*service->getPath()));
					stream << "<span class=\"" << jp.getCommercialLine()->getStyle() << "\"><span class=\"linesmall\">" << jp.getCommercialLine()->getShortName() << "</span></span> ";
					if(!jp.getDirection().empty())
					{
						stream << " / " << jp.getDirection();
					}
				}
				catch (...)
				{
					stream << "Ligne " << (*itrs)->get<LineCode>();
				}
				if ((*itrs)->get<ReservationRuleId>() != 0)
				{
					stream << HTMLModule::getHTMLImage("/admin/img/resa_compulsory.png", "Place réservée sur ce tronçon");
				}
				stream << " de " << (*itrs)->get<DeparturePlaceName>();
				stream << " le " << language.getWeekDayName((*itrs)->get<DepartureTime>().date().day_of_week()) << " " << (*itrs)->get<DepartureTime>();
				stream << " jusqu'à " << (*itrs)->get<ArrivalPlaceName>();
				if(!(*itrs)->get<ArrivalTime>().is_not_a_date_time())
				{
					stream << " le " << language.getWeekDayName((*itrs)->get<ArrivalTime>().date().day_of_week()) << " " << (*itrs)->get<ArrivalTime>();
				}
				stream << "</li>";
			}
			stream << "</ul>";
		}



		void ResaModule::CallOpen( const server::Session* session )
		{
			recursive_mutex::scoped_lock lock(_sessionsCallIdsMutex);

			CallClose(session);

			RegistryKeyType entryId(ResaDBLog::AddCallEntry(session->getUser().get()));

			_sessionsCallIds[session] = entryId;
		}



		void ResaModule::CallClose( const server::Session* session )
		{
			recursive_mutex::scoped_lock lock(_sessionsCallIdsMutex);

			_SessionsCallIdMap::iterator it(_sessionsCallIds.find(session));

			if (it != _sessionsCallIds.end())
			{
				ResaDBLog::UpdateCallEntryDate(it->second);
				_sessionsCallIds.erase(it);
			}
		}



		RegistryKeyType ResaModule::GetCurrentCallId( const server::Session* session )
		{
			recursive_mutex::scoped_lock lock(_sessionsCallIdsMutex);

			_SessionsCallIdMap::iterator it(_sessionsCallIds.find(session));

			return (it == _sessionsCallIds.end()) ? 0 : it->second;
		}



		boost::shared_ptr<security::Profile> ResaModule::GetBasicResaCustomerProfile()
		{
			return _basicProfile;
		}



		string ResaModule::GetStatusIcon(ReservationStatus status)
		{
			switch(status)
			{
			case OPTION: return "stop_blue.png";
			case ACKNOWLEDGED_OPTION: return "stop_blue.png";
			case TO_BE_DONE: return "stop_green.png";
			case ACKNOWLEDGED: return "stop_green.png";
			case CANCELLATION_TO_ACK: return "cross.png";
			case CANCELLED: return "cross.png";
			case CANCELLED_AFTER_DELAY: return "asterisk_red.png";
			case ACKNOWLEDGED_CANCELLED_AFTER_DELAY: return "asterisk_red.png";
			case SHOULD_BE_AT_WORK: return "car.png";
			case AT_WORK: return "car.png";
			case NO_SHOW: return "user_cross.png";
			case SHOULD_BE_DONE: return "tick.png";
			case DONE: return "tick.png";
			default:
				break;
			}
			return string();
		}



		string ResaModule::GetStatusText(ReservationStatus status )
		{
			switch(status)
			{
			case OPTION: return "option";
			case ACKNOWLEDGED_OPTION: return "option confirmée";
			case TO_BE_DONE: return "en attente";
			case ACKNOWLEDGED: return "confirmé";
			case CANCELLATION_TO_ACK: return "annulation à confirmer";
			case CANCELLED: return "annulé";
			case CANCELLED_AFTER_DELAY: return "annulé hors délai";
			case ACKNOWLEDGED_CANCELLED_AFTER_DELAY: return "annullation hors délai confirmée";
			case SHOULD_BE_AT_WORK: return "prévu en cours";
			case AT_WORK: return "en cours";
			case NO_SHOW: return "absence";
			case SHOULD_BE_DONE: return "prévu terminé";
			case DONE: return "terminé";
			default:
				break;
			}
			return string();
		}



		boost::shared_ptr<security::Profile> ResaModule::GetAutoResaResaCustomerProfile()
		{
			return _autoresaProfile;
		}



		OnlineReservationRule* ResaModule::GetReservationContact()
		{
			return _reservationContact.get();
		}



		void ResaModule::ParameterCallback( const std::string& name, const std::string& value )
		{
			if(name == _RESERVATION_CONTACT_PARAMETER)
			{
				RegistryKeyType id(lexical_cast<RegistryKeyType>(value));

				if(id > 0)
				{
					_reservationContact = OnlineReservationRuleTableSync::GetEditable(id, Env::GetOfficialEnv());
				}
				else
				{
					_reservationContact.reset();
				}
			}
			else if(name == _JOURNEY_PLANNER_WEBSITE)
			{
				RegistryKeyType id(lexical_cast<RegistryKeyType>(value));

				if(id > 0)
				{
					_journeyPlannerConfig = PTServiceConfigTableSync::GetEditable(id, Env::GetOfficialEnv());
				}
				else
				{
					_journeyPlannerConfig.reset();
				}
			}
			else if(name == MODULE_PARAMETER_MAX_SEATS_ALLOWED)
			{
				try
				{
					size_t number(lexical_cast<size_t>(value));

					if(number > 0)
					{
						_maxSeats = number;
					}
				}
				catch(bad_lexical_cast)
				{
				}
			}
		}



		void ResaModule::addAdminPageParameters(
			ParametersMap& map,
			const server::Request& request
		) const	{

			bool buttonIsAllowed(
				request.getSession() &&
				request.getSession()->hasProfile() &&
				request.getSession()->getUser()->getProfile()->isAuthorized<ResaRight>(READ)
			);

			if(buttonIsAllowed)
			{
				RegistryKeyType callId(ResaModule::GetCurrentCallId(request.getSession().get()));

				if (callId == 0)
				{ // Case call start

					AdminActionFunctionRequest<CallBeginAction,ReservationRoutePlannerAdmin> callRequest(
						request
					);
					stringstream os;
					os << callRequest.getClientURL() << Request::PARAMETER_STARTER;
					ParametersMap pm(callRequest.getParametersMap());
					pm.remove(BaseReservationActionAdmin::PARAMETER_CUSTOMER_ID);
					pm.outputURI(os);
					map.insert(DATA_SWITCH_CALL_URL, os.str());
				}
				else
				{
					map.insert(DATA_CURRENT_CALL_ID, callId);

					AdminActionFunctionRequest<CallEndAction,ResaEditLogEntryAdmin> callRequest(
						request
					);
					callRequest.getPage()->setEntry(
						DBLogEntryTableSync::Get(
							callId,
							*request.getFunction()->getEnv()
					)	);
					map.insert(DATA_SWITCH_CALL_URL, callRequest.getURL());

					Env env;
					boost::shared_ptr<DBLogEntry> entry(DBLogEntryTableSync::GetEditable(callId, env));
					map.insert(DATA_CURRENT_CALL_TIMESTAMP, to_iso_string(entry->getDate()));
				}
			}
		}



		pt_website::PTServiceConfig* ResaModule::GetJourneyPlannerWebsite()
		{
			return _journeyPlannerConfig.get();
		}



		size_t ResaModule::GetMaxSeats()
		{
			return _maxSeats;
		}



		void ResaModule::AddReservationByService( const Reservation& reservation )
		{
			recursive_mutex::scoped_lock lock(_reservationsByServiceMutex);

			try
			{
				RegistryTableType tableId(decodeTableId(reservation.get<ServiceId>()));
				if(tableId == ScheduledServiceTableSync::TABLE.ID)
				{
					_reservationsByService[Env::GetOfficialEnv().get<ScheduledService>(reservation.get<ServiceId>()).get()].insert(&reservation);
				}
				else if(tableId == FreeDRTTimeSlotTableSync::TABLE.ID)
				{
					_reservationsByService[Env::GetOfficialEnv().get<FreeDRTTimeSlot>(reservation.get<ServiceId>()).get()].insert(&reservation);
				}
			}
			catch(ObjectNotFoundException<ScheduledService>&)
			{
			}
			catch(ObjectNotFoundException<FreeDRTTimeSlot>&)
			{
			}
		}



		void ResaModule::RemoveReservationByService( const Reservation& reservation )
		{
			recursive_mutex::scoped_lock lock(_reservationsByServiceMutex);

			RegistryTableType tableId(decodeTableId(reservation.get<ServiceId>()));
			try
			{
				if(tableId == ScheduledServiceTableSync::TABLE.ID)
				{
					_reservationsByService[Env::GetOfficialEnv().get<ScheduledService>(reservation.get<ServiceId>()).get()].erase(&reservation);
				}
				else if(tableId == FreeDRTTimeSlotTableSync::TABLE.ID)
				{
					_reservationsByService[Env::GetOfficialEnv().get<FreeDRTTimeSlot>(reservation.get<ServiceId>()).get()].erase(&reservation);
				}
			}
			catch(ObjectNotFoundException<ScheduledService>&)
			{
			}
			catch(ObjectNotFoundException<FreeDRTTimeSlot>&)
			{
			}
		}



		const ResaModule::ReservationsByService::mapped_type& ResaModule::GetReservationsByService(
			const graph::Service& service
		){
			return _reservationsByService[&service];
		}
}	}

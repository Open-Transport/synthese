
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
#include "CommercialLineTableSync.h"
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

		ResaModule::_SessionsCallIdMap ResaModule::_sessionsCallIds;
		shared_ptr<Profile> ResaModule::_basicProfile;
		shared_ptr<Profile> ResaModule::_autoresaProfile;
		shared_ptr<Profile> ResaModule::_adminProfile;
		recursive_mutex ResaModule::_sessionsCallIdsMutex;
		shared_ptr<OnlineReservationRule> ResaModule::_reservationContact;
		shared_ptr<PTServiceConfig> ResaModule::_journeyPlannerConfig;
		ResaModule::ReservationsByService ResaModule::_reservationsByService;
		boost::recursive_mutex ResaModule::_reservationsByServiceMutex;
	}

	namespace server
	{
		template<> const string ModuleClassTemplate<ResaModule>::NAME("TAD Réservation");



		template<> void ModuleClassTemplate<ResaModule>::PreInit()
		{
			RegisterParameter(ResaModule::_RESERVATION_CONTACT_PARAMETER, "0", &ResaModule::ParameterCallback);
			RegisterParameter(ResaModule::_JOURNEY_PLANNER_WEBSITE, "0", &ResaModule::ParameterCallback);
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
				shared_ptr<Right> r(new GlobalRight);
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
				shared_ptr<Right> r2(new GlobalRight);
				r2->setPrivateLevel(FORBIDDEN);
				r2->setPublicLevel(FORBIDDEN);
				ResaModule::_autoresaProfile->cleanRights();
				ResaModule::_autoresaProfile->addRight(r2);
				shared_ptr<Right> r3(new ResaRight);
				r3->setPrivateLevel(WRITE);
				r3->setPublicLevel(FORBIDDEN);
				ResaModule::_autoresaProfile->addRight(r3);
				ProfileTableSync::Save(ResaModule::_autoresaProfile.get());
				Log::GetInstance().debug("Autoresa resa profile checked");
			}
		}



		template<> void ModuleClassTemplate<ResaModule>::End()
		{
			UnregisterParameter(ResaModule::_RESERVATION_CONTACT_PARAMETER);
			UnregisterParameter(ResaModule::_JOURNEY_PLANNER_WEBSITE);
		}
	}

	namespace resa
	{
		void ResaModule::DisplayReservations(
			std::ostream& stream,
			const ReservationTransaction& tr,
			const Language& language
		){
			stream << tr.getSeats() << " place" << ((tr.getSeats() > 1) ? "s" : "") << " au nom de " << tr.getCustomerName() << " (" << tr.getCustomerPhone() << ") sur :";
			stream << "<ul>";
			ReservationTransaction::Reservations rs(tr.getReservations());
			for (ReservationTransaction::Reservations::const_iterator itrs(rs.begin()); itrs != rs.end(); ++itrs)
			{
				stream << "<li>";
				try
				{
					Env env;
					shared_ptr<const CommercialLine> line(CommercialLineTableSync::Get((*itrs)->getLineId(), env));
					stream << "<span class=\"" << line->getStyle() << "\"><span class=\"linesmall\">" << line->getShortName() << "</span></span> ";
				}
				catch (...)
				{
					stream << "Ligne " << (*itrs)->getLineCode();
				}
				if ((*itrs)->getReservationRuleId() != 0)
				{
					stream << HTMLModule::getHTMLImage("resa_compulsory.png", "Place réservée sur ce tronçon");
				}
				stream << " de " << (*itrs)->getDeparturePlaceName();
				stream << " le " << language.getWeekDayName((*itrs)->getDepartureTime().date().day_of_week()) << " " << (*itrs)->getDepartureTime();
				stream << " jusqu'à " << (*itrs)->getArrivalPlaceName();
				if(!(*itrs)->getArrivalTime().is_not_a_date_time())
				{
					stream << " le " << language.getWeekDayName((*itrs)->getArrivalTime().date().day_of_week()) << " " << (*itrs)->getArrivalTime();
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
				RegistryKeyType callId(ResaModule::GetCurrentCallId(request.getSession()));

				if (callId == 0)
				{ // Case call start

					AdminActionFunctionRequest<CallBeginAction,ReservationRoutePlannerAdmin> callRequest(
						request
					);
					map.insert(DATA_SWITCH_CALL_URL, callRequest.getURL());
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
					shared_ptr<DBLogEntry> entry(DBLogEntryTableSync::GetEditable(callId, env));
					map.insert(DATA_CURRENT_CALL_TIMESTAMP, to_iso_string(entry->getDate()));
				}
			}
		}



		pt_website::PTServiceConfig* ResaModule::GetJourneyPlannerWebsite()
		{
			return _journeyPlannerConfig.get();
		}



		void ResaModule::AddReservationByService( const Reservation& reservation )
		{
			recursive_mutex::scoped_lock lock(_reservationsByServiceMutex);

			try
			{
				RegistryTableType tableId(decodeTableId(reservation.getServiceId()));
				if(tableId == ScheduledServiceTableSync::TABLE.ID)
				{
					_reservationsByService[Env::GetOfficialEnv().get<ScheduledService>(reservation.getServiceId()).get()].insert(&reservation);
				}
				else if(tableId == FreeDRTTimeSlotTableSync::TABLE.ID)
				{
					_reservationsByService[Env::GetOfficialEnv().get<FreeDRTTimeSlot>(reservation.getServiceId()).get()].insert(&reservation);
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

			RegistryTableType tableId(decodeTableId(reservation.getServiceId()));
			try
			{
				if(tableId == ScheduledServiceTableSync::TABLE.ID)
				{
					_reservationsByService[Env::GetOfficialEnv().get<ScheduledService>(reservation.getServiceId()).get()].erase(&reservation);
				}
				else if(tableId == FreeDRTTimeSlotTableSync::TABLE.ID)
				{
					_reservationsByService[Env::GetOfficialEnv().get<FreeDRTTimeSlot>(reservation.getServiceId()).get()].erase(&reservation);
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

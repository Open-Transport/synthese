
/** ResaModule class implementation.
	@file ResaModule.cpp

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

#include "ResaModule.h"
#include "DateTime.h"
#include "HTMLModule.h"
#include "Profile.h"
#include "ProfileTableSync.h"
#include "User.h"
#include "UserTableSync.h"
#include "DBLogEntry.h"
#include "DBLogEntryTableSync.h"
#include "Session.h"
#include "ActionFunctionRequest.h"
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

#include "OnlineReservationRule.h"
#include "OnlineReservationRuleTableSync.h"

#include <boost/foreach.hpp>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace html;
	using namespace security;
	using namespace env;
	using namespace server;
	using namespace admin;
	using namespace time;
	using namespace dblog;
	using namespace util;
	using namespace resa;

	template<> const std::string util::FactorableTemplate<ModuleClass,ResaModule>::FACTORY_KEY("31_resa");

	namespace resa
	{
		const string ResaModule::_BASIC_PROFILE_NAME("Basic Resa Customer");	// Never change this or the database will be corrupted
		const string ResaModule::_AUTORESA_PROFILE_NAME("Autoresa Resa Customer");	// Never change this or the database will be corrupted
		const string ResaModule::_ADMIN_PROFILE_NAME("Resa admin user");	// Never change this or the database will be corrupted
		const string ResaModule::_RESERVATION_CONTACT_PARAMETER("reservation_contact");

		ResaModule::_SessionsCallIdMap ResaModule::_sessionsCallIds;
		shared_ptr<Profile> ResaModule::_basicProfile;
		shared_ptr<Profile> ResaModule::_autoresaProfile;
		shared_ptr<Profile> ResaModule::_adminProfile;
		recursive_mutex ResaModule::_sessionsCallIdsMutex;
		shared_ptr<OnlineReservationRule> ResaModule::_reservationContact;
	}

	namespace server
	{
		template<> const string ModuleClassTemplate<ResaModule>::NAME("TAD Réservation");
		
		template<> void ModuleClassTemplate<ResaModule>::PreInit()
		{
			RegisterParameter(ResaModule::_RESERVATION_CONTACT_PARAMETER, "0", &ResaModule::ParameterCallback);
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
			}
		}
		
		template<> void ModuleClassTemplate<ResaModule>::End()
		{
		}
	}

	namespace resa
	{
		void ResaModule::DisplayReservations(
			std::ostream& stream,
			const ReservationTransaction& tr
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
				if ((*itrs)->getReservationRuleId() != UNKNOWN_VALUE)
				{
					stream << HTMLModule::getHTMLImage("resa_compulsory.png", "Place réservée sur ce tronçon");
				}
				stream << " de " << (*itrs)->getDeparturePlaceName();
				stream << " le " << (*itrs)->getDepartureTime().getDate().getTextWeekDay() << " " << (*itrs)->getDepartureTime().toString();
				stream << " jusqu'à " << (*itrs)->getArrivalPlaceName();
				stream << " le " << (*itrs)->getArrivalTime().getDate().getTextWeekDay() << " " <<(*itrs)->getArrivalTime().toString();
				stream << "</li>";
			}
			stream << "</ul>";
		}

		void ResaModule::CallOpen( const server::Session* session )
		{
			recursive_mutex::scoped_lock lock(_sessionsCallIdsMutex);

			CallClose(session);
				
			uid entryId(ResaDBLog::AddCallEntry(session->getUser().get()));

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

			return (it == _sessionsCallIds.end()) ? UNKNOWN_VALUE : it->second;
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
			case TO_BE_DONE: return "stop_green.png";
			case CANCELLED: return "cross.png";
			case CANCELLED_AFTER_DELAY: return "asterisk_red.png";
			case AT_WORK: return "car.png";
			case NO_SHOW: return "user_cross.png";
			case DONE: return "tick.png";
			}
			return string();
		}



		string ResaModule::GetStatusText(ReservationStatus status )
		{
			switch(status)
			{
			case OPTION: return "option";
			case TO_BE_DONE: return "confirmé";
			case CANCELLED: return "annulé";
			case CANCELLED_AFTER_DELAY: return "annulé hors délai";
			case AT_WORK: return "en cours";
			case NO_SHOW: return "absence";
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
					ResaModule::_reservationContact = OnlineReservationRuleTableSync::GetEditable(id, Env::GetOfficialEnv());				
				}
			}
		}
	}
}

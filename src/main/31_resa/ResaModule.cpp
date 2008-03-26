
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

#include "05_html/HTMLTable.h"
#include "05_html/HTMLModule.h"

#include "12_security/Profile.h"
#include "12_security/ProfileTableSync.h"

#include "30_server/Session.h"

#include "31_resa/Reservation.h"
#include "31_resa/ReservationTransaction.h"
#include "31_resa/ResaDBLog.h"

#include "15_env/CommercialLine.h"
#include "15_env/CommercialLineTableSync.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace html;
	using namespace security;
	using namespace env;

	template<> const std::string util::FactorableTemplate<util::ModuleClass, resa::ResaModule>::FACTORY_KEY("31_resa");

	namespace resa
	{
		const string ResaModule::_BASIC_PROFILE_NAME("Basic Resa Customer");	// Never change this or the database will be corrupted

		ResaModule::_SessionsCallIdMap ResaModule::_sessionsCallIds;
		shared_ptr<Profile> ResaModule::_basicProfile;

		std::string ResaModule::getName() const
		{
			return "TAD Réservation";
		}

		void ResaModule::DisplayReservations( std::ostream& stream, const ReservationTransaction* tr)
		{
			stream << tr->getSeats() << " place" << ((tr->getSeats() > 1) ? "s" : "") << " au nom de " << tr->getCustomerName() << " (" << tr->getCustomerPhone() << ") sur :";
			stream << "<ul>";
			ReservationTransaction::Reservations rs(tr->getReservations());
			for (ReservationTransaction::Reservations::const_iterator itrs(rs.begin()); itrs != rs.end(); ++itrs)
			{
				stream << "<li>";
				try
				{
					shared_ptr<const CommercialLine> line(CommercialLineTableSync::Get((*itrs)->getLineId()));
					stream << "<span class=\"" << line->getStyle() << "\"><span class=\"linesmall\">" << line->getShortName() << "</span></span> ";
				}
				catch (...)
				{
					stream << "Ligne " << (*itrs)->getLineCode();
				}
				if ((*itrs)->getReservationRuleId() != UNKNOWN_VALUE)
				{
					stream << HTMLModule::getHTMLImage("resa_compulsory.png", "Place réservée sur ce tronçon");
					stream << HTMLModule::getHTMLImage(ResaModule::GetStatusIcon((*itrs)->getStatus()), (*itrs)->getFullStatusText());
				}
				stream << " de " << (*itrs)->getDepartureTime().toString();
				stream << " à " << (*itrs)->getDeparturePlaceName();
				stream << " jusqu'à " << (*itrs)->getArrivalPlaceName();
				stream << " à " << (*itrs)->getArrivalTime().toString();
				stream << "</li>";
			}
			stream << "</ul>";
		}

		void ResaModule::CallOpen( const server::Session* session )
		{
			CallClose(session);
				
			uid entryId(ResaDBLog::AddCallEntry(session->getUser().get()));

			_sessionsCallIds[session] = entryId;
		}

		void ResaModule::CallClose( const server::Session* session )
		{
			_SessionsCallIdMap::iterator it(_sessionsCallIds.find(session));

			if (it != _sessionsCallIds.end())
			{
				ResaDBLog::UpdateCallEntryDate(it->second);
				_sessionsCallIds.erase(it);
			}
		}

		uid ResaModule::GetCurrentCallId( const server::Session* session )
		{
			_SessionsCallIdMap::iterator it(_sessionsCallIds.find(session));

			return (it == _sessionsCallIds.end()) ? UNKNOWN_VALUE : it->second;
		}



		void ResaModule::initialize()
		{
			vector<shared_ptr<Profile> > profiles(ProfileTableSync::Search(_BASIC_PROFILE_NAME));
			if (profiles.size() == 0)
				_basicProfile.reset(new Profile);
			else
				_basicProfile = profiles.front();
			_basicProfile->setName(_BASIC_PROFILE_NAME);
			shared_ptr<Right> r(new GlobalRight);
			r->setPrivateLevel(FORBIDDEN);
			r->setPublicLevel(FORBIDDEN);
			_basicProfile->cleanRights();
			_basicProfile->addRight(r);
			ProfileTableSync::save(_basicProfile.get());
		}



		boost::shared_ptr<security::Profile> ResaModule::GetBasicResaCustomerProfile()
		{
			return _basicProfile;
		}



		string ResaModule::GetStatusIcon(ReservationStatus status)
		{
			switch(status)
			{
			case ReservationStatus::OPTION: return "bullet_yellow.png";
			case ReservationStatus::TO_BE_DONE: return "bullet_green.png";
			case ReservationStatus::CANCELLED: return "bullet_black.png";
			case ReservationStatus::CANCELLED_AFTER_DELAY: "error.png";
			case ReservationStatus::AT_WORK: return "car.png";
			case ReservationStatus::NO_SHOW: return "exclamation.png";
			case ReservationStatus::DONE: return "bullet_white.png";
			}
			return string();
		}



		string ResaModule::GetStatusText(ReservationStatus status )
		{
			switch(status)
			{
			case ReservationStatus::OPTION: return "option";
			case ReservationStatus::TO_BE_DONE: return "confirmé";
			case ReservationStatus::CANCELLED: return "annulé";
			case ReservationStatus::CANCELLED_AFTER_DELAY: "annulé hors délai";
			case ReservationStatus::AT_WORK: return "en cours";
			case ReservationStatus::NO_SHOW: return "absence";
			case ReservationStatus::DONE: return "terminé";
			}
			return string();
		}
	}
}

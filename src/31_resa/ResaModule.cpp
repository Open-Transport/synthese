
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

#include "01_util/Conversion.h"

#include "04_time/DateTime.h"

#include "05_html/HTMLModule.h"

#include "12_security/Profile.h"
#include "12_security/ProfileTableSync.h"
#include "12_security/User.h"
#include "12_security/UserTableSync.h"

#include "13_dblog/DBLogEntry.h"
#include "13_dblog/DBLogEntryTableSync.h"

#include "30_server/Session.h"
#include "30_server/ActionFunctionRequest.h"

#include "31_resa/Reservation.h"
#include "31_resa/ReservationTableSync.h"
#include "31_resa/ResaRight.h"
#include "31_resa/ReservationTransaction.h"
#include "31_resa/ReservationTransactionTableSync.h"
#include "31_resa/ResaDBLog.h"
#include "31_resa/ResaCustomerAdmin.h"
#include "31_resa/CancelReservationAction.h"
#include "31_resa/ResaEditLogEntryAdmin.h"

#include "15_env/CommercialLine.h"
#include "15_env/CommercialLineTableSync.h"

#include "32_admin/AdminRequest.h"

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
			ProfileTableSync::Save(_basicProfile.get());
		}



		boost::shared_ptr<security::Profile> ResaModule::GetBasicResaCustomerProfile()
		{
			return _basicProfile;
		}



		string ResaModule::GetStatusIcon(ReservationStatus status)
		{
			switch(status)
			{
			case OPTION: return "bullet_yellow.png";
			case TO_BE_DONE: return "bullet_green.png";
			case CANCELLED: return "bullet_black.png";
			case CANCELLED_AFTER_DELAY: "error.png";
			case AT_WORK: return "car.png";
			case NO_SHOW: return "exclamation.png";
			case DONE: return "bullet_white.png";
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
			case CANCELLED_AFTER_DELAY: "annulé hors délai";
			case AT_WORK: return "en cours";
			case NO_SHOW: return "absence";
			case DONE: return "terminé";
			}
			return string();
		}



		void ResaModule::DisplayResaDBLog(
			ostream& stream
			, Env& logEnv
			, const std::string& parameterDate
			, server::FunctionRequest<admin::AdminRequest>& searchRequest
			, server::ActionFunctionRequest<CancelReservationAction,admin::AdminRequest>& cancelRequest
			, const html::ResultHTMLTable::RequestParameters& _requestParameters
			, bool displayCustomer
		){
			// Request
			FunctionRequest<AdminRequest> editRequest(searchRequest);
			editRequest.getFunction()->setPage<ResaEditLogEntryAdmin>();
			
			// Rights
			bool writingRight(searchRequest.isAuthorized<ResaRight>(WRITE,WRITE));

			// Results
			ResultHTMLTable::ResultParameters resultParameters;
			resultParameters.setFromResult(_requestParameters, resats);

			// Display
			DateTime now(TIME_CURRENT);
			ResultHTMLTable::HeaderVector ht;
			ht.push_back(make_pair(parameterDate, "Date"));
			if (displayCustomer)
				ht.push_back(make_pair(string(), "Client"));
			ht.push_back(make_pair(string(), "Objet"));
			ht.push_back(make_pair(string(), "Description"));
			ht.push_back(make_pair(string(), "Opérateur"));
			ht.push_back(make_pair(string(), "Actions"));
			ResultHTMLTable rt(ht, searchRequest.getHTMLForm(), _requestParameters, resultParameters);
			stream << rt.open();

			BOOST_FOREACH(shared_ptr<DBLogEntry> entry, env;template getRegistry<DBLogEntry>())
			{
				DBLogEntry::Content content(entry->getContent());
				ResaDBLog::_EntryType entryType(static_cast<ResaDBLog::_EntryType>(Conversion::ToInt(content[ResaDBLog::COL_TYPE])));
				shared_ptr<ReservationTransaction> tr;
				ReservationStatus status(NO_RESERVATION);
				const User* entryUser(entry->getUser());
				shared_ptr<const User> customer;
				if (displayCustomer && entry->getObjectId() > 0)
					try
					{
						customer = UserTableSync::Get(entry->getObjectId());
					}
					catch(...)
					{

					}

				if (Conversion::ToLongLong(content[ResaDBLog::COL_RESA]) > 0)
				{
					tr = ReservationTransactionTableSync::GetEditable(Conversion::ToLongLong(content[ResaDBLog::COL_RESA]));
					//ReservationTableSync::search(tr.get());
					status = tr->getStatus();
					cancelRequest.getAction()->setTransaction(tr);
				}

				if (entryType == ResaDBLog::CALL_ENTRY)
				{
					DateTime d(DateTime::FromSQLTimestamp(content[ResaDBLog::COL_DATE2]));

					stream << rt.row();
					stream << rt.col(1,string(),true) << entry->getDate().toString();
					if (displayCustomer)
					{
						stream << rt.col(1, string(), true);
						if (customer.get())
							stream << customer->getFullName();
					}
					stream << rt.col(1,string(),true) << HTMLModule::getHTMLImage("phone.png","Appel");
					stream << rt.col(1,string(),true) << "APPEL";
					if (!d.isUnknown())
						stream << " jusqu'à " << d.toString() << " (" << (d.getSecondsDifference(entry->getDate())) << " s)";
					stream << rt.col(1,string(),true) << entryUser->getFullName();
					stream << rt.col(1,string(),true);
					if(searchRequest.isAuthorized<ResaRight>(DELETE_RIGHT,UNKNOWN_RIGHT_LEVEL)
						||	searchRequest.isAuthorized<ResaRight>(UNKNOWN_RIGHT_LEVEL, WRITE) && entryUser == searchRequest.getSession()->getUser().get()
					){
						editRequest.setObjectId(entry->getKey());
						stream << HTMLModule::getLinkButton(editRequest.getURL(), "Modifier", string(), "pencil.png");
					}

				}
				else
				{
					stream << rt.row();

					stream << rt.col() << entry->getDate().toString();

					if (displayCustomer)
					{
						stream << rt.col();
						if (customer.get())
							stream << customer->getFullName();
					}
					stream << rt.col();
					switch (entryType)
					{
					case ResaDBLog::RESERVATION_ENTRY:
						stream << HTMLModule::getHTMLImage("resa_compulsory.png", "Réservation");
						stream << HTMLModule::getHTMLImage(ResaModule::GetStatusIcon(status), tr->getFullStatusText());
						break;

					case ResaDBLog::CANCELLATION_ENTRY:
						stream << HTMLModule::getHTMLImage("bullet_delete.png", "Annulation de réservation");
						break;

					case ResaDBLog::DELAYED_CANCELLATION_ENTRY:
						stream << HTMLModule::getHTMLImage("error.png", "Annulation de réservation hors délai");
						break;

					case ResaDBLog::NO_SHOW:
						stream << HTMLModule::getHTMLImage("exclamation.png", "Absence");
						break;
					}

					stream << rt.col();
					switch (entryType)
					{
					case ResaDBLog::RESERVATION_ENTRY:
						ResaModule::DisplayReservations(stream, tr.get());
						break;

					case ResaDBLog::CANCELLATION_ENTRY:
						stream << "ANNULATION de : ";
						ResaModule::DisplayReservations(stream, tr.get());
						break;

					case ResaDBLog::DELAYED_CANCELLATION_ENTRY:
						stream << "ANNULATION HORS DELAI de : ";
						ResaModule::DisplayReservations(stream, tr.get());
						break;

					case ResaDBLog::NO_SHOW:
						stream << "ABSENCE sur : ";
						ResaModule::DisplayReservations(stream, tr.get());
						break;
					}

					stream << rt.col() << entryUser->getFullName();


					stream << rt.col();
					if (writingRight)
					{
						switch(status)
						{
						case OPTION:
							stream << HTMLModule::getLinkButton(cancelRequest.getURL(), "Annuler", "Etes-vous sûr de vouloir annuler la réservation ?", "bullet_delete.png");
							break;

						case TO_BE_DONE:
							stream << HTMLModule::getLinkButton(cancelRequest.getURL(), "Annuler hors délai", "Etes-vous sûr de vouloir annuler la réservation (hors délai) ?", "error.png");
							break;

						case AT_WORK:
							stream << HTMLModule::getLinkButton(cancelRequest.getURL(), "Noter absence", "Etes-vous sûr de noter l'absence du client à l'arrêt ?", "exclamation.png");
							break;
						}
					}
				}
			}

			stream << rt.close();

		}
	}
}


/** XMLReservationFunction class implementation.
	@file XMLReservationFunction.cpp
	@author Hugues
	@date 2009

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

#include "XMLReservationFunction.h"

#include "Profile.h"
#include "ResaRight.h"
#include "RequestException.h"
#include "Request.h"
#include "ReservationTableSync.h"
#include "ReservationTransactionTableSync.h"
#include "Reservation.h"
#include "ReservationTransaction.h"
#include "ResaModule.h"
#include "Session.h"
#include "User.h"

#include <boost/date_time/posix_time/posix_time.hpp>

using namespace std;
using namespace boost;
using namespace boost::posix_time;


namespace synthese
{
	using namespace util;
	using namespace server;
	using namespace security;

	template<> const string util::FactorableTemplate<Function,resa::XMLReservationFunction>::FACTORY_KEY("XMLReservationFunction");

	namespace resa
	{
		ParametersMap XMLReservationFunction::_getParametersMap() const
		{
			ParametersMap map;
			if(_resa.get())
			{
				map.insert(Request::PARAMETER_OBJECT_ID, _resa->getKey());
			}
			return map;
		}

		void XMLReservationFunction::_setFromParametersMap(const ParametersMap& map)
		{
			try
			{
				_resa = ReservationTransactionTableSync::Get(
					map.get<RegistryKeyType>(Request::PARAMETER_OBJECT_ID),
					*getEnv()
				);
				ReservationTableSync::Search(*getEnv(), _resa->getKey());
			}
			catch(ObjectNotFoundException<ReservationTransactionTableSync>)
			{
				throw RequestException("No such reservation");
			}
		}



		util::ParametersMap XMLReservationFunction::run( std::ostream& stream, const Request& request ) const
		{
			boost::shared_ptr<const ReservationTransaction> resa(_resa);
			if(!resa.get() && request.getActionCreatedId())
			{
				resa = ReservationTransactionTableSync::Get(*request.getActionCreatedId(), *getEnv());
				ReservationTableSync::Search(*getEnv(), resa->getKey());
			}

			string departurePlaceName;
			string arrivalPlaceName;
			posix_time::ptime travelDate;
			if(resa.get() && !resa->getReservations().empty())
			{
				departurePlaceName = (*resa->getReservations().begin())->get<DeparturePlaceName>();
				travelDate = (*resa->getReservations().begin())->get<DepartureTime>();
				arrivalPlaceName = (*resa->getReservations().rbegin())->get<ArrivalPlaceName>();
			}

			stream <<
				"<?xml version=\"1.0\" encoding=\"UTF-8\"?>" <<
				"<reservation xsi:noNamespaceSchemaLocation=\"http://rcsmobility.com/xsd/xml_reservation_function.xsd\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"" <<
				" id=\"" << (resa.get() ? lexical_cast<string>(resa->getKey()) : string()) << "\"" <<
				" customerId=\"" << (resa.get() ? lexical_cast<string>(resa->get<Customer>()->getKey()) : string()) << "\"" <<
				" cancellationDeadLine=\"" << (resa.get() ? posix_time::to_iso_extended_string(resa->getReservationDeadLine()) : string()) << "\"" <<
				" departureStop=\"" << departurePlaceName << "\"" <<
				" arrivalStop=\"" << arrivalPlaceName << "\"" <<
				" travelDate=\"" << (travelDate.is_not_a_date_time() ? string()  : posix_time::to_iso_extended_string(travelDate)) << "\"" <<
				" customerName=\"" << (resa.get() ? resa->get<CustomerName>() : string()) << "\"" <<
				" customerPhone=\"" << (resa.get() ? resa->get<CustomerPhone>() : string()) << "\"" <<
				" status=\"" << (resa.get() ? ResaModule::GetStatusText(resa->getStatus()) : "N.A.") << "\"" <<
				" canBeCancelled=\"" << (resa.get() ? lexical_cast<string>(second_clock::local_time() < resa->getReservationDeadLine() && resa->get<CancellationTime>().is_not_a_date_time()) : "0") << "\"" <<
				" seats=\"" << (resa.get() ? lexical_cast<string>(resa->get<Seats>()) : "0") << "\"";
			if(resa.get() && !resa->get<CancellationTime>().is_not_a_date_time())
			{
				stream << " cancellationDateTime=\"" << posix_time::to_iso_extended_string(resa->get<CancellationTime>()) << "\"";
			}
			stream << ">";

			if(resa.get())
			{
				BOOST_FOREACH(const Reservation* r, resa->getReservations())
				{
					stream <<
						"<chunk" <<
						" departurePlaceName=\"" << r->get<DeparturePlaceName>() << "\"" <<
						" departureDateTime=\"" << posix_time::to_iso_extended_string(r->get<DepartureTime>()) << "\"" <<
						" arrivalPlaceName=\"" << r->get<ArrivalPlaceName>() << "\"" <<
						" arrivalDateTime=\"" << posix_time::to_iso_extended_string(r->get<ArrivalTime>()) << "\"" <<
						" lineNumber=\"" << r->get<LineCode>() << "\"" <<
						" />";
				}
			}
			stream << "</reservation>";

			return util::ParametersMap();
		}



		bool XMLReservationFunction::isAuthorized(const server::Session* session) const
		{
			return
				(session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<ResaRight>(WRITE)) ||
				(session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<ResaRight>(UNKNOWN_RIGHT_LEVEL, WRITE));
		}



		std::string XMLReservationFunction::getOutputMimeType() const
		{
			return "text/xml";
		}



		XMLReservationFunction::XMLReservationFunction():
		FactorableTemplate<server::Function,XMLReservationFunction>()
		{
			setEnv(boost::shared_ptr<Env>(new Env));
		}
	}
}

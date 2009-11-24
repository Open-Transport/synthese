
/** XMLReservationFunction class implementation.
	@file XMLReservationFunction.cpp
	@author Hugues
	@date 2009

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

#include "ResaRight.h"
#include "RequestException.h"
#include "XMLReservationFunction.h"
#include "Request.h"
#include "ReservationTableSync.h"
#include "ReservationTransactionTableSync.h"
#include "Reservation.h"
#include "ReservationTransaction.h"
#include "ResaModule.h"

#include <boost/date_time/posix_time/posix_time.hpp>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace util;
	using namespace server;
	using namespace security;
	using namespace time;

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
			if(!map.getOptional<RegistryKeyType>(Request::PARAMETER_OBJECT_ID))
			{
				_request->setActionWillCreateObject();
			}
			else
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
		}

		void XMLReservationFunction::_run( std::ostream& stream ) const
		{
			shared_ptr<const ReservationTransaction> resa(_resa);
			if(!resa.get() && _request->getActionCreatedId())
			{
				resa = ReservationTransactionTableSync::Get(*_request->getActionCreatedId(), *getEnv());
				ReservationTableSync::Search(*getEnv(), resa->getKey());
			}

			stream <<
				"<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>" <<
				"<reservation xsi:noNamespaceSchemaLocation=\"http://rcsmobility.com/xsd/xml_reservation_function.xsd\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"" <<
				" id=\"" << (resa.get() ? lexical_cast<string>(resa->getKey()) : string()) << "\"" <<
				" customerId=\"" << (resa.get() ? lexical_cast<string>(resa->getCustomerUserId()) : string()) << "\"" <<
				" cancellationDeadLine=\"" << (resa.get() ? posix_time::to_iso_extended_string(resa->getReservationDeadLine().toPosixTime()) : string()) << "\"" <<
				" departureStop=\"" << (resa.get() ? string()  : string()) << "\"" <<
				" arrivalStop=\"" << (resa.get() ? string()  : string()) << "\"" <<
				" travelDate=\"" << (resa.get() ? string()  : string()) << "\"" <<
				" customerName=\"" << (resa.get() ? resa->getCustomerName() : string()) << "\"" <<
				" customerPhone=\"" << (resa.get() ? resa->getCustomerPhone() : string()) << "\"" <<
				" status=\"" << (resa.get() ? ResaModule::GetStatusText(resa->getStatus()) : "N.A.") << "\"" <<
				" canBeCancelled=\"" << (resa.get() ? lexical_cast<string>(resa->getReservationDeadLine() > DateTime(TIME_CURRENT)) : "0") << "\"" <<
				" seats=\"" << (resa.get() ? lexical_cast<string>(resa->getSeats()) : "0") << "\"";
			if(resa.get() && !resa->getCancellationTime().isUnknown())
			{
				stream << " cancellationDateTime=\"" << posix_time::to_iso_extended_string(resa->getCancellationTime().toPosixTime()) << "\"";
			}
			stream << ">";

			if(resa.get())
			{
				BOOST_FOREACH(const Reservation* r, resa->getReservations())
				{
					stream <<
						"<chunk" <<
						" departurePlaceName=\"" << r->getDeparturePlaceName() << "\"" <<
						" departureDateTime=\"" << posix_time::to_iso_extended_string(r->getDepartureTime().toPosixTime()) << "\"" <<
						" arrivalPlaceName=\"" << r->getArrivalPlaceName() << "\"" <<
						" arrivalDateTime=\"" << posix_time::to_iso_extended_string(r->getArrivalTime().toPosixTime()) << "\"" <<
						" lineNumber=\"" << r->getLineCode() << "\"" <<
						" />";
				}
			}
			stream << "</reservation>";
		}
		
		
		
		bool XMLReservationFunction::_isAuthorized() const
		{
			return
				_request->isAuthorized<ResaRight>(WRITE) ||
				_request->isAuthorized<ResaRight>(UNKNOWN_RIGHT_LEVEL, WRITE);
		}



		std::string XMLReservationFunction::getOutputMimeType() const
		{
			return "text/xml";
		}



		XMLReservationFunction::XMLReservationFunction():
		FactorableTemplate<server::Function,XMLReservationFunction>()
		{
			setEnv(shared_ptr<Env>(new Env));
		}
	}
}

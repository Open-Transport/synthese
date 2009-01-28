
/** ReservationsListInterfaceElement class implementation.
	@file ReservationsListInterfaceElement.cpp
	@author Hugues Romain
	@date 2007

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

#include "ReservationsListInterfaceElement.h"

#include "ReservationTransactionTableSync.h"
#include "ReservationTableSync.h"
#include "ReservationTransaction.h"
#include "Reservation.h"
#include "ReservationItemInterfacePage.h"

#include "CommercialLine.h"
#include "CommercialLineTableSync.h"
#include "ScheduledService.h"
#include "ScheduledServiceTableSync.h"
#include "UseRule.h"
#include "AdvancedSelectTableSync.h"

#include "ValueElementList.h"
#include "InterfacePageException.h"
#include "Interface.h"

#include "HTMLTable.h"

#include "Date.h"

#include "Conversion.h"

#include "Request.h"
#include "Session.h"

#include "User.h"

#include <map>
#include <boost/foreach.hpp>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace interfaces;
	using namespace server;
	using namespace html;
	using namespace time;
	using namespace env;
	using namespace util;
	using namespace graph;
	
	namespace util
	{
		template<> const string FactorableTemplate<LibraryInterfaceElement, resa::ReservationsListInterfaceElement>::FACTORY_KEY("reservations_list");
	}

	namespace resa
	{


		typedef struct
		{
			typedef vector<shared_ptr<ReservationTransaction> > Reservations;
			const Service*			service;
			Reservations			reservations;
			int						seatsNumber;
			bool					overflow;
			bool					status;
			
			shared_ptr<Reservation>	getReservation(
				const ReservationTransaction* transaction
			) const	{
				const ReservationTransaction::Reservations& r(transaction->getReservations());
				for (ReservationTransaction::Reservations::const_iterator ite(r.begin()); ite != r.end(); ++ite)
					if ((*ite)->getServiceId() == service->getKey())
						return *ite;
			}
		} ServiceReservations ;

		void ReservationsListInterfaceElement::storeParameters(ValueElementList& vel)
		{
			if (vel.size() != 5)
				throw InterfacePageException("Bad parameter value in ReservationsListInterfaceElement");

			_displayFutureReservations = vel.front();
			_displayOldReservations = vel.front();
			_displayCanceledReservations = vel.front();
			_first = vel.front();
			_number = vel.front();
		}

		string ReservationsListInterfaceElement::display(
			ostream& stream
			, const ParametersVector& parameters
			, VariablesMap& variables
			, const void* object /*= NULL*/
			, const server::Request* request /*= NULL*/
		) const {

//			bool displayCanceled(_displayCancelled ? Conversion::ToBool(_displayCancelled->getValue(parameters, variables, object, request)) : false);
//			string titleCellStyle = _titleCellStyle->getValue(parameters, variables, object, request);
//			string tableStyle = _tableStyle->getValue(parameters, variables, object, request);

			if (!request->getSession())
				return string();

			DateTime minDate(TIME_UNKNOWN);
			DateTime maxDate(TIME_UNKNOWN);

			if (Conversion::ToBool(_displayFutureReservations->getValue(parameters, variables, object, request)))
				minDate = DateTime(TIME_CURRENT);
			if (Conversion::ToBool(_displayOldReservations->getValue(parameters, variables, object, request)))
				maxDate = DateTime(TIME_CURRENT);

			Env env;
			ReservationTransactionTableSync::Search(
				env,
				request->getSession()->getUser()->getKey()
				, minDate
				, maxDate
				, Conversion::ToBool(_displayCanceledReservations->getValue(parameters, variables, object, request))
				, Conversion::ToInt(_first->getValue(parameters, variables, object, request))
				, Conversion::ToInt(_number->getValue(parameters, variables, object, request))
				, DOWN_LINKS_LOAD_LEVEL
			);
			BOOST_FOREACH(shared_ptr<ReservationTransaction> tran, env.getRegistry<ReservationTransaction>())
			{
				const ReservationItemInterfacePage* page(_page->getInterface()->getPage<ReservationItemInterfacePage>());
				page->display(stream, *tran, variables, request);
			}

			return string();
		}

		ReservationsListInterfaceElement::ReservationsListInterfaceElement()
			: util::FactorableTemplate<interfaces::LibraryInterfaceElement, ReservationsListInterfaceElement>()
		{

		}


		ReservationsListInterfaceElement::~ReservationsListInterfaceElement()
		{
		}
	}
}

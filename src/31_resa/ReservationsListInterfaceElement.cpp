
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

#include "31_resa/ReservationTransactionTableSync.h"
#include "31_resa/ReservationTableSync.h"
#include "31_resa/ReservationTransaction.h"
#include "31_resa/Reservation.h"
#include "31_resa/ReservationItemInterfacePage.h"

#include "15_env/CommercialLine.h"
#include "15_env/CommercialLineTableSync.h"
#include "15_env/ScheduledService.h"
#include "15_env/ScheduledServiceTableSync.h"
#include "15_env/ReservationRule.h"
#include "15_env/AdvancedSelectTableSync.h"

#include "11_interfaces/ValueElementList.h"
#include "11_interfaces/InterfacePageException.h"
#include "11_interfaces/Interface.h"

#include "05_html/HTMLTable.h"

#include "04_time/Date.h"

#include "01_util/Conversion.h"

#include "30_server/Request.h"
#include "30_server/Session.h"

#include "12_security/User.h"

#include <map>

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
					if ((*ite)->getServiceId() == service->getId())
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

			vector<shared_ptr<ReservationTransaction> > resats(ReservationTransactionTableSync::search(
				request->getSession()->getUser()->getKey()
				, minDate
				, maxDate
				, Conversion::ToBool(_displayCanceledReservations->getValue(parameters, variables, object, request))
				, Conversion::ToInt(_first->getValue(parameters, variables, object, request))
				, Conversion::ToInt(_number->getValue(parameters, variables, object, request))
			));
			for (vector<shared_ptr<ReservationTransaction> >::const_iterator itr(resats.begin()); itr != resats.end(); ++itr)
			{
				vector<shared_ptr<Reservation> > resas(ReservationTableSync::search(itr->get()));

				const ReservationItemInterfacePage* page(_page->getInterface()->getPage<ReservationItemInterfacePage>());
				page->display(stream, **itr, variables, request);
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


/** ResaCustomerAdmin class implementation.
	@file ResaCustomerAdmin.cpp
	@author Hugues Romain
	@date 2008

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

#include "ResaCustomerAdmin.h"
#include "ResaCustomersAdmin.h"
#include "ResaModule.h"

#include "31_resa/ReservationTransaction.h"
#include "31_resa/ReservationTransactionTableSync.h"
#include "31_resa/Reservation.h"
#include "31_resa/ReservationTableSync.h"

#include "05_html/HTMLTable.h"

#include "30_server/QueryString.h"

#include "32_admin/AdminParametersException.h"

#include "12_security/User.h"
#include "12_security/UserTableSync.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace admin;
	using namespace interfaces;
	using namespace server;
	using namespace util;
	using namespace resa;
	using namespace html;
	using namespace security;

	namespace util
	{
		template<> const string FactorableTemplate<AdminInterfaceElement, ResaCustomerAdmin>::FACTORY_KEY("ResaCustomerAdmin");
	}

	namespace admin
	{
		template<> const string AdminInterfaceElementTemplate<ResaCustomerAdmin>::ICON("user.png");
		template<> const string AdminInterfaceElementTemplate<ResaCustomerAdmin>::DEFAULT_TITLE("Client inconnu");
	}

	namespace resa
	{
		const string ResaCustomerAdmin::PARAMETER_DISPLAY_CANCELLED("dc");

		ResaCustomerAdmin::ResaCustomerAdmin()
			: AdminInterfaceElementTemplate<ResaCustomerAdmin>()
		{ }
		
		void ResaCustomerAdmin::setFromParametersMap(const ParametersMap& map)
		{
			uid id(map.getUid(QueryString::PARAMETER_OBJECT_ID, true, FACTORY_KEY));
			try
			{
				_customer = UserTableSync::Get(id);
			}
			catch (...)
			{
				throw AdminParametersException("Bad user id");
			}

			_displayCancelled = map.getBool(PARAMETER_DISPLAY_CANCELLED, false, false, FACTORY_KEY);
		}
		
		void ResaCustomerAdmin::display(ostream& stream, VariablesMap& variables, const FunctionRequest<AdminRequest>* request) const
		{
			HTMLTable t(0,"adminresults");
			stream << t.open();

			vector<shared_ptr<ReservationTransaction> > resats(ReservationTransactionTableSync::search(
				_customer->getKey()
				, _displayCancelled
				));
			for (vector<shared_ptr<ReservationTransaction> >::const_iterator itr(resats.begin()); itr != resats.end(); ++itr)
			{

				vector<shared_ptr<Reservation> > resas(ReservationTableSync::search(itr->get()));

				for (vector<shared_ptr<Reservation> >::const_iterator it(resas.begin()); it != resas.end(); ++it)
					ResaModule::DisplayReservation(stream, t, it->get());
			}

			stream << t.close();
		}

		bool ResaCustomerAdmin::isAuthorized(const FunctionRequest<AdminRequest>* request) const
		{
			/// @todo Implement the right control;
			return true;
		}
		
		AdminInterfaceElement::PageLinks ResaCustomerAdmin::getSubPagesOfParent(
			const PageLink& parentLink
			, const AdminInterfaceElement& currentPage
			, const server::FunctionRequest<admin::AdminRequest>* request
		) const	{
			AdminInterfaceElement::PageLinks links;
			/// @todo Implement it or leave empty
			// Example
			// if(parentLink.factoryKey == ResaCustomersAdmin::FACTORY_KEY && parentLink.parameterValue == ResaModule::FACTORY_KEY)
			//	links.push_back(getPageLink());
			return links;
		}
		
		AdminInterfaceElement::PageLinks ResaCustomerAdmin::getSubPages(
			const PageLink& parentLink
			, const AdminInterfaceElement& currentPage
			, const server::FunctionRequest<admin::AdminRequest>* request
		) const {
			AdminInterfaceElement::PageLinks links;
			/// @todo Implement it or remove the method to get the default behaviour
			return links;
		}


		std::string ResaCustomerAdmin::getTitle() const
		{
			return _customer.get() ? _customer->getName() : DEFAULT_TITLE;
		}

		std::string ResaCustomerAdmin::getParameterName() const
		{
			return _customer.get() ? QueryString::PARAMETER_OBJECT_ID : string();
		}

		std::string ResaCustomerAdmin::getParameterValue() const
		{
			return _customer.get() ? Conversion::ToString(_customer->getKey()) : string();
		}
	}
}

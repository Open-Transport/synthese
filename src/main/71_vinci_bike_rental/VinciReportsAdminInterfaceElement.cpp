
/** VinciReportsAdminInterfaceElement class implementation.
	@file VinciReportsAdminInterfaceElement.cpp

	This file belongs to the VINCI BIKE RENTAL SYNTHESE module
	Copyright (C) 2006 Vinci Park 
	Contact : Raphaël Murat - Vinci Park <rmurat@vincipark.com>

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

#include "04_time/Date.h"

#include "VinciReportsAdminInterfaceElement.h"

using namespace std;

namespace synthese
{
	using namespace admin;
	using namespace interfaces;
	using namespace server;
	using namespace time;

	namespace vinci
	{
		VinciReportsAdminInterfaceElement::VinciReportsAdminInterfaceElement()
			: AdminInterfaceElement("home", AdminInterfaceElement::EVER_DISPLAYED) {}

		const std::string VinciReportsAdminInterfaceElement::PARAM_START_DATE = "vraiepsd";
		const std::string VinciReportsAdminInterfaceElement::PARAM_END_DATE = "vraieped";

		string VinciReportsAdminInterfaceElement::getTitle() const
		{
			return "Etats journaliers";
		}

		void VinciReportsAdminInterfaceElement::display(ostream& stream, const Request* request) const
		{
			// Report Launch request
			AdminRequest* updateRequest = Factory<Request>::create<AdminRequest>();
			updateRequest->copy(request);
			updateRequest->setPage(Factory<AdminInterfaceElement>::create<VinciCustomerAdminInterfaceElement>());
			updateRequest->setAction(Factory<Action>::create<VinciUpdateCustomerAction>());

			stream
				<< "<table>"
				<< "<tr><td>Date début (AAAA/MM/JJ)</td><td><input name=\"" << PARAM_START_DATE << " /></td></tr>"
				<< "<tr><td>Date fin (AAAA/MM/JJ)</td><td><input name=\"" <<¨PARAM_END_DATE << " /></td></tr>"
				<< "<tr><td>Nombre de locations</td><td></td></tr>"
				<< "<tr><td>Nombre de validations</td><td></td></tr>"
				<< "<tr><td>Encaissements effectués</td><td></td></tr>"
				<< "<tr><td>tri par tarif</td><td></td></tr>"
				<< "</table>"
				;
		}

		void VinciReportsAdminInterfaceElement::setFromParametersMap(const Request::ParametersMap& map)
		{
			Date startDate;
			Date endDate;
			Request::ParametersMap::iterator it;
			it = map.find(PARAM_START_DATE);
			if (it != map.end())
				startDate = Date::FromSQLDate(it->second);
			it = map.find(PARAM_END_DATE);
			if (it != map.end())
				endDate = Date::FromSQLDate(it->second);
            if (!startDate.isUnknown() && !endDate.isUnknown())
			{
				
			}
		}
	}
}


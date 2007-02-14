
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

#include "32_admin/AdminRequest.h"

#include "VinciReportsAdminInterfaceElement.h"
#include "VinciBikeRentalModule.h"

#include "57_accounting/TransactionPartTableSync.h"

using namespace std;

namespace synthese
{
	using namespace admin;
	using namespace interfaces;
	using namespace server;
	using namespace time;
	using namespace accounts;

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

		void VinciReportsAdminInterfaceElement::display(ostream& stream, const AdminRequest* request) const
		{
			// Report Launch request
			AdminRequest* reportRequest = Factory<Request>::create<AdminRequest>();
			reportRequest->copy(request);
			reportRequest->setPage(Factory<AdminInterfaceElement>::create<VinciReportsAdminInterfaceElement>());
			
			stream
				<< reportRequest->getHTMLFormHeader("report")
				<< "<table>"
				<< "<tr><td>Date début (AAAA/MM/JJ)</td><td><input name=\"" << PARAM_START_DATE << "\" ";
			if (!_startDate.isUnknown())
				stream << " value=\"" << _startDate.toSQLString(false) << "\"";
			stream << " /></td></tr>"
				<< "<tr><td>Date fin (AAAA/MM/JJ)</td><td><input name=\"" << PARAM_END_DATE << "\" ";
			if (!_endDate.isUnknown())
				stream << " value=\"" << _endDate.toSQLString(false) << "\"";
			stream << "	/></td></tr>"
				<< "<tr><td>Nombre de locations</td><td></td></tr>"
				<< "<tr><td>Nombre de validations</td><td></td></tr>"
				<< "<tr><td>Encaissements effectués</td><td></td></tr>"
				<< "<tr><td>tri par tarif</td><td></td></tr>"
				<< "</table>"
				<< "<input type=\"submit\" value=\"OK\" /></form>"
				<< "<table>"
				;
			for(map<int,int>::const_iterator it = _results.begin(); it != _results.end(); ++it)
			{
				stream << "<tr><td>" << it->first << "</td><td>" << it->second << "</td></tr>";
			}
			stream << "</table>";
		}

		void VinciReportsAdminInterfaceElement::setFromParametersMap(const AdminRequest::ParametersMap& map)
		{
			Request::ParametersMap::const_iterator it;
			it = map.find(PARAM_START_DATE);
			if (it != map.end())
				_startDate = Date::FromSQLDate(it->second);
			it = map.find(PARAM_END_DATE);
			if (it != map.end())
				_endDate = Date::FromSQLDate(it->second);
            if (!_startDate.isUnknown() && !_endDate.isUnknown())
			{
				_results = TransactionPartTableSync::count(VinciBikeRentalModule::getAccount(VinciBikeRentalModule::VINCI_SERVICES_BIKE_RENT_TICKETS_ACCOUNT_CODE), _startDate, _endDate);
			}
		}
	}
}


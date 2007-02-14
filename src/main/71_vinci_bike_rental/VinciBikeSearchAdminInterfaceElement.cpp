
/** VinciBikeSearchAdminInterfaceElement class implementation.
	@file VinciBikeSearchAdminInterfaceElement.cpp

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

#include "32_admin/AdminRequest.h"

#include "71_vinci_bike_rental/VinciAddBike.h"
#include "71_vinci_bike_rental/VinciBike.h"
#include "71_vinci_bike_rental/VinciBikeTableSync.h"
#include "71_vinci_bike_rental/VinciBikeAdminInterfaceElement.h"
#include "71_vinci_bike_rental/VinciBikeSearchAdminInterfaceElement.h"

using namespace std;

namespace synthese
{
	using namespace admin;
	using namespace interfaces;
	using namespace server;

	namespace vinci
	{
		const std::string VinciBikeSearchAdminInterfaceElement::PARAMETER_SEARCH_NUMBER = "searchnumber";
		const std::string VinciBikeSearchAdminInterfaceElement::PARAMETER_SEARCH_CADRE = "searchcadre";

		VinciBikeSearchAdminInterfaceElement::VinciBikeSearchAdminInterfaceElement()
			: AdminInterfaceElement("home", AdminInterfaceElement::EVER_DISPLAYED) {}

		string VinciBikeSearchAdminInterfaceElement::getTitle() const
		{
			return "Parc de vélos";
		}

		void VinciBikeSearchAdminInterfaceElement::display(ostream& stream, const AdminRequest* request) const
		{
			// AddStatus
			AdminRequest* addStatusRequest = Factory<Request>::create<AdminRequest>();
			addStatusRequest->copy(request);
			addStatusRequest->setPage(Factory<AdminInterfaceElement>::create<VinciBikeSearchAdminInterfaceElement>());

			// AddBike
			AdminRequest* addBikeRequest = Factory<Request>::create<AdminRequest>();
			addBikeRequest->copy(request);
			addBikeRequest->setPage(Factory<AdminInterfaceElement>::create<VinciBikeAdminInterfaceElement>());
			addBikeRequest->setAction(Factory<Action>::create<VinciAddBike>());

			// Open a bike
			AdminRequest* viewBikeRequest = Factory<Request>::create<AdminRequest>();
			viewBikeRequest->copy(request);
			viewBikeRequest->setPage(Factory<AdminInterfaceElement>::create<VinciBikeAdminInterfaceElement>());

			stream
				<< addStatusRequest->getHTMLFormHeader("search")
				<< "<h1>Recherche de vélo</h1>"
				<< "Numéro : <input name=\"" << PARAMETER_SEARCH_NUMBER << "\" /> "
				<< "Cadre : <input name=\"" << PARAMETER_SEARCH_CADRE << "\" /> "
				<< "<input type=\"submit\" value=\"Rechercher\" />"
				<< "</form>"
				<< "<h1>Résultat de la recherche</h1>"
				;

			stream
				<< addBikeRequest->getHTMLFormHeader("create")
				<< "<table><tr><th>Numéro</th><th>Cadre</th></tr>"
				;
			for (vector<VinciBike*>::const_iterator it = _bikes.begin(); it != _bikes.end(); ++it)
			{
				viewBikeRequest->setObjectId((*it)->getKey());
				stream
					<< "<tr>"
					<< "<td>" << viewBikeRequest->getHTMLLink((*it)->getNumber()) << "</td>"
					<< "<td>" << viewBikeRequest->getHTMLLink((*it)->getMarkedNumber()) << "</td>"
					<< "</tr>";
			}
			if (_activeSearch)
			{
				stream
					<< "<tr>"
					<< "<td><input name=\"" << VinciAddBike::PARAMETER_NUMBER << "\" value=\"" << _bikeNumber << "\" /></td>"
					<< "<td><input name=\"" << VinciAddBike::PARAMETER_MARKED_NUMBER << "\" value=\"" << _cadreNumber << "\" /></td>"
					<< "<td><input type=\"submit\" value=\"Créer\" /></td>"
					<< "</tr>"
					;
			}
			stream << "</table></form>";

			delete addStatusRequest;
		}

		void VinciBikeSearchAdminInterfaceElement::setFromParametersMap(const admin::AdminRequest::ParametersMap& map)
		{
			server::Request::ParametersMap::const_iterator it = map.find(PARAMETER_SEARCH_NUMBER);
			if (it != map.end())
				_bikeNumber = it->second;

			it  = map.find(PARAMETER_SEARCH_CADRE);
			if (it != map.end())
                _cadreNumber = it->second;
			
			_bikes = VinciBikeTableSync::search(_bikeNumber, _cadreNumber);
			_activeSearch = (_cadreNumber != "" || _bikeNumber != "");
		}

		VinciBikeSearchAdminInterfaceElement::~VinciBikeSearchAdminInterfaceElement()
		{
			for (vector<VinciBike*>::iterator it = _bikes.begin(); it != _bikes.end(); ++it)
				delete *it;
		}
	}
}


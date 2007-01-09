
/** VinciBikeAdminInterfaceElement class implementation.
	@file VinciBikeAdminInterfaceElement.cpp

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

#include "71_vinci_bike_rental/VinciBike.h"
#include "71_vinci_bike_rental/VinciBikeTableSync.h"
#include "71_vinci_bike_rental/VinciBikeAdminInterfaceElement.h"

namespace synthese
{
	using namespace server;
	using namespace admin;
	using namespace util;

	namespace vinci
	{
		VinciBikeAdminInterfaceElement::VinciBikeAdminInterfaceElement()
			: AdminInterfaceElement("vincibikes", AdminInterfaceElement::DISPLAYED_IF_CURRENT) {}


		std::string VinciBikeAdminInterfaceElement::getTitle() const
		{
			return "Vélo " + _bike->getNumber();
		}

		void VinciBikeAdminInterfaceElement::display(std::ostream& stream, const server::Request* request /*= NULL*/ ) const
		{
			// Update bike request
			AdminRequest* updateBikeRequest = Factory<Request>::create<AdminRequest>();
			updateBikeRequest->copy(request);
			updateBikeRequest->setPage(Factory<AdminInterfaceElement>::create<VinciBikeAdminInterfaceElement>());

			// Display of data board
			stream
				<< "<h1>Données</h1>"
				<< updateBikeRequest->getHTMLFormHeader("update")
				<< "<table>"
				<< "<tr><td>Numéro :</td><td><input value=\"" << _bike->getNumber() << "\" /></td></tr>"
				<< "<tr><td>Cadre :</td><td><input value=\"" << _bike->getMarkedNumber() << "\" /></td></tr>"
				<< "</table></form>"
				;

			// Display of history
			stream
				<< "<h1>Historique</h1>"
				;

			// Cleaning
			delete updateBikeRequest;
		}

		void VinciBikeAdminInterfaceElement::setFromParametersMap(const server::Request::ParametersMap& map)
		{
			Request::ParametersMap::const_iterator it = map.find(Request::PARAMETER_OBJECT_ID);
			if (it != map.end())
				_bike = VinciBikeTableSync::get(Conversion::ToLongLong(it->second));
		}
	}
}

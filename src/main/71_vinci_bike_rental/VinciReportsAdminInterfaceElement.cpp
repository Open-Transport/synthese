
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

#include "VinciReportsAdminInterfaceElement.h"

using namespace std;

namespace synthese
{
	using namespace admin;
	using namespace interfaces;
	using namespace server;

	namespace vinci
	{
		VinciReportsAdminInterfaceElement::VinciReportsAdminInterfaceElement()
			: AdminInterfaceElement("home", AdminInterfaceElement::EVER_DISPLAYED) {}

		string VinciReportsAdminInterfaceElement::getTitle() const
		{
			return "Etats journaliers";
		}

		void VinciReportsAdminInterfaceElement::display(ostream& stream, const Request* request) const
		{
			stream
				<< "<table>"
				<< "<tr><td>Date début</td><td><input /></td></tr>"
				<< "<tr><td>Date fin</td><td><input /></td></tr>"
				<< "<tr><td>Nombre de locations</td><td></td></tr>"
				<< "<tr><td>Nombre de validations</td><td></td></tr>"
				<< "<tr><td>Encaissements effectués</td><td></td></tr>"
				<< "<tr><td>tri par tarif</td><td></td></tr>"
				<< "</table>"
				;
		}

		void VinciReportsAdminInterfaceElement::setFromParametersMap(const server::Request::ParametersMap& map)
		{

		}
	}
}


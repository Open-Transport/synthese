
/** BroadcastPointAdmin class implementation.
	@file BroadcastPointAdmin.cpp

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

#include "01_util/Html.h"

#include "BroadcastPointAdmin.h"

using namespace std;

namespace synthese
{
	using namespace admin;
	using namespace interfaces;
	using namespace server;
	using namespace util;

	namespace departurestable
	{
		BroadcastPointAdmin::BroadcastPointAdmin()
			: AdminInterfaceElement("broadcastpoints", AdminInterfaceElement::DISPLAYED_IF_CURRENT) {}

		void BroadcastPointAdmin::setFromParametersMap(const server::Request::ParametersMap& map)
		{
			/// @todo Initialize internal attributes from the map
		}

		string BroadcastPointAdmin::getTitle() const
		{
			/// @todo Change the title of the page
			return "title";
		}

		void BroadcastPointAdmin::display(ostream& stream, const Request* request) const
		{
			stream
				<< "<h1>Emplacements d'affichage de la zone d'arrêt</h1>"
				<< "<table>";

			// Physical stop list

			{
				stream
					<< "<tr><td>Arrêt physique " << "</td>"
					<< "<td>" << Html::getSubmitButton("Activer") << "</td>"
					<< "</tr>";
			}

			// Other broadcastpoints list
			{
				stream
					<< "<tr><td>" << Html::getTextInput("", "") << "</td>"
					<< "<td>" << Html::getSubmitButton("Renommer") << "</td>"
					<< "</tr>";
			}

			stream
				<< "<tr><td>" << Html::getTextInput("", "", "(entrez le nom ici)") << "</td>"
				<< "<td>" << Html::getSubmitButton("Ajouter") << "</td>"
				<< "</tr>";
		}
	}
}

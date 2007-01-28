
/** DisplayTypesAdmin class implementation.
	@file DisplayTypesAdmin.cpp

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

#include "DisplayTypesAdmin.h"

using namespace std;

namespace synthese
{
	using namespace admin;
	using namespace interfaces;
	using namespace server;

	namespace departurestable
	{
		/// @todo Verify the parent constructor parameters
		DisplayTypesAdmin::DisplayTypesAdmin()
			: AdminInterfaceElement("displays", AdminInterfaceElement::EVER_DISPLAYED) {}

		void DisplayTypesAdmin::setFromParametersMap(const server::Request::ParametersMap& map)
		{
			/// @todo Initialize internal attributes from the map
		}

		string DisplayTypesAdmin::getTitle() const
		{
			return "Types d'afficheurs";
		}

		void DisplayTypesAdmin::display(ostream& stream, const Request* request) const
		{
			stream
				<< "<P>Liste des types d'afficheurs disponibles :</P>"
				<< "<TABLE><TR><th>Nom</th><th>Interface</th><th>Lignes</th><th>Actions</th></tr>";

			// Display types list
			{
				stream
					<< "<TR>"
					<< "<TD><INPUT type=\"text\" size=\"14\" value=\"Oscar\" name=\"Text2\"></TD>"
					<< "<TD><SELECT name=\"Select1\">";

				// Interfaces list
				
				stream
					<< "</SELECT></TD>"
					<< "<TD><SELECT name=\"Select2\">"
					<< "<OPTION value=\"\" selected> </OPTION>";

				for (int i=1; i<100; ++i)
					stream << "<option value=\"" << i << "\">" << i << "</option>";

				stream
					<< "</SELECT></TD>"
					<< "<TD><INPUT type=\"button\" name=\"Modifier\"></TD>"
					<< "</TR>"
					;
			}

			// New type
			stream
				<< "<TR>"
				<< "<TD><input type=\"text\" size=\"14\" value=\"(Entrez le nom ici)\" name=\"Text2\" /></TD>"
				<< "<TD><SELECT name=\"Select2\">"
				<< "<OPTION value=\"\" selected> </OPTION>";

			for (int i=1; i<100; ++i)
				stream << "<option value=\"" << i << "\">" << i << "</option>";

			stream
				<< "</SELECT></TD>"
				<< "<TD><INPUT type=\"button\" name=\"Ajouter\"></TD>"
				<< "</TR></TABLE>";
		}
	}
}

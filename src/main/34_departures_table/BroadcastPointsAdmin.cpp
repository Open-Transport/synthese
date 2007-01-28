
/** BroadcastPointsAdmin class implementation.
	@file BroadcastPointsAdmin.cpp

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

#include "BroadcastPointsAdmin.h"

using namespace std;

namespace synthese
{
	using namespace admin;
	using namespace interfaces;
	using namespace server;

	namespace departurestable
	{
		BroadcastPointsAdmin::BroadcastPointsAdmin()
			: AdminInterfaceElement("home", AdminInterfaceElement::EVER_DISPLAYED) {}

		void BroadcastPointsAdmin::setFromParametersMap(const server::Request::ParametersMap& map)
		{
			/// @todo Initialize internal attributes from the map
		}

		string BroadcastPointsAdmin::getTitle() const
		{
			return "Points de diffusion";
		}

		void BroadcastPointsAdmin::display(ostream& stream, const Request* request) const
		{
			stream
				<< "<table><tr><td>Commun</td><td><INPUT type=\"text\" name=\"Text2\" /></td>"
				<< "<td>Nom</td><td><INPUT type=\"text\" name=\"Text2\" /></td>"
				<< "<td>Terminaux d'affichage</td><td><SELECT name=\"Select1\">"
				<< "<OPTION selected value=\"\">(filtre désactivé)</OPTION>"
				<< "<OPTION value=\"\">Au moins un</OPTION>"
				<< "<OPTION value=\"\">Aucun</OPTION>"
				<< "</SELECT></td></tr>"
				<< "<tr><td>Ligne</td><td><SELECT name=\"Select1\">"
				<< "<OPTION value=\"\" selected>(toutes)</OPTION>";

			// Lines list
				
			stream
				<< "</SELECT></td></tr>"
				<< "<tr><td colspan=\"6\"><INPUT type=\"button\" value=\"Rechercher\" name=\"Button6\" /></td></tr></table>"

				<< "<P>Résultats de la recherche (tous) :</P>"
				<< "<TABLE><TR>"
				<< "<th>Commune</th><th>Nom zone d'arrêt</th><th>Actions</th></tr>";

			// Places list
			{
				stream
					<< "<TR><td>TOULOUSE</td><td>Matabiau</td>"
					<< "<td><INPUT type=\"button\" value=\"Editer\" name=\"Modifier\"></td>"
					<< "</tr>";
			}
			stream
				<< "</TABLE>"
				<< "<P align=\"right\">Emplacements&nbsp;suivants &gt;</P>"
				<< "<P>Cliquer sur un titre de colonne pour trier le tableau.</P>";			
		}
	}
}

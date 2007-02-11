
/** DisplayMaintenanceAdmin class implementation.
	@file DisplayMaintenanceAdmin.cpp

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

#include "DisplayMaintenanceAdmin.h"

using namespace std;

namespace synthese
{
	using namespace admin;
	using namespace interfaces;
	using namespace server;
	using namespace util;

	namespace departurestable
	{
		/// @todo Verify the parent constructor parameters
		DisplayMaintenanceAdmin::DisplayMaintenanceAdmin()
			: AdminInterfaceElement("superior", AdminInterfaceElement::EVER_DISPLAYED) {}

		void DisplayMaintenanceAdmin::setFromParametersMap(const server::Request::ParametersMap& map)
		{
			/// @todo Initialize internal attributes from the map
		}

		string DisplayMaintenanceAdmin::getTitle() const
		{
			/// @todo Change the title of the page
			return "title";
		}

		void DisplayMaintenanceAdmin::display(ostream& stream, const Request* request) const
		{
			stream
				<< "<h1>Paramètres de maintenance</h1>"
				<< "<table>"
				<< "<tr><td>Nombre de contrôles par jour</td><td>" << Html::getSelectNumberInput("", 0, 1440, 0, 10) << "</td></tr>"
				<< "<tr><td>Afficheur déclaré en service</td><td>" << Html::getOuiNonRadioInput("", true) << "</td></tr>"
				<< "<tr><td>Message de maintenance</td><td>" << Html::getTextAreaInput("", "", 30, 3) << "</td></tr>"
				<< "<tr><td colspan=\"2\">" << Html::getSubmitButton("Enregistrer les modifications") << "</td></tr>"
				<< "</table></form>"
				<< "<h1>Contrôle de cohérence des données</h1>"
				<< "<table>"
				<< "<tr><td>Etat</td><td>" /*pastille* <FONT face=\"Wingdings\" color=\"#00cc00\">
				<FONT color=\"#ff9900\">l<FONT face=\"Verdana\"> <STRONG>Warning </STRONG><EM>(depuis 4j 22h 
				35min)</EM></FONT></FONT></FONT></P>" */ << "</td></tr>"
				<< "<tr><td>Motif de l'alerte</td><td>Aucune ligne ne dessert l'afficheur</td></tr>"
				<< "<tr><td>Date du dernier contrôle positif</td><td>13/9/2006 8:30</td></tr>"
				<< "</table>"
				<< "<h1>Contrôle du matériel d'affichage</h1>"
				<< "<table>"
				<< "<tr><td>Etat</td><td>" /*^pastille <FONT face=\"Wingdings\"><FONT color=\"#ff0000\">l<FONT face=\"Verdana\">
				<STRONG>Error </STRONG><EM>(depuis 1j 12h 23min)</EM></FONT></FONT></FONT></P> */ << "</td><tr>"
				<< "<tr><td>Date du dernier contrôle</td><td>21/09/2006 16:30</td></tr>"
				<< "<tr><td>Date du dernier contrôle positif : 20/09/2006 15:30</td></tr>"
				<< "<tr><td>Motif de l'alerte</td><td>Température trop élevée, terminal éteint</td></tr>"
				<< "<tr><td colspan=\"2\"><a href=\"admin_log_viewer.htm\">Accéder au journal de maintenance de l'afficheur</a></td></tr>"
				<< "</table>";
		}
	}
}


/** MessagesAdmin class implementation.
	@file MessagesAdmin.cpp

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

#include "MessagesAdmin.h"

using namespace std;

namespace synthese
{
	using namespace admin;
	using namespace interfaces;
	using namespace server;

	namespace messages
	{
		/// @todo Verify the parent constructor parameters
		MessagesAdmin::MessagesAdmin()
			: AdminInterfaceElement("home", AdminInterfaceElement::EVER_DISPLAYED) {}

		void MessagesAdmin::setFromParametersMap(const server::Request::ParametersMap& map)
		{
			/// @todo Initialize internal attributes from the map
		}

		string MessagesAdmin::getTitle() const
		{
			return "Messages";
		}

		void MessagesAdmin::display(ostream& stream, const Request* request) const
		{
			stream
				<< "<table><tr><td>Date d�but :</td><td><INPUT type=\"text\" size=\"14\" name=\"Text2\" /></td>"
				<< "<td>Date fin</td><td><INPUT type=\"text\" size=\"14\" name=\"Text2\" /></td>"
				<< "<td>Zone d'arr�t</td><td><SELECT name=\"Select1\">"
				<< "<option value=\"\">(tous les arr�ts)</option>";

			// List of stop zones

			stream
				<< "</SELECT></td></tr>"
				<< "<tr><td>Ligne</td><td><SELECT name=\"Select1\">"
				<< "<option value=\"\">(toutes les lignes)</option>";

			// List of lines

			stream
				<< "</SELECT></td><td>Statut</td><td><SELECT name=\"Select1\">"
				<< "<OPTION value=\"\">(tous les �tats)</OPTION>"
				<< "<OPTION value=\"\">Diffusion termin�e</OPTION>"
				<< "<OPTION value=\"\">En cours de diffusion</OPTION>"
				<< "<OPTION value=\"\">En cours avec date de fin</OPTION>"
				<< "<OPTION value=\"\">En cours sans date de fin</OPTION>"
				<< "<OPTION value=\"\">Diffusion ult�rieure</OPTION>"
				<< "</SELECT></td><td>Conflit</td>"
				
				<< "<td><SELECT name=\"Select1\">"
				<< "<OPTION value=\"\">(toutes situations)</OPTION>"
				<< "<OPTION value=\"\">Sans conflits</OPTION>"
				<< "<OPTION value=\"\">Prioritaires sur compl�mentaires</OPTION>"
				<< "<OPTION value=\"\">En conflit</OPTION>"
				<< "</SELECT></td></tr>"
				
				<< "<tr><td>Type</td><td><SELECT name=\"Select1\">"
				<< "<OPTION value=\"\">(tous les types)</OPTION>"
				<< "<OPTION value=\"\">Prioritaire</OPTION>"
				<< "<OPTION value=\"\">Compl�mentaire</OPTION>"
				<< "</SELECT></td></tr>"

				<< "<tr><td colspan=\"6\"><INPUT type=\"button\" value=\"Rechercher\" name=\"Button6\"></td></tr>"

				<< "</table>"

				<< "<P>R�sultats de la recherche (tous) :</P>"
				
				<< "<table>"
				<< "<TR><th>Sel</th><th>Dates</th><th>Message</th><th>Type</th><th>Etat</th><th>Confilt</th><th>Actions</th></tr>";

			// Messages list
			{
				stream
					<< "<tr><TD><INPUT type=\"radio\" value=\"Radio1\" name=\"RadioGroup\"></TD>"
					<< "<td>d�s le 2/9/2006</td>"
					<< "<td>Travaux � Esquriol...</td>"
					<< "<td>Compl�mentaire</td>"
					<< "<td></td>" // Bullet
					<< "<td></td>" // Bullet
					<< "<td><INPUT type=\"button\" value=\"Editer\" name=\"Modifier\">"
					<< "<INPUT type=\"button\" value=\"Arr�ter\" name=\"Modifier\"></TD>"
					<< "</TR>";
			}

			stream
				<< "</TABLE>"
				<< "<P>(s�lectionnez un message existant pour cr�er une copie)</P>"
				<< "<P><INPUT type=\"button\" value=\"Nouvelle diffusion de message\" name=\"Button7\"></P>"
				<< "<P><INPUT type=\"button\" value=\"Nouvelle diffusion de sc�nario\" name=\"Button7\">"
				<< "<SELECT name=\"Select1\">";

			// List of scenarios

			stream
				<< "</SELECT></P>"
				<< "<P align=\"right\">Messages&nbsp;suivants &gt;</P>"
				<< "<P>Cliquer sur un titre de colonne pour trier le tableau.</P>";
		}
	}
}


/** MessagesLibraryAdmin class implementation.
	@file MessagesLibraryAdmin.cpp

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

#include "MessagesLibraryAdmin.h"

using namespace std;

namespace synthese
{
	using namespace admin;
	using namespace interfaces;
	using namespace server;

	namespace messages
	{
		/// @todo Verify the parent constructor parameters
		MessagesLibraryAdmin::MessagesLibraryAdmin()
			: AdminInterfaceElement("messages", AdminInterfaceElement::EVER_DISPLAYED) {}

		void MessagesLibraryAdmin::setFromParametersMap(const server::Request::ParametersMap& map)
		{
			/// @todo Initialize internal attributes from the map
		}

		string MessagesLibraryAdmin::getTitle() const
		{
			return "Biblioth�que";
		}

		void MessagesLibraryAdmin::display(ostream& stream, const Request* request) const
		{
			stream
				<< "<h1>Mod�les de textes destin�s aux messages compl�mentaires</h1>"
				<< "<table>"
				<< "<tr><th>Nom</th><th>Texte&nbsp;court</th><th>Texte&nbsp;long</th><th>Actions</th></tr>";

			// List of text templates
			{
				stream
					<< "<TR>"
					<< "<TD><INPUT type=\"text\" size=\"13\" value=\"Travaux Esquirol\" name=\"Text1\" /></TD>"
					<< "<TD><INPUT type=\"text\" size=\"13\" value=\"Ligne d�vi�e Esquirol\" name=\"Text1\"></TD>"
					<< "<TD><INPUT type=\"text\" size=\"25\" value=\"La ligne 14 est d�vi�e � Esquirol\" name=\"Text1\"></td>"
					<< "<td><INPUT type=\"button\" value=\"Modifier\" name=\"Modifier\"><INPUT type=\"button\" value=\"Supprimer\" name=\"Modifier\"></TD>"
					<< "</tr>";
			}

			stream
				<< "<TR>"
				<< "<TD><INPUT type=\"text\" size=\"13\" name=\"Text1\"></TD>"
				<< "<TD><INPUT type=\"text\" size=\"13\" name=\"Text1\"></TD>"
				<< "<TD><INPUT type=\"text\" size=\"25\" name=\"Text1\"></TD>"
				<< "<TD><INPUT type=\"button\" value=\"Ajouter\" name=\"Button7\"></TD>"
				<< "</TR>"

				<< "</TABLE>"

				<< "<h1>Mod�les de textes destin�s aux messages prioritaires</h1>"

				<< "<table>"
				<< "<tr><th>Nom</th><th>Texte&nbsp;court</th><th>Texte&nbsp;long</th><th>Actions</th></tr>";

			// List of text templates
			{
				stream
					<< "<TR>"
					<< "<td><INPUT type=\"text\" size=\"13\" value=\"Gr�ve g�n�rale\" name=\"Text1\"></TD>"
					<< "<td><TEXTAREA name=\"Textarea1\" rows=\"2\" cols=\"11\">Aucune circulation&lt;br /&gt;R�seau en gr�ve</TEXTAREA></TD>"
					<< "<TD><TEXTAREA name=\"Textarea2\" rows=\"2\" cols=\"20\">Aucun service ne circule Le r�seau est en gr�ve</TEXTAREA></TD>"
					<< "<TD><INPUT value=\"Modifier\" name=\"Modifier\"><INPUT onclick=\"alert('Etes vous sur de vouloir supprimer le mod�le s�lectionn� ?')\" type=\"button\" value=\"Supprimer\" name=\"Modifier\"></td>"
					<< "</tr>";
			}

			stream
				<< "<TR>"
				<< "<TD><INPUT type=\"text\" size=\"13\" name=\"Text1\"></td>"
				<< "<TD><TEXTAREA name=\"Textarea1\" rows=\"2\" cols=\"11\"></TEXTAREA></TD>"
				<< "<TD><TEXTAREA name=\"Textarea2\" rows=\"2\" cols=\"20\"></TEXTAREA></TD>"
				<< "<TD><INPUT type=\"button\" value=\"Ajouter\" name=\"Button7\"></TD>"
				<< "</TR></TABLE>"

				<< "<h1>Sc�narios</h1>"
				<< "<table>"
				<< "<TR><th>Sel</th><th>Nom</th><th>Actions</th></tr>";

			// List of scenarios
			{
				stream
					<< "<TR>"
					<< "<TD><INPUT type=\"radio\" value=\"Radio2\" name=\"RadioGroup\"></TD>"
					<< "<TD>Interruption totale m�tro</TD>"
					<< "<TD><INPUT type=\"button\" value=\"Editer\" name=\"Modifier\" /><INPUT onclick=\"alert('Etes-vous s�r de vouloir supprimer le sc�nario s�lectionn� ?');\" type=\"button\" value=\"Supprimer\" name=\"Modifier\" /></TD>"
					<< "</tr>";
			}

			stream
				<< "<TR>"
				<< "<TD colSpan=\"2\">(s�lectionnez un&nbsp;sc�nario existant pour cr�er une copie)</td>"
				<< "<TD><INPUT type=\"button\" value=\"Ajouter\" name=\"Button7\" language=\"javascript\" onclick=\"location='admin_messages_scenarios.htm';\"></TD>"
				<< "</TR></TABLE>";
		}
	}
}

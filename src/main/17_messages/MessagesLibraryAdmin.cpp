
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

#include "01_util/Html.h"

#include "17_messages/MessagesLibraryAdmin.h"
#include "17_messages/TextTemplate.h"
#include "17_messages/TextTemplateTableSync.h"
#include "17_messages/UpdateTextTemplateAction.h"
#include "17_messages/DeleteTextTemplateAction.h"

using namespace std;

namespace synthese
{
	using namespace admin;
	using namespace interfaces;
	using namespace server;
	using namespace util;

	namespace messages
	{
		/// @todo Verify the parent constructor parameters
		MessagesLibraryAdmin::MessagesLibraryAdmin()
			: AdminInterfaceElement("messages", AdminInterfaceElement::EVER_DISPLAYED) {}

		void MessagesLibraryAdmin::setFromParametersMap(const AdminRequest::ParametersMap& map)
		{
			/// @todo Initialize internal attributes from the map
		}

		string MessagesLibraryAdmin::getTitle() const
		{
			return "Bibliothèque";
		}

		void MessagesLibraryAdmin::display(ostream& stream, const AdminRequest* request) const
		{
			AdminRequest* updateRequest = Factory<Request>::create<AdminRequest>();
			updateRequest->copy(request);
			updateRequest->setPage(Factory<AdminInterfaceElement>::create<MessagesLibraryAdmin>());
			updateRequest->setAction(Factory<Action>::create<UpdateTextTemplateAction>());

			AdminRequest* deleteRequest = Factory<Request>::create<AdminRequest>();
			updateRequest->copy(request);
			updateRequest->setPage(Factory<AdminInterfaceElement>::create<MessagesLibraryAdmin>());
			updateRequest->setAction(Factory<Action>::create<DeleteTextTemplateAction>());

			stream
				<< "<h1>Modèles de textes destinés aux messages complémentaires</h1>"
				<< "<table>"
				<< "<tr><th>Nom</th><th>Texte&nbsp;court</th><th>Texte&nbsp;long</th><th>Actions</th></tr>";

			vector<TextTemplate*> tw = TextTemplateTableSync::search(ALARM_LEVEL_WARNING);
			for (vector<TextTemplate*>::iterator itw = tw.begin(); itw != tw.end(); ++itw)
			{
				TextTemplate* t = *itw;
				stream
					<< "<tr>"
					<< updateRequest->getHTMLFormHeader("upd" + Conversion::ToString(t->getKey()))
					<< "<td>" << Html::getTextInput("", t->getName()) << "</td>"
					<< "<td>" << Html::getTextInput("", t->getShortMessage()) << "</td>"
					<< "<td>" << Html::getTextInput("", t->getLongMessage()) << "</td>"
					<< "<td>"
					<< Html::getHiddenInput(UpdateTextTemplateAction::PARAMETER_TEXT_ID, Conversion::ToString(t->getKey()))
					<< Html::getSubmitButton("Modifier") << "</td>"
					<< "</form>"
					<< "<td>" 
					<< deleteRequest->getHTMLFormHeader("del" + Conversion::ToString(t->getKey()))
					<< Html::getHiddenInput(DeleteTextTemplateAction::PARAMETER_TEXT_ID, Conversion::ToString(t->getKey()))
					<< Html::getSubmitButton("Supprimer") 
					<< "</form></td>"
					<< "</tr>";
				delete *itw;
			}

			stream
				<< "<TR>"
				<< "<TD><INPUT type=\"text\" size=\"13\" name=\"Text1\"></TD>"
				<< "<TD><INPUT type=\"text\" size=\"13\" name=\"Text1\"></TD>"
				<< "<TD><INPUT type=\"text\" size=\"25\" name=\"Text1\"></TD>"
				<< "<TD>" << Html::getSubmitButton("Ajouter") << "</TD>"
				<< "</TR>"

				<< "</TABLE>"

				<< "<h1>Modèles de textes destinés aux messages prioritaires</h1>"

				<< "<table>"
				<< "<tr><th>Nom</th><th>Texte&nbsp;court</th><th>Texte&nbsp;long</th><th>Actions</th></tr>";

			vector<TextTemplate*> te = TextTemplateTableSync::search(ALARM_LEVEL_ERROR);
			for (vector<TextTemplate*>::iterator ite = te.begin(); ite != te.end(); ++ite)
			{
				/// @todo See if we use textarea
				TextTemplate* t = *ite;
				stream
					<< "<tr>"
					<< updateRequest->getHTMLFormHeader("upd" + Conversion::ToString(t->getKey()))
					<< "<td>" << Html::getTextInput("", t->getName()) << "</td>"
					<< "<td>" << Html::getTextInput("", t->getShortMessage()) << "</td>"
					<< "<td>" << Html::getTextInput("", t->getLongMessage()) << "</td>"
					<< "<td>" 
					<< Html::getHiddenInput(UpdateTextTemplateAction::PARAMETER_TEXT_ID, Conversion::ToString(t->getKey()))
					<< Html::getSubmitButton("Modifier") << "</td></form>"
					<< "<td>"
					<< deleteRequest->getHTMLFormHeader("del" + Conversion::ToString(t->getKey()))
					<< Html::getHiddenInput(UpdateTextTemplateAction::PARAMETER_TEXT_ID, Conversion::ToString(t->getKey()))
					<< Html::getSubmitButton("Supprimer") 
					<< "</form></td>"
					<< "</tr>";
				delete *ite;
			}
			stream
				<< "<TR>"
				<< "<TD><INPUT type=\"text\" size=\"13\" name=\"Text1\"></td>"
				<< "<TD><TEXTAREA name=\"Textarea1\" rows=\"2\" cols=\"11\"></TEXTAREA></TD>"
				<< "<TD><TEXTAREA name=\"Textarea2\" rows=\"2\" cols=\"20\"></TEXTAREA></TD>"
				<< "<TD>" << Html::getSubmitButton("Ajouter") << "</TD>"
				<< "</TR></TABLE>"

				<< "<h1>Scénarios</h1>"
				<< "<table>"
				<< "<TR><th>Sel</th><th>Nom</th><th>Actions</th></tr>";

			// List of scenarios
			{
				stream
					<< "<TR>"
					<< "<TD><INPUT type=\"radio\" value=\"Radio2\" name=\"RadioGroup\"></TD>"
					<< "<TD>Interruption totale métro</TD>"
					<< "<TD>" << Html::getSubmitButton("Modifier") << Html::getSubmitButton("Supprimer") << "</TD>"
					<< "</tr>";
			}

			stream
				<< "<TR>"
				<< "<TD colSpan=\"2\">(sélectionnez un&nbsp;scénario existant pour créer une copie)</td>"
				<< "<TD>" << Html::getSubmitButton("Ajouter") << "</TD>"
				<< "</TR></TABLE>";
		}
	}
}

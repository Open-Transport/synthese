
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
#include "01_util/HtmlTable.h"

#include "17_messages/Scenario.h"
#include "17_messages/MessagesLibraryAdmin.h"
#include "17_messages/TextTemplate.h"
#include "17_messages/TextTemplateTableSync.h"
#include "17_messages/UpdateTextTemplateAction.h"
#include "17_messages/DeleteTextTemplateAction.h"
#include "17_messages/MessagesScenarioAdmin.h"
#include "17_messages/DeleteScenarioAction.h"
#include "17_messages/AddScenarioAction.h"
#include "17_messages/TextTemplateAddAction.h"
#include "17_messages/MessagesModule.h"

#include "32_admin/ResultHTMLTable.h"

using namespace std;

namespace synthese
{
	using namespace admin;
	using namespace interfaces;
	using namespace server;
	using namespace util;

	namespace messages
	{
		MessagesLibraryAdmin::MessagesLibraryAdmin()
			: AdminInterfaceElement("messages", AdminInterfaceElement::EVER_DISPLAYED) {}

		void MessagesLibraryAdmin::setFromParametersMap(const AdminRequest::ParametersMap& map)
		{
			
		}

		string MessagesLibraryAdmin::getTitle() const
		{
			return "Bibliothèque";
		}

		void MessagesLibraryAdmin::display(ostream& stream, interfaces::VariablesMap& variables, const AdminRequest* request) const
		{
			AdminRequest* updateRequest = Factory<Request>::create<AdminRequest>();
			updateRequest->copy(request);
			updateRequest->setPage(Factory<AdminInterfaceElement>::create<MessagesLibraryAdmin>());
			updateRequest->setAction(Factory<Action>::create<UpdateTextTemplateAction>());

			AdminRequest* deleteRequest = Factory<Request>::create<AdminRequest>();
			deleteRequest->copy(request);
			deleteRequest->setPage(Factory<AdminInterfaceElement>::create<MessagesLibraryAdmin>());
			deleteRequest->setAction(Factory<Action>::create<DeleteTextTemplateAction>());

			AdminRequest* addRequest = Factory<Request>::create<AdminRequest>();
			addRequest->copy(request);
			addRequest->setPage(Factory<AdminInterfaceElement>::create<MessagesLibraryAdmin>());
			addRequest->setAction(Factory<Action>::create<TextTemplateAddAction>());

			AdminRequest* updateScenarioRequest = Factory<Request>::create<AdminRequest>();
			updateScenarioRequest->copy(request);
			updateScenarioRequest->setPage(Factory<AdminInterfaceElement>::create<MessagesScenarioAdmin>());
			
			AdminRequest* deleteScenarioRequest = Factory<Request>::create<AdminRequest>();
			deleteScenarioRequest->copy(request);
			deleteScenarioRequest->setPage(Factory<AdminInterfaceElement>::create<MessagesLibraryAdmin>());
			deleteScenarioRequest->setAction(Factory<Action>::create<DeleteScenarioAction>());

			AdminRequest* addScenarioRequest = Factory<Request>::create<AdminRequest>();
			addScenarioRequest->copy(request);
			addScenarioRequest->setPage(Factory<AdminInterfaceElement>::create<MessagesScenarioAdmin>());
			addScenarioRequest->setAction(Factory<Action>::create<AddScenarioAction>());
			addScenarioRequest->setActionFailedPage(Factory<AdminInterfaceElement>::create<MessagesLibraryAdmin>());

			stream << "<h1>Modèles de textes destinés aux messages complémentaires</h1>";

			vector<string> h1;
			h1.push_back("Nom");
			h1.push_back("Texte&nbsp;court");
			h1.push_back("Texte&nbsp;long");
			h1.push_back("Actions");
			HtmlTable t1(h1);

			stream << t1.open();

			vector<TextTemplate*> tw = TextTemplateTableSync::search(ALARM_LEVEL_INFO);
			for (vector<TextTemplate*>::iterator itw = tw.begin(); itw != tw.end(); ++itw)
			{
				TextTemplate* t = *itw;
				stream
					<< "<tr>"
					<< updateRequest->getHTMLFormHeader("upd" + Conversion::ToString(t->getKey()))
					<< "<td>" << Html::getTextInput(UpdateTextTemplateAction::PARAMETER_NAME, t->getName()) << "</td>"
					<< "<td>" << Html::getTextInput(UpdateTextTemplateAction::PARAMETER_SHORT_MESSAGE, t->getShortMessage()) << "</td>"
					<< "<td>" << Html::getTextInput(UpdateTextTemplateAction::PARAMETER_LONG_MESSAGE, t->getLongMessage()) << "</td>"
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
				<< addRequest->getHTMLFormHeader("add1")
				<< "<tr>"
				<< "<td>" << Html::getTextInput(TextTemplateAddAction::PARAMETER_NAME, string()) << "</td>"
				<< "<td>" << Html::getTextInput(TextTemplateAddAction::PARAMETER_SHORT_MESSAGE, string()) << "</td>"
				<< "<td>" << Html::getTextInput(TextTemplateAddAction::PARAMETER_LONG_MESSAGE, string()) << "</td>"
				<< "<td>"
					<< Html::getHiddenInput(TextTemplateAddAction::PARAMETER_TYPE, Conversion::ToString((int) ALARM_LEVEL_INFO))
					<< Html::getSubmitButton("Ajouter") << "</td>"
				<< "</tr></form>";

			stream << t1.close();

			stream << "<h1>Modèles de textes destinés aux messages prioritaires</h1>";

			vector<string> h2;
			h2.push_back("Nom");
			h2.push_back("Texte&nbsp;court");
			h2.push_back("Texte&nbsp;long");
			h2.push_back("Actions");
			HtmlTable t2(h2);

			stream << t2.open();

			vector<TextTemplate*> te = TextTemplateTableSync::search(ALARM_LEVEL_WARNING);
			for (vector<TextTemplate*>::iterator ite = te.begin(); ite != te.end(); ++ite)
			{
				/// @todo See if we use textarea
				TextTemplate* t = *ite;
				stream
					<< "<tr>"
					<< updateRequest->getHTMLFormHeader("upd" + Conversion::ToString(t->getKey()))
					<< "<td>" << Html::getTextInput(UpdateTextTemplateAction::PARAMETER_NAME, t->getName()) << "</td>"
					<< "<td>" << Html::getTextInput(UpdateTextTemplateAction::PARAMETER_SHORT_MESSAGE, t->getShortMessage()) << "</td>"
					<< "<td>" << Html::getTextInput(UpdateTextTemplateAction::PARAMETER_LONG_MESSAGE, t->getLongMessage()) << "</td>"
					<< "<td>" 
					<< Html::getHiddenInput(UpdateTextTemplateAction::PARAMETER_TEXT_ID, Conversion::ToString(t->getKey()))
					<< Html::getSubmitButton("Modifier") << "</td></form>"
					<< "<td>"
					<< deleteRequest->getHTMLFormHeader("del" + Conversion::ToString(t->getKey()))
					<< Html::getHiddenInput(DeleteTextTemplateAction::PARAMETER_TEXT_ID, Conversion::ToString(t->getKey()))
					<< Html::getSubmitButton("Supprimer") 
					<< "</form></td>"
					<< "</tr>";
				delete *ite;
			}
			stream
				<< addRequest->getHTMLFormHeader("add2")
				<< "<tr>"
				<< "<td>" << Html::getTextInput(TextTemplateAddAction::PARAMETER_NAME, string()) << "</td>"
				<< "<td>" << Html::getTextInput(TextTemplateAddAction::PARAMETER_SHORT_MESSAGE, string()) << "</td>"
				<< "<td>" << Html::getTextInput(TextTemplateAddAction::PARAMETER_LONG_MESSAGE, string()) << "</td>"
				<< "<td>"
				<< Html::getHiddenInput(TextTemplateAddAction::PARAMETER_TYPE, Conversion::ToString((int) ALARM_LEVEL_WARNING))
				<< Html::getSubmitButton("Ajouter") << "</td>"
				<< "</tr></form>";

			stream << t2.close();

			stream << "<h1>Scénarios</h1>";

			ResultHTMLTable::HeaderVector h3;
			h3.push_back(make_pair(string(), "Nom"));
			h3.push_back(make_pair(string(), "Actions"));
			ResultHTMLTable t3(h3, NULL, string(), true, addScenarioRequest, AddScenarioAction::PARAMETER_TEMPLATE_ID);
			stream << t3.open();
			
			for (Scenario::Registry::const_iterator it = MessagesModule::getScenarii().begin(); it != MessagesModule::getScenarii().end(); ++it)
			{
				Scenario* scenario = it->second;
				updateScenarioRequest->setObjectId(scenario->getKey());
				deleteScenarioRequest->setObjectId(scenario->getKey());
				stream << t3.row();
				stream << t3.col() << scenario->getName();
				stream << t3.col() << Html::getLinkButton(updateScenarioRequest->getURL(), "Modifier")
					<< " " << Html::getLinkButton(deleteScenarioRequest->getURL(), "Supprimer", "Etes-vous sûr de vouloir supprimer le scénario " + scenario->getName() + " ?");
			}

			stream << t3.row();
			stream << t3.col() << Html::getTextInput(AddScenarioAction::PARAMETER_NAME, string(), "Entrez le nom ici");
			stream << t3.col() << Html::getHiddenInput(AddScenarioAction::PARAMETER_IS_TEMPLATE, Conversion::ToString(true))
				<< Html::getSubmitButton("Ajouter");
			stream << t3.close();

			delete deleteRequest;
			delete updateRequest;
			delete deleteScenarioRequest;
			delete updateScenarioRequest;
			delete addScenarioRequest;
			delete addRequest;
		}
	}
}

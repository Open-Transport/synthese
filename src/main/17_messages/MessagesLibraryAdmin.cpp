
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

#include "05_html/ActionResultHTMLTable.h"
#include "05_html/HTMLForm.h"

#include "17_messages/ScenarioTemplate.h"
#include "17_messages/ScenarioTableSync.h"
#include "17_messages/MessagesLibraryAdmin.h"
#include "17_messages/TextTemplate.h"
#include "17_messages/TextTemplateTableSync.h"
#include "17_messages/UpdateTextTemplateAction.h"
#include "17_messages/DeleteTextTemplateAction.h"
#include "17_messages/MessagesScenarioAdmin.h"
#include "17_messages/DeleteScenarioAction.h"
#include "17_messages/AddScenarioAction.h"
#include "17_messages/TextTemplateAddAction.h"

#include "30_server/ActionFunctionRequest.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace admin;
	using namespace interfaces;
	using namespace server;
	using namespace util;
	using namespace html;
	using namespace time;

	namespace messages
	{
		const std::string MessagesLibraryAdmin::PARAMETER_NAME = "nam";
		const std::string MessagesLibraryAdmin::PARAMETER_SHORT_TEXT = "stx";
		const std::string MessagesLibraryAdmin::PARAMETER_LONG_TEXT = "ltx";

		MessagesLibraryAdmin::MessagesLibraryAdmin()
			: AdminInterfaceElement("messages", AdminInterfaceElement::EVER_DISPLAYED) {}

		void MessagesLibraryAdmin::setFromParametersMap(const ParametersMap& map)
		{
			_requestParameters = ResultHTMLTable::getParameters(map, PARAMETER_NAME, ResultHTMLTable::UNLIMITED_SIZE);
		}

		string MessagesLibraryAdmin::getTitle() const
		{
			return "Bibliothèque";
		}

		void MessagesLibraryAdmin::display(ostream& stream, interfaces::VariablesMap& variables, const server::FunctionRequest<admin::AdminRequest>* request) const
		{
			FunctionRequest<AdminRequest> searchRequest(request);
			searchRequest.getFunction()->setPage<MessagesLibraryAdmin>();

			ActionFunctionRequest<UpdateTextTemplateAction,AdminRequest> updateRequest(request);
			updateRequest.getFunction()->setPage<MessagesLibraryAdmin>();
			
			ActionFunctionRequest<DeleteTextTemplateAction,AdminRequest> deleteRequest(request);
			deleteRequest.getFunction()->setPage<MessagesLibraryAdmin>();
			
			ActionFunctionRequest<TextTemplateAddAction,AdminRequest> addRequest(request);
			addRequest.getFunction()->setPage<MessagesLibraryAdmin>();
			
			FunctionRequest<AdminRequest> updateScenarioRequest(request);
			updateScenarioRequest.getFunction()->setPage<MessagesScenarioAdmin>();
			
			ActionFunctionRequest<DeleteScenarioAction,AdminRequest> deleteScenarioRequest(request);
			deleteScenarioRequest.getFunction()->setPage<MessagesLibraryAdmin>();
			
			ActionFunctionRequest<AddScenarioAction,AdminRequest> addScenarioRequest(request);
			addScenarioRequest.getFunction()->setPage<MessagesScenarioAdmin>();
			addScenarioRequest.getFunction()->setActionFailedPage(Factory<AdminInterfaceElement>::create<MessagesLibraryAdmin>());

			stream << "<h1>Modèles de textes destinés aux messages complémentaires</h1>";

			vector<shared_ptr<TextTemplate> > tw = TextTemplateTableSync::search(
				ALARM_LEVEL_INFO
				, string(), NULL
				, 0, -1
				, _requestParameters.orderField == PARAMETER_NAME
				, _requestParameters.orderField == PARAMETER_SHORT_TEXT
				, _requestParameters.orderField == PARAMETER_LONG_TEXT
				, _requestParameters.raisingOrder
				);

			ResultHTMLTable::HeaderVector h1;
			h1.push_back(make_pair(PARAMETER_NAME, "Nom"));
			h1.push_back(make_pair(PARAMETER_SHORT_TEXT, "Texte&nbsp;court"));
			h1.push_back(make_pair(PARAMETER_LONG_TEXT, "Texte&nbsp;long"));
			h1.push_back(make_pair(string(), "Actions"));
			ResultHTMLTable t1(h1, searchRequest.getHTMLForm(), _requestParameters, ActionResultHTMLTable::getParameters(_requestParameters, tw));

			stream << t1.open();

			for (vector<shared_ptr<TextTemplate> >::iterator itw = tw.begin(); itw != tw.end(); ++itw)
			{
				shared_ptr<TextTemplate> t = *itw;

				HTMLForm uf(updateRequest.getHTMLForm("upd" + Conversion::ToString(t->getKey())));
				uf.addHiddenField(UpdateTextTemplateAction::PARAMETER_TEXT_ID, Conversion::ToString(t->getKey()));

				HTMLForm df(deleteRequest.getHTMLForm());
				df.addHiddenField(DeleteTextTemplateAction::PARAMETER_TEXT_ID, Conversion::ToString(t->getKey()));

				stream << uf.open();
				stream
					<< "<tr>"
					<< "<td>" << uf.getTextInput(UpdateTextTemplateAction::PARAMETER_NAME, t->getName()) << "</td>"
					<< "<td>" << uf.getTextInput(UpdateTextTemplateAction::PARAMETER_SHORT_MESSAGE, t->getShortMessage()) << "</td>"
					<< "<td>" << uf.getTextInput(UpdateTextTemplateAction::PARAMETER_LONG_MESSAGE, t->getLongMessage()) << "</td>"
					<< "<td>" << uf.getSubmitButton("Modifier") << "</td>"
					<< "</form>"
					<< "<td>" << df.getLinkButton("Supprimer","Etes-vous sûr(e) de vouloir supprimer le modèle sélectionné ?")
					<< "</td>"
					<< "</tr>";
				stream << uf.close();
			}

			HTMLForm af(addRequest.getHTMLForm("add1"));
			af.addHiddenField(TextTemplateAddAction::PARAMETER_TYPE, Conversion::ToString((int) ALARM_LEVEL_INFO));
			stream << af.open();
			stream
				<< "<tr>"
				<< "<td>" << af.getTextInput(TextTemplateAddAction::PARAMETER_NAME, string()) << "</td>"
				<< "<td>" << af.getTextInput(TextTemplateAddAction::PARAMETER_SHORT_MESSAGE, string()) << "</td>"
				<< "<td>" << af.getTextInput(TextTemplateAddAction::PARAMETER_LONG_MESSAGE, string()) << "</td>"
				<< "<td>" << af.getSubmitButton("Ajouter") << "</td>"
				<< "</tr>";
			stream << af.close();

			stream << t1.close();

			stream << "<h1>Modèles de textes destinés aux messages prioritaires</h1>";

			vector<shared_ptr<TextTemplate> > te = TextTemplateTableSync::search(
				ALARM_LEVEL_WARNING
				, string(), NULL
				, 0, -1
				, _requestParameters.orderField == PARAMETER_NAME
				, _requestParameters.orderField == PARAMETER_SHORT_TEXT
				, _requestParameters.orderField == PARAMETER_LONG_TEXT
				, _requestParameters.raisingOrder
				);

			ResultHTMLTable::HeaderVector h2;
			h2.push_back(make_pair(PARAMETER_NAME, "Nom"));
			h2.push_back(make_pair(PARAMETER_SHORT_TEXT, "Texte&nbsp;court"));
			h2.push_back(make_pair(PARAMETER_LONG_TEXT, "Texte&nbsp;long"));
			h2.push_back(make_pair(string(), "Actions"));
			ResultHTMLTable t2(h2, searchRequest.getHTMLForm(), _requestParameters, ResultHTMLTable::getParameters(_requestParameters, te));

			stream << t2.open();

			for (vector<shared_ptr<TextTemplate> >::iterator ite = te.begin(); ite != te.end(); ++ite)
			{
				shared_ptr<TextTemplate> t = *ite;

				/// @todo See if we use textarea
				HTMLForm uf(updateRequest.getHTMLForm("upd" + Conversion::ToString(t->getKey())));
				uf.addHiddenField(UpdateTextTemplateAction::PARAMETER_TEXT_ID, Conversion::ToString(t->getKey()));
				
				HTMLForm df(deleteRequest.getHTMLForm("del" + Conversion::ToString(t->getKey())));
				df.addHiddenField(DeleteTextTemplateAction::PARAMETER_TEXT_ID, Conversion::ToString(t->getKey()));
				
				stream << uf.open();
				stream
					<< "<tr>"
					<< "<td>" << uf.getTextInput(UpdateTextTemplateAction::PARAMETER_NAME, t->getName()) << "</td>"
					<< "<td>" << uf.getTextInput(UpdateTextTemplateAction::PARAMETER_SHORT_MESSAGE, t->getShortMessage()) << "</td>"
					<< "<td>" << uf.getTextInput(UpdateTextTemplateAction::PARAMETER_LONG_MESSAGE, t->getLongMessage()) << "</td>"
					<< "<td>" << uf.getSubmitButton("Modifier") << "</td>"
					<< "<td>" << df.getLinkButton("Supprimer","Etes-vous sûr(e) de vouloir supprimer le modèle sélectionné ?") << "</td>"
					<< "</tr>";
				stream << uf.close();
			}
			HTMLForm af2(addRequest.getHTMLForm("add2"));
			af2.addHiddenField(TextTemplateAddAction::PARAMETER_TYPE, Conversion::ToString((int) ALARM_LEVEL_WARNING));
			stream << af2.open();
			stream
				<< "<tr>"
				<< "<td>" << af2.getTextInput(TextTemplateAddAction::PARAMETER_NAME, string()) << "</td>"
				<< "<td>" << af2.getTextInput(TextTemplateAddAction::PARAMETER_SHORT_MESSAGE, string()) << "</td>"
				<< "<td>" << af2.getTextInput(TextTemplateAddAction::PARAMETER_LONG_MESSAGE, string()) << "</td>"
				<< "<td>" << af2.getSubmitButton("Ajouter") << "</td>"
				<< "</tr>";
			stream << af2.close();

			stream << t2.close();

			stream << "<h1>Scénarios</h1>";

			vector<shared_ptr<ScenarioTemplate> > sv = ScenarioTableSync::searchTemplate(
				string(), NULL
				, 0, -1
				, _requestParameters.orderField == PARAMETER_NAME
				, _requestParameters.raisingOrder
				);

			ActionResultHTMLTable::HeaderVector h3;
			h3.push_back(make_pair(PARAMETER_NAME, "Nom"));
			h3.push_back(make_pair(string(), "Actions"));
			ActionResultHTMLTable t3(h3, searchRequest.getHTMLForm(), _requestParameters, ActionResultHTMLTable::ResultParameters(), addScenarioRequest.getHTMLForm("addscenario"), AddScenarioAction::PARAMETER_TEMPLATE_ID);
			stream << t3.open();
			
			for (vector<shared_ptr<ScenarioTemplate> >::const_iterator it = sv.begin(); it != sv.end(); ++it)
			{
				shared_ptr<ScenarioTemplate> scenario = *it;
				updateScenarioRequest.setObjectId(scenario->getKey());
				deleteScenarioRequest.setObjectId(scenario->getKey());
				stream << t3.row();
				stream << t3.col() << scenario->getName();
				stream << t3.col() << HTMLModule::getLinkButton(updateScenarioRequest.getURL(), "Modifier")
					<< " " << HTMLModule::getLinkButton(deleteScenarioRequest.getURL(), "Supprimer", "Etes-vous sûr de vouloir supprimer le scénario " + scenario->getName() + " ?");
			}

			stream << t3.row();
			stream << t3.col() << t3.getActionForm().getTextInput(AddScenarioAction::PARAMETER_NAME, string(), "Entrez le nom ici");
			stream << t3.col() << t3.getActionForm().getSubmitButton("Ajouter");
			stream << t3.close();
		}

		bool MessagesLibraryAdmin::isAuthorized( const server::FunctionRequest<AdminRequest>* request ) const
		{
			return true;
		}

		std::string MessagesLibraryAdmin::getIcon() const
		{
			return "package.png";
		}
	}
}

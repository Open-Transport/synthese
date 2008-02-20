
/** MessagesTemplateLibraryAdmin class implementation.
	@file MessagesTemplateLibraryAdmin.cpp
	@author Hugues Romain
	@date 2008

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

#include "MessagesTemplateLibraryAdmin.h"
#include "MessagesModule.h"

#include "05_html/ActionResultHTMLTable.h"
#include "05_html/HTMLForm.h"

#include "30_server/ActionFunctionRequest.h"
#include "30_server/QueryString.h"

#include "32_admin/AdminParametersException.h"
#include "32_admin/ModuleAdmin.h"
#include "32_admin/AdminRequest.h"

#include "17_messages/TextTemplate.h"
#include "17_messages/TextTemplateTableSync.h"
#include "17_messages/UpdateTextTemplateAction.h"
#include "17_messages/DeleteTextTemplateAction.h"
#include "17_messages/TextTemplateAddAction.h"
#include "17_messages/MessagesLibraryRight.h"
#include "17_messages/MessagesModule.h"


using namespace std;
using namespace boost;

namespace synthese
{
	using namespace admin;
	using namespace interfaces;
	using namespace server;
	using namespace util;
	using namespace messages;
	using namespace html;
	using namespace security;

	namespace util
	{
		template<> const string FactorableTemplate<AdminInterfaceElement, MessagesTemplateLibraryAdmin>::FACTORY_KEY("1MessagesTemplateLibraryAdmin");
	}

	namespace admin
	{
		template<> const string AdminInterfaceElementTemplate<MessagesTemplateLibraryAdmin>::ICON("package.png");
		template<> const string AdminInterfaceElementTemplate<MessagesTemplateLibraryAdmin>::DEFAULT_TITLE("Bibliothèque de textes");
	}

	namespace messages
	{
		const std::string MessagesTemplateLibraryAdmin::PARAMETER_NAME = "nam";
		const std::string MessagesTemplateLibraryAdmin::PARAMETER_SHORT_TEXT = "stx";
		const std::string MessagesTemplateLibraryAdmin::PARAMETER_LONG_TEXT = "ltx";

		MessagesTemplateLibraryAdmin::MessagesTemplateLibraryAdmin()
			: AdminInterfaceElementTemplate<MessagesTemplateLibraryAdmin>()
		{ }
		
		void MessagesTemplateLibraryAdmin::setFromParametersMap(const ParametersMap& map)
		{
			_requestParameters = ResultHTMLTable::getParameters(map.getMap(), PARAMETER_NAME, ResultHTMLTable::UNLIMITED_SIZE);
		}
		
		void MessagesTemplateLibraryAdmin::display(ostream& stream, VariablesMap& variables, const FunctionRequest<AdminRequest>* request) const
		{
			FunctionRequest<AdminRequest> searchRequest(request);
			searchRequest.getFunction()->setPage<MessagesTemplateLibraryAdmin>();

			ActionFunctionRequest<UpdateTextTemplateAction,AdminRequest> updateRequest(request);
			updateRequest.getFunction()->setPage<MessagesTemplateLibraryAdmin>();

			ActionFunctionRequest<DeleteTextTemplateAction,AdminRequest> deleteRequest(request);
			deleteRequest.getFunction()->setPage<MessagesTemplateLibraryAdmin>();

			ActionFunctionRequest<TextTemplateAddAction,AdminRequest> addRequest(request);
			addRequest.getFunction()->setPage<MessagesTemplateLibraryAdmin>();

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
			af2.addHiddenField(TextTemplateAddAction::PARAMETER_TYPE, Conversion::ToString(static_cast<int>(ALARM_LEVEL_WARNING)));
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
		}

		bool MessagesTemplateLibraryAdmin::isAuthorized(const FunctionRequest<AdminRequest>* request) const
		{
			return request->isAuthorized<MessagesLibraryRight>(READ);
		}
		
		AdminInterfaceElement::PageLinks MessagesTemplateLibraryAdmin::getSubPagesOfParent(
			const PageLink& parentLink
			, const AdminInterfaceElement& currentPage
			, const server::FunctionRequest<admin::AdminRequest>* request
			) const	{
			AdminInterfaceElement::PageLinks links;
			if(parentLink.factoryKey == admin::ModuleAdmin::FACTORY_KEY && parentLink.parameterValue == MessagesModule::FACTORY_KEY)
				links.push_back(getPageLink());
			return links;
		}
		
		AdminInterfaceElement::PageLinks MessagesTemplateLibraryAdmin::getSubPages(
			const AdminInterfaceElement& currentPage
			, const server::FunctionRequest<admin::AdminRequest>* request
		) const {
			AdminInterfaceElement::PageLinks links;
			return links;
		}

		bool MessagesTemplateLibraryAdmin::isPageVisibleInTree( const AdminInterfaceElement& currentPage ) const
		{
			return true;
		}
	}
}

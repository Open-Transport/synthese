
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
#include "17_messages/MessagesScenarioAdmin.h"
#include "17_messages/DeleteScenarioAction.h"
#include "17_messages/AddScenarioAction.h"
#include "17_messages/MessagesAdmin.h"
#include "17_messages/MessagesLibraryRight.h"
#include "17_messages/MessagesModule.h"
#include "17_messages/ScenarioFolderAdd.h"
#include "17_messages/ScenarioFolderTableSync.h"
#include "17_messages/ScenarioFolder.h"

#include "30_server/ActionFunctionRequest.h"
#include "30_server/QueryString.h"

#include "32_admin/AdminRequest.h"
#include "32_admin/ModuleAdmin.h"
#include "32_admin/AdminParametersException.h"

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
	using namespace messages;
	using namespace security;

	namespace util
	{
		template<> const string FactorableTemplate<AdminInterfaceElement,MessagesLibraryAdmin>::FACTORY_KEY("2messageslibrary");
	}

	namespace admin
	{
		template<> const string AdminInterfaceElementTemplate<MessagesLibraryAdmin>::ICON("package.png");
		template<> const string AdminInterfaceElementTemplate<MessagesLibraryAdmin>::DEFAULT_TITLE("Bibliothèque de scénarios");
	}

	namespace messages
	{
		const std::string MessagesLibraryAdmin::PARAMETER_NAME = "nam";
		const string MessagesLibraryAdmin::PARAMETER_FOLDER_ID("fi");

		
		void MessagesLibraryAdmin::setFromParametersMap(const ParametersMap& map)
		{
			_requestParameters = ResultHTMLTable::getParameters(map.getMap(), PARAMETER_NAME, ResultHTMLTable::UNLIMITED_SIZE);
			setFolderId(map.getUid(PARAMETER_FOLDER_ID, false, FACTORY_KEY));
		}

		void MessagesLibraryAdmin::display(ostream& stream, interfaces::VariablesMap& variables, const server::FunctionRequest<admin::AdminRequest>* request) const
		{
			FunctionRequest<AdminRequest> searchRequest(request);
			searchRequest.getFunction()->setPage<MessagesLibraryAdmin>();
			searchRequest.getFunction()->setParameter(PARAMETER_FOLDER_ID, Conversion::ToString(_folderId));

			FunctionRequest<AdminRequest> updateScenarioRequest(request);
			updateScenarioRequest.getFunction()->setPage<MessagesScenarioAdmin>();
			
			ActionFunctionRequest<DeleteScenarioAction,AdminRequest> deleteScenarioRequest(request);
			deleteScenarioRequest.getFunction()->setPage<MessagesLibraryAdmin>();
			deleteScenarioRequest.getFunction()->setParameter(PARAMETER_FOLDER_ID, Conversion::ToString(_folderId));
			
			ActionFunctionRequest<AddScenarioAction,AdminRequest> addScenarioRequest(request);
			addScenarioRequest.getFunction()->setPage<MessagesScenarioAdmin>();
			addScenarioRequest.getFunction()->setActionFailedPage<MessagesLibraryAdmin>();
			addScenarioRequest.getFunction()->setParameter(PARAMETER_FOLDER_ID, Conversion::ToString(_folderId));
			addScenarioRequest.getAction()->setFolderId(_folderId);

			ActionFunctionRequest<ScenarioFolderAdd,AdminRequest> addFolderRequest(request);
			addFolderRequest.getFunction()->setPage<MessagesLibraryAdmin>();
			addFolderRequest.getFunction()->setActionFailedPage<MessagesLibraryAdmin>();
			addFolderRequest.getAction()->setParentId(_folderId);

			FunctionRequest<AdminRequest> goFolderRequest(request);
			goFolderRequest.getFunction()->setPage<MessagesLibraryAdmin>();

			if (_folderId > 0)
			{
				stream << "<h1>Répertoire</h1>";
			}

			stream << "<h1>Scénarios</h1>";

			vector<shared_ptr<ScenarioTemplate> > sv = ScenarioTableSync::searchTemplate(
				_folderId
				, string(), NULL
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
				deleteScenarioRequest.getAction()->setScenario(scenario);
				stream << t3.row(Conversion::ToString(scenario->getKey()));
				stream << t3.col() << scenario->getName();
				stream << t3.col()
					<< HTMLModule::getLinkButton(updateScenarioRequest.getURL(), "Modifier", string(), "cog_edit.png")
					<< " " << HTMLModule::getLinkButton(deleteScenarioRequest.getURL(), "Supprimer", "Etes-vous sûr de vouloir supprimer le scénario " + scenario->getName() + " ?", "cog_delete.png");
			}

			stream << t3.row();
			stream << t3.col() << t3.getActionForm().getTextInput(AddScenarioAction::PARAMETER_NAME, string(), "Entrez le nom ici");
			stream << t3.col() << t3.getActionForm().getSubmitButton("Ajouter");
			stream << t3.close();

			stream << "<h1>Sous-répertoires</h1>";

			vector<shared_ptr<ScenarioFolder> > folders(ScenarioFolderTableSync::search(_folderId));
			if (folders.empty())
			{
				stream << "<p>Aucun sous-répertoire.</p>";
			}
			else
			{
				stream << "<ul>";
				for (vector<shared_ptr<ScenarioFolder> >::const_iterator it(folders.begin()); it != folders.end(); ++it)
				{
					static_pointer_cast<MessagesLibraryAdmin,AdminInterfaceElement>(goFolderRequest.getFunction()->getPage())->setFolderId((*it)->getKey());
					stream << "<li>" << HTMLModule::getHTMLImage("folder.png","") << HTMLModule::getHTMLLink(goFolderRequest.getURL(), (*it)->getName()) << "</li>";
				}
				stream << "</ul>";
			}

			HTMLForm f(addFolderRequest.getHTMLForm());
			stream << f.open();
			stream << f.getTextInput(ScenarioFolderAdd::PARAMETER_NAME,"","(Entrez le nom du répertoire ici)");
			stream << f.getSubmitButton("Créer le sous-répertoire");
			stream << f.close();

		}

		bool MessagesLibraryAdmin::isAuthorized( const server::FunctionRequest<AdminRequest>* request ) const
		{
			return request->isAuthorized<MessagesLibraryRight>(READ);
		}

		MessagesLibraryAdmin::MessagesLibraryAdmin()
			: AdminInterfaceElementTemplate<MessagesLibraryAdmin>()
		{
			
		}

		AdminInterfaceElement::PageLinks MessagesLibraryAdmin::getSubPagesOfParent( const PageLink& parentLink , const AdminInterfaceElement& currentPage		, const server::FunctionRequest<admin::AdminRequest>* request
			) const
		{
			AdminInterfaceElement::PageLinks links;
			if (parentLink.factoryKey == admin::ModuleAdmin::FACTORY_KEY && parentLink.parameterValue == MessagesModule::FACTORY_KEY)
			{
				links.push_back(_pageLink);
			}
			return links;
		}

		AdminInterfaceElement::PageLinks MessagesLibraryAdmin::getSubPages( const AdminInterfaceElement& currentPage, const server::FunctionRequest<admin::AdminRequest>* request ) const
		{
			PageLinks links;

			// Folders
			vector<shared_ptr<ScenarioFolder> > folders(ScenarioFolderTableSync::search((Conversion::ToLongLong(_pageLink.parameterValue) > 0) ? Conversion::ToLongLong(_pageLink.parameterValue) : 0));
			for (vector<shared_ptr<ScenarioFolder> >::const_iterator it(folders.begin()); it != folders.end(); ++it)
			{
				PageLink link;
				link.factoryKey = MessagesLibraryAdmin::FACTORY_KEY;
				link.icon = "folder.png";
				link.name = (*it)->getName();
				link.parameterName = MessagesLibraryAdmin::PARAMETER_FOLDER_ID;
				link.parameterValue = Conversion::ToString((*it)->getKey());
				links.push_back(link);
			}
			
			// Scenarios
			vector<shared_ptr<ScenarioTemplate> > sv(ScenarioTableSync::searchTemplate((Conversion::ToLongLong(_pageLink.parameterValue) > 0) ? Conversion::ToLongLong(_pageLink.parameterValue) : 0));
			for (vector<shared_ptr<ScenarioTemplate> >::const_iterator it(sv.begin()); it != sv.end(); ++it)
			{
				PageLink link;
				link.factoryKey = MessagesScenarioAdmin::FACTORY_KEY;
				link.icon = MessagesScenarioAdmin::ICON;
				link.name = (*it)->getName();
				link.parameterName = QueryString::PARAMETER_OBJECT_ID;
				link.parameterValue = Conversion::ToString((*it)->getKey());
				links.push_back(link);
			}
			
			return links;
		}

		server::ParametersMap MessagesLibraryAdmin::getParametersMap() const
		{
			server::ParametersMap map;
			map.insert(PARAMETER_FOLDER_ID, _folderId);
			return map;
		}

		void MessagesLibraryAdmin::setFolderId( uid id)
		{
			if (id > 0)
			{
				try
				{
					_folderId = id;
					_folder = ScenarioFolderTableSync::Get(_folderId);
					_pageLink.name = _folder->getName();
					_pageLink.icon = "folder.png";
					_pageLink.parameterName = PARAMETER_FOLDER_ID;
					_pageLink.parameterValue = Conversion::ToString(_folderId);
				}
				catch (...)
				{
					throw AdminParametersException("Bad folder ID");
				}
			}
			else
			{
				_folderId = 0;
			}
		}
	}
}

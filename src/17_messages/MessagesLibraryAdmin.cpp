
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

#include "ActionResultHTMLTable.h"
#include "HTMLForm.h"
#include "HTMLList.h"
#include "PropertiesHTMLTable.h"

#include "ScenarioTemplate.h"
#include "ScenarioTableSync.h"
#include "MessagesLibraryAdmin.h"
#include "MessagesScenarioAdmin.h"
#include "DeleteScenarioAction.h"
#include "AddScenarioAction.h"
#include "MessagesAdmin.h"
#include "MessagesLibraryRight.h"
#include "MessagesModule.h"
#include "ScenarioFolderAdd.h"
#include "ScenarioFolderTableSync.h"
#include "ScenarioFolder.h"
#include "ScenarioFolderRemoveAction.h"
#include "ScenarioFolderUpdateAction.h"
#include "ScenarioTemplateInheritedTableSync.h"

#include "ActionFunctionRequest.h"
#include "QueryString.h"

#include "AdminRequest.h"
#include "ModuleAdmin.h"
#include "AdminParametersException.h"

#include <boost/foreach.hpp>

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

		
		void MessagesLibraryAdmin::setFromParametersMap(const ParametersMap& map)
		{
			_requestParameters.setFromParametersMap(map.getMap(), PARAMETER_NAME, ResultHTMLTable::UNLIMITED_SIZE);
			setFolderId(map.getUid(QueryString::PARAMETER_OBJECT_ID, false, FACTORY_KEY));

			ScenarioTemplateInheritedTableSync::Search(
				_env,
				_folderId
				, string(), NULL
				, 0, -1
				, _requestParameters.orderField == PARAMETER_NAME
				, _requestParameters.raisingOrder
			);
			ScenarioFolderTableSync::Search(
				_env,
				_folderId
			);
		}

		void MessagesLibraryAdmin::display(ostream& stream, interfaces::VariablesMap& variables, const server::FunctionRequest<admin::AdminRequest>* request) const
		{
			// Requests
			FunctionRequest<AdminRequest> searchRequest(request);
			searchRequest.getFunction()->setPage<MessagesLibraryAdmin>();
			searchRequest.setObjectId(_folderId);

			FunctionRequest<AdminRequest> updateScenarioRequest(request);
			updateScenarioRequest.getFunction()->setPage<MessagesScenarioAdmin>();
			
			ActionFunctionRequest<DeleteScenarioAction,AdminRequest> deleteScenarioRequest(request);
			deleteScenarioRequest.getFunction()->setPage<MessagesLibraryAdmin>();
			deleteScenarioRequest.setObjectId(_folderId);
			
			ActionFunctionRequest<AddScenarioAction,AdminRequest> addScenarioRequest(request);
			addScenarioRequest.getFunction()->setPage<MessagesScenarioAdmin>();
			addScenarioRequest.getFunction()->setActionFailedPage<MessagesLibraryAdmin>();
			addScenarioRequest.getAction()->setFolderId(_folderId);

			ActionFunctionRequest<ScenarioFolderAdd,AdminRequest> addFolderRequest(request);
			addFolderRequest.getFunction()->setPage<MessagesLibraryAdmin>();
			addFolderRequest.getFunction()->setActionFailedPage<MessagesLibraryAdmin>();
			addFolderRequest.getAction()->setParentId(_folderId);

			FunctionRequest<AdminRequest> goFolderRequest(request);
			goFolderRequest.getFunction()->setPage<MessagesLibraryAdmin>();

			ActionFunctionRequest<ScenarioFolderRemoveAction,AdminRequest> removeFolderRequest(request);
			removeFolderRequest.getAction()->setFolder(_folder);
			removeFolderRequest.getFunction()->setPage<MessagesLibraryAdmin>();

			ActionFunctionRequest<ScenarioFolderUpdateAction,AdminRequest> updateFolderRequest(request);
			updateFolderRequest.getAction()->setFolderId(_folder.get() ? _folder->getKey() : 0);
			updateFolderRequest.getFunction()->setPage<MessagesLibraryAdmin>();
			updateFolderRequest.setObjectId(_folder.get() ? _folder->getKey() : 0);

			// Search
			ResultHTMLTable::ResultParameters p;
			p.setFromResult(_requestParameters, _env.getEditableRegistry<ScenarioTemplate>());

			if (_folderId > 0)
			{
				stream << "<h1>Répertoire</h1>";

				if (_env.getRegistry<ScenarioTemplate>().empty() && _env.getRegistry<ScenarioFolder>().empty())
					stream << "<p>" << HTMLModule::getLinkButton(removeFolderRequest.getURL(), "Supprimer", "Etes-vous sûr de vouloir supprimer le répertoire "+ _folder->getName() +" ?", "folder_delete.png") << "</p>";

				PropertiesHTMLTable t(updateFolderRequest.getHTMLForm());
				stream << t.open();
				stream << t.cell("Nom", t.getForm().getTextInput(ScenarioFolderUpdateAction::PARAMETER_NAME, _folder->getName()));
				stream << t.cell("Répertoire parent", t.getForm().getSelectInput(ScenarioFolderUpdateAction::PARAMETER_PARENT_FOLDER_ID, MessagesModule::GetScenarioFoldersLabels(0,string(),_folder->getKey()), _folder->getParentId()));
				stream << t.close();
			}

			stream << "<h1>Scénarios</h1>";

			ActionResultHTMLTable::HeaderVector h3;
			h3.push_back(make_pair(PARAMETER_NAME, "Nom"));
			h3.push_back(make_pair(string(), "Actions"));
			h3.push_back(make_pair(string(), "Actions"));
			ActionResultHTMLTable t3(h3, searchRequest.getHTMLForm(), _requestParameters, p, addScenarioRequest.getHTMLForm("addscenario"), AddScenarioAction::PARAMETER_TEMPLATE_ID);
			stream << t3.open();
			
			BOOST_FOREACH(shared_ptr<ScenarioTemplate> scenario, _env.getRegistry<ScenarioTemplate>())
			{
				updateScenarioRequest.setObjectId(scenario->getKey());
				deleteScenarioRequest.getAction()->setScenario(scenario);
				stream << t3.row(Conversion::ToString(scenario->getKey()));
				stream << t3.col() << scenario->getName();
				stream << t3.col() << HTMLModule::getLinkButton(updateScenarioRequest.getURL(), "Modifier", string(), "cog_edit.png");
				stream << t3.col() << HTMLModule::getLinkButton(deleteScenarioRequest.getURL(), "Supprimer", "Etes-vous sûr de vouloir supprimer le scénario " + scenario->getName() + " ?", "cog_delete.png");
			}

			stream << t3.row();
			stream << t3.col() << t3.getActionForm().getTextInput(AddScenarioAction::PARAMETER_NAME, string(), "Entrez le nom ici");
			stream << t3.col(2) << t3.getActionForm().getSubmitButton("Ajouter");
			stream << t3.close();

			stream << "<h1>Sous-répertoires</h1>";

			if (_env.getRegistry<ScenarioFolder>().empty())
			{
				stream << "<p>Aucun sous-répertoire.</p>";
			}
			
			HTMLForm f(addFolderRequest.getHTMLForm());
			HTMLList l;
			stream << f.open() << l.open();

			BOOST_FOREACH(shared_ptr<ScenarioFolder> folder, _env.getRegistry<ScenarioFolder>())
			{
				static_pointer_cast<MessagesLibraryAdmin,AdminInterfaceElement>(goFolderRequest.getFunction()->getPage())->setFolderId(folder->getKey());
				stream << l.element("folder");
				stream << HTMLModule::getHTMLLink(goFolderRequest.getURL(), folder->getName());
			}
			
			stream << l.element("folder");
			stream << f.getImageSubmitButton("add.png", "Ajouter");
			stream << f.getTextInput(ScenarioFolderAdd::PARAMETER_NAME,"","(Entrez le nom du répertoire ici)");
			stream << l.close() << f.close();

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
				links.push_back(getPageLink());
			}
			return links;
		}

		AdminInterfaceElement::PageLinks MessagesLibraryAdmin::getSubPages( const AdminInterfaceElement& currentPage, const server::FunctionRequest<admin::AdminRequest>* request ) const
		{
			PageLinks links;

			// Folders
			Env env;
			ScenarioFolderTableSync::Search(env, _folder.get() ? _folder->getKey() : 0);
			BOOST_FOREACH(shared_ptr<ScenarioFolder> cfolder, env.getRegistry<ScenarioFolder>())
			{
				PageLink link;
				link.factoryKey = MessagesLibraryAdmin::FACTORY_KEY;
				link.icon = "folder.png";
				link.name = cfolder->getName();
				link.parameterName = QueryString::PARAMETER_OBJECT_ID;
				link.parameterValue = Conversion::ToString(cfolder->getKey());
				links.push_back(link);
			}
			
			// Scenarios
			ScenarioTemplateInheritedTableSync::Search(env, _folder.get() ? _folder->getKey() : 0);
			BOOST_FOREACH(shared_ptr<ScenarioTemplate> tpl, env.getRegistry<ScenarioTemplate>())
			{
				PageLink link;
				link.factoryKey = MessagesScenarioAdmin::FACTORY_KEY;
				link.icon = MessagesScenarioAdmin::ICON;
				link.name = tpl->getName();
				link.parameterName = QueryString::PARAMETER_OBJECT_ID;
				link.parameterValue = Conversion::ToString(tpl->getKey());
				links.push_back(link);
			}
			
			return links;
		}

		server::ParametersMap MessagesLibraryAdmin::getParametersMap() const
		{
			server::ParametersMap map;
			if (_folder.get())
				map.insert(QueryString::PARAMETER_OBJECT_ID, _folder->getKey());
			return map;
		}

		void MessagesLibraryAdmin::setFolderId( uid id)
		{
			if (id > 0)
			{
				try
				{
					_folderId = id;
					_folder = ScenarioFolderTableSync::Get(_folderId, _env);
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

		std::string MessagesLibraryAdmin::getTitle() const
		{
			return _folder.get() ? _folder->getName() : DEFAULT_TITLE;
		}

		std::string MessagesLibraryAdmin::getParameterName() const
		{
			return _folder.get() ? QueryString::PARAMETER_OBJECT_ID : string();
		}

		std::string MessagesLibraryAdmin::getParameterValue() const
		{
			return _folder.get() ? Conversion::ToString(_folder->getKey()) : string();
		}

		std::string MessagesLibraryAdmin::getIcon() const
		{
			return _folder.get() ? "folder.png" : ICON;
		}

		bool MessagesLibraryAdmin::isPageVisibleInTree( const AdminInterfaceElement& currentPage ) const
		{
			return !_folder.get();
		}
	}
}

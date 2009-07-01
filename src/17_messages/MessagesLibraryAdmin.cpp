
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
#include "AdminActionFunctionRequest.hpp"
#include "AdminFunctionRequest.hpp"

#include "AdminInterfaceElement.h"
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

		
		void MessagesLibraryAdmin::setFromParametersMap(
			const ParametersMap& map,
			bool doDisplayPreparationActions			
		){
			_requestParameters.setFromParametersMap(
				map.getMap(),
				PARAMETER_NAME,
				ResultHTMLTable::UNLIMITED_SIZE
			);
			
			if(_request->getActionWillCreateObject()) return;
			
			optional<RegistryKeyType> id(
				map.getOptional<RegistryKeyType>(Request::PARAMETER_OBJECT_ID)
			);

			if (id)
			{
				try
				{
					_folder = ScenarioFolderTableSync::Get(*id, _getEnv());
				}
				catch (...)
				{
					throw AdminParametersException("Bad folder ID");
				}
			}

			if(!doDisplayPreparationActions) return;

			ScenarioTemplateInheritedTableSync::Search(
				_getEnv(),
				_folder.get() ? _folder->getKey() : 0
				, string(), NULL
				, 0, -1
				, _requestParameters.orderField == PARAMETER_NAME
				, _requestParameters.raisingOrder
			);
			ScenarioFolderTableSync::Search(
				_subFoldersEnv,
				_folder.get() ? _folder->getKey() : 0
			);
		}



		server::ParametersMap MessagesLibraryAdmin::getParametersMap() const
		{
			ParametersMap m(_requestParameters.getParametersMap());
			if(_folder.get()) m.insert(Request::PARAMETER_OBJECT_ID, _folder->getKey());
			return m;
		}



		void MessagesLibraryAdmin::display(
			ostream& stream,
			interfaces::VariablesMap& variables
		) const {
			// Requests
			AdminFunctionRequest<MessagesLibraryAdmin> searchRequest(_request);
			searchRequest.getPage()->setFolder(_folder);

			AdminFunctionRequest<MessagesScenarioAdmin> updateScenarioRequest(_request);
			
			AdminActionFunctionRequest<DeleteScenarioAction,MessagesLibraryAdmin> deleteScenarioRequest(_request);
			
			AdminActionFunctionRequest<AddScenarioAction,MessagesScenarioAdmin> addScenarioRequest(_request);
			addScenarioRequest.getFunction()->setActionFailedPage<MessagesLibraryAdmin>();
			addScenarioRequest.getAction()->setFolder(_folder);
			addScenarioRequest.setActionWillCreateObject();

			AdminActionFunctionRequest<ScenarioFolderAdd,MessagesLibraryAdmin> addFolderRequest(_request);
			addFolderRequest.getFunction()->setActionFailedPage<MessagesLibraryAdmin>();
			addFolderRequest.getAction()->setParent(_folder);
			addFolderRequest.setActionWillCreateObject();

			AdminFunctionRequest<MessagesLibraryAdmin> goFolderRequest(_request);

			// Search
			ResultHTMLTable::ResultParameters p;
			p.setFromResult(_requestParameters, _getEnv().getEditableRegistry<ScenarioTemplate>());

			if (_folder.get())
			{
				stream << "<h1>Répertoire</h1>";

				if(	_getEnv().getRegistry<ScenarioTemplate>().empty() &&
					_getEnv().getRegistry<ScenarioFolder>().empty()
				){
					AdminActionFunctionRequest<ScenarioFolderRemoveAction,MessagesLibraryAdmin> removeFolderRequest(_request);
					removeFolderRequest.getAction()->setFolder(_folder);
					removeFolderRequest.getPage()->setFolder(
						_folder->getParent() ?
						ScenarioFolderTableSync::Get(
							_folder->getParent()->getKey(),
							_getEnv()
						):
						shared_ptr<const ScenarioFolder>()
					);
					
					stream <<
						"<p>" <<
						HTMLModule::getLinkButton(
							removeFolderRequest.getURL(),
							"Supprimer",
							"Etes-vous sûr de vouloir supprimer le répertoire "+ _folder->getName() +" ?",
							"folder_delete.png"
						) << "</p>"
					;
				}

				AdminActionFunctionRequest<ScenarioFolderUpdateAction,MessagesLibraryAdmin> updateFolderRequest(_request);
				updateFolderRequest.getAction()->setFolder(_folder);
				
				PropertiesHTMLTable t(updateFolderRequest.getHTMLForm());
				stream << t.open();
				stream << t.cell("Nom", t.getForm().getTextInput(ScenarioFolderUpdateAction::PARAMETER_NAME, _folder->getName()));
				stream << t.cell("Répertoire parent", t.getForm().getSelectInput(ScenarioFolderUpdateAction::PARAMETER_PARENT_FOLDER_ID, MessagesModule::GetScenarioFoldersLabels(0,string(),_folder->getKey()), _folder->getParent() ? _folder->getParent()->getKey() : 0));
				stream << t.close();
			}

			stream << "<h1>Scénarios</h1>";

			ActionResultHTMLTable::HeaderVector h3;
			h3.push_back(make_pair(PARAMETER_NAME, "Nom"));
			h3.push_back(make_pair(string(), "Actions"));
			h3.push_back(make_pair(string(), "Actions"));
			ActionResultHTMLTable t3(h3, searchRequest.getHTMLForm(), _requestParameters, p, addScenarioRequest.getHTMLForm("addscenario"), AddScenarioAction::PARAMETER_TEMPLATE_ID);
			stream << t3.open();
			
			BOOST_FOREACH(shared_ptr<ScenarioTemplate> scenario, _getEnv().getRegistry<ScenarioTemplate>())
			{
				updateScenarioRequest.getPage()->setScenario(scenario);
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

			if (_subFoldersEnv.getRegistry<ScenarioFolder>().empty())
			{
				stream << "<p>Aucun sous-répertoire.</p>";
			}
			
			HTMLForm f(addFolderRequest.getHTMLForm());
			HTMLList l;
			stream << f.open() << l.open();

			BOOST_FOREACH(shared_ptr<ScenarioFolder> folder, _subFoldersEnv.getRegistry<ScenarioFolder>())
			{
				if(folder == _folder) continue;

				goFolderRequest.getPage()->setFolder(folder);
				stream << l.element("folder");
				stream << HTMLModule::getHTMLLink(goFolderRequest.getURL(), folder->getName());
			}
			
			stream << l.element("folder");
			stream << f.getImageSubmitButton("add.png", "Ajouter");
			stream << f.getTextInput(ScenarioFolderAdd::PARAMETER_NAME,"","(Entrez le nom du répertoire ici)");
			stream << l.close() << f.close();

		}

		bool MessagesLibraryAdmin::isAuthorized() const
		{
			return _request->isAuthorized<MessagesLibraryRight>(READ);
		}

		MessagesLibraryAdmin::MessagesLibraryAdmin()
			: AdminInterfaceElementTemplate<MessagesLibraryAdmin>()
		{
			
		}

		AdminInterfaceElement::PageLinks MessagesLibraryAdmin::getSubPagesOfModule(
			const std::string& moduleKey,
			boost::shared_ptr<const AdminInterfaceElement> currentPage
		) const	{
			AdminInterfaceElement::PageLinks links;
			
			if(	moduleKey == MessagesModule::FACTORY_KEY)
			{
				const MessagesLibraryAdmin* ma(
					dynamic_cast<const MessagesLibraryAdmin*>(currentPage.get())
				);
				
				if(	ma &&
					!ma->_folder.get()
				){
					AddToLinks(links, currentPage);
				}
				else
				{
					AddToLinks(links, getNewPage());
				}
			}
			return links;
		}

		AdminInterfaceElement::PageLinks MessagesLibraryAdmin::getSubPages(
			shared_ptr<const AdminInterfaceElement> currentPage
		) const {
			PageLinks links;

			const MessagesLibraryAdmin* la(
				dynamic_cast<const MessagesLibraryAdmin*>(currentPage.get())
			);
			const MessagesScenarioAdmin* sa(
				dynamic_cast<const MessagesScenarioAdmin*>(currentPage.get())
			);
			
			// Folders
			Env env;
			ScenarioFolderTableSync::Search(env, _folder.get() ? _folder->getKey() : 0);
			BOOST_FOREACH(shared_ptr<ScenarioFolder> cfolder, env.getRegistry<ScenarioFolder>())
			{
				if(	la &&
					la->_folder->getKey() == cfolder->getKey()
				){
					AddToLinks(links, currentPage);
				}
				else
				{
					shared_ptr<MessagesLibraryAdmin> p(
						getNewOtherPage<MessagesLibraryAdmin>()
					);
					p->setFolder(cfolder);
					AddToLinks(links, p);
				}
			}
			
			// Scenarios
			ScenarioTemplateInheritedTableSync::Search(
				env,
				_folder.get() ? _folder->getKey() : 0
			);
			BOOST_FOREACH(shared_ptr<ScenarioTemplate> tpl, env.getRegistry<ScenarioTemplate>())
			{
				if(	sa &&
					sa->getScenario()->getKey() == tpl->getKey()
				){
					AddToLinks(links, currentPage);
				}
				else
				{
					shared_ptr<MessagesScenarioAdmin> p(
						getNewOtherPage<MessagesScenarioAdmin>()
					);
					p->setScenario(tpl);
					AddToLinks(links, p);
				}
			}
			
			return links;
		}



		std::string MessagesLibraryAdmin::getTitle() const
		{
			return _folder.get() ? _folder->getName() : DEFAULT_TITLE;
		}


		std::string MessagesLibraryAdmin::getIcon() const
		{
			return _folder.get() ? "folder.png" : ICON;
		}

		bool MessagesLibraryAdmin::isPageVisibleInTree( const AdminInterfaceElement& currentPage ) const
		{
			return !_folder.get();
		}
		
		boost::shared_ptr<const ScenarioFolder> MessagesLibraryAdmin::getFolder() const
		{
			return _folder;
		}
		
		void MessagesLibraryAdmin::setFolder(boost::shared_ptr<ScenarioFolder> folder)
		{
			_folder = const_pointer_cast<const ScenarioFolder>(folder);
		}
	
		void MessagesLibraryAdmin::setFolder(boost::shared_ptr<const ScenarioFolder> folder)
		{
			_folder = folder;
		}
	}
}

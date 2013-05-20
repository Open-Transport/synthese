
/** MessagesTemplateLibraryAdmin class implementation.
	@file MessagesTemplateLibraryAdmin.cpp
	@author Hugues Romain
	@date 2008

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>

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
#include "Profile.h"
#include "PropertiesHTMLTable.h"
#include "User.h"
#include "HTMLList.h"
#include "AdminActionFunctionRequest.hpp"
#include "AdminFunctionRequest.hpp"
#include "AdminParametersException.h"
#include "ModuleAdmin.h"
#include "AdminInterfaceElement.h"
#include "TextTemplate.h"
#include "TextTemplateTableSync.h"
#include "UpdateTextTemplateAction.h"
#include "RemoveObjectAction.hpp"
#include "TextTemplateAddAction.h"
#include "MessagesLibraryRight.h"
#include "MessagesModule.h"
#include "TextTemplateFolderUpdateAction.h"

#include <boost/foreach.hpp>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace admin;
	using namespace server;
	using namespace util;
	using namespace messages;
	using namespace html;
	using namespace security;
	using namespace db;

	namespace util
	{
		template<> const string FactorableTemplate<AdminInterfaceElement, MessagesTemplateLibraryAdmin>::FACTORY_KEY("1MessagesTemplateLibraryAdmin");
	}

	namespace admin
	{
		template<> const string AdminInterfaceElementTemplate<MessagesTemplateLibraryAdmin>::ICON("folder.png");
		template<> const string AdminInterfaceElementTemplate<MessagesTemplateLibraryAdmin>::DEFAULT_TITLE("Bibliothèque de textes");
	}

	namespace messages
	{
		const string MessagesTemplateLibraryAdmin::PARAMETER_FOLDER_ID("fo");



		MessagesTemplateLibraryAdmin::MessagesTemplateLibraryAdmin()
			: AdminInterfaceElementTemplate<MessagesTemplateLibraryAdmin>()
		{ }



		void MessagesTemplateLibraryAdmin::setFromParametersMap(
			const ParametersMap& map
		){
			RegistryKeyType id(map.getDefault<RegistryKeyType>(PARAMETER_FOLDER_ID, 0));
			if (id > 0)
			{
				try
				{
					_folder = TextTemplateTableSync::Get(id, _getEnv());
				}
				catch(ObjectNotFoundException<TextTemplate>&)
				{
				}
			}
		}



		util::ParametersMap MessagesTemplateLibraryAdmin::getParametersMap() const
		{
			ParametersMap m;
			if(_folder.get())
			{
				m.insert(PARAMETER_FOLDER_ID, _folder->getKey());
			}
			return m;
		}



		void MessagesTemplateLibraryAdmin::display(
			ostream& stream,
			const server::Request& _request
		) const	{

			// Requests
			AdminActionFunctionRequest<UpdateTextTemplateAction,MessagesTemplateLibraryAdmin> updateRequest(_request, *this);

			AdminActionFunctionRequest<RemoveObjectAction,MessagesTemplateLibraryAdmin> deleteRequest(_request, *this);

			AdminActionFunctionRequest<TextTemplateAddAction,MessagesTemplateLibraryAdmin> addRequest(_request, *this);
			addRequest.getAction()->setParentId(_folder.get() ? _folder->getKey() : RegistryKeyType(0));

			AdminActionFunctionRequest<TextTemplateFolderUpdateAction,MessagesTemplateLibraryAdmin> updateFolderRequest(_request, *this);


			// Rights
			bool updateRight(_request.getUser()->getProfile()->isAuthorized<MessagesLibraryRight>(WRITE));
			bool deleteRight(_request.getUser()->getProfile()->isAuthorized<MessagesLibraryRight>(DELETE_RIGHT));

			// Search
			TextTemplateTableSync::SearchResult templates(
				TextTemplateTableSync::Search(
					_getEnv(),
					_folder.get() ? _folder->getKey() : 0
					, false
					, string()
					, NULL
					, 0
					, optional<size_t>()
					, true
					, false
					, false
					, true
			)	);

			TextTemplateTableSync::SearchResult folders(
				TextTemplateTableSync::Search(
					_getEnv(),
					_folder.get() ? _folder->getKey() : 0
					, true
					, string()
					, NULL
					, 0
					, optional<size_t>()
					, true
					, false
					, false
					, true
			)	);

/*			stream << "<h1>Répertoires</h1>";
			HTMLList l;
			BOOST_FOREACH(const boost::shared_ptr<TextTemplate>& folder, folders)
			{
			}

			if (_folder.get())
			{
				stream << "<h2>Propriétés</h2>";

				if (tenv.getRegistry<TextTemplate>().empty() && fenv.getRegistry<TextTemplate>().empty())
					stream << "<p>" << HTMLModule::getLinkButton(deleteRequest.getURL(), "Supprimer", "Etes-vous sûr de vouloir supprimer le répertoire "+ _folder->getName() +" ?", "folder_delete.png") << "</p>";

				PropertiesHTMLTable t(updateFolderRequest.getHTMLForm());
				stream << t.open();
				stream << t.cell("Nom", t.getForm().getTextInput(UpdateTextTemplateAction::PARAMETER_NAME, _folder->getName()));
				stream << t.cell("Répertoire parent", t.getForm().getSelectInput(UpdateTextTemplateAction::PARAMETER_FOLDER_ID, MessagesModule::GetScenarioFoldersLabels(0,string(),_folder->getKey()), _folder->getParentId()));
				stream << t.close();
			}
*/

			if(!templates.empty())
			{
				stream << "<h1>Modèles de texte</h1>";
				BOOST_FOREACH(const boost::shared_ptr<TextTemplate>& tt, templates)
				{
					// Variables
					deleteRequest.getAction()->setObjectId(tt->getKey());
					updateRequest.getAction()->setTemplate(tt);

					// Display
					stream << "<h2>";
					if (deleteRight)
						stream << HTMLModule::getLinkButton(deleteRequest.getURL(), "Supprimer", "Etes-vous sûr de vouloir supprimer le modèle "+ tt->getName()+" ?", "/admin/img/page_delete.png") << " ";
					stream << tt->getName() << "</h2>";

					PropertiesHTMLTable t(updateRequest.getHTMLForm("up"+ lexical_cast<string>(tt->getKey())));
					t.getForm().setUpdateRight(updateRight);
					stream << t.open();
					stream << t.cell("Nom", t.getForm().getTextInput(UpdateTextTemplateAction::PARAMETER_NAME, tt->getName()));
					stream << t.cell("Texte court", t.getForm().getTextAreaInput(UpdateTextTemplateAction::PARAMETER_SHORT_MESSAGE, tt->getShortMessage(), 2, 60, false));
					stream << t.cell("Texte long", t.getForm().getTextAreaInput(UpdateTextTemplateAction::PARAMETER_LONG_MESSAGE, tt->getLongMessage(), 6, 60, false));
					stream << t.close();
				}
			}

			if (updateRight)
			{
				stream << "<h1>Nouveau modèle de textes</h1>";
				addRequest.getAction()->setIsFolder(false);
				PropertiesHTMLTable ta(addRequest.getHTMLForm("add"));
				stream << ta.open();
				stream << ta.cell("Nom", ta.getForm().getTextInput(TextTemplateAddAction::PARAMETER_NAME, string()));
				stream << ta.cell("Texte court", ta.getForm().getTextAreaInput(TextTemplateAddAction::PARAMETER_SHORT_MESSAGE, string(), 2, 60, false));
				stream << ta.cell("Texte long", ta.getForm().getTextAreaInput(TextTemplateAddAction::PARAMETER_LONG_MESSAGE, string(), 6, 60, false));
				stream << ta.close();
			}
		}



		bool MessagesTemplateLibraryAdmin::isAuthorized(
			const security::User& user
		) const	{
			return user.getProfile()->isAuthorized<MessagesLibraryRight>(READ);
		}



		AdminInterfaceElement::PageLinks MessagesTemplateLibraryAdmin::getSubPagesOfModule(
			const ModuleClass& module,
			const AdminInterfaceElement& currentPage,
			const server::Request& request
		) const	{
			AdminInterfaceElement::PageLinks links;

			if(	dynamic_cast<const MessagesModule*>(&module) &&
				request.getUser() &&
				request.getUser()->getProfile() &&
				isAuthorized(*request.getUser())
			){
				links.push_back(getNewPage<MessagesTemplateLibraryAdmin>());
			}
			return links;
		}


		bool MessagesTemplateLibraryAdmin::isPageVisibleInTree(
			const AdminInterfaceElement& currentPage,
			const server::Request& request
		) const {
			return true;
		}



		std::string MessagesTemplateLibraryAdmin::getTitle() const
		{
			return _folder.get() ? _folder->getName() : DEFAULT_TITLE;
		}
	}
}

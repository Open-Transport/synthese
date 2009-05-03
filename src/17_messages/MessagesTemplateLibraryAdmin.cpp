
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

#include "PropertiesHTMLTable.h"
#include "HTMLList.h"

#include "ActionFunctionRequest.h"
#include "Request.h"

#include "AdminParametersException.h"
#include "ModuleAdmin.h"
#include "AdminRequest.h"

#include "TextTemplate.h"
#include "TextTemplateTableSync.h"
#include "UpdateTextTemplateAction.h"
#include "DeleteTextTemplateAction.h"
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
			const ParametersMap& map,
			bool doDisplayPreparationActions
		){
			uid id(map.getUid(PARAMETER_FOLDER_ID, false, FACTORY_KEY));
			if (id > 0)
			{
				try
				{
					_folder = TextTemplateTableSync::Get(id, _env);
				}
				catch(ObjectNotFoundException<TextTemplate>& e)
				{
				}
			}
		}
		
		
		
		server::ParametersMap MessagesTemplateLibraryAdmin::getParametersMap() const
		{
			ParametersMap m;
			if(_folder.get())
			{
				m.insert(PARAMETER_FOLDER_ID, _folder->getKey());
			}
			return m;
		}



		void MessagesTemplateLibraryAdmin::display(ostream& stream, VariablesMap& variables) const
		{
			// Requests
			ActionFunctionRequest<UpdateTextTemplateAction,AdminRequest> updateRequest(_request);
			updateRequest.getFunction()->setSamePage(this);
			
			ActionFunctionRequest<DeleteTextTemplateAction,AdminRequest> deleteRequest(_request);
			deleteRequest.getFunction()->setSamePage(this);
			
			ActionFunctionRequest<TextTemplateAddAction,AdminRequest> addRequest(_request);
			addRequest.getFunction()->setSamePage(this);
			
			ActionFunctionRequest<TextTemplateFolderUpdateAction,AdminRequest> updateFolderRequest(_request);
			updateFolderRequest.getFunction()->setSamePage(this);
			

			// Rights
			bool updateRight(_request->isAuthorized<MessagesLibraryRight>(WRITE));
			bool deleteRight(_request->isAuthorized<MessagesLibraryRight>(DELETE_RIGHT));

			// Search
			Env tenv;
			TextTemplateTableSync::Search(
				tenv,
				ALARM_LEVEL_UNKNOWN
				, _folder.get() ? _folder->getKey() : 0
				, false
				, string()
				, NULL
				, 0
				, -1
				, true
				, false
				, false
				, true
			);

			Env fenv;
			TextTemplateTableSync::Search(
				fenv,
				ALARM_LEVEL_UNKNOWN
				, _folder.get() ? _folder->getKey() : 0
				, true
				, string()
				, NULL
				, 0
				, -1
				, true
				, false
				, false
				, true
			);

/*			stream << "<h1>Répertoires</h1>";
			HTMLList l;
			BOOST_FOREACH(shared_ptr<TextTemplate> folder, fenv.getRegistry<TextTemplate>())
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

			if(!tenv.getRegistry<TextTemplate>().empty())
			{
				stream << "<h1>Modèles de texte</h1>";
				BOOST_FOREACH(shared_ptr<TextTemplate> tt, tenv.getRegistry<TextTemplate>())
				{
					// Variables
					deleteRequest.getAction()->setTemplate(tt);
					updateRequest.getAction()->setTemplate(tt);

					// Display
					stream << "<h2>";
					if (deleteRight)
						stream << HTMLModule::getLinkButton(deleteRequest.getURL(), "Supprimer", "Etes-vous sûr de vouloir supprimer le modèle "+ tt->getName()+" ?", "page_delete.png") << " ";
					stream << tt->getName() << "</h2>";

					PropertiesHTMLTable t(updateRequest.getHTMLForm("up"+Conversion::ToString(tt->getKey())));
					t.getForm().setUpdateRight(updateRight);
					stream << t.open();
					stream << t.cell("Nom", t.getForm().getTextInput(UpdateTextTemplateAction::PARAMETER_NAME, tt->getName()));
					stream << t.cell("Texte court", t.getForm().getTextAreaInput(UpdateTextTemplateAction::PARAMETER_SHORT_MESSAGE, tt->getShortMessage(), 2, 60));
					stream << t.cell("Texte long", t.getForm().getTextAreaInput(UpdateTextTemplateAction::PARAMETER_LONG_MESSAGE, tt->getLongMessage(), 6, 60));
					stream << t.close();
				}
			}

			if (updateRight)
			{
				stream << "<h1>Nouveau modèle de textes</h1>"; 
				PropertiesHTMLTable ta(addRequest.getHTMLForm("add"));
				stream << ta.open();
				stream << ta.cell("Nom", ta.getForm().getTextInput(TextTemplateAddAction::PARAMETER_NAME, string()));
				stream << ta.cell("Texte court", ta.getForm().getTextAreaInput(TextTemplateAddAction::PARAMETER_SHORT_MESSAGE, string(), 2, 60));
				stream << ta.cell("Texte long", ta.getForm().getTextAreaInput(TextTemplateAddAction::PARAMETER_LONG_MESSAGE, string(), 6, 60));
				stream << ta.close();
			}
		}



		bool MessagesTemplateLibraryAdmin::isAuthorized() const
		{
			return _request->isAuthorized<MessagesLibraryRight>(READ);
		}
		


		AdminInterfaceElement::PageLinks MessagesTemplateLibraryAdmin::getSubPagesOfParent(
			const PageLink& parentLink
			, const AdminInterfaceElement& currentPage
		) const	{
			AdminInterfaceElement::PageLinks links;
			if(parentLink.factoryKey == admin::ModuleAdmin::FACTORY_KEY && parentLink.parameterValue == MessagesModule::FACTORY_KEY)
			{
				MessagesTemplateLibraryAdmin a;
				a.setRequest(_request);
				links.push_back(a.getPageLink());
			}
			return links;
		}
		
		AdminInterfaceElement::PageLinks MessagesTemplateLibraryAdmin::getSubPages(
			const AdminInterfaceElement& currentPage
		) const {
			AdminInterfaceElement::PageLinks links;
			return links;
		}

		bool MessagesTemplateLibraryAdmin::isPageVisibleInTree( const AdminInterfaceElement& currentPage ) const
		{
			return true;
		}



		std::string MessagesTemplateLibraryAdmin::getTitle() const
		{
			return _folder.get() ? _folder->getName() : DEFAULT_TITLE;
		}



		std::string MessagesTemplateLibraryAdmin::getParameterName() const
		{
			return _folder.get() ? Request::PARAMETER_OBJECT_ID : string();
		}



		std::string MessagesTemplateLibraryAdmin::getParameterValue() const
		{
			return _folder.get() ? Conversion::ToString(_folder->getKey()) : string();
		}
	}
}

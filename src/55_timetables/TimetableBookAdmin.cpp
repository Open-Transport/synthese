
/** TimetableBookAdmin class implementation.
	@file TimetableBookAdmin.cpp
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

#include "TimetableBookAdmin.h"

#include "ActionResultHTMLTable.h"
#include "HTMLModule.h"
#include "HTMLList.h"

#include "TimetableModule.h"
#include "Timetable.h"
#include "TimetableTableSync.h"
#include "TimetableAdmin.h"
#include "TimetableAddAction.h"
#include "TimetableRight.h"

#include "AdminRequest.h"
#include "AdminActionFunctionRequest.hpp"
#include "AdminFunctionRequest.hpp"

#include "ModuleAdmin.h"
#include "AdminInterfaceElement.h"
#include "AdminParametersException.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace admin;
	using namespace interfaces;
	using namespace server;
	using namespace util;
	using namespace timetables;
	using namespace html;
	using namespace security;

	namespace util
	{
		template<> const string FactorableTemplate<AdminInterfaceElement, TimetableBookAdmin>::FACTORY_KEY("TimetableBookAdmin");
	}

	namespace admin
	{
		template<> const string AdminInterfaceElementTemplate<TimetableBookAdmin>::ICON("table_multiple.png");
		template<> const string AdminInterfaceElementTemplate<TimetableBookAdmin>::DEFAULT_TITLE("Documents");
	}

	namespace timetables
	{
		const string TimetableBookAdmin::PARAMETER_RANK("rk");
		const string TimetableBookAdmin::PARAMETER_TITLE("ti");

		TimetableBookAdmin::TimetableBookAdmin()
			: AdminInterfaceElementTemplate<TimetableBookAdmin>()
		{ }
		
		void TimetableBookAdmin::setFromParametersMap(
			const ParametersMap& map,
			bool objectWillBeCreatedLater
		){
			
			_requestParameters.setFromParametersMap(
				map.getMap(),
				PARAMETER_RANK,
				optional<size_t>()
			);
			
			if(objectWillBeCreatedLater) return;

			optional<RegistryKeyType> id(
				map.getOptional<RegistryKeyType>(Request::PARAMETER_OBJECT_ID)
			);
			if(id)
			{
				try
				{
					_book = TimetableTableSync::Get(*id, _getEnv());
				}
				catch(...)
				{
					throw AdminParametersException("No such book");
				}
				if (!_book->getIsBook())
					throw AdminParametersException("Timetable is not a book");
			}
		}
		
		
		
		server::ParametersMap TimetableBookAdmin::getParametersMap() const
		{
			ParametersMap m(_requestParameters.getParametersMap());
			if(_book.get()) m.insert(Request::PARAMETER_OBJECT_ID, _book->getKey());
			return m;
		}
		

		
		void TimetableBookAdmin::display(
			ostream& stream,
			VariablesMap& variables,
					const server::FunctionRequest<admin::AdminRequest>& _request
		) const {
			// Requests
			
//			ActionFunctionRequest<TimetableBookRemoveAction,AdminRequest> removeFolderRequest(request);
//			removeFolderRequest.getAction()->setBook(_book);
//			removeFolderRequest.getFunction()->setPage<TimetableBookAdmin>();

//			ActionFunctionRequest<TimetableBookUpdateAction,AdminRequest> updateFolderRequest(request);
//			updateFolderRequest.getAction()->setFolderId(_book.get() ? _book->getKey() : 0);
//			updateFolderRequest.getFunction()->setPage<TimetableBookAdmin>();
//			updateFolderRequest.setObjectId(_book.get() ? _book->getKey() : 0);

			// Folder properties
			if (_book.get())
			{
				stream << "<h1>Document</h1>";

//				if (tt.empty() && folders.empty() && _folder.get())
//					stream << "<p>" << HTMLModule::getLinkButton(removeFolderRequest.getURL(), "Supprimer", "Etes-vous sûr de vouloir supprimer le répertoire "+ _folder->getName() +" ?", "folder_delete.png") << "</p>";

//				PropertiesHTMLTable t(updateFolderRequest.getHTMLForm());
//				stream << t.open();
//				stream << t.cell("Nom", t.getForm().getTextInput(ScenarioFolderUpdateAction::PARAMETER_NAME, _folder->getName()));
//				stream << t.cell("Document parent", t.getForm().getSelectInput(ScenarioFolderUpdateAction::PARAMETER_PARENT_FOLDER_ID, MessagesModule::GetScenarioFoldersLabels(0,string(),_folder->getKey()), _folder->getParentId()));
//				stream << t.close();
			}

			// Pages
			stream << "<h1>Fiches horaires</h1>";

			// Search
			TimetableTableSync::SearchResult timetables(
				TimetableTableSync::Search(
					_getEnv(),
					_book.get() ? _book->getKey() : 0
					, _requestParameters.orderField == PARAMETER_RANK
					, _requestParameters.orderField == PARAMETER_TITLE
					, _requestParameters.raisingOrder
			)	);
			
			AdminFunctionRequest<TimetableBookAdmin> searchRequest(_request);
			AdminActionFunctionRequest<TimetableAddAction,TimetableAdmin> addTimetableRequest(_request);
			addTimetableRequest.getAction()->setBook(_book);
			addTimetableRequest.setActionWillCreateObject();

			ActionResultHTMLTable::HeaderVector h3;
			h3.push_back(make_pair(string(), string()));
			h3.push_back(make_pair(string(), HTMLModule::getHTMLImage("arrow_up.png", "^")));
			h3.push_back(make_pair(string(), HTMLModule::getHTMLImage("arrow_down.png", "V")));
			h3.push_back(make_pair(PARAMETER_RANK, "Rang"));
			h3.push_back(make_pair(PARAMETER_TITLE, "Titre"));
			h3.push_back(make_pair(string(), "Actions"));
			h3.push_back(make_pair(string(), "Actions"));
			h3.push_back(make_pair(string(), "Actions"));
			ActionResultHTMLTable t3(
				h3,
				searchRequest.getHTMLForm(),
				_requestParameters,
				timetables,
				addTimetableRequest.getHTMLForm("addtimetable"),
				TimetableAddAction::PARAMETER_RANK
			);
			stream << t3.open();
			int lastRank(UNKNOWN_VALUE);
			int maxRank(TimetableTableSync::GetMaxRank(_book.get() ? _book->getKey() : 0));
			
			// Links to folders or timetable edition
			AdminFunctionRequest<TimetableBookAdmin> goFolderRequest(_request);
			AdminFunctionRequest<TimetableAdmin> editTimetableRequest(_request);
			BOOST_FOREACH(shared_ptr<Timetable> tt, timetables)
			{
				if (tt->getIsBook())
					goFolderRequest.getPage()->setBook(tt);
				else
					editTimetableRequest.getPage()->setTimetable(tt);

				lastRank = tt->getRank();

				stream << t3.row(Conversion::ToString(lastRank));
				stream <<
					t3.col() <<
					HTMLModule::getHTMLImage(
						tt->getIsBook() ? "table_multiple.png" : "table.png",
						tt->getIsBook() ? "Document" : "Fiche horaire"
					);
				stream << t3.col();
				if (lastRank > 0)
				{
					stream << HTMLModule::getHTMLLink(string(), HTMLModule::getHTMLImage("arrow_up.png", "^"));
				}
				stream << t3.col();
				if (lastRank < maxRank)
				{
					stream << HTMLModule::getHTMLLink(string(), HTMLModule::getHTMLImage("arrow_down.png", "V"));
				}
				stream << t3.col() << lastRank;
				stream << t3.col() << tt->getTitle();
				stream <<
					t3.col() <<
					HTMLModule::getLinkButton(
						tt->getIsBook() ? goFolderRequest.getURL() : editTimetableRequest.getURL(),
						"Ouvrir",
						string(),
						"table_edit.png"
					);
				stream << t3.col();
				if (tt->getIsBook())
				{
					stream << HTMLModule::getLinkButton(string(), "Dupliquer", string(), "table_add.png");
				}
				stream <<
					t3.col() <<
					HTMLModule::getLinkButton(
						string(),
						"Supprimer",
						"Etes-vous sûr de vouloir supprimer la fiche horaire "+ tt->getTitle() +" ?", "table_delete.png"
					);
			}
			stream << t3.row(Conversion::ToString(++lastRank));
			vector<pair<bool, string> > booknotbook;
			booknotbook.push_back(make_pair(true, HTMLModule::getHTMLImage("table_multiple.png","Document")));
			booknotbook.push_back(make_pair(false, HTMLModule::getHTMLImage("table.png","Fiche horaire")));
			stream << t3.col(3) << t3.getActionForm().getRadioInputCollection(TimetableAddAction::PARAMETER_IS_BOOK, booknotbook, false);
			stream << t3.col() << lastRank;
			stream << t3.col() << t3.getActionForm().getTextInput(TimetableAddAction::PARAMETER_TITLE, string(), "(titre de la nouvelle fiche horaire)");
			stream << t3.col() << t3.getActionForm().getSubmitButton("Créer");

			stream << t3.close();
		}

		
		
		AdminInterfaceElement::PageLinks TimetableBookAdmin::getSubPagesOfModule(
			const std::string& moduleKey,
			shared_ptr<const AdminInterfaceElement> currentPage,
				const server::FunctionRequest<admin::AdminRequest>& request
		) const	{
			AdminInterfaceElement::PageLinks links;

			if(	moduleKey == TimetableModule::FACTORY_KEY &&
				isAuthorized(request)
			){
				const TimetableBookAdmin* ta(
					dynamic_cast<const TimetableBookAdmin*>(currentPage.get())
				);

				if(	ta &&
					!ta->_book.get())
				{
					AddToLinks(links, currentPage);
				}
				else
				{
					AddToLinks(links, getNewPage());
				}
			}
			return links;
		}
		
		
		
		AdminInterfaceElement::PageLinks TimetableBookAdmin::getSubPages(
			shared_ptr<const AdminInterfaceElement> currentPage,
				const server::FunctionRequest<admin::AdminRequest>& request
		) const {
			const TimetableBookAdmin* ba(
				dynamic_cast<const TimetableBookAdmin*>(currentPage.get())
			);
			
			const TimetableAdmin* ta(
				dynamic_cast<const TimetableAdmin*>(currentPage.get())
			);

			AdminInterfaceElement::PageLinks links;

			// Subpages
			TimetableTableSync::SearchResult timetables(
				TimetableTableSync::Search(*_env, _book.get() ? _book->getKey() : 0)
			);
			BOOST_FOREACH(shared_ptr<Timetable> tt, timetables)
			{
				if(tt->getIsBook())
				{
					if(	ba &&
						ba->_book.get() &&
						tt->getKey() == ba->_book->getKey()
					){
						AddToLinks(links, currentPage);
					}
					else
					{
						shared_ptr<TimetableBookAdmin> page(
							getNewOtherPage<TimetableBookAdmin>()
						);
						page->setBook(tt);
						AddToLinks(links, page);
					}
				}
				else
				{
					if(	ta &&
						ta->getTimetable().get() &&
						ta->getTimetable()->getKey() == tt->getKey()
					){
						AddToLinks(links, currentPage);
					}
					else
					{
						shared_ptr<TimetableAdmin> page(
							getNewOtherPage<TimetableAdmin>()
						);
						page->setTimetable(tt);
						AddToLinks(links, page);
					}
				}
			}

			return links;
		}

		
		
		std::string TimetableBookAdmin::getTitle() const
		{
			return _book.get() ? _book->getTitle() : DEFAULT_TITLE;
		}



		bool TimetableBookAdmin::isAuthorized(
				const server::FunctionRequest<admin::AdminRequest>& _request
			) const
		{
			return _request.isAuthorized<TimetableRight>(READ);
		}
		
		void TimetableBookAdmin::setBook(boost::shared_ptr<Timetable> value)
		{
			_book = const_pointer_cast<const Timetable, Timetable>(value);
		}
	}
}

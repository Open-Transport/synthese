
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

#include "05_html/ActionResultHTMLTable.h"
#include "05_html/HTMLModule.h"
#include "05_html/HTMLList.h"

#include "35_timetables/TimetableModule.h"
#include "35_timetables/Timetable.h"
#include "35_timetables/TimetableTableSync.h"
#include "35_timetables/TimetableAdmin.h"
#include "35_timetables/TimetableAddAction.h"
#include "35_timetables/TimetableRight.h"

#include "30_server/QueryString.h"
#include "30_server/ActionFunctionRequest.h"
#include "30_server/Request.h"

#include "32_admin/ModuleAdmin.h"
#include "32_admin/AdminRequest.h"
#include "32_admin/AdminParametersException.h"

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
		
		void TimetableBookAdmin::setFromParametersMap(const ParametersMap& map)
		{
			_requestParameters.setFromParametersMap(map.getMap(), PARAMETER_RANK, ResultHTMLTable::UNLIMITED_SIZE);
			uid id(map.getUid(QueryString::PARAMETER_OBJECT_ID, false, FACTORY_KEY));
			if (id > 0)
			{
				try
				{
					_book = TimetableTableSync::Get(id);
				}
				catch(...)
				{
					throw AdminParametersException("No such book");
				}
				if (!_book->getIsBook())
					throw AdminParametersException("Timetable is not a book");
			}
		}
		
		void TimetableBookAdmin::display(ostream& stream, VariablesMap& variables, const FunctionRequest<AdminRequest>* request) const
		{
			// Requests
			FunctionRequest<AdminRequest> searchRequest(request);
			searchRequest.getFunction()->setPage<TimetableBookAdmin>();
			searchRequest.setObjectId(_book.get() ? _book->getKey() : 0);

//			ActionFunctionRequest<TimetableBookRemoveAction,AdminRequest> removeFolderRequest(request);
//			removeFolderRequest.getAction()->setBook(_book);
//			removeFolderRequest.getFunction()->setPage<TimetableBookAdmin>();

//			ActionFunctionRequest<TimetableBookUpdateAction,AdminRequest> updateFolderRequest(request);
//			updateFolderRequest.getAction()->setFolderId(_book.get() ? _book->getKey() : 0);
//			updateFolderRequest.getFunction()->setPage<TimetableBookAdmin>();
//			updateFolderRequest.setObjectId(_book.get() ? _book->getKey() : 0);

			ActionFunctionRequest<TimetableAddAction,AdminRequest> addTimetableRequest(request);
			addTimetableRequest.getAction()->setBook(_book);
			addTimetableRequest.getFunction()->setPage<TimetableAdmin>();

			FunctionRequest<AdminRequest> editTimetableRequest(request);
			editTimetableRequest.getFunction()->setPage<TimetableAdmin>();

			FunctionRequest<AdminRequest> goFolderRequest(request);
			goFolderRequest.getFunction()->setPage<TimetableBookAdmin>();

			// Search
			vector<shared_ptr<Timetable> > tt(TimetableTableSync::Search(
				_book.get() ? _book->getKey() : 0
				, _requestParameters.orderField == PARAMETER_RANK
				, _requestParameters.orderField == PARAMETER_TITLE
				, _requestParameters.raisingOrder
			));
			ResultHTMLTable::ResultParameters tt_rp;
			tt_rp.setFromResult(_requestParameters, tt);


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

			ActionResultHTMLTable::HeaderVector h3;
			h3.push_back(make_pair(string(), string()));
			h3.push_back(make_pair(string(), HTMLModule::getHTMLImage("arrow_up.png", "^")));
			h3.push_back(make_pair(string(), HTMLModule::getHTMLImage("arrow_down.png", "V")));
			h3.push_back(make_pair(PARAMETER_RANK, "Rang"));
			h3.push_back(make_pair(PARAMETER_TITLE, "Titre"));
			h3.push_back(make_pair(string(), "Actions"));
			h3.push_back(make_pair(string(), "Actions"));
			h3.push_back(make_pair(string(), "Actions"));
			ActionResultHTMLTable t3(h3, searchRequest.getHTMLForm(), _requestParameters, tt_rp, addTimetableRequest.getHTMLForm("addtimetable"), TimetableAddAction::PARAMETER_RANK);
			stream << t3.open();
			int lastRank(UNKNOWN_VALUE);
			int maxRank(TimetableTableSync::GetMaxRank(_book.get() ? _book->getKey() : 0));
			for (vector<shared_ptr<Timetable> >::const_iterator ittt(tt.begin()); ittt != tt.end(); ++ittt)
			{
				if ((*ittt)->getIsBook())
					goFolderRequest.setObjectId((*ittt)->getKey());
				else
					editTimetableRequest.setObjectId((*ittt)->getKey());

				lastRank = (*ittt)->getRank();

				stream << t3.row(Conversion::ToString(lastRank));
				stream << t3.col() << HTMLModule::getHTMLImage((*ittt)->getIsBook() ? "table_multiple.png" : "table.png", (*ittt)->getIsBook() ? "Document" : "Fiche horaire");
				stream << t3.col();
				if (lastRank > 0)
					stream << HTMLModule::getHTMLLink(string(), HTMLModule::getHTMLImage("arrow_up.png", "^"));
				stream << t3.col();
				if (lastRank < maxRank)
					stream << HTMLModule::getHTMLLink(string(), HTMLModule::getHTMLImage("arrow_down.png", "V"));;
				stream << t3.col() << lastRank;
				stream << t3.col() << (*ittt)->getTitle();
				stream << t3.col() << HTMLModule::getLinkButton((*ittt)->getIsBook() ? goFolderRequest.getURL() : editTimetableRequest.getURL(), "Modifier", string(), "table_edit.png");
				stream << t3.col();
				if ((*ittt)->getIsBook())
					stream << HTMLModule::getLinkButton(string(), "Dupliquer", string(), "table_add.png");
				stream << t3.col() << HTMLModule::getLinkButton(string(), "Supprimer", "Etes-vous sûr de vouloir supprimer la fiche horaire "+ (*ittt)->getTitle() +" ?", "table_delete.png");
			}
			stream << t3.row(Conversion::ToString(++lastRank));
			vector<pair<bool, string> > booknotbook;
			booknotbook.push_back(make_pair(true, HTMLModule::getHTMLImage("table_multiple.png","Document")));
			booknotbook.push_back(make_pair(false, HTMLModule::getHTMLImage("table.png","Fiche horaire")));
			stream << t3.col(3) << t3.getActionForm().getRadioInput(TimetableAddAction::PARAMETER_IS_BOOK, booknotbook, false);
			stream << t3.col() << lastRank;
			stream << t3.col() << t3.getActionForm().getTextInput(TimetableAddAction::PARAMETER_TITLE, string(), "(titre de la nouvelle fiche horaire)");
			stream << t3.col() << t3.getActionForm().getSubmitButton("Créer");

			stream << t3.close();
		}

		bool TimetableBookAdmin::isAuthorized(const FunctionRequest<AdminRequest>* request) const
		{
			return request->isAuthorized<TimetableRight>(READ);
		}
		
		AdminInterfaceElement::PageLinks TimetableBookAdmin::getSubPagesOfParent(
			const PageLink& parentLink
			, const AdminInterfaceElement& currentPage
			, const server::FunctionRequest<admin::AdminRequest>* request
		) const	{
			AdminInterfaceElement::PageLinks links;

			if(parentLink.factoryKey == admin::ModuleAdmin::FACTORY_KEY && parentLink.parameterValue == TimetableModule::FACTORY_KEY)
				links.push_back(getPageLink());

			return links;
		}
		
		AdminInterfaceElement::PageLinks TimetableBookAdmin::getSubPages(
			const AdminInterfaceElement& currentPage
			, const server::FunctionRequest<admin::AdminRequest>* request
		) const {
			AdminInterfaceElement::PageLinks links;

			// Subpages
			vector<shared_ptr<Timetable> > sv(TimetableTableSync::Search(_book.get() ? _book->getKey() : 0));
			for (vector<shared_ptr<Timetable> >::const_iterator it(sv.begin()); it != sv.end(); ++it)
			{
				PageLink link;
				link.factoryKey = (*it)->getIsBook() ? TimetableBookAdmin::FACTORY_KEY : TimetableAdmin::FACTORY_KEY;
				link.icon = (*it)->getIsBook() ? TimetableBookAdmin::ICON :  TimetableAdmin::ICON;
				link.name = (*it)->getTitle();
				link.parameterName = QueryString::PARAMETER_OBJECT_ID;
				link.parameterValue = Conversion::ToString((*it)->getKey());
				links.push_back(link);
			}

			return links;
		}

		
		
		std::string TimetableBookAdmin::getTitle() const
		{
			return _book.get() ? _book->getTitle() : DEFAULT_TITLE;
		}

		std::string TimetableBookAdmin::getParameterName() const
		{
			return _book.get() ? QueryString::PARAMETER_OBJECT_ID : string();
		}

		std::string TimetableBookAdmin::getParameterValue() const
		{
			return _book.get() ? Conversion::ToString(_book->getKey()) : string();
		}
	}
}

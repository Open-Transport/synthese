
/** ResaLogAdmin class implementation.
	@file ResaLogAdmin.cpp
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

#include "ResaLogAdmin.h"

#include "ResaModule.h"
#include "ResaDBLog.h"
#include "ResaRight.h"
#include "CancelReservationAction.h"
#include "QueryString.h"
#include "Request.h"
#include "AdminParametersException.h"
#include "ModuleAdmin.h"
#include "AdminRequest.h"
#include "SearchFormHTMLTable.h"
#include "DBLogEntry.h"
#include "DBLogEntryTableSync.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace admin;
	using namespace interfaces;
	using namespace server;
	using namespace util;
	using namespace resa;
	using namespace html;
	using namespace time;
	using namespace dblog;
	using namespace security;

	namespace util
	{
		template<> const string FactorableTemplate<AdminInterfaceElement, ResaLogAdmin>::FACTORY_KEY("ResaLogAdmin");
	}

	namespace admin
	{
		template<> const string AdminInterfaceElementTemplate<ResaLogAdmin>::ICON("book_open.png");
		template<> const string AdminInterfaceElementTemplate<ResaLogAdmin>::DEFAULT_TITLE("Journal");
	}

	namespace resa
	{
		const string ResaLogAdmin::PARAMETER_DATE("da");

		ResaLogAdmin::ResaLogAdmin()
			: AdminInterfaceElementTemplate<ResaLogAdmin>()
			, _searchDate(TIME_CURRENT)
		{ }
		
		void ResaLogAdmin::setFromParametersMap(
			const ParametersMap& map,
			bool doDisplayPreparationActions
		){
			_requestParameters.setFromParametersMap(map.getMap(), PARAMETER_DATE, 50, false);
			_searchDate = map.getDate(PARAMETER_DATE, false, FACTORY_KEY);

			if(!doDisplayPreparationActions) return;

			// Search
			DateTime searchStartDate(_searchDate);
			DateTime searchEndDate(searchStartDate);
			searchEndDate += 1;
			DBLogEntryTableSync::Search(
				_env,
				ResaDBLog::FACTORY_KEY
				, searchStartDate
				, searchEndDate
				, UNKNOWN_VALUE // _searchUser.get() ? _searchUser->getKey() : UNKNOWN_VALUE
				, DBLogEntry::DB_LOG_UNKNOWN
				, UNKNOWN_VALUE // _searchObjectId
				, "" // _searchText
				, _requestParameters.first
				, _requestParameters.maxSize
				, _requestParameters.orderField == PARAMETER_DATE
				, false // _resultTableRequestParameters.orderField == PARAMETER_SEARCH_USER
				, false // _resultTableRequestParameters.orderField == PARAMETER_SEARCH_TYPE
				, _requestParameters.raisingOrder
			);
		}
		
		
		
		server::ParametersMap ResaLogAdmin::getParametersMap() const
		{
			ParametersMap m(_requestParameters.getParametersMap());
			m.insert(PARAMETER_DATE, _searchDate);
			return m;
		}

		
		void ResaLogAdmin::display(ostream& stream, VariablesMap& variables) const
		{
			// Requests
			FunctionRequest<AdminRequest> searchRequest(_request);
			searchRequest.getFunction()->setSamePage(this);

			ActionFunctionRequest<CancelReservationAction,AdminRequest> cancelRequest(_request);
			cancelRequest.getFunction()->setSamePage(this);

			ResultHTMLTable::ResultParameters rp;
			rp.setFromResult(_requestParameters, _env.getEditableRegistry<DBLogEntry>());
			
			// Search form
			SearchFormHTMLTable st(searchRequest.getHTMLForm());
			stream << st.open();
			stream << st.cell("Date", st.getForm().getCalendarInput(PARAMETER_DATE, _searchDate));
			stream << st.close();

			// Results
			ResaModule::DisplayResaDBLog(
				stream
				, _env
				, PARAMETER_DATE
				, searchRequest
				, cancelRequest
				, _requestParameters
				, true
			);
		}

		bool ResaLogAdmin::isAuthorized(
		) const	{
			return _request->isAuthorized<ResaRight>(READ, UNKNOWN_RIGHT_LEVEL);
		}
		
		AdminInterfaceElement::PageLinks ResaLogAdmin::getSubPagesOfParent(
			const PageLink& parentLink
			, const AdminInterfaceElement& currentPage
		) const	{
			AdminInterfaceElement::PageLinks links;
			if(parentLink.factoryKey == ModuleAdmin::FACTORY_KEY && parentLink.parameterValue == ResaModule::FACTORY_KEY)
				links.push_back(getPageLink());
			return links;
		}
	}
}

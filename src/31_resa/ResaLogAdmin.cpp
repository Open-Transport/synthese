
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

#include "31_resa/ResaModule.h"
#include "31_resa/ResaDBLog.h"
#include "31_resa/ResaRight.h"
#include "31_resa/CancelReservationAction.h"

#include "30_server/QueryString.h"
#include "30_server/Request.h"

#include "32_admin/AdminParametersException.h"
#include "32_admin/ModuleAdmin.h"
#include "32_admin/AdminRequest.h"

#include "05_html/SearchFormHTMLTable.h"

#include "13_dblog/DBLogEntry.h"
#include "13_dblog/DBLogEntryTableSync.h"

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
		
		void ResaLogAdmin::setFromParametersMap(const ParametersMap& map)
		{
			_requestParameters.setFromParametersMap(map.getMap(), PARAMETER_DATE, 50, false);
			_searchDate = map.getDate(PARAMETER_DATE, false, FACTORY_KEY);
		}
		
		void ResaLogAdmin::display(ostream& stream, VariablesMap& variables, const FunctionRequest<AdminRequest>* request) const
		{
			// Requests
			FunctionRequest<AdminRequest> searchRequest(request);
			searchRequest.getFunction()->setPage<ResaLogAdmin>();

			ActionFunctionRequest<CancelReservationAction,AdminRequest> cancelRequest(request);
			cancelRequest.getFunction()->setPage<ResaLogAdmin>();
			cancelRequest.setObjectId(request->getObjectId());

			// Search
			DateTime searchStartDate(_searchDate);
			DateTime searchEndDate(searchStartDate);
			searchEndDate += 1;
			vector<shared_ptr<DBLogEntry> >	result(DBLogEntryTableSync::search(
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
			));
			ResultHTMLTable::ResultParameters rp;
			rp.setFromResult(_requestParameters, result);
			
			// Search form
			SearchFormHTMLTable st(searchRequest.getHTMLForm());
			stream << st.open();
			stream << st.cell("Date", st.getForm().getCalendarInput(PARAMETER_DATE, _searchDate));
			stream << st.close();

			// Results
			ResaModule::DisplayResaDBLog(
				stream
				, result
				, PARAMETER_DATE
				, searchRequest
				, cancelRequest
				, _requestParameters
				, true
			);
		}

		bool ResaLogAdmin::isAuthorized(const FunctionRequest<AdminRequest>* request) const
		{
			return request->isAuthorized<ResaRight>(READ, UNKNOWN_RIGHT_LEVEL);
		}
		
		AdminInterfaceElement::PageLinks ResaLogAdmin::getSubPagesOfParent(
			const PageLink& parentLink
			, const AdminInterfaceElement& currentPage
			, const server::FunctionRequest<admin::AdminRequest>* request
		) const	{
			AdminInterfaceElement::PageLinks links;
			if(parentLink.factoryKey == ModuleAdmin::FACTORY_KEY && parentLink.parameterValue == ResaModule::FACTORY_KEY)
				links.push_back(getPageLink());
			return links;
		}
	}
}

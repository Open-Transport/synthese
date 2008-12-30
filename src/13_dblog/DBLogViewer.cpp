////////////////////////////////////////////////////////////////////////////////
/// DBLogViewer class implementation.
///	@file DBLogViewer.cpp
///	@author Hugues Romain
///
///	This file belongs to the SYNTHESE project (public transportation specialized
///	software)
///	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>
///
///	This program is free software; you can redistribute it and/or
///	modify it under the terms of the GNU General Public License
///	as published by the Free Software Foundation; either version 2
///	of the License, or (at your option) any later version.
///
///	This program is distributed in the hope that it will be useful,
///	but WITHOUT ANY WARRANTY; without even the implied warranty of
///	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
///	GNU General Public License for more details.
///
///	You should have received a copy of the GNU General Public License
///	along with this program; if not, write to the Free Software Foundation,
///	Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
////////////////////////////////////////////////////////////////////////////////

#include "01_util/Constants.h"
#include "ResultHTMLTable.h"
#include "SearchFormHTMLTable.h"
#include "InterfaceModule.h"
#include "SecurityModule.h"
#include "User.h"
#include "UserTableSync.h"
#include "DBLog.h"
#include "DBLogViewer.h"
#include "DBLogModule.h"
#include "DBLogEntryTableSync.h"
#include "DBLogRight.h"
#include "QueryString.h"
#include "Request.h"
#include "AdminParametersException.h"
#include "AdminModule.h"
#include "AdminRequest.h"
#include "ModuleAdmin.h"

#include <sstream>
#include <boost/shared_ptr.hpp>
#include <boost/foreach.hpp>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace admin;
	using namespace interfaces;
	using namespace server;
	using namespace util;
	using namespace security;
	using namespace html;
	using namespace time;
	using namespace dblog;

	namespace util
	{
		template<> const string FactorableTemplate<AdminInterfaceElement,DBLogViewer>::FACTORY_KEY("dblog");
	}

	namespace admin
	{
		template<> const string AdminInterfaceElementTemplate<DBLogViewer>::ICON("book_open.png");
		template<> const string AdminInterfaceElementTemplate<DBLogViewer>::DEFAULT_TITLE("Journal inconnu");
	}

	namespace dblog
	{
		const string DBLogViewer::PARAMETER_LOG_KEY = "dlvk";
		const string DBLogViewer::PARAMETER_SEARCH_USER = "dlvsu";
		const string DBLogViewer::PARAMETER_SEARCH_TYPE = "dlvst";
		const string DBLogViewer::PARAMETER_START_DATE = "dlvsd";
		const string DBLogViewer::PARAMETER_END_DATE = "dlved";
		const string DBLogViewer::PARAMETER_SEARCH_TEXT = "dlvsx";

		DBLogViewer::DBLogViewer()
			: AdminInterfaceElementTemplate<DBLogViewer>()
			, _searchLevel(DBLogEntry::DB_LOG_UNKNOWN), _searchStartDate(TIME_UNKNOWN)
			, _searchEndDate(TIME_UNKNOWN), _searchObjectId(UNKNOWN_VALUE)
		{}

		void DBLogViewer::setFromParametersMap(const ParametersMap& map)
		{
			// Log key
			setLogKey(map.getString(PARAMETER_LOG_KEY, true, FACTORY_KEY));

			// Start Date
			_searchStartDate = map.getDateTime(PARAMETER_START_DATE, false, FACTORY_KEY);

			// End Date
			_searchEndDate = map.getDateTime(PARAMETER_END_DATE, false, FACTORY_KEY);

			// User
			uid id(map.getUid(PARAMETER_SEARCH_USER, false, FACTORY_KEY));
			if (id > 0)
				_searchUser = UserTableSync::Get(id, _env);

			// Level
			int num(map.getInt(PARAMETER_SEARCH_TYPE, false, FACTORY_KEY));
			if (num != UNKNOWN_VALUE)
				_searchLevel = static_cast<DBLogEntry::Level>(num);

			// Text
			_searchText = map.getString(PARAMETER_SEARCH_TEXT, false, FACTORY_KEY);

			// Object
			_searchObjectId = map.getUid(QueryString::PARAMETER_OBJECT_ID, false, FACTORY_KEY);

			// table parameters
			_resultTableRequestParameters.setFromParametersMap(map.getMap(), PARAMETER_START_DATE, 30, false);

			// Search
			DBLogEntryTableSync::Search(
				_env,
				_dbLog->getFactoryKey()
				, _searchStartDate
				, _searchEndDate
				, _searchUser.get() ? _searchUser->getKey() : UNKNOWN_VALUE
				, _searchLevel
				, _searchObjectId
				, _searchText
				, _resultTableRequestParameters.first
				, _resultTableRequestParameters.maxSize
				, _resultTableRequestParameters.orderField == PARAMETER_START_DATE
				, _resultTableRequestParameters.orderField == PARAMETER_SEARCH_USER
				, _resultTableRequestParameters.orderField == PARAMETER_SEARCH_TYPE
				, _resultTableRequestParameters.raisingOrder
				);
			_resultTableResultParameters.setFromResult(_resultTableRequestParameters, _env.getEditableRegistry<DBLogEntry>());
		}


		void DBLogViewer::display(ostream& stream, interfaces::VariablesMap& variables) const
		{
			// Requests
			FunctionRequest<AdminRequest> searchRequest(_request);
			searchRequest.getFunction()->setPage<DBLogViewer>();

			stream << "<h1>Recherche d'entrées</h1>";

			SearchFormHTMLTable st(searchRequest.getHTMLForm("search"));
			st.getForm().addHiddenField(PARAMETER_LOG_KEY, _dbLog->getFactoryKey());
			stream << st.open();
			stream << st.cell("Date début", st.getForm().getCalendarInput(PARAMETER_START_DATE, _searchStartDate));
			stream << st.cell("Date fin", st.getForm().getCalendarInput(PARAMETER_END_DATE, _searchEndDate));
			stream << st.cell("Utilisateur", st.getForm().getSelectInput(PARAMETER_SEARCH_USER, SecurityModule::getUserLabels(true), _searchUser ? _searchUser->getKey() : 0));
			stream << st.cell("Type", st.getForm().getSelectInput(PARAMETER_SEARCH_TYPE, DBLogModule::getEntryLevelLabels(true), (int) _searchLevel));
			stream << st.cell("Texte", st.getForm().getTextInput(PARAMETER_SEARCH_TEXT, _searchText));
			stream << st.close();
			
			stream << "<h1>Résultat de la recherche</h1>";

			ResultHTMLTable::HeaderVector v;
			v.push_back(make_pair(PARAMETER_SEARCH_TYPE, "Type"));
			v.push_back(make_pair(PARAMETER_START_DATE, "Date"));
			v.push_back(make_pair(PARAMETER_SEARCH_USER, "Utilisateur"));
			v.push_back(make_pair(string(), "Objet"));
			DBLog::ColumnsVector customCols = _dbLog->getColumnNames();
			for (DBLog::ColumnsVector::const_iterator it = customCols.begin(); it != customCols.end(); ++it)
				v.push_back(make_pair(string(), *it));

			ResultHTMLTable t(v, st.getForm(), _resultTableRequestParameters, _resultTableResultParameters, InterfaceModule::getVariableFromMap(variables, AdminModule::ICON_PATH_INTERFACE_VARIABLE));

			stream << t.open();
			
			BOOST_FOREACH(shared_ptr<DBLogEntry> dbe, _env.getRegistry<DBLogEntry>())
			{
				stream << t.row();
				stream << t.col() << HTMLModule::getHTMLImage(DBLogModule::getEntryIcon(dbe->getLevel()), DBLogModule::getEntryLevelLabel(dbe->getLevel()));
				stream << t.col() << dbe->getDate().toString(true);
				stream << t.col() << (dbe->getUser() ? dbe->getUser()->getLogin() : "(supprimé)");
				stream << t.col() << ((dbe->getObjectId() > 0) ? _dbLog->getObjectName(dbe->getObjectId()) : string());

				DBLog::ColumnsVector cols = _dbLog->parse(*dbe);
				for (DBLog::ColumnsVector::const_iterator it = cols.begin(); it != cols.end(); ++it)
					stream << t.col() << *it;
			}
			
			stream << t.close();
		}

		bool DBLogViewer::isAuthorized() const
		{
			return _request->isAuthorized<DBLogRight>(READ);
		}



		AdminInterfaceElement::PageLinks DBLogViewer::getSubPagesOfParent(
			const PageLink& parentLink
			, const AdminInterfaceElement& currentPage
		) const	{
			AdminInterfaceElement::PageLinks links;
			if (parentLink.factoryKey == ModuleAdmin::FACTORY_KEY && parentLink.parameterValue == DBLogModule::FACTORY_KEY)
			{
				vector<shared_ptr<DBLog> > logs(Factory<DBLog>::GetNewCollection());
				BOOST_FOREACH(const shared_ptr<DBLog> loge, logs)
				{
					AdminInterfaceElement::PageLink link(getPageLink());
					link.name = loge->getName();
					link.parameterName = PARAMETER_LOG_KEY;
					link.parameterValue = loge->getFactoryKey();
					links.push_back(link);
				}
			}
			return links;
		}

		AdminInterfaceElement::PageLinks DBLogViewer::getSubPages( const AdminInterfaceElement& currentPage) const
		{
			return AdminInterfaceElement::PageLinks();
		}

		std::string DBLogViewer::getTitle() const
		{
			return _dbLog.get() ? _dbLog->getName() : DEFAULT_TITLE;
		}

		std::string DBLogViewer::getParameterName() const
		{
			return _dbLog.get() ? PARAMETER_LOG_KEY : string();
		}

		std::string DBLogViewer::getParameterValue() const
		{
			return _dbLog.get() ? _dbLog->getFactoryKey() : string();
		}

		void DBLogViewer::setLogKey( const std::string& key )
		{
			if (!Factory<DBLog>::contains(key))
				throw AdminParametersException("Invalid log key : " + key);
			_dbLog.reset(Factory<DBLog>::create(key));
		}

		server::ParametersMap DBLogViewer::getParametersMap() const
		{
			server::ParametersMap m;
			if (_dbLog.get())
				m.insert(PARAMETER_LOG_KEY, _dbLog->getFactoryKey());
			return m;
		}
	}
}

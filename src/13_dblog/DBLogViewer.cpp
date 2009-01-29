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


		DBLogViewer::DBLogViewer()
		:	AdminInterfaceElementTemplate<DBLogViewer>(),
			_viewer(FACTORY_KEY)
		{}

		void DBLogViewer::setFromParametersMap(const ParametersMap& map)
		{
			_viewer.set(
				map,
				map.getString(PARAMETER_LOG_KEY, true, FACTORY_KEY)
			);
		}


		void DBLogViewer::display(
			ostream& stream,
			interfaces::VariablesMap& variables
		) const {
			stream << "<h1>Journal</h1>";

			// Requests
			FunctionRequest<AdminRequest> searchRequest(_request);
			searchRequest.getFunction()->setPage<DBLogViewer>();

			_viewer.display(
				stream,
				searchRequest,
				true, false
			);
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

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
#include "DBLogViewer.h"
#include "DBLogModule.h"
#include "DBLogEntryTableSync.h"
#include "DBLogRight.h"
#include "AdminParametersException.h"
#include "AdminModule.h"
#include "AdminInterfaceElement.h"
#include "ActionFunctionRequest.h"
#include "ModuleAdmin.h"
#include "DBLog.h"
#include "DBLogPurgeAction.h"
#include "PropertiesHTMLTable.h"

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

		void DBLogViewer::setFromParametersMap(
			const ParametersMap& map,
			bool doDisplayPreparationActions
		){
			_viewer.set(
				map,
				map.get<string>(PARAMETER_LOG_KEY)
			);
		}
		
		
		
		server::ParametersMap DBLogViewer::getParametersMap() const
		{
			ParametersMap m(_viewer.getParametersMap());
			m.insert(PARAMETER_LOG_KEY, _viewer.getLogKey());
			return m;
		}



		void DBLogViewer::display(
			ostream& stream,
			interfaces::VariablesMap& variables
		) const {
			stream << "<h1>Journal</h1>";

			// Requests
			FunctionRequest<AdminRequest> searchRequest(_request);

			ActionFunctionRequest<DBLogPurgeAction, AdminRequest> purgeRequest(_request);
			purgeRequest.getAction()->setDBLog(_viewer.getLogKey());

			_viewer.display(
				stream,
				searchRequest,
				true, false
			);

			if(purgeRequest.getAction()->_isAuthorized())
			{
				stream << "<h1>Purge</h1>";

				PropertiesHTMLTable t(purgeRequest.getHTMLForm("purge"));
				stream << t.open();
				stream << t.cell("Date max", t.getForm().getCalendarInput(DBLogPurgeAction::PARAMETER_END_DATE, DateTime(TIME_CURRENT)));
				stream << t.close();
			}
		}

		bool DBLogViewer::isAuthorized() const
		{
			return
				_request->isAuthorized<DBLogRight>(READ) &&
				(	_viewer.getLogKey().empty() ||
					_viewer.isAuthorized(*_request)
				)
			;
		}



		AdminInterfaceElement::PageLinks DBLogViewer::getSubPagesOfParent(
			const PageLink& parentLink
			, const AdminInterfaceElement& currentPage
		) const	{
			AdminInterfaceElement::PageLinks links;
			if(	parentLink.factoryKey == ModuleAdmin::FACTORY_KEY &&
				parentLink.parameterValue == DBLogModule::FACTORY_KEY
			){
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



		AdminInterfaceElement::PageLinks DBLogViewer::getSubPages(
			const AdminInterfaceElement& currentPage
		) const {
			return AdminInterfaceElement::PageLinks();
		}
		
		

		std::string DBLogViewer::getTitle() const
		{
			return
				_viewer.getLogKey().empty() ?
				DEFAULT_TITLE :
				_viewer.getLogName()
			;
		}



		std::string DBLogViewer::getParameterName() const
		{
			return
				_viewer.getLogKey().empty() ?
				string() :
				PARAMETER_LOG_KEY
			;
		}



		std::string DBLogViewer::getParameterValue() const
		{
			return _viewer.getLogKey();
		}



		void DBLogViewer::setLogKey( const std::string& key )
		{
			try
			{
				_viewer.setLogKey(key);
			}
			catch(Exception& e)
			{
				throw AdminParametersException("Invalid log key : " + key);
			}
		}
	}
}

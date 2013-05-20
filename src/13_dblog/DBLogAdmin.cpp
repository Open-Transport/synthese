////////////////////////////////////////////////////////////////////////////////
/// DBLogAdmin class implementation.
///	@file DBLogAdmin.cpp
///	@author Hugues Romain
///
///	This file belongs to the SYNTHESE project (public transportation specialized
///	software)
///	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>
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

#include "UtilConstants.h"
#include "ResultHTMLTable.h"
#include "SearchFormHTMLTable.h"
#include "SecurityModule.h"
#include "User.h"
#include "UserTableSync.h"
#include "DBLogAdmin.h"
#include "DBLogModule.h"
#include "DBLogEntryTableSync.h"
#include "DBLogRight.h"
#include "AdminParametersException.h"
#include "AdminModule.h"
#include "AdminInterfaceElement.h"
#include "AdminActionFunctionRequest.hpp"
#include "ModuleAdmin.h"
#include "DBLog.h"
#include "DBLogPurgeAction.h"
#include "PropertiesHTMLTable.h"
#include "AdminFunctionRequest.hpp"

#include <sstream>
#include <boost/shared_ptr.hpp>
#include <boost/foreach.hpp>

using namespace std;
using namespace boost;
using namespace boost::posix_time;

namespace synthese
{
	using namespace admin;
	using namespace server;
	using namespace util;
	using namespace security;
	using namespace html;
	using namespace dblog;

	namespace util
	{
		template<> const string FactorableTemplate<AdminInterfaceElement,DBLogAdmin>::FACTORY_KEY("dblog");
	}

	namespace admin
	{
		template<> const string AdminInterfaceElementTemplate<DBLogAdmin>::ICON("book_open.png");
		template<> const string AdminInterfaceElementTemplate<DBLogAdmin>::DEFAULT_TITLE("Journal inconnu");
	}

	namespace dblog
	{
		const string DBLogAdmin::PARAMETER_LOG_KEY = "dlvk";


		DBLogAdmin::DBLogAdmin()
		:	AdminInterfaceElementTemplate<DBLogAdmin>(),
			_viewer(FACTORY_KEY)
		{}

		void DBLogAdmin::setFromParametersMap(
			const ParametersMap& map
		){
			_viewer.set(
				map,
				map.get<string>(PARAMETER_LOG_KEY)
			);
		}



		util::ParametersMap DBLogAdmin::getParametersMap() const
		{
			ParametersMap m(_viewer.getParametersMap());
			m.insert(PARAMETER_LOG_KEY, _viewer.getLogKey());
			return m;
		}



		void DBLogAdmin::display(
			ostream& stream,
			const server::Request& _request
		) const {
			stream << "<h1>Journal</h1>";

			// Requests
			AdminFunctionRequest<DBLogAdmin> searchRequest(_request, *this);

			AdminActionFunctionRequest<DBLogPurgeAction, DBLogAdmin> purgeRequest(_request, *this);
			purgeRequest.getAction()->setDBLog(_viewer.getLogKey());

			_viewer.display(
				stream,
				searchRequest,
				true, false
			);

			if(purgeRequest.getAction()->isAuthorized(_request.getSession().get()))
			{
				stream << "<h1>Purge</h1>";

				PropertiesHTMLTable t(purgeRequest.getHTMLForm("purge"));
				stream << t.open();
				stream << t.cell(
					"Date max",
					t.getForm().getCalendarInput(DBLogPurgeAction::PARAMETER_END_DATE, second_clock::local_time())
				);
				stream << t.close();
			}
		}

		bool DBLogAdmin::isAuthorized(
			const security::User& user
		) const	{
			return _viewer.isAuthorized(*user.getProfile());
		}



		AdminInterfaceElement::PageLinks DBLogAdmin::getSubPagesOfModule(
			const ModuleClass& module,
			const AdminInterfaceElement& currentPage,
			const server::Request& request
		) const	{
			AdminInterfaceElement::PageLinks links;

			if(	dynamic_cast<const DBLogModule*>(&module) &&
				request.getUser() &&
				request.getUser()->getProfile()
			){
				vector<boost::shared_ptr<DBLog> > logs(Factory<DBLog>::GetNewCollection());
				BOOST_FOREACH(const boost::shared_ptr<DBLog> loge, logs)
				{
					if(!loge->isAuthorized(*request.getUser()->getProfile(), READ) || loge->getName().empty()) continue;

					boost::shared_ptr<DBLogAdmin> p(
						getNewPage<DBLogAdmin>()
					);
					p->_viewer.setLogKey(
						loge->getFactoryKey()
					);
					links.push_back(p);
				}
			}
			return links;
		}



		std::string DBLogAdmin::getTitle() const
		{
			return
				_viewer.getLogKey().empty() ?
				DEFAULT_TITLE :
				_viewer.getLogName()
			;
		}



		void DBLogAdmin::setLogKey( const std::string& key )
		{
			try
			{
				_viewer.setLogKey(key);
			}
			catch(Exception&)
			{
				throw AdminParametersException("Invalid log key : " + key);
			}
		}


		bool DBLogAdmin::_hasSameContent(const AdminInterfaceElement& other) const
		{
			return _viewer.getLogKey() == static_cast<const DBLogAdmin&>(other)._viewer.getLogKey();
		}

	}
}

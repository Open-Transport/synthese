
/** ThreadsAdmin class implementation.
	@file ThreadsAdmin.cpp
	@author Hugues
	@date 2009

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

#include "ServerModule.h"
#include "ThreadsAdmin.h"
#include "AdminParametersException.h"
#include "ServerAdminRight.h"
#include "ResultHTMLTable.h"
#include "AdminActionFunctionRequest.hpp"
#include "ThreadKillAction.h"

#include <boost/date_time/posix_time/posix_time.hpp>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace admin;
	using namespace interfaces;
	using namespace server;
	using namespace util;
	using namespace html;
	using namespace security;

	namespace util
	{
		template<> const string FactorableTemplate<AdminInterfaceElement, ThreadsAdmin>::FACTORY_KEY("ThreadsAdmin");
	}

	namespace admin
	{
		template<> const string AdminInterfaceElementTemplate<ThreadsAdmin>::ICON("cog.png");
		template<> const string AdminInterfaceElementTemplate<ThreadsAdmin>::DEFAULT_TITLE("Threads");
	}

	namespace server
	{
		void ThreadsAdmin::setFromParametersMap(const ParametersMap& map, bool)
		{
		}
		
		void ThreadsAdmin::display(ostream& stream, VariablesMap& variables, const FunctionRequest<AdminRequest>& request) const
		{
			bool killRight(request.isAuthorized<ServerAdminRight>(DELETE_RIGHT));
			AdminActionFunctionRequest<ThreadKillAction,ThreadsAdmin> killRequest(request);

			const ServerModule::Threads& threads(ServerModule::GetThreads());
			stream << "<h1>" << threads.size() << " threads</h1>";

			HTMLTable::ColsVector c;
			c.push_back("ID");
			c.push_back("Role");
			c.push_back("Status");
			c.push_back("Query");
			c.push_back("Duration");
			if(killRight)
			{
				c.push_back("Action");
			}
			HTMLTable t(c, ResultHTMLTable::CSS_CLASS);
			stream << t.open();

			BOOST_FOREACH(const ServerModule::Threads::value_type& it, threads)
			{
				const ServerModule::ThreadInfo& info(it.second);
				stream << t.row();
				stream << t.col() << it.first;
				stream << t.col() << info.description;
				if(info.theThread->get_id() == this_thread::get_id())
				{
					stream << " <b>(current thread)</b>";
				}
				stream << t.col();
				switch(info.status)
				{
				case ServerModule::ThreadInfo::THREAD_WAITING:
					stream << "Waiting";
					break;
				case ServerModule::ThreadInfo::THREAD_ANALYSING_REQUEST:
					stream << "Analysing a query string";
					break;
				case ServerModule::ThreadInfo::THREAD_RUNNING_ACTION:
					stream << "Running an action";
					break;
				case ServerModule::ThreadInfo::THREAD_RUNNING_FUNCTION:
					stream << "Running a function";
					break;
				}
				stream << t.col();
				if(info.status != ServerModule::ThreadInfo::THREAD_WAITING)
				{
					stream << info.queryString;
				}
				posix_time::time_duration d(posix_time::microsec_clock::local_time() - info.lastChangeTime);
				stream << t.col() << d.total_seconds() << " s";
				if(killRight)
				{
					stream << t.col();
					if(	info.theThread->get_id() != this_thread::get_id()
					){
						killRequest.getAction()->setThreadId(it.first);
						stream << HTMLModule::getLinkButton(killRequest.getURL(),"Kill","Are you sure to kill the thread "+ it.first +" ?", "stop.png");
					}
				}
			}

			stream << t.close();
		}

		bool ThreadsAdmin::isAuthorized(const FunctionRequest<AdminRequest>& request) const
		{
			return request.isAuthorized<ServerAdminRight>(READ);
		}



		AdminInterfaceElement::PageLinks ThreadsAdmin::getSubPagesOfModule(
			const std::string& moduleKey,
			const AdminInterfaceElement& currentPage,
			const server::FunctionRequest<admin::AdminRequest>& request
		) const	{
			PageLinks links;

			if(	moduleKey == ServerModule::FACTORY_KEY &&
				isAuthorized(request)
			){
				AddToLinks(links, getNewPage());
			}
			return links;
		}



		server::ParametersMap ThreadsAdmin::getParametersMap() const
		{
			return ParametersMap();
		}
	}
}

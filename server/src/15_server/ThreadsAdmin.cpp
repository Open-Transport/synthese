
/** ThreadsAdmin class implementation.
	@file ThreadsAdmin.cpp
	@author Hugues
	@date 2009

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>

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

#include "ThreadsAdmin.h"

#include "Profile.h"
#include "ServerModule.h"
#include "User.h"
#include "AdminParametersException.h"
#include "ServerAdminRight.h"
#include "ResultHTMLTable.h"
#include "AdminActionFunctionRequest.hpp"
#include "ThreadKillAction.h"
#include "Profile.h"
#include "QuitAction.hpp"
#include "StaticActionRequest.h"

#include <boost/date_time/posix_time/posix_time.hpp>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace admin;
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
		void ThreadsAdmin::setFromParametersMap(const ParametersMap& map)
		{
		}

		void ThreadsAdmin::display(
			ostream& stream,
			const Request& request
		) const	{

			bool killRight(request.getUser()->getProfile()->isAuthorized<ServerAdminRight>(DELETE_RIGHT));
			AdminActionFunctionRequest<ThreadKillAction,ThreadsAdmin> killRequest(request, *this);

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
						stream << HTMLModule::getLinkButton(killRequest.getURL(),"Kill","Are you sure to kill the thread "+ it.first +" ?", "/admin/img/stop.png");
					}
				}
			}

			stream << t.close();

			stream << "<h1>Arrêt du serveur</h1>";

			StaticActionRequest<QuitAction> quitAction(request);
			stream << "<p>";
			stream << HTMLModule::getLinkButton(quitAction.getURL(), "Arrêter le serveur", "Etes-vous sûr de vouloir arrêter le serveur ?");
			stream << "</p>";
		}

		bool ThreadsAdmin::isAuthorized(
			const security::User& user
		) const	{
			return user.getProfile()->isAuthorized<ServerAdminRight>(READ);
		}



		AdminInterfaceElement::PageLinks ThreadsAdmin::getSubPagesOfModule(
			const ModuleClass& module,
			const AdminInterfaceElement& currentPage,
			const server::Request& request
		) const	{
			PageLinks links;

			if(	dynamic_cast<const ServerModule*>(&module) &&
				request.getUser() &&
				request.getUser()->getProfile() &&
				isAuthorized(*request.getUser())
			){
				links.push_back(getNewCopiedPage());
			}
			return links;
		}



		util::ParametersMap ThreadsAdmin::getParametersMap() const
		{
			return ParametersMap();
		}
	}
}

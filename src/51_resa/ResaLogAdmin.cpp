
/** ResaLogAdmin class implementation.
	@file ResaLogAdmin.cpp
	@author Hugues Romain
	@date 2008

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

#include "ResaLogAdmin.h"

#include "ResaEditLogEntryAdmin.h"
#include "ResaModule.h"
#include "ResaDBLog.h"
#include "ResaRight.h"
#include "User.h"
#include "CancelReservationAction.h"
#include "Request.h"
#include "Profile.h"
#include "AdminParametersException.h"
#include "ModuleAdmin.h"
#include "AdminInterfaceElement.h"
#include "SearchFormHTMLTable.h"
#include "DBLogEntry.h"
#include "DBLogEntryTableSync.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace admin;
	using namespace server;
	using namespace util;
	using namespace resa;
	using namespace html;
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
		ResaLogAdmin::ResaLogAdmin()
			: AdminInterfaceElementTemplate<ResaLogAdmin>(),
			_log("log")
		{ }

		void ResaLogAdmin::setFromParametersMap(
			const ParametersMap& map
		){
			_log.set(
				map,
				ResaDBLog::FACTORY_KEY
			);
		}



		util::ParametersMap ResaLogAdmin::getParametersMap() const
		{
			ParametersMap m(_log.getParametersMap());
			return m;
		}


		void ResaLogAdmin::display(
			ostream& stream,
			const server::Request& _request
		) const	{

			stream << "<h1>Journal</h1>";

			// Results
			_log.display(
				stream,
				_request,
				true,
				true
			);
		}

		bool ResaLogAdmin::isAuthorized(
			const security::User& user
		) const	{
			return
				user.getProfile()->isAuthorized<ResaRight>(READ, UNKNOWN_RIGHT_LEVEL) &&
				ResaDBLog::IsAuthorized(*user.getProfile(), READ)
			;
		}

		AdminInterfaceElement::PageLinks ResaLogAdmin::getSubPagesOfModule(
			const ModuleClass& module,
			const AdminInterfaceElement& currentPage,
			const server::Request& request
		) const	{
			AdminInterfaceElement::PageLinks links;
			if(	dynamic_cast<const ResaModule*>(&module) &&
				request.getUser() &&
				request.getUser()->getProfile() &&
				isAuthorized(*request.getUser()))
			{
				links.push_back(getNewCopiedPage());
			}
			return links;
		}



		AdminInterfaceElement::PageLinks ResaLogAdmin::getSubPages(
			const AdminInterfaceElement& currentPage,
			const server::Request& request
		) const	{
			AdminInterfaceElement::PageLinks links;

			const ResaEditLogEntryAdmin* rp(
				dynamic_cast<const ResaEditLogEntryAdmin*>(&currentPage)
			);

			if(rp)
			{
				boost::shared_ptr<ResaEditLogEntryAdmin> p(getNewPage<ResaEditLogEntryAdmin>());
				p->setEntry(rp->getEntry());
				links.push_back(p);
			}

			return links;
		}
	}
}

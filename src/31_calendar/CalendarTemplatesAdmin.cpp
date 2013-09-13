
/** CalendarTemplatesAdmin class implementation.
	@file CalendarTemplatesAdmin.cpp
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

#include "CalendarTemplatesAdmin.h"

#include "CalendarModule.h"
#include "Profile.h"
#include "ResultHTMLTable.h"
#include "User.h"
#include "HTMLModule.h"
#include "HTMLForm.h"

#include "CalendarTemplateAdmin.h"
#include "CalendarTemplateTableSync.h"
#include "CalendarTemplatePropertiesUpdateAction.h"
#include "RemoveObjectAction.hpp"
#include "CalendarRight.h"
#include "CalendarTemplateElementTableSync.h"

#include "Request.h"
#include "AdminFunctionRequest.hpp"
#include "AdminActionFunctionRequest.hpp"

#include "ModuleAdmin.h"
#include "AdminInterfaceElement.h"
#include "AdminParametersException.h"

#include <boost/foreach.hpp>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace admin;
	using namespace server;
	using namespace util;
	using namespace calendar;
	using namespace html;
	using namespace security;
	using namespace db;

	namespace util
	{
		template<> const string FactorableTemplate<AdminInterfaceElement, CalendarTemplatesAdmin>::FACTORY_KEY("CalendarTemplatesAdmin");
	}

	namespace admin
	{
		template<> const string AdminInterfaceElementTemplate<CalendarTemplatesAdmin>::ICON("calendar.png");
		template<> const string AdminInterfaceElementTemplate<CalendarTemplatesAdmin>::DEFAULT_TITLE("Calendriers");
	}

	namespace calendar
	{
		CalendarTemplatesAdmin::CalendarTemplatesAdmin()
			: AdminInterfaceElementTemplate<CalendarTemplatesAdmin>()
		{ }

		void CalendarTemplatesAdmin::setFromParametersMap(
			const ParametersMap& map
		){
		}



		ParametersMap CalendarTemplatesAdmin::getParametersMap() const
		{
			ParametersMap m;
			return m;
		}


		void CalendarTemplatesAdmin::display(
			ostream& stream,
			const server::Request& _request
		) const {
			// Requests

			AdminActionFunctionRequest<CalendarTemplatePropertiesUpdateAction,CalendarTemplateAdmin> addCalendar(_request);
			addCalendar.setActionFailedPage<CalendarTemplatesAdmin>();
			addCalendar.setActionWillCreateObject();

			AdminActionFunctionRequest<RemoveObjectAction,CalendarTemplatesAdmin> removeCalendar(_request, *this);

			// Display
			stream << "<h1>Calendriers</h1>";

			CalendarTemplateTableSync::SearchResult calendars(
				CalendarTemplateTableSync::Search(
					Env::GetOfficialEnv(),
					optional<string>(),
					optional<RegistryKeyType>(),
					true,
					true,
					0,
					optional<size_t>(),
					UP_LINKS_LOAD_LEVEL,
					RegistryKeyType(0)
			)	);

			HTMLForm f(addCalendar.getHTMLForm("add"));
			HTMLTable::ColsVector c;
			c.push_back("Description");
			c.push_back("Actions");
			c.push_back("Actions");
			HTMLTable t(c,ResultHTMLTable::CSS_CLASS);
			stream << f.open() << t.open();

			AdminFunctionRequest<CalendarTemplateAdmin> editCalendar(_request);
			BOOST_FOREACH(
				boost::shared_ptr<CalendarTemplate> ct,
				calendars
			){
				editCalendar.getPage()->setCalendar(const_pointer_cast<const CalendarTemplate>(ct));
				removeCalendar.getAction()->setObjectId(ct->getKey());

				stream << t.row();
				stream << t.col() << ct->getName();
				stream <<
					t.col() <<
					HTMLModule::getLinkButton(
						editCalendar.getURL(),
						"Ouvrir",
						string(),
						"/admin/img/calendar_edit.png"
					)
				;
				stream << t.col();
				CalendarTemplateElementTableSync::SearchResult result(
					CalendarTemplateElementTableSync::Search(
						_getEnv(), optional<RegistryKeyType>(), ct->getKey(), 0, 1
				)	);
				if(result.empty())
				{
					stream <<
						HTMLModule::getLinkButton(
							removeCalendar.getURL(),
							"Supprimer",
							"Etes-vous sûr de vouloir supprimer le calendrier "+ ct->getName() +" ?",
							"calendar_delete.png"
						)
					;
				}
			}

			stream << t.row();
			stream <<
				t.col() <<
				f.getTextInput(CalendarTemplatePropertiesUpdateAction::PARAMETER_NAME, string(), "(nom du calendrier)")
			;
			stream << t.col() << f.getSubmitButton("Ajouter");

			stream << t.close() << f.close();
		}

		bool CalendarTemplatesAdmin::isAuthorized(
			const security::User& user
		) const	{
			return user.getProfile()->isAuthorized<CalendarRight>(READ);
		}



		AdminInterfaceElement::PageLinks CalendarTemplatesAdmin::getSubPagesOfModule(
			const ModuleClass& module,
			const AdminInterfaceElement& currentPage,
			const server::Request& request
		) const	{
			AdminInterfaceElement::PageLinks links;

			if(	dynamic_cast<const CalendarModule*>(&module) &&
				request.getUser() &&
				request.getUser()->getProfile() &&
				isAuthorized(*request.getUser())
			){
				links.push_back(getNewCopiedPage());
			}
			return links;
		}



		AdminInterfaceElement::PageLinks CalendarTemplatesAdmin::getSubPages(
			const AdminInterfaceElement& currentPage,
			const server::Request& request
		) const {
			AdminInterfaceElement::PageLinks links;

			if(	dynamic_cast<const CalendarTemplatesAdmin*>(&currentPage) ||
				dynamic_cast<const CalendarTemplateAdmin*>(&currentPage)
			){

				// Subpages
				CalendarTemplateTableSync::SearchResult calendars(
					CalendarTemplateTableSync::Search(
						Env::GetOfficialEnv(),
						optional<string>(),
						optional<RegistryKeyType>(),
						true,
						true,
						0,
						optional<size_t>(),
						UP_LINKS_LOAD_LEVEL,
						RegistryKeyType(0)
				)	);
				BOOST_FOREACH(const boost::shared_ptr<CalendarTemplate>& ct, calendars)
				{
					boost::shared_ptr<CalendarTemplateAdmin> page(
						getNewPage<CalendarTemplateAdmin>()
					);
					page->setCalendar(ct);
					links.push_back(page);
				}
			}

			return links;
		}
}	}

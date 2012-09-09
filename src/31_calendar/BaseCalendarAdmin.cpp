
/** BaseCalendarAdmin class implementation.
	@file BaseCalendarAdmin.cpp

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

#include "BaseCalendarAdmin.hpp"

#include "Calendar.h"
#include "ResultHTMLTable.h"
#include "Session.h"
#include "HTMLModule.h"
#include "StaticActionRequest.h"
#include "BaseCalendarUpdateAction.hpp"
#include "AdminModule.h"
#include "PropertiesHTMLTable.h"
#include "CalendarTemplateTableSync.h"
#include "AdminFunctionRequest.hpp"
#include "CalendarTemplateAdmin.h"
#include "CalendarLink.hpp"

#include <boost/date_time/gregorian/greg_date.hpp>
#include <boost/lexical_cast.hpp>
#include <sstream>

using namespace boost;
using namespace std;
using namespace boost::gregorian;

namespace synthese
{
	using namespace server;
	using namespace html;
	using namespace admin;
	using namespace util;

	namespace calendar
	{
		void BaseCalendarAdmin::Display(
			std::ostream& stream,
			const Calendar& object,
			boost::optional<const BaseAdminFunctionRequest&> updateRequest,
			boost::optional<RegistryKeyType> parentTemplateId,
			boost::optional<RegistryKeyType> parentTemplateId2
		){
			if(updateRequest)
			{
				// Checking preconditions
				assert(dynamic_cast<const BaseCalendarUpdateAction*>(updateRequest->getAction().get()));

				// Getting the current session variables
				std::string sessionStartDateStr(
					updateRequest->getSession()->getSessionVariable(
						BaseCalendarUpdateAction::SESSION_VARIABLE_SERVICE_ADMIN_START_DATE
				)	);
				boost::gregorian::date sessionStartDate;
				if(!sessionStartDateStr.empty())
				{
					sessionStartDate = boost::gregorian::from_string(sessionStartDateStr);
				}
				std::string sessionEndDateStr(
					updateRequest->getSession()->getSessionVariable(
						BaseCalendarUpdateAction::SESSION_VARIABLE_SERVICE_ADMIN_END_DATE
				)	);
				boost::gregorian::date sessionEndDate;
				if(!sessionEndDateStr.empty())
				{
					sessionEndDate = boost::gregorian::from_string(sessionEndDateStr);
				}
				std::string sessionCalendarTemplateIdStr(
					updateRequest->getSession()->getSessionVariable(
						BaseCalendarUpdateAction::SESSION_VARIABLE_SERVICE_ADMIN_CALENDAR_TEMPLATE_ID
				)	);
				util::RegistryKeyType sessionCalendarTemplateId(0);
				if(!sessionCalendarTemplateIdStr.empty())
				{
					sessionCalendarTemplateId = boost::lexical_cast<util::RegistryKeyType>(
						sessionCalendarTemplateIdStr
					);
				}
				std::string sessionCalendarTemplateId2Str(
					updateRequest->getSession()->getSessionVariable(
						BaseCalendarUpdateAction::SESSION_VARIABLE_SERVICE_ADMIN_CALENDAR_TEMPLATE_ID2
				)	);
				util::RegistryKeyType sessionCalendarTemplateId2(0);
				if(!sessionCalendarTemplateId2Str.empty())
				{
					sessionCalendarTemplateId2 = boost::lexical_cast<util::RegistryKeyType>(
						sessionCalendarTemplateId2Str
					);
				}


				// Link defined calendar
				stream << "<h1>Définition par lien</h1>";

				if(object.getCalendarLinks().empty())
				{
					stream << "<p class=\"info\">Lorsque un calendrier est défini par lien, la définition par date est impossible et certaines fonctions temps réel peuvent ne pas fonctionner.</p>";
				}

				AdminFunctionRequest<CalendarTemplateAdmin> openCalendarRequest(*updateRequest);

				// Add link form
				BaseCalendarUpdateAction& action(
					dynamic_cast<BaseCalendarUpdateAction&>(
						const_cast<Action&>(
							*updateRequest->getAction()
				)	)	);
				action.setAddLink(true);
				HTMLForm calendarLinkAddForm(updateRequest->getHTMLForm("calendar_link_add"));
				action.setAddLink(false);
				stream << calendarLinkAddForm.open();

				HTMLTable::ColsVector vs;
				vs.push_back("Date début");
				vs.push_back("Date fin");
				vs.push_back("Calendrier jours");
				vs.push_back("Calendrier période");
				vs.push_back("Action");
				HTMLTable tc(vs, ResultHTMLTable::CSS_CLASS);
				stream << tc.open();
				BOOST_FOREACH(Calendar::CalendarLinks::value_type link, object.getCalendarLinks())
				{
					stream << tc.row();
					stream << tc.col() << link->getStartDate();
					stream << tc.col() << link->getEndDate();

					// Calendar days
					stream << tc.col();
					if(link->getCalendarTemplate())
					{
						openCalendarRequest.getPage()->setCalendar(Env::GetOfficialEnv().getSPtr(link->getCalendarTemplate()));
						stream << HTMLModule::getHTMLLink(openCalendarRequest.getURL(), link->getCalendarTemplate()->getName());
					}

					// Calendar period
					stream << tc.col();
					if(link->getCalendarTemplate2())
					{
						openCalendarRequest.getPage()->setCalendar(Env::GetOfficialEnv().getSPtr(link->getCalendarTemplate2()));
						stream << HTMLModule::getHTMLLink(openCalendarRequest.getURL(), link->getCalendarTemplate2()->getName());
					}

					// Remove button
					action.setLinkToRemove(link->getKey());
					stream << tc.col() << HTMLModule::getLinkButton(
						updateRequest->getURL(),
						"Supprimer",
						"Etes-vous sûr de vouloir supprimer le lien ?"
					);
					action.setLinkToRemove(optional<RegistryKeyType>());
				}
				stream << tc.row();
				stream << tc.col() << calendarLinkAddForm.getCalendarInput(BaseCalendarUpdateAction::PARAMETER_START_DATE, sessionStartDate);
				stream << tc.col() << calendarLinkAddForm.getCalendarInput(BaseCalendarUpdateAction::PARAMETER_END_DATE, sessionEndDate);
				stream << tc.col() << calendarLinkAddForm.getSelectInput(
						BaseCalendarUpdateAction::PARAMETER_CALENDAR_TEMPLATE_ID,
						CalendarTemplateTableSync::GetCalendarTemplatesList(
							"(aucun)",
							optional<RegistryKeyType>(),
							parentTemplateId
						),
						optional<RegistryKeyType>(sessionCalendarTemplateId)
				);
				stream << tc.col() << calendarLinkAddForm.getSelectInput(
						BaseCalendarUpdateAction::PARAMETER_CALENDAR_TEMPLATE_ID2,
						CalendarTemplateTableSync::GetCalendarTemplatesList(
							"(aucun)",
							optional<RegistryKeyType>(),
							parentTemplateId2
						),
						optional<RegistryKeyType>(sessionCalendarTemplateId2)
				);
				stream << tc.col() << calendarLinkAddForm.getSubmitButton("Ajouter");
				stream << tc.close();
				stream << calendarLinkAddForm.close();

				if(object.isLinked())
				{
					stream << "<h2>Dates forcées</h2>";

					// Form
					HTMLForm dateAdd(updateRequest->getHTMLForm("dateAdd"));
					stream << dateAdd.open();

					// Header
					HTMLTable::ColsVector v;
					v.push_back("Date");
					v.push_back(string());
					HTMLTable t(v, ResultHTMLTable::CSS_CLASS);
					stream << t.open();

					// Loop on dates
					date nodate(not_a_date_time);
					BOOST_FOREACH(const date& d, object.getDatesToForce())
					{
						stream << t.row();
						stream << t.col() << d;

						// Remove button
						const_cast<BaseCalendarUpdateAction*>(
							dynamic_cast<const BaseCalendarUpdateAction*>(
								updateRequest->getAction().get()
						)	)->setDateToForceToRemove(d);
						stream << t.col() << HTMLModule::getLinkButton(updateRequest->getURL(), "Supprimer", "Etes-vous sûr de vouloir supprimer la date ?");
						const_cast<BaseCalendarUpdateAction*>(
							dynamic_cast<const BaseCalendarUpdateAction*>(
								updateRequest->getAction().get()
						)	)->setDateToForceToRemove(nodate);
					}

					// Add form
					stream << t.row();
					stream << t.col();
					date now(gregorian::day_clock::local_day());
					stream << dateAdd.getCalendarInput(BaseCalendarUpdateAction::PARAMETER_ADD_DATE_TO_FORCE, now);
					stream << t.col();
					stream << dateAdd.getSubmitButton("Ajouter");

					stream << t.close();
					stream << dateAdd.close();
				}

				if(object.isLinked())
				{
					stream << "<h2>Dates bloquées</h2>";

					// Form
					HTMLForm dateAdd(updateRequest->getHTMLForm("bdateAdd"));
					stream << dateAdd.open();

					// Header
					HTMLTable::ColsVector v;
					v.push_back("Date");
					v.push_back(string());
					HTMLTable t(v, ResultHTMLTable::CSS_CLASS);
					stream << t.open();

					// Loop on dates
					date nodate(not_a_date_time);
					BOOST_FOREACH(const date& d, object.getDatesToBypass())
					{
						stream << t.row();
						stream << t.col() << d;

						// Remove button
						const_cast<BaseCalendarUpdateAction*>(
							dynamic_cast<const BaseCalendarUpdateAction*>(
								updateRequest->getAction().get()
						)	)->setDateToBypassToRemove(d);
						stream << t.col() << HTMLModule::getLinkButton(updateRequest->getURL(), "Supprimer", "Etes-vous sûr de vouloir supprimer la date ?");
						const_cast<BaseCalendarUpdateAction*>(
							dynamic_cast<const BaseCalendarUpdateAction*>(
								updateRequest->getAction().get()
						)	)->setDateToBypassToRemove(nodate);
					}

					// Add form
					stream << t.row();
					stream << t.col();
					date now(gregorian::day_clock::local_day());
					stream << dateAdd.getCalendarInput(BaseCalendarUpdateAction::PARAMETER_ADD_DATE_TO_BYPASS, now);
					stream << t.col();
					stream << dateAdd.getSubmitButton("Ajouter");

					stream << t.close();
					stream << dateAdd.close();
				}

				if(!object.isLinked())
				{
					stream << "<h1>Définition par date</h1>";

					// Display of the form
					PropertiesHTMLTable p(updateRequest->getHTMLForm("applycalendar"));
					stream << p.open();
					stream << p.cell(
						"Date début",
						p.getForm().getCalendarInput(BaseCalendarUpdateAction::PARAMETER_START_DATE, sessionStartDate)
					);
					stream << p.cell(
						"Date fin",
						p.getForm().getCalendarInput(BaseCalendarUpdateAction::PARAMETER_END_DATE, sessionEndDate)
					);
					stream << p.cell(
						"Période",
						p.getForm().getTextInput(
							BaseCalendarUpdateAction::PARAMETER_PERIOD,
							"1",
							std::string(),
							admin::AdminModule::CSS_2DIGIT_INPUT
					)	);
					stream << p.cell(
						"Modèle",
						p.getForm().getSelectInput(
							BaseCalendarUpdateAction::PARAMETER_CALENDAR_TEMPLATE_ID,
							calendar::CalendarTemplateTableSync::GetCalendarTemplatesList("(aucun)"),
							boost::optional<util::RegistryKeyType>(sessionCalendarTemplateId)
					)	);
					stream << p.cell(
						"Ajout",
						p.getForm().getOuiNonRadioInput(BaseCalendarUpdateAction::PARAMETER_ADD, true)
					);
					stream << p.close();
				}
			}


			//////////////////////////////////////////////////////////////////////////
			// The result table
			stream << "<h1>Résultat</h1>";

			// Declarations
			boost::gregorian::date minDate;
			boost::gregorian::date maxDate;
			if(	object.empty()
			){
				minDate = boost::gregorian::day_clock::local_day();
				maxDate = minDate;
			}
			else
			{
				minDate = object.getFirstActiveDate();
				maxDate = object.getLastActiveDate();
			}
			boost::gregorian::date firstDate(minDate - boost::gregorian::date_duration(minDate.day_of_week() == 0 ? 13 : minDate.day_of_week() + 6));
			boost::gregorian::date lastDate(maxDate + boost::gregorian::date_duration(maxDate.day_of_week() == 0 ? 7 : 14 - maxDate.day_of_week()));
			int white(7);
			bool first(true);
			bool leftWhite(false);

			// Columns
			html::HTMLTable::ColsVector c;
			c.push_back("&nbsp;");
			c.push_back("Lu");
			c.push_back("Ma");
			c.push_back("Me");
			c.push_back("Je");
			c.push_back("Ve");
			c.push_back("Sa");
			c.push_back("Di");

			// Opening
			html::HTMLTable t(c, html::ResultHTMLTable::CSS_CLASS);
			stream << t.open();

			// Cells
			for(boost::gregorian::date curDate(firstDate);
				curDate <= lastDate;
				curDate += boost::gregorian::date_duration(1)
			){
				// Value of the calendar at this date
				bool value(object.isActive(curDate));

				if(	curDate.day_of_week() == 1)
				{
					stream << t.row();
					stream << t.col(1, std::string(), true);
					if (curDate.day()<8 || first)
					{
						stream  << "<a name=\"" << curDate.year() << "-" << curDate.month() << "\"></a>" << curDate.month() << " " << curDate.year();
					}
					else
					{
						stream << "&nbsp;";
					}
					if(first)
						first = false;
				}

				leftWhite = false;
				std::stringstream style;
				style << "background-color:" << (value ? "green" : "red") << ";";
				if( curDate.day() == 1)
				{
					white = 7;
					if (curDate.day() == 1)
						leftWhite = true;
				}
				if (white || leftWhite)
				{
					style << "border-style:solid; border-color:#C0C0A0; border-width:";
					if (white > 0)
					{
						style << "1";
						--white;
					}
					else
					{
						style << "0";
					}
					style << "px 0px 0px " << leftWhite << "px;";
				}
				else
				{
					style << "border:none;";
				}
				stream << t.col(1, std::string(), false, style.str());

				if(updateRequest)
				{
					BaseCalendarUpdateAction& action(
						dynamic_cast<BaseCalendarUpdateAction&>(
							const_cast<Action&>(
								*updateRequest->getAction()
					)	)	);
					action.setDate(curDate);
					action.setAdd(!value);
					stream << html::HTMLModule::getHTMLLink(
						updateRequest->getURL(),
						boost::lexical_cast<std::string>(curDate.day())
					);
				}
				else
				{
					stream << curDate.day();
				}
			}

			// Closing
			stream << t.close();
		}
}	}

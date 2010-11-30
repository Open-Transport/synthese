////////////////////////////////////////////////////////////////////////////////
/// MessagesAdmin class implementation.
///	@file MessagesAdmin.cpp
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

#include "SearchFormHTMLTable.h"
#include "ActionResultHTMLTable.h"
#include "05_html/Constants.h"
#include "StaticActionFunctionRequest.h"
#include "SentScenario.h"
#include "AlarmRecipient.h"
#include "AlarmTableSync.h"
#include "ScenarioTableSync.h"
#include "MessagesAdmin.h"
#include "MessagesScenarioAdmin.h"
#include "MessagesModule.h"
#include "NewScenarioSendAction.h"
#include "ScenarioStopAction.h"
#include "MessagesRight.h"
#include "MessagesModule.h"
#include "ScenarioSentAlarmInheritedTableSync.h"
#include "ScenarioTemplateInheritedTableSync.h"
#include "ScenarioTemplate.h"
#include "AdminInterfaceElement.h"
#include "ModuleAdmin.h"
#include "AdminModule.h"
#include "AdminParametersException.h"
#include "AdminActionFunctionRequest.hpp"
#include "AdminFunctionRequest.hpp"
#include "MessageAdmin.h"
#include "Profile.h"

#include <boost/foreach.hpp>

using namespace boost;
using namespace std;
using namespace boost::posix_time;


namespace synthese
{
	using namespace admin;
	using namespace server;
	using namespace util;
	using namespace html;
	using namespace messages;
	using namespace security;

	namespace util
	{
		template <> const string FactorableTemplate<AdminInterfaceElement,MessagesAdmin>::FACTORY_KEY("0messages");
	}

	namespace admin
	{
		template <> const string AdminInterfaceElementTemplate<MessagesAdmin>::ICON("note.png");
		template <> const string AdminInterfaceElementTemplate<MessagesAdmin>::DEFAULT_TITLE("Messages diffusés");
	}

	namespace messages
	{
		const std::string MessagesAdmin::PARAMETER_SEARCH_DATE = "sd";
		const std::string MessagesAdmin::PARAMETER_SEARCH_LEVEL = "sl";
		const std::string MessagesAdmin::PARAMETER_SEARCH_STATUS = "st";
		const std::string MessagesAdmin::PARAMETER_SEARCH_NAME = "na";
		const std::string MessagesAdmin::PARAMETER_SEARCH_CONFLICT = "sc";

		const std::string MessagesAdmin::CSS_ALARM_DISABLED = "alarmdisabled";
		const std::string MessagesAdmin::CSS_ALARM_DISPLAYED_WITHOUT_END_DATE = "alarmdisplayedwithoutenddate";
		const std::string MessagesAdmin::CSS_ALARM_WILL_BE_DISPLAYED = "alarmwillbedisplayed";
		const std::string MessagesAdmin::CSS_ALARM_DISPLAYED_WITH_END_DATE = "alarmdisplayedwithenddate";

		MessagesAdmin::MessagesAdmin()
			: AdminInterfaceElementTemplate<MessagesAdmin>()
			, _date(second_clock::local_time())
			, _searchStatus(SentScenarioInheritedTableSync::BROADCAST_RUNNING)
//			_searchLevel(ALARM_LEVEL_UNKNOWN)
//			, _searchConflict(ALARM_CONFLICT_UNKNOWN)
		{}

		void MessagesAdmin::setFromParametersMap(
			const ParametersMap& map
		){
				_parametersMap = map;

				if(!map.getDefault<string>(PARAMETER_SEARCH_DATE).empty())
				{
					_date = time_from_string(map.get<string>(PARAMETER_SEARCH_DATE));
				}

//				optional<int> num = map.getOptional<int>(PARAMETER_SEARCH_CONFLICT);
//				if (num)
//					_searchConflict = static_cast<AlarmConflict>(*num);

				int num = map.getDefault<int>(PARAMETER_SEARCH_STATUS, UNKNOWN_VALUE);

				if (num != UNKNOWN_VALUE)
					_searchStatus = static_cast<SentScenarioInheritedTableSync::StatusSearch>(num);

				optional<RegistryKeyType> id(map.getOptional<RegistryKeyType>(PARAMETER_SEARCH_LEVEL));
//				if(id > encodeUId(ScenarioTableSync::TABLE.ID, 0, 0))
				{
//					_searchLevel = ALARM_LEVEL_SCENARIO;
					if(id)
					_searchScenario = ScenarioTemplateInheritedTableSync::Get(*id, _getEnv()).get();
				}
//				else if (id != 0)
//				{
//					_searchLevel = static_cast<AlarmLevel>(
//						map.getDefault<int>(PARAMETER_SEARCH_LEVEL, UNKNOWN_VALUE)
//					);
//				}

				_searchName = map.getOptional<string>(PARAMETER_SEARCH_NAME);

				_requestParameters.setFromParametersMap(map.getMap(), PARAMETER_SEARCH_LEVEL, 15, false);
		}



		server::ParametersMap MessagesAdmin::getParametersMap() const
		{
			ParametersMap m(_requestParameters.getParametersMap());
//			m.insert(PARAMETER_SEARCH_DATE, _date);
//			m.insert(PARAMETER_SEARCH_CONFLICT, static_cast<int>(_searchConflict));
			m.insert(PARAMETER_SEARCH_STATUS, static_cast<int>(_searchStatus));
//			m.insert(PARAMETER_SEARCH_LEVEL, static_cast<int>(_searchLevel));
			return m;
		}



		void MessagesAdmin::display(
			ostream& stream,
			const admin::AdminRequest& _request
		) const	{

			// Requests
			AdminFunctionRequest<MessagesAdmin> searchRequest(_request);

			AdminActionFunctionRequest<NewScenarioSendAction,MessagesScenarioAdmin> newScenarioRequest(_request);
			newScenarioRequest.getFunction()->setActionFailedPage<MessagesAdmin>();
			newScenarioRequest.setActionWillCreateObject();
			
			AdminFunctionRequest<MessagesScenarioAdmin> scenarioRequest(_request);

			AdminActionFunctionRequest<ScenarioStopAction,MessagesAdmin> scenarioStopRequest(_request);
			
			vector<pair<optional<SentScenarioInheritedTableSync::StatusSearch>, string> > statusMap;
			statusMap.push_back(make_pair(SentScenarioInheritedTableSync::BROADCAST_RUNNING, "En diffusion / prévu"));
			statusMap.push_back(make_pair(SentScenarioInheritedTableSync::BROADCAST_RUNNING_WITH_END, "&nbsp;&gt;&nbsp;En cours avec date de fin"));
			statusMap.push_back(make_pair(SentScenarioInheritedTableSync::BROADCAST_RUNNING_WITHOUT_END, "&nbsp;&gt;&nbsp;En cours sans date de fin"));
			statusMap.push_back(make_pair(SentScenarioInheritedTableSync::FUTURE_BROADCAST, "&nbsp;&gt;&nbsp;Diffusion ultérieure"));
			statusMap.push_back(make_pair(SentScenarioInheritedTableSync::BROADCAST_OVER, "Archivés"));
			statusMap.push_back(make_pair(SentScenarioInheritedTableSync::BROADCAST_DRAFT, "Brouillons"));
			
			ptime now(second_clock::local_time());

			stream << "<h1>Recherche</h1>";

			SearchFormHTMLTable s(searchRequest.getHTMLForm());
			stream << s.open();
			stream << s.cell("Date", s.getForm().getCalendarInput(PARAMETER_SEARCH_DATE, _date));

			vector<shared_ptr<AlarmRecipient> > recipients(Factory<AlarmRecipient>::GetNewCollection());
			BOOST_FOREACH(const shared_ptr<AlarmRecipient> recipient, recipients)
			{
				AlarmRecipientSearchFieldsMap m(recipient->getSearchFields(s.getForm(), _parametersMap));
				for (AlarmRecipientSearchFieldsMap::iterator itm = m.begin(); itm != m.end(); ++itm)
				{
					stream << s.cell(itm->second.label, itm->second.htmlField);
				}
			}

			stream << s.cell("Statut", s.getForm().getSelectInput(PARAMETER_SEARCH_STATUS, statusMap, optional<SentScenarioInheritedTableSync::StatusSearch>(_searchStatus)));
//			stream << s.cell(
//				"Superposition",
//				s.getForm().getSelectInput(PARAMETER_SEARCH_CONFLICT, MessagesModule::getConflictLabels(true), _searchConflict)
//			);
			stream << s.cell(
				"Modèle",
				s.getForm().getSelectInput(
					PARAMETER_SEARCH_LEVEL,
					MessagesModule::GetScenarioTemplatesLabels(
						"(tous)"
					),
					_searchScenario ?
						(*_searchScenario ? (*_searchScenario)->getKey() : 0):
						optional<RegistryKeyType>()
				)
			);

			stream << s.close();

			stream << "<h1>Résultats de la recherche</h1>";

			SentScenarioInheritedTableSync::SearchResult scenarios(
				SentScenarioInheritedTableSync::Search(
					_getEnv(),
					_searchName,
					_searchStatus,
					_date,
					_searchScenario ?
						(*_searchScenario ? (*_searchScenario)->getKey() : 0):
						optional<RegistryKeyType>(),
					_requestParameters.first
					, _requestParameters.maxSize
					, _requestParameters.orderField == PARAMETER_SEARCH_DATE
					, _requestParameters.orderField == PARAMETER_SEARCH_NAME
					, _requestParameters.orderField == PARAMETER_SEARCH_STATUS
					, _requestParameters.raisingOrder
			)	);
			
			ActionResultHTMLTable::HeaderVector v1;
			v1.push_back(make_pair(string(), string("Statut")));
			v1.push_back(make_pair(PARAMETER_SEARCH_DATE, string("Dates")));
			v1.push_back(make_pair(PARAMETER_SEARCH_LEVEL, string("Modèle")));
			v1.push_back(make_pair(PARAMETER_SEARCH_NAME, string("Nom")));
//			v1.push_back(make_pair(PARAMETER_SEARCH_CONFLICT, string("Conflit")));
			v1.push_back(make_pair(string(), string("Actions")));
			ActionResultHTMLTable t1(
				v1,
				searchRequest.getHTMLForm(),
				_requestParameters,
				scenarios,
				newScenarioRequest.getHTMLForm("newscen"),
				NewScenarioSendAction::PARAMETER_MESSAGE_TO_COPY
			);
			
			stream << t1.open();

			BOOST_FOREACH(shared_ptr<SentScenario> message, scenarios)
			{
				bool isDisplayedWithEndDate(
					(message->getPeriodStart().is_not_a_date_time() || message->getPeriodStart() <= now)
					&& !message->getPeriodEnd().is_not_a_date_time()
					&& message->getPeriodEnd() >= now
					&& message->getIsEnabled()
				);
				bool isDisplayedWithoutEndDate(
					(message->getPeriodStart().is_not_a_date_time() || message->getPeriodStart() <= now)
					&& message->getPeriodEnd().is_not_a_date_time()
					&& message->getIsEnabled()
				);
				bool willBeDisplayed(
					!message->getPeriodStart().is_not_a_date_time()
					&& message->getPeriodStart() > now
					&& message->getIsEnabled()
				);
				string rowColorCSS;
				if (!message->getIsEnabled())
				{
					rowColorCSS = CSS_ALARM_DISABLED;
				}
				if(isDisplayedWithoutEndDate)
				{
					rowColorCSS = CSS_ALARM_DISPLAYED_WITHOUT_END_DATE;
				}
				if (willBeDisplayed)
				{
					rowColorCSS = CSS_ALARM_WILL_BE_DISPLAYED;
				}
				if (isDisplayedWithEndDate)
				{
					rowColorCSS = CSS_ALARM_DISPLAYED_WITH_END_DATE;
				}
				stream << t1.row(Conversion::ToString(message->getKey()), rowColorCSS);

				// Dates
				stream << t1.col();
				if (!message->getIsEnabled())
				{
					if (!message->getPeriodEnd().is_not_a_date_time() && message->getPeriodEnd() < now)
					{
						stream << "Archivé";
					}
					else
					{
						stream << "Brouillon";
					}
				}
				else
				{
					if (!message->getPeriodEnd().is_not_a_date_time() && message->getPeriodEnd() < now)
					{
						stream << "Archivé";
					}
					else if(message->getPeriodStart().is_not_a_date_time() || message->getPeriodStart() <= now)
					{
						stream << "En cours";
					}
					else
					{
						stream << "Prévu";
					}
				}
				
				
				stream << t1.col();
				if (message->getPeriodStart().is_not_a_date_time() && message->getPeriodEnd().is_not_a_date_time())
				{
					stream << "Diffusion permanente";
				}
				if (message->getPeriodStart().is_not_a_date_time() && !message->getPeriodEnd().is_not_a_date_time())
				{
					stream << "Jusqu'au " << message->getPeriodEnd();
				}
				if (!message->getPeriodStart().is_not_a_date_time() && message->getPeriodEnd().is_not_a_date_time())
				{
					stream << "A compter du " << message->getPeriodStart();
				}
				if (!message->getPeriodStart().is_not_a_date_time() && !message->getPeriodEnd().is_not_a_date_time())
				{
					stream << "Du " << message->getPeriodStart() << " au " << message->getPeriodEnd();
				}

				// Type
//				stream << t1.col() << MessagesModule::getLevelLabel(message.level);
				
				stream << t1.col() << message->getName();
				//stream << t1.col(); // Bullet
//				stream << t1.col() << MessagesModule::getConflictLabel(message.conflict); /// @todo put a graphic bullet
				stream << t1.col();

				scenarioRequest.getPage()->setScenario(message);

				stream << HTMLModule::getLinkButton(scenarioRequest.getURL(), "Ouvrir");
				if (message->isApplicable(now))
				{
					scenarioStopRequest.getAction()->setScenario(message);
					stream << "&nbsp;" << HTMLModule::getLinkButton(scenarioStopRequest.getURL(), "Arrêter", "Etes-vous sûr de vouloir arrêter la diffusion des messages ?", "stop.png");
				}
			}
			stream << t1.row(string());
			stream << t1.col();
			stream << t1.col();
			stream <<
				t1.col() <<
				t1.getActionForm().getSelectInput(
					NewScenarioSendAction::PARAMETER_TEMPLATE,
					MessagesModule::GetScenarioTemplatesLabels(string(),"(pas de modéle)"),
					optional<RegistryKeyType>()
				)
			;
			stream << t1.col(2) << t1.getActionForm().getSubmitButton("Nouvelle diffusion");

			stream << t1.close();
		}



		bool MessagesAdmin::isAuthorized(const security::User& user
		) const	{
			return user.getProfile()->isAuthorized<MessagesRight>(READ);
		}

		AdminInterfaceElement::PageLinks MessagesAdmin::getSubPagesOfModule(
			const string& moduleKey,
			const AdminInterfaceElement& currentPage,
			const admin::AdminRequest& request
		) const	{
			AdminInterfaceElement::PageLinks links;
			
			if(	moduleKey == MessagesModule::FACTORY_KEY &&
				request.getUser() &&
				request.getUser()->getProfile() &&
				isAuthorized(*request.getUser())
			){
				links.push_back(getNewPage());
			}
			return links;
		}

		bool MessagesAdmin::isPageVisibleInTree(
			const AdminInterfaceElement& currentPage,
			const admin::AdminRequest& request
		) const {
			return true;
		}



		AdminInterfaceElement::PageLinks MessagesAdmin::getSubPages(
			const AdminInterfaceElement& currentPage,
			const admin::AdminRequest& request
		) const	{
		
			AdminInterfaceElement::PageLinks links;
			
			const MessagesScenarioAdmin* sa(
				dynamic_cast<const MessagesScenarioAdmin*>(&currentPage)
			);
			const MessageAdmin* ma(
				dynamic_cast<const MessageAdmin*>(&currentPage)
			);
			
			if(	sa &&
				dynamic_cast<const SentScenario*>(sa->getScenario().get()) ||
				ma &&
				dynamic_cast<const SentAlarm*>(ma->getAlarm().get())
			){
				shared_ptr<MessagesScenarioAdmin> p(
					getNewOtherPage<MessagesScenarioAdmin>()
				);
				p->setScenario(
					SentScenarioInheritedTableSync::GetEditable(
						sa ? sa->getScenario()->getKey() : ma->getAlarm()->getScenario()->getKey(),
						_getEnv()
				)	);
				links.push_back(p);
			}
			return links;
		}
	}
}

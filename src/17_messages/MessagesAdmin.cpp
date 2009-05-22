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

#include "TimeParseException.h"
#include "DateTime.h"
#include "SearchFormHTMLTable.h"
#include "ActionResultHTMLTable.h"
#include "05_html/Constants.h"
#include "InterfaceModule.h"
#include "ActionFunctionRequest.h"
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
#include "AdminRequest.h"
#include "ModuleAdmin.h"
#include "AdminModule.h"
#include "AdminParametersException.h"
#include "Request.h"
#include "MessageAdmin.h"

#include <boost/foreach.hpp>

using namespace boost;
using namespace std;

namespace synthese
{
	using namespace admin;
	using namespace interfaces;
	using namespace server;
	using namespace util;
	using namespace time;
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
			, _date(TIME_CURRENT)
			, _searchStatus(SentScenarioInheritedTableSync::BROADCAST_RUNNING)
//			_searchLevel(ALARM_LEVEL_UNKNOWN)
//			, _searchConflict(ALARM_CONFLICT_UNKNOWN)
		{}

		void MessagesAdmin::setFromParametersMap(
			const ParametersMap& map,
			bool doDisplayPreparationActions
		){
			try
			{
				_parametersMap = map;

				_date = map.getDateTime(PARAMETER_SEARCH_DATE, false, FACTORY_KEY);

//				int num = map.getInt(PARAMETER_SEARCH_CONFLICT, false, FACTORY_KEY);
//				if (num != UNKNOWN_VALUE)
//					_searchConflict = static_cast<AlarmConflict>(num);

				int num = map.getInt(PARAMETER_SEARCH_STATUS, false, FACTORY_KEY);

				if (num != UNKNOWN_VALUE)
					_searchStatus = static_cast<SentScenarioInheritedTableSync::StatusSearch>(num);

				RegistryKeyType id(map.getUid(PARAMETER_SEARCH_LEVEL, false, FACTORY_KEY));
//				if(id > encodeUId(ScenarioTableSync::TABLE.ID, 0, 0, 0))
				{
//					_searchLevel = ALARM_LEVEL_SCENARIO;
					if(id != UNKNOWN_VALUE)
					_searchScenario = ScenarioTemplateInheritedTableSync::Get(id, _env).get();
				}
//				else if (id != UNKNOWN_VALUE)
//				{
//					_searchLevel = static_cast<AlarmLevel>(
//						map.getInt(PARAMETER_SEARCH_LEVEL, false, FACTORY_KEY)
//					);
//				}

				_searchName = map.getOptionalString(PARAMETER_SEARCH_NAME);

				_requestParameters.setFromParametersMap(map.getMap(), PARAMETER_SEARCH_LEVEL, 15, false);

				if(!doDisplayPreparationActions) return;

				SentScenarioInheritedTableSync::Search(
					_env,
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
				);
				_resultParameters.setFromResult(_requestParameters, _env.getEditableRegistry<SentScenario>());
			}
			catch (TimeParseException e)
			{
				throw AdminParametersException("Date invalide");
			}
		}



		server::ParametersMap MessagesAdmin::getParametersMap() const
		{
			ParametersMap m(_requestParameters.getParametersMap());
			m.insert(PARAMETER_SEARCH_DATE, _date);
//			m.insert(PARAMETER_SEARCH_CONFLICT, static_cast<int>(_searchConflict));
			m.insert(PARAMETER_SEARCH_STATUS, static_cast<int>(_searchStatus));
//			m.insert(PARAMETER_SEARCH_LEVEL, static_cast<int>(_searchLevel));
			return m;
		}



		void MessagesAdmin::display(ostream& stream, interfaces::VariablesMap& variables) const
		{
			// Requests
			FunctionRequest<AdminRequest> searchRequest(_request);
			searchRequest.getFunction()->setSamePage(this);

			ActionFunctionRequest<NewScenarioSendAction,AdminRequest> newScenarioRequest(_request);
			newScenarioRequest.getFunction()->setPage<MessagesScenarioAdmin>();
			newScenarioRequest.getFunction()->setActionFailedPage<MessagesAdmin>();
			newScenarioRequest.setObjectId(Request::UID_WILL_BE_GENERATED_BY_THE_ACTION);
			
			FunctionRequest<AdminRequest> scenarioRequest(_request);
			scenarioRequest.getFunction()->setPage<MessagesScenarioAdmin>();

			ActionFunctionRequest<ScenarioStopAction,AdminRequest> scenarioStopRequest(_request);
			scenarioStopRequest.getFunction()->setSamePage(this);
			
			vector<pair<SentScenarioInheritedTableSync::StatusSearch, string> > statusMap;
			statusMap.push_back(make_pair(SentScenarioInheritedTableSync::BROADCAST_RUNNING, "En diffusion / prévu"));
			statusMap.push_back(make_pair(SentScenarioInheritedTableSync::BROADCAST_RUNNING_WITH_END, "&nbsp;&gt;&nbsp;En cours avec date de fin"));
			statusMap.push_back(make_pair(SentScenarioInheritedTableSync::BROADCAST_RUNNING_WITHOUT_END, "&nbsp;&gt;&nbsp;En cours sans date de fin"));
			statusMap.push_back(make_pair(SentScenarioInheritedTableSync::FUTURE_BROADCAST, "&nbsp;&gt;&nbsp;Diffusion ultérieure"));
			statusMap.push_back(make_pair(SentScenarioInheritedTableSync::BROADCAST_OVER, "Archivés"));
			statusMap.push_back(make_pair(SentScenarioInheritedTableSync::BROADCAST_DRAFT, "Brouillons"));
			
			DateTime now(TIME_CURRENT);

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

			stream << s.cell("Statut", s.getForm().getSelectInput(PARAMETER_SEARCH_STATUS, statusMap, _searchStatus));
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
						UNKNOWN_VALUE
				)
			);

			stream << s.close();

			stream << "<h1>Résultats de la recherche</h1>";

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
				_resultParameters,
				newScenarioRequest.getHTMLForm("newscen"),
				NewScenarioSendAction::PARAMETER_MESSAGE_TO_COPY,
				InterfaceModule::getVariableFromMap(variables, AdminModule::ICON_PATH_INTERFACE_VARIABLE)
			);
			
			stream << t1.open();

			BOOST_FOREACH(shared_ptr<const SentScenario> message, _env.getRegistry<SentScenario>())
			{
				bool isDisplayedWithEndDate(
					(message->getPeriodStart().isUnknown() || message->getPeriodStart() <= now)
					&& !message->getPeriodEnd().isUnknown()
					&& message->getPeriodEnd() >= now
					&& message->getIsEnabled()
				);
				bool isDisplayedWithoutEndDate(
					(message->getPeriodStart().isUnknown() || message->getPeriodStart() <= now)
					&& message->getPeriodEnd().isUnknown()
					&& message->getIsEnabled()
				);
				bool willBeDisplayed(
					!message->getPeriodStart().isUnknown()
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
					if (!message->getPeriodEnd().isUnknown() && message->getPeriodEnd() < now)
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
					if (!message->getPeriodEnd().isUnknown() && message->getPeriodEnd() < now)
					{
						stream << "Archivé";
					}
					if(message->getPeriodStart().isUnknown() || message->getPeriodStart() <= now)
					{
						stream << "En cours";
					}
					else
					{
						stream << "Prévu";
					}
				}
				
				
				stream << t1.col();
				if (message->getPeriodStart().isUnknown() && message->getPeriodEnd().isUnknown())
				{
					stream << "Diffusion permanente";
				}
				if (message->getPeriodStart().isUnknown() && !message->getPeriodEnd().isUnknown())
				{
					stream << "Jusqu'au " << message->getPeriodEnd().toString();
				}
				if (!message->getPeriodStart().isUnknown() && message->getPeriodEnd().isUnknown())
				{
					stream << "A compter du " << message->getPeriodStart().toString();
				}
				if (!message->getPeriodStart().isUnknown() && !message->getPeriodEnd().isUnknown())
				{
					stream << "Du " << message->getPeriodStart().toString() << " au " << message->getPeriodEnd().toString();
				}

				// Type
//				stream << t1.col() << MessagesModule::getLevelLabel(message.level);
				
				stream << t1.col() << message->getName();
				//stream << t1.col(); // Bullet
//				stream << t1.col() << MessagesModule::getConflictLabel(message.conflict); /// @todo put a graphic bullet
				stream << t1.col();

				scenarioRequest.setObjectId(message->getKey());

				stream << HTMLModule::getLinkButton(scenarioRequest.getURL(), "Modifier");
				if (message->isApplicable(DateTime(TIME_CURRENT)))
				{
					scenarioStopRequest.setObjectId(message->getKey());
					stream << "&nbsp;" << HTMLModule::getLinkButton(scenarioStopRequest.getURL(), "Arrêter", "Etes-vous sûr de vouloir arrêter la diffusion des messages ?", "stop.png");
				}
			}
			stream << t1.row();
			stream << t1.col();
			stream << t1.col();
			stream <<
				t1.col() <<
				t1.getActionForm().getSelectInput(
					NewScenarioSendAction::PARAMETER_TEMPLATE,
					MessagesModule::GetScenarioTemplatesLabels(string(),"(pas de modéle)"),
					uid(UNKNOWN_VALUE)
				)
			;
			stream << t1.col(2) << t1.getActionForm().getSubmitButton("Nouvelle diffusion");

			stream << t1.close();
		}



		bool MessagesAdmin::isAuthorized(
		) const	{
			return _request->isAuthorized<MessagesRight>(READ);
		}

		AdminInterfaceElement::PageLinks MessagesAdmin::getSubPagesOfParent( const PageLink& parentLink , const AdminInterfaceElement& currentPage
		) const	{
			AdminInterfaceElement::PageLinks links;
			if (parentLink.factoryKey == ModuleAdmin::FACTORY_KEY && parentLink.parameterValue == MessagesModule::FACTORY_KEY)
			{
				links.push_back(getPageLink());
			}
			return links;
		}

		bool MessagesAdmin::isPageVisibleInTree( const AdminInterfaceElement& currentPage ) const
		{
			return true;
		}



		AdminInterfaceElement::PageLinks MessagesAdmin::getSubPages(
			const AdminInterfaceElement& currentPage
		) const	{
			AdminInterfaceElement::PageLinks links;
			if(currentPage.getFactoryKey() == MessagesScenarioAdmin::FACTORY_KEY)
			{
				const MessagesScenarioAdmin& currentSPage(static_cast<const MessagesScenarioAdmin&>(currentPage));
				if (dynamic_cast<const SentScenario*>(currentSPage.getScenario().get()))
					links.push_back(currentPage.getPageLink());
			}
			else if(currentPage.getFactoryKey() == MessageAdmin::FACTORY_KEY)
			{
				const MessageAdmin& currentSPage(static_cast<const MessageAdmin&>(currentPage));
				shared_ptr<const Alarm> alarm(currentSPage.getAlarm());
				const Scenario* scenario(alarm->getScenario());
				if (dynamic_cast<const SentScenario*>(scenario))
				{
					AdminInterfaceElement::PageLink link(getPageLink());
					link.factoryKey = MessagesScenarioAdmin::FACTORY_KEY;
					link.name = scenario->getName();
					link.icon = MessagesScenarioAdmin::ICON;
					link.parameterName = Request::PARAMETER_OBJECT_ID;
					link.parameterValue = Conversion::ToString(scenario->getKey());
					links.push_back(link);
				}
			}
			return links;
		}
	}
}

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
#include "SingleSentAlarm.h"
#include "SentScenario.h"
#include "AlarmRecipient.h"
#include "AlarmTableSync.h"
#include "ScenarioTableSync.h"
#include "MessagesAdmin.h"
#include "MessageAdmin.h"
#include "MessagesScenarioAdmin.h"
#include "MessagesModule.h"
#include "NewScenarioSendAction.h"
#include "AlarmStopAction.h"
#include "ScenarioStopAction.h"
#include "MessagesRight.h"
#include "MessagesModule.h"
#include "SingleSentAlarmInheritedTableSync.h"
#include "ScenarioSentAlarmInheritedTableSync.h"
#include "ScenarioTemplateInheritedTableSync.h"
#include "ScenarioTemplate.h"
#include "SentScenarioInheritedTableSync.h"
#include "AdminRequest.h"
#include "ModuleAdmin.h"
#include "AdminModule.h"
#include "AdminParametersException.h"
#include "QueryString.h"

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
		const std::string MessagesAdmin::PARAMETER_SEARCH_CONFLICT = "sc";

		const std::string MessagesAdmin::CSS_ALARM_DISABLED = "alarmdisabled";
		const std::string MessagesAdmin::CSS_ALARM_DISPLAYED_WITHOUT_END_DATE = "alarmdisplayedwithoutenddate";
		const std::string MessagesAdmin::CSS_ALARM_WILL_BE_DISPLAYED = "alarmwillbedisplayed";
		const std::string MessagesAdmin::CSS_ALARM_DISPLAYED_WITH_END_DATE = "alarmdisplayedwithenddate";

		MessagesAdmin::MessagesAdmin()
			: AdminInterfaceElementTemplate<MessagesAdmin>()
			, _date(TIME_UNKNOWN)
			, _searchStatus(BROADCAST_RUNNING),
			_searchLevel(ALARM_LEVEL_UNKNOWN)
			, _searchConflict(ALARM_CONFLICT_UNKNOWN)
		{}

		void MessagesAdmin::setFromParametersMap(
			const ParametersMap& map,
			bool doDisplayPreparationActions
		){
			try
			{
				_parametersMap = map;

				_date = map.getDate(PARAMETER_SEARCH_DATE, false, FACTORY_KEY);

				int num = map.getInt(PARAMETER_SEARCH_CONFLICT, false, FACTORY_KEY);
				if (num != UNKNOWN_VALUE)
					_searchConflict = static_cast<AlarmConflict>(num);

				num = map.getInt(PARAMETER_SEARCH_STATUS, false, FACTORY_KEY);

				if (num != UNKNOWN_VALUE)
					_searchStatus = static_cast<StatusSearch>(num);

				RegistryKeyType id(map.getUid(PARAMETER_SEARCH_LEVEL, false, FACTORY_KEY));
				if(id > encodeUId(ScenarioTableSync::TABLE.ID, 0, 0, 0))
				{
					_searchLevel = ALARM_LEVEL_SCENARIO;
					_searchScenario = ScenarioTemplateInheritedTableSync::Get(id, _env);
				}
				else if (id != UNKNOWN_VALUE)
				{
					_searchLevel = static_cast<AlarmLevel>(
						map.getInt(PARAMETER_SEARCH_LEVEL, false, FACTORY_KEY)
					);
				}

				_requestParameters.setFromParametersMap(map.getMap(), PARAMETER_SEARCH_LEVEL, 15, false);

				if(!doDisplayPreparationActions) return;

				_messages = GetSentMessages(
					_date,
					_searchStatus,
					_searchConflict,
					_searchLevel,
					_searchScenario.get() ? _searchScenario->getKey() : UNKNOWN_VALUE,
					_requestParameters.first
					, _requestParameters.maxSize
					, _requestParameters.orderField == PARAMETER_SEARCH_DATE
					, _requestParameters.orderField == PARAMETER_SEARCH_LEVEL
					, _requestParameters.orderField == PARAMETER_SEARCH_STATUS
					, _requestParameters.orderField == PARAMETER_SEARCH_CONFLICT
					, _requestParameters.raisingOrder
				);
				_resultParameters.setFromResult(_requestParameters, _messages);
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
			m.insert(PARAMETER_SEARCH_CONFLICT, static_cast<int>(_searchConflict));
			m.insert(PARAMETER_SEARCH_STATUS, static_cast<int>(_searchStatus));
			m.insert(PARAMETER_SEARCH_LEVEL, static_cast<int>(_searchLevel));
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
			newScenarioRequest.setObjectId(QueryString::UID_WILL_BE_GENERATED_BY_THE_ACTION);
			
			FunctionRequest<AdminRequest> alarmRequest(_request);
			alarmRequest.getFunction()->setPage<MessageAdmin>();

			FunctionRequest<AdminRequest> scenarioRequest(_request);
			scenarioRequest.getFunction()->setPage<MessagesScenarioAdmin>();

			ActionFunctionRequest<AlarmStopAction,AdminRequest> stopRequest(_request);
			stopRequest.getFunction()->setSamePage(this);
			
			ActionFunctionRequest<ScenarioStopAction,AdminRequest> scenarioStopRequest(_request);
			scenarioStopRequest.getFunction()->setSamePage(this);
			
			vector<pair<StatusSearch, string> > statusMap;
			statusMap.push_back(make_pair(BROADCAST_RUNNING, "En diffusion / prévu"));
			statusMap.push_back(make_pair(BROADCAST_RUNNING_WITH_END, "&nbsp;&gt;&nbsp;En cours avec date de fin"));
			statusMap.push_back(make_pair(BROADCAST_RUNNING_WITHOUT_END, "&nbsp;&gt;&nbsp;En cours sans date de fin"));
			statusMap.push_back(make_pair(FUTURE_BROADCAST, "&nbsp;&gt;&nbsp;Diffusion ultérieure"));
			statusMap.push_back(make_pair(BROADCAST_OVER, "Archivés"));
			statusMap.push_back(make_pair(BROADCAST_DRAFT, "Brouillons"));
			
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
			stream << s.cell(
				"Conflit",
				s.getForm().getSelectInput(PARAMETER_SEARCH_CONFLICT, MessagesModule::getConflictLabels(true), _searchConflict)
			);
			stream << s.cell(
				"Type",
				s.getForm().getSelectInput(
					PARAMETER_SEARCH_LEVEL,
					MessagesModule::GetLevelLabelsWithScenarios(true),
					(_searchLevel == ALARM_LEVEL_SCENARIO) ?
						(_searchScenario.get() ? _searchScenario->getKey() : 0) :
						static_cast<RegistryKeyType>(_searchLevel)
				)
			);

			stream << s.close();

			stream << "<h1>Résultats de la recherche</h1>";

			ActionResultHTMLTable::HeaderVector v1;
			v1.push_back(make_pair(string(), string("Statut")));
			v1.push_back(make_pair(PARAMETER_SEARCH_DATE, string("Dates")));
			v1.push_back(make_pair(PARAMETER_SEARCH_LEVEL, string("Type")));
			v1.push_back(make_pair(string(), string("Message")));
			//v1.push_back(make_pair(PARAMETER_SEARCH_STATUS, string("Etat")));
			v1.push_back(make_pair(PARAMETER_SEARCH_CONFLICT, string("Conflit")));
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

			BOOST_FOREACH(SentMessage message, _messages)
			{
				bool isDisplayedWithEndDate(
					(message.startTime.isUnknown() || message.startTime <= now)
					&& !message.endTime.isUnknown()
					&& message.endTime >= now
					&& message.enabled
				);
				bool isDisplayedWithoutEndDate(
					(message.startTime.isUnknown() || message.startTime <= now)
					&& message.endTime.isUnknown()
					&& message.enabled
				);
				bool willBeDisplayed(
					!message.startTime.isUnknown()
					&& message.startTime > now
					&& message.enabled
				);
				string rowColorCSS;
				if (!message.enabled)
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
				stream << t1.row(Conversion::ToString(message.id), rowColorCSS);

				// Dates
				stream << t1.col();
				if (!message.enabled)
				{
					if (!message.endTime.isUnknown() && message.endTime < now)
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
					if(message.startTime.isUnknown() || message.startTime <= now)
					{
						stream << "En cours";
					}
					else
					{
						stream << "Prévu";
					}
				}
				
				
				stream << t1.col();
				if (message.startTime.isUnknown() && message.endTime.isUnknown())
				{
					stream << "Diffusion permanente";
				}
				if (message.startTime.isUnknown() && !message.endTime.isUnknown())
				{
					stream << "Jusqu'au " << message.endTime.toString();
				}
				if (!message.startTime.isUnknown() && message.endTime.isUnknown())
				{
					stream << "A compter du " << message.startTime.toString();
				}
				if (!message.startTime.isUnknown() && !message.endTime.isUnknown())
				{
					stream << "Du " << message.startTime.toString() << " au " << message.endTime.toString();
				}

				// Type
				stream << t1.col() << MessagesModule::getLevelLabel(message.level);
				
				stream << t1.col() << message.name;
				//stream << t1.col(); // Bullet
				stream << t1.col() << MessagesModule::getConflictLabel(message.conflict); /// @todo put a graphic bullet
				stream << t1.col();
				if(message.level == ALARM_LEVEL_SCENARIO)
				{
					scenarioRequest.setObjectId(message.id);
				//scenarioStopRequest.setObjectId(scenario->getKey());

					stream << HTMLModule::getLinkButton(scenarioRequest.getURL(), "Modifier");
				/*if (scenario->isApplicable(DateTime(TIME_CURRENT)))
					stream << "&nbsp;" << HTMLModule::getLinkButton(scenarioStopRequest.getURL(), "Arrêter", "Etes-vous sûr de vouloir arrêter la diffusion des messages ?", "stop.png");*/
				} else {
					alarmRequest.setObjectId(message.id);
					stream << HTMLModule::getLinkButton(alarmRequest.getURL(), "Modifier");
				}
			}
			stream << t1.row();
			stream << t1.col();
			stream << t1.col();
			stream <<
				t1.col() <<
				t1.getActionForm().getSelectInput(
					NewScenarioSendAction::PARAMETER_TEMPLATE,
					MessagesModule::GetScenarioTemplatesLabels("(message seul)"),
					uid(UNKNOWN_VALUE)
				)
			;
			stream << t1.col(3) << t1.getActionForm().getSubmitButton("Nouvelle diffusion");

			stream << t1.close();

			
/*			stream << "<h1>Résultats de la recherche : messages seuls</h1>";

			ActionResultHTMLTable::HeaderVector v;
			v.push_back(make_pair(PARAMETER_SEARCH_START, string("Dates")));
			v.push_back(make_pair(string(), string("Message")));
			v.push_back(make_pair(PARAMETER_SEARCH_STATUS, string("Etat")));
			v.push_back(make_pair(PARAMETER_SEARCH_CONFLICT, string("Conflit")));
			v.push_back(make_pair(string(), string("Actions")));
			ActionResultHTMLTable t(v, searchRequest.getHTMLForm(), _requestParameters, _alarmResultParameters, newMessageRequest.getHTMLForm("newmess"), NewMessageAction::PARAMETER_MESSAGE_TEMPLATE, InterfaceModule::getVariableFromMap(variables, AdminModule::ICON_PATH_INTERFACE_VARIABLE));

			stream << t.open();

			BOOST_FOREACH(shared_ptr<SingleSentAlarm> alarm, _env.getRegistry<SingleSentAlarm>())
			{
				alarmRequest.setObjectId(alarm->getKey());
				stopRequest.setObjectId(alarm->getKey());
				bool alarmIsDisabled =
					!alarm->getIsEnabled();
				bool alarmIsDisplayedWithEndDate = 
					(alarm->getPeriodStart().isUnknown() || alarm->getPeriodStart() <= now)
					&& !alarm->getPeriodEnd().isUnknown()
					&& alarm->getPeriodEnd() >= now
					&& alarm->getIsEnabled();
				bool alarmIsDisplayedWithoutEndDate =
					(alarm->getPeriodStart().isUnknown() || alarm->getPeriodStart() <= now)
					&& alarm->getPeriodEnd().isUnknown()
					&& alarm->getIsEnabled();
				bool alarmWillBeDisplayed =
					!alarm->getPeriodStart().isUnknown()
					&& alarm->getPeriodStart() > now
					&& alarm->getIsEnabled();
				string rowColorCSS;
				if (alarmIsDisabled)
					rowColorCSS = CSS_ALARM_DISABLED;
				if (alarmIsDisplayedWithoutEndDate)
					rowColorCSS = CSS_ALARM_DISPLAYED_WITHOUT_END_DATE;
				if (alarmWillBeDisplayed)
					rowColorCSS = CSS_ALARM_WILL_BE_DISPLAYED;
				if (alarmIsDisplayedWithEndDate)
					rowColorCSS = CSS_ALARM_DISPLAYED_WITH_END_DATE;
				stream << t.row(Conversion::ToString(alarm->getKey()), rowColorCSS);
				stream << t.col();
				if (!alarm->getIsEnabled())
					stream << "Non diffusé";
				else
				{
					if (alarm->getPeriodStart().isUnknown() && alarm->getPeriodEnd().isUnknown())
						stream << "Diffusion permanente";
					if (alarm->getPeriodStart().isUnknown() && !alarm->getPeriodEnd().isUnknown())
						stream << "Jusqu'au " << alarm->getPeriodEnd().toString();
					if (!alarm->getPeriodStart().isUnknown() && alarm->getPeriodEnd().isUnknown())
						stream << "A compter du " << alarm->getPeriodStart().toString();
					if (!alarm->getPeriodStart().isUnknown() && !alarm->getPeriodEnd().isUnknown())
						stream << "Du " << alarm->getPeriodStart().toString() << " au " << alarm->getPeriodEnd().toString();
				}
				stream << t.col() << alarm->getShortMessage();
				stream << t.col() << MessagesModule::getLevelLabel(alarm->getLevel());
				stream << t.col(); // Bullet
				if (!alarm->getIsEnabled())
					stream << HTMLModule::getHTMLImage(IMG_URL_INFO, "Désactivé");
				else if (!alarm->getComplements().recipientsNumber)
					stream << HTMLModule::getHTMLImage(IMG_URL_INFO, "Pas de destinataire");
				else
				{
					stream << HTMLModule::getHTMLImage(IMG_URL_INFO, "Diffusion sur " + Conversion::ToString(alarm->getComplements().recipientsNumber) + "&nbsp;afficheur" + ((alarm->getComplements().recipientsNumber > 1) ? "s" : "") );
				}

				stream << t.col();
				switch(alarm->getComplements().conflictStatus)
				{
				case ALARM_WARNING_ON_INFO:
					/// @Todo Describe which info is overrided by the warning
					stream << HTMLModule::getHTMLImage(IMG_URL_INFO, MessagesModule::getConflictLabel(alarm->getComplements().conflictStatus));
					break;

				case ALARM_INFO_UNDER_WARNING:
					/// @Todo Describe which warning overrides the info
					stream << HTMLModule::getHTMLImage(IMG_URL_WARNING, MessagesModule::getConflictLabel(alarm->getComplements().conflictStatus));
					break;

				case ALARM_CONFLICT:
					/// @Todo Describe the conflict
					stream << HTMLModule::getHTMLImage(IMG_URL_ERROR, MessagesModule::getConflictLabel(alarm->getComplements().conflictStatus));
					break;
				}
				stream << t.col() << HTMLModule::getLinkButton(alarmRequest.getURL(), "Modifier", string(), "note_edit.png");
				if (alarm->isApplicable(DateTime(TIME_CURRENT)))
					stream << "&nbsp;" << HTMLModule::getLinkButton(stopRequest.getURL(), "Arrêter", "Etes-vous sûr de vouloir arrêter la diffusion du message ?", "stop.png");
			}

			stream << t.close();*/
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
	}
}

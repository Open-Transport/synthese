
/** MessagesAdmin class implementation.
	@file MessagesAdmin.cpp

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


#include "04_time/TimeParseException.h"
#include "04_time/DateTime.h"

#include "05_html/SearchFormHTMLTable.h"
#include "05_html/ActionResultHTMLTable.h"

#include "11_interfaces/InterfaceModule.h"

#include "30_server/ActionFunctionRequest.h"

#include "17_messages/SingleSentAlarm.h"
#include "17_messages/SentScenario.h"
#include "17_messages/AlarmRecipient.h"
#include "17_messages/AlarmTableSync.h"
#include "17_messages/ScenarioTableSync.h"
#include "17_messages/MessagesAdmin.h"
#include "17_messages/MessageAdmin.h"
#include "17_messages/MessagesScenarioAdmin.h"
#include "17_messages/MessagesModule.h"
#include "17_messages/NewScenarioSendAction.h"
#include "17_messages/NewMessageAction.h"
#include "17_messages/AlarmStopAction.h"
#include "17_messages/ScenarioStopAction.h"

#include "32_admin/AdminRequest.h"
#include "32_admin/AdminModule.h"
#include "32_admin/AdminParametersException.h"

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

	namespace messages
	{
		const std::string MessagesAdmin::PARAMETER_SEARCH_START = "mass";
		const std::string MessagesAdmin::PARAMETER_SEARCH_END = "mase";
		const std::string MessagesAdmin::PARAMETER_SEARCH_LEVEL = "masl";
		const std::string MessagesAdmin::PARAMETER_SEARCH_STATUS = "mast";
		const std::string MessagesAdmin::PARAMETER_SEARCH_CONFLICT = "masc";
		
		MessagesAdmin::MessagesAdmin()
			: AdminInterfaceElement("home", AdminInterfaceElement::EVER_DISPLAYED)
			, _startDate(TIME_UNKNOWN), _endDate(TIME_UNKNOWN)
			, _searchStatus(ALL_STATUS), _searchLevel(ALARM_LEVEL_UNKNOWN)
			, _searchConflict(ALARM_CONFLICT_UNKNOWN)
		{}

		void MessagesAdmin::setFromParametersMap(const ParametersMap& map)
		{
			try
			{
				_parametersMap = map;

				ParametersMap::const_iterator it = map.find(PARAMETER_SEARCH_START);
				if (it != map.end() && !it->second.empty())
				{
					_startDate.FromString(it->second);
				}

				it = map.find(PARAMETER_SEARCH_END);
				if (it != map.end() && !it->second.empty())
				{
					_endDate.FromString(it->second);
				}

				it = map.find(PARAMETER_SEARCH_CONFLICT);
				if (it != map.end())
					_searchConflict = (AlarmConflict) Conversion::ToInt(it->second);

				it = map.find(PARAMETER_SEARCH_STATUS);
				if (it != map.end())
					_searchStatus = (StatusSearch) Conversion::ToInt(it->second);

				it = map.find(PARAMETER_SEARCH_LEVEL);
				if (it != map.end())
					_searchLevel = (AlarmLevel) Conversion::ToInt(it->second);

				_requestParameters = ActionResultHTMLTable::getParameters(map, PARAMETER_SEARCH_LEVEL, 15);

				_result = AlarmTableSync::searchSingleSent(
					_startDate
					, _endDate
					, _requestParameters.first
					, _requestParameters.maxSize
					, _requestParameters.orderField == PARAMETER_SEARCH_START
					, _requestParameters.orderField == PARAMETER_SEARCH_LEVEL
					, _requestParameters.orderField == PARAMETER_SEARCH_STATUS
					, _requestParameters.orderField == PARAMETER_SEARCH_CONFLICT
					, _requestParameters.raisingOrder					
					);
				_alarmResultParameters = ActionResultHTMLTable::getParameters(_requestParameters, _result);

				_scenarioResult = ScenarioTableSync::searchSent(
					_startDate
					, _endDate
					, std::string()
					, _requestParameters.first
					, _requestParameters.maxSize
					, _requestParameters.orderField == PARAMETER_SEARCH_START
					, _requestParameters.orderField == PARAMETER_SEARCH_LEVEL
					, _requestParameters.orderField == PARAMETER_SEARCH_STATUS
					, _requestParameters.orderField == PARAMETER_SEARCH_CONFLICT
					, _requestParameters.raisingOrder
					);
				_scenarioResultParameters = ActionResultHTMLTable::getParameters(_requestParameters, _scenarioResult);
			}
			catch (TimeParseException e)
			{
				throw AdminParametersException("Date invalide");
			}

		}

		string MessagesAdmin::getTitle() const
		{
			return "Messages";
		}

		void MessagesAdmin::display(ostream& stream, interfaces::VariablesMap& variables, const server::FunctionRequest<admin::AdminRequest>* request) const
		{
			FunctionRequest<AdminRequest> searchRequest(request);
			searchRequest.getFunction()->setPage<MessagesAdmin>();

			ActionFunctionRequest<NewMessageAction,AdminRequest> newMessageRequest(request);
			newMessageRequest.getFunction()->setPage<MessageAdmin>();
			newMessageRequest.getFunction()->setActionFailedPage<MessagesAdmin>();

			ActionFunctionRequest<NewScenarioSendAction,AdminRequest> newScenarioRequest(request);
			newScenarioRequest.getFunction()->setPage(Factory<AdminInterfaceElement>::create<MessagesScenarioAdmin>());
			newScenarioRequest.getFunction()->setActionFailedPage(Factory<AdminInterfaceElement>::create<MessagesAdmin>());

			FunctionRequest<AdminRequest> alarmRequest(request);
			alarmRequest.getFunction()->setPage(Factory<AdminInterfaceElement>::create<MessageAdmin>());

			FunctionRequest<AdminRequest> scenarioRequest(request);
			scenarioRequest.getFunction()->setPage(Factory<AdminInterfaceElement>::create<MessagesScenarioAdmin>());

			ActionFunctionRequest<AlarmStopAction,AdminRequest> stopRequest(request);
			stopRequest.getFunction()->setPage(Factory<AdminInterfaceElement>::create<MessagesAdmin>());
			
			ActionFunctionRequest<ScenarioStopAction,AdminRequest> scenarioStopRequest(request);
			scenarioStopRequest.getFunction()->setPage(Factory<AdminInterfaceElement>::create<MessagesAdmin>());
			
			vector<pair<StatusSearch, string> > statusMap;
			statusMap.push_back(make_pair(ALL_STATUS, "(tous les états)"));
			statusMap.push_back(make_pair(BROADCAST_OVER, "Diffusion terminée"));
			statusMap.push_back(make_pair(BROADCAST_RUNNING, "En cours de diffusion"));
			statusMap.push_back(make_pair(BROADCAST_RUNNING_WITH_END, "En cours avec date de fin"));
			statusMap.push_back(make_pair(BROADCAST_RUNNING_WITHOUT_END, "En cours sans date de fin"));
			statusMap.push_back(make_pair(FUTURE_BROADCAST, "Diffusion ultérieure"));

			stream << "<h1>Recherche</h1>";

			SearchFormHTMLTable s(searchRequest.getHTMLForm());
			stream << s.open();
			stream << s.cell("Date début", s.getForm().getCalendarInput(PARAMETER_SEARCH_START, _startDate));
			stream << s.cell("Date fin", s.getForm().getCalendarInput(PARAMETER_SEARCH_END, _endDate));

			for (Factory<AlarmRecipient>::Iterator it = Factory<AlarmRecipient>::begin(); it != Factory<AlarmRecipient>::end(); ++it)
			{
				AlarmRecipientSearchFieldsMap m = it->getSearchFields(s.getForm(), _parametersMap);
				for (AlarmRecipientSearchFieldsMap::iterator itm = m.begin(); itm != m.end(); ++itm)
                    stream << s.cell(itm->second.label, itm->second.htmlField);
			}

			stream << s.cell("Statut", s.getForm().getSelectInput(PARAMETER_SEARCH_STATUS, statusMap, _searchStatus));
			stream << s.cell("Conflit", s.getForm().getSelectInput(PARAMETER_SEARCH_CONFLICT, MessagesModule::getConflictLabels(true), _searchConflict));
			stream << s.cell("Type", s.getForm().getSelectInput(PARAMETER_SEARCH_LEVEL, MessagesModule::getLevelLabels(true), _searchLevel));

			stream << s.close();

			stream << "<h1>Résultats de la recherche : messages selon scénario</h1>";

			ActionResultHTMLTable::HeaderVector v1;
			v1.push_back(make_pair(PARAMETER_SEARCH_START, string("Dates")));
			v1.push_back(make_pair(string(), string("Scénario")));
			v1.push_back(make_pair(PARAMETER_SEARCH_STATUS, string("Etat")));
			v1.push_back(make_pair(PARAMETER_SEARCH_CONFLICT, string("Conflit")));
			v1.push_back(make_pair(string(), string("Actions")));
			ActionResultHTMLTable t1(v1, searchRequest.getHTMLForm(), _requestParameters, _scenarioResultParameters, newScenarioRequest.getHTMLForm("newscen"), string(), InterfaceModule::getVariableFromMap(variables, AdminModule::ICON_PATH_INTERFACE_VARIABLE));
			
			stream << t1.open();

			for (vector<shared_ptr<SentScenario> >::const_iterator it = _scenarioResult.begin(); it != _scenarioResult.end(); ++it)
			{
				shared_ptr<SentScenario> scenario = *it;
				scenarioRequest.setObjectId(scenario->getKey());
				stream << t1.row();
				stream << t1.col() << scenario->getPeriodStart().toString();
				stream << t1.col() << scenario->getName();
				stream << t1.col(); // Bullet
				stream << t1.col(); // Bullet
				stream << t1.col() << HTMLModule::getLinkButton(scenarioRequest.getURL(), "Modifier");
				stream << t1.col() << HTMLModule::getLinkButton(scenarioStopRequest.getURL(), "Arrêter", "Etes-vous sûr de vouloir arrêter la diffusion des messages ?");
			}
			stream << t1.row();
			stream << t1.col(4) << t1.getActionForm().getSelectInput(NewScenarioSendAction::PARAMETER_TEMPLATE, MessagesModule::getScenarioTemplatesLabels(), uid(0));
			stream << t1.col() << t1.getActionForm().getSubmitButton("Nouvelle diffusion de scénario");

			stream << t1.close();

			
			stream << "<h1>Résultats de la recherche : messages seuls</h1>";

			ActionResultHTMLTable::HeaderVector v;
			v.push_back(make_pair(PARAMETER_SEARCH_START, string("Dates")));
			v.push_back(make_pair(string(), string("Message")));
			v.push_back(make_pair(PARAMETER_SEARCH_LEVEL, string("Type")));
			v.push_back(make_pair(PARAMETER_SEARCH_STATUS, string("Etat")));
			v.push_back(make_pair(PARAMETER_SEARCH_CONFLICT, string("Conflit")));
			v.push_back(make_pair(string(), string("Actions")));
			ActionResultHTMLTable t(v, searchRequest.getHTMLForm(), _requestParameters, _alarmResultParameters, newMessageRequest.getHTMLForm("newmess"), NewMessageAction::PARAMETER_IS_TEMPLATE, InterfaceModule::getVariableFromMap(variables, AdminModule::ICON_PATH_INTERFACE_VARIABLE));

			stream << t.open();

			for (vector<shared_ptr<SingleSentAlarm> >::const_iterator it= _result.begin(); it != _result.end(); ++it)
			{
				shared_ptr<SingleSentAlarm> alarm = *it;
				alarmRequest.setObjectId(alarm->getKey());
				stream << t.row(Conversion::ToString(alarm->getKey()));
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
				stream << t.col(); // Bullet
				stream << t.col() << HTMLModule::getLinkButton(alarmRequest.getURL(), "Modifier");
				if (alarm->isApplicable(DateTime()))
					stream << "&nbsp;" << HTMLModule::getLinkButton(stopRequest.getURL(), "Arrêter", "Etes-vous sûr de vouloir arrêter la diffusion du message ?");
			}
			stream << t.row();
			stream << t.col(5) << "(Sélectionnez un message pour le copier)";
			stream << t.col() << t.getActionForm().getSubmitButton("Nouvelle diffusion de message");

			stream << t.close();
		}

		bool MessagesAdmin::isAuthorized( const server::FunctionRequest<admin::AdminRequest>* request ) const
		{
			return true;
		}
	}
}

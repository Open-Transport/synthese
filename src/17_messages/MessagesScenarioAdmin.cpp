////////////////////////////////////////////////////////////////////////////////
/// MessagesScenarioAdmin class implementation.
///	@file MessagesScenarioAdmin.cpp
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

#include "ActionResultHTMLTable.h"
#include "PropertiesHTMLTable.h"
#include "MessagesScenarioAdmin.h"
#include "MessagesAdmin.h"
#include "MessageAdmin.h"
#include "Scenario.h"
#include "ScenarioTemplate.h"
#include "SentScenario.h"
#include "ScenarioTableSync.h"
#include "AlarmTableSync.h"
#include "ScenarioNameUpdateAction.h"
#include "ScenarioUpdateDatesAction.h"
#include "DeleteAlarmAction.h"
#include "NewMessageAction.h"
#include "MessagesLibraryAdmin.h"
#include "MessagesModule.h"
#include "MessagesRight.h"
#include "MessagesLibraryRight.h"
#include "ScenarioSentAlarmInheritedTableSync.h"
#include "AlarmTemplateInheritedTableSync.h"
#include "ActionFunctionRequest.h"
#include "QueryString.h"
#include "AdminParametersException.h"
#include "AdminRequest.h"

#include <boost/foreach.hpp>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace admin;
	using namespace interfaces;
	using namespace server;
	using namespace util;
	using namespace html;
	using namespace messages;
	using namespace security;	

	namespace util
	{
		template<> const string FactorableTemplate<AdminInterfaceElement,MessagesScenarioAdmin>::FACTORY_KEY("messagesscenario");
	}

	namespace admin
	{
		template<> const string AdminInterfaceElementTemplate<MessagesScenarioAdmin>::ICON("cog.png");
		template<> const string AdminInterfaceElementTemplate<MessagesScenarioAdmin>::DEFAULT_TITLE("Scénario inconnu");
	}

	namespace messages
	{
		const string MessagesScenarioAdmin::TAB_MESSAGES("m");
		const string MessagesScenarioAdmin::TAB_PARAMETERS("p");
		const string MessagesScenarioAdmin::TAB_VARIABLES("v");

		void MessagesScenarioAdmin::setFromParametersMap(const ParametersMap& map)
		{
			uid id(map.getUid(QueryString::PARAMETER_OBJECT_ID, true, FACTORY_KEY));
			if (id == QueryString::UID_WILL_BE_GENERATED_BY_THE_ACTION)
				return;

			try
			{
				_scenario = ScenarioTableSync::Get(id, _env);
				_sentScenario = dynamic_pointer_cast<const SentScenario, const Scenario>(_scenario);
				_templateScenario = dynamic_pointer_cast<const ScenarioTemplate, const Scenario>(_scenario);
			}
			catch(...)
			{
				throw AdminParametersException("Specified scenario not found");
			}
		}


		void MessagesScenarioAdmin::display(ostream& stream, interfaces::VariablesMap& variables
		) const	{
			ActionFunctionRequest<ScenarioNameUpdateAction,AdminRequest> updateRequest(_request);
			updateRequest.getFunction()->setPage<MessagesScenarioAdmin>();
			updateRequest.setObjectId(_scenario->getKey());

			FunctionRequest<AdminRequest> messRequest(_request);
			messRequest.getFunction()->setPage<MessageAdmin>();

			ActionFunctionRequest<DeleteAlarmAction,AdminRequest> deleteRequest(_request);
			deleteRequest.getFunction()->setPage<MessagesScenarioAdmin>();
			deleteRequest.setObjectId(_scenario->getKey());

			ActionFunctionRequest<NewMessageAction,AdminRequest> addRequest(_request);
			addRequest.getFunction()->setPage<MessageAdmin>();
			addRequest.getAction()->setScenarioId(_scenario->getKey());
			addRequest.getAction()->setIsTemplate(_templateScenario.get() != NULL);

			stream << "<h1>Propriétés</h1>";
			PropertiesHTMLTable tp(updateRequest.getHTMLForm("update"));
			stream << tp.open();
			stream << tp.cell("Nom", tp.getForm().getTextInput(ScenarioNameUpdateAction::PARAMETER_NAME, _scenario->getName()));
			if (_templateScenario.get())
			{
				stream << tp.cell("Répertoire", tp.getForm().getSelectInput(ScenarioNameUpdateAction::PARAMETER_FOLDER_ID, MessagesModule::GetScenarioFoldersLabels(), _templateScenario->getFolderId()));
			}
			stream << tp.close();

			////////////////////////////////////////////////////////////////////
			// TAB PARAMETERS
			if (openTabContent(stream, TAB_PARAMETERS))
			{
				ActionFunctionRequest<ScenarioUpdateDatesAction, AdminRequest> updateDatesRequest(_request);
				updateDatesRequest.getFunction()->setPage<MessagesScenarioAdmin>();
				updateDatesRequest.setObjectId(_scenario->getKey());

				stream << "<h1>Propriétés</h1>";
				PropertiesHTMLTable udt(updateDatesRequest.getHTMLForm("update_dates"));

				stream << udt.open();
				stream << udt.title("Paramètres");
				stream << udt.cell("Début diffusion", udt.getForm().getCalendarInput(ScenarioUpdateDatesAction::PARAMETER_START_DATE, _sentScenario->getPeriodStart()));
				stream << udt.cell("Fin diffusion", udt.getForm().getCalendarInput(ScenarioUpdateDatesAction::PARAMETER_END_DATE, _sentScenario->getPeriodEnd()));
				stream << udt.cell("Actif", udt.getForm().getOuiNonRadioInput(ScenarioUpdateDatesAction::PARAMETER_ENABLED, _sentScenario->getIsEnabled()));

				if (!_sentScenario->getTemplate()->getVariables().empty())
				{
					stream << udt.title("Variables (* = champ obligatoire)");
					const ScenarioTemplate::VariablesMap& variables(_sentScenario->getTemplate()->getVariables());
					BOOST_FOREACH(const ScenarioTemplate::VariablesMap::value_type variable, variables)
					{
						string value;
						SentScenario::VariablesMap::const_iterator it(_sentScenario->getVariables().find(variable.second.code));
						if (it != _sentScenario->getVariables().end()) value = it->second;

						stream << udt.cell(
							variable.second.code + (variable.second.compulsory ? "*" : "") + (variable.second.helpMessage.empty() ? string() : (" ("+ HTMLModule::getHTMLImage("info.png", "Info : ") + variable.second.helpMessage + ")"))
							, udt.getForm().getTextInput(ScenarioUpdateDatesAction::PARAMETER_VARIABLE, value)
						);
					}
				}
				stream << udt.close();
			}

			////////////////////////////////////////////////////////////////////
			// TAB MESSAGES
			if (openTabContent(stream, TAB_MESSAGES))
			{
				stream << "<h1>Messages</h1>";

				vector<shared_ptr<Alarm> > v;
				Env env;

				if (_sentScenario.get())
				{
					ScenarioSentAlarmInheritedTableSync::Search(env, _sentScenario.get());
					BOOST_FOREACH(shared_ptr<ScenarioSentAlarm> alarm, env.getRegistry<ScenarioSentAlarm>())
					{
						v.push_back(static_pointer_cast<Alarm, ScenarioSentAlarm>(alarm));
					}
				}
				else
				{
					AlarmTemplateInheritedTableSync::Search(env, _templateScenario.get());
					BOOST_FOREACH(shared_ptr<AlarmTemplate> alarm, env.getRegistry<AlarmTemplate>())
					{
						v.push_back(static_pointer_cast<Alarm, AlarmTemplate>(alarm));
					}
				}

				ActionResultHTMLTable::HeaderVector h;
				h.push_back(make_pair(string(), "Message"));
				h.push_back(make_pair(string(), "Emplacement"));
				h.push_back(make_pair(string(), "Actions"));
				h.push_back(make_pair(string(), "Actions"));
				ActionResultHTMLTable t(h, HTMLForm(string(), string()), ActionResultHTMLTable::RequestParameters(), ActionResultHTMLTable::ResultParameters(), addRequest.getHTMLForm("add"), NewMessageAction::PARAMETER_MESSAGE_TEMPLATE);

				stream << t.open();

				BOOST_FOREACH(shared_ptr<Alarm> alarm, v)
				{
					messRequest.setObjectId(alarm->getKey());
					deleteRequest.getAction()->setAlarmId(alarm->getKey());

					stream << t.row(Conversion::ToString(alarm->getKey()));
					stream << t.col() << alarm->getShortMessage();
					stream << t.col() << ""; // Emplacement
					stream << t.col() << HTMLModule::getLinkButton(messRequest.getURL(), "Modifier");
					stream << t.col() << HTMLModule::getLinkButton(deleteRequest.getURL(), "Supprimer", "Etes-vous sûr de vouloir supprimer le message du scénario ?");
				}

				stream << t.row();
				stream << t.col(2) << "(sélectionnez un&nbsp;message existant pour créer une copie)";
				stream << t.col() << t.getActionForm().getSubmitButton("Ajouter");
				stream << t.col();
				stream << t.close();
			}

			////////////////////////////////////////////////////////////////////
			// TAB VARIABLES
			if (openTabContent(stream, TAB_VARIABLES))
			{
				
			}
		}

		bool MessagesScenarioAdmin::isAuthorized() const
		{
			if (_scenario.get() == NULL) return false;
			if (dynamic_pointer_cast<const SentScenario, const Scenario>(_scenario).get() != NULL) return _request->isAuthorized<MessagesRight>(READ);
			return _request->isAuthorized<MessagesLibraryRight>(READ);
		}

		MessagesScenarioAdmin::MessagesScenarioAdmin()
			: AdminInterfaceElementTemplate<MessagesScenarioAdmin>()
		{

		}

		AdminInterfaceElement::PageLinks MessagesScenarioAdmin::getSubPagesOfParent( const PageLink& parentLink , const AdminInterfaceElement& currentPage
		) const
		{
			AdminInterfaceElement::PageLinks links;
			if (currentPage.getFactoryKey() == FACTORY_KEY)
			{
				const MessagesScenarioAdmin& currentSPage(static_cast<const MessagesScenarioAdmin&>(currentPage));
				if (parentLink.factoryKey == (currentSPage._sentScenario.get() ? MessagesAdmin::FACTORY_KEY : MessagesLibraryAdmin::FACTORY_KEY))
				{
					links.push_back(currentPage.getPageLink());
				}
			}

			if (currentPage.getFactoryKey() == MessageAdmin::FACTORY_KEY && parentLink.factoryKey == MessagesAdmin::FACTORY_KEY)
			{
				const MessageAdmin& currentSPage(static_cast<const MessageAdmin&>(currentPage));
				shared_ptr<const ScenarioSentAlarm> alarm(dynamic_pointer_cast<const ScenarioSentAlarm, const Alarm>(currentSPage.getAlarm()));
				if (alarm.get())
				{
					const SentScenario* scenario(alarm->getScenario());
					AdminInterfaceElement::PageLink link(getPageLink());
					link.factoryKey = MessagesScenarioAdmin::FACTORY_KEY;
					link.name = scenario->getName();
					link.icon = MessagesScenarioAdmin::ICON;
					link.parameterName = QueryString::PARAMETER_OBJECT_ID;
					link.parameterValue = Conversion::ToString(scenario->getKey());
					links.push_back(link);
				}
			}

			return links;
		}

		AdminInterfaceElement::PageLinks MessagesScenarioAdmin::getSubPages( const AdminInterfaceElement& currentPage) const
		{
			AdminInterfaceElement::PageLinks links;
			Env env;
			if (_sentScenario.get())
			{
				ScenarioSentAlarmInheritedTableSync::Search(env, _sentScenario.get(), 0, 0, false, false, false, false, UP_LINKS_LOAD_LEVEL);
				BOOST_FOREACH(shared_ptr<ScenarioSentAlarm> alarm, env.getRegistry<ScenarioSentAlarm>())
				{
					AdminInterfaceElement::PageLink link(getPageLink());
					link.factoryKey = MessageAdmin::FACTORY_KEY;
					link.name = alarm->getShortMessage();
					link.icon = MessageAdmin::ICON;
					link.parameterName = QueryString::PARAMETER_OBJECT_ID;
					link.parameterValue = Conversion::ToString(alarm->getKey());
					links.push_back(link);
				}
			}
			else if (_templateScenario.get())
			{
				AlarmTemplateInheritedTableSync::Search(env, _templateScenario.get(), 0, 0, false, false, UP_LINKS_LOAD_LEVEL);
				BOOST_FOREACH(shared_ptr<AlarmTemplate> alarm, env.getRegistry<AlarmTemplate>())
				{
					AdminInterfaceElement::PageLink link(getPageLink());
					link.factoryKey = MessageAdmin::FACTORY_KEY;
					link.name = alarm->getShortMessage();
					link.icon = MessageAdmin::ICON;
					link.parameterName = QueryString::PARAMETER_OBJECT_ID;
					link.parameterValue = Conversion::ToString(alarm->getKey());
					links.push_back(link);
				}
			}

			return links;
		}

		std::string MessagesScenarioAdmin::getTitle() const
		{
			return _scenario.get() ? _scenario->getName() : DEFAULT_TITLE;
		}

		std::string MessagesScenarioAdmin::getParameterName() const
		{
			return _scenario.get() ? QueryString::PARAMETER_OBJECT_ID : string();
		}

		std::string MessagesScenarioAdmin::getParameterValue() const
		{
			return _scenario.get() ? Conversion::ToString(_scenario->getKey()) : string();
		}



		void MessagesScenarioAdmin::_buildTabs(
		) const {
			_tabs.clear();

			if (_sentScenario.get() != NULL)
			{
				_tabs.push_back(Tab("Paramètres de diffusion", TAB_PARAMETERS, true));
			}
			_tabs.push_back(Tab("Messages diffusés", TAB_MESSAGES, true));
			if (_templateScenario.get() != NULL)
			{
				_tabs.push_back(Tab("Variables", TAB_VARIABLES, true));
			}

			_tabBuilded = true;
		}
	}
}

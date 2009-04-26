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
#include "ActionException.h"
#include "MessagesLog.h"
#include "MessagesLibraryLog.h"
#include "ScenarioFolder.h"

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
		template<> const string AdminInterfaceElementTemplate<MessagesScenarioAdmin>::DEFAULT_TITLE("Sc�nario inconnu");
	}

	namespace messages
	{
		const string MessagesScenarioAdmin::TAB_MESSAGES("m");
		const string MessagesScenarioAdmin::TAB_PARAMETERS("p");
		const string MessagesScenarioAdmin::TAB_VARIABLES("v");
		const string MessagesScenarioAdmin::TAB_LOG("l");

		void MessagesScenarioAdmin::setFromParametersMap(
			const ParametersMap& map,
			bool doDisplayPreparationActions
		){
			uid id(map.getUid(QueryString::PARAMETER_OBJECT_ID, true, FACTORY_KEY));
			if (id == QueryString::UID_WILL_BE_GENERATED_BY_THE_ACTION)
				return;

			try
			{
				_scenario = ScenarioTableSync::Get(id, _env, UP_LINKS_LOAD_LEVEL);
				_sentScenario = dynamic_pointer_cast<const SentScenario, const Scenario>(_scenario);
				_templateScenario = dynamic_pointer_cast<const ScenarioTemplate, const Scenario>(_scenario);
			}
			catch(...)
			{
				throw AdminParametersException("Specified scenario not found");
			}
			
			if(_sentScenario.get())
			{
				_generalLogView.set(map, MessagesLog::FACTORY_KEY, _scenario->getKey());
			}
			else
			{
				_generalLogView.set(map, MessagesLibraryLog::FACTORY_KEY, _scenario->getKey());
			}
		}



		server::ParametersMap MessagesScenarioAdmin::getParametersMap() const
		{
			ParametersMap m(_generalLogView.getParametersMap());
			if(_scenario.get())
				m.insert(QueryString::PARAMETER_OBJECT_ID, _scenario->getKey());
			return m;
		}



		void MessagesScenarioAdmin::display(
			ostream& stream,
			interfaces::VariablesMap& variables
		) const	{

			////////////////////////////////////////////////////////////////////
			// TAB PARAMETERS
			if (openTabContent(stream, TAB_PARAMETERS))
			{
				ActionFunctionRequest<ScenarioUpdateDatesAction, AdminRequest> updateDatesRequest(_request);
				updateDatesRequest.getFunction()->setPage<MessagesScenarioAdmin>();
				updateDatesRequest.setObjectId(_scenario->getKey());
				updateDatesRequest.getAction()->setScenarioId(_scenario->getKey());

				stream << "<h1>Param�tres</h1>";
				PropertiesHTMLTable udt(updateDatesRequest.getHTMLForm("update_dates"));

				stream << udt.open();
				stream << udt.title("Propri�t�s");
				stream << udt.cell("Nom", udt.getForm().getTextInput(ScenarioUpdateDatesAction::PARAMETER_NAME, _scenario->getName()));
				if (_templateScenario.get())
				{
					stream << udt.cell("R�pertoire", udt.getForm().getSelectInput(ScenarioUpdateDatesAction::PARAMETER_FOLDER_ID, MessagesModule::GetScenarioFoldersLabels(), _templateScenario->getFolder() ? _templateScenario->getFolder()->getKey() : 0));
				}
				if(_sentScenario.get())
				{
					stream << udt.title("Diffusion");
					stream << udt.cell("D�but diffusion", udt.getForm().getCalendarInput(ScenarioUpdateDatesAction::PARAMETER_START_DATE, _sentScenario->getPeriodStart()));
					stream << udt.cell("Fin diffusion", udt.getForm().getCalendarInput(ScenarioUpdateDatesAction::PARAMETER_END_DATE, _sentScenario->getPeriodEnd()));

					stream << udt.cell("Actif", udt.getForm().getOuiNonRadioInput(ScenarioUpdateDatesAction::PARAMETER_ENABLED, _sentScenario->getIsEnabled()));
				

					if (_sentScenario->getTemplate() && !_sentScenario->getTemplate()->getVariables().empty())
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
								, udt.getForm().getTextInput(ScenarioUpdateDatesAction::PARAMETER_VARIABLE + variable.second.code, value)
							);
						}
					}
				}
				stream << udt.close();
			}

			////////////////////////////////////////////////////////////////////
			// TAB MESSAGES
			if (openTabContent(stream, TAB_MESSAGES))
			{
				FunctionRequest<AdminRequest> messRequest(_request);
				messRequest.getFunction()->setPage<MessageAdmin>();

				ActionFunctionRequest<DeleteAlarmAction,AdminRequest> deleteRequest(_request);
				deleteRequest.getFunction()->setPage<MessagesScenarioAdmin>();
				deleteRequest.setObjectId(_scenario->getKey());

				ActionFunctionRequest<NewMessageAction,AdminRequest> addRequest(_request);
				addRequest.getFunction()->setPage<MessageAdmin>();
				addRequest.setObjectId(QueryString::UID_WILL_BE_GENERATED_BY_THE_ACTION);
				addRequest.getAction()->setScenarioId(_scenario->getKey());

				stream << "<h1>Messages</h1>";

				vector<shared_ptr<Alarm> > v;
				Env env;

				if (_sentScenario.get())
				{
					ScenarioSentAlarmInheritedTableSync::Search(env, _sentScenario->getKey());
					BOOST_FOREACH(shared_ptr<SentAlarm> alarm, env.getRegistry<SentAlarm>())
					{
						v.push_back(static_pointer_cast<Alarm, SentAlarm>(alarm));
					}
				}
				else
				{
					AlarmTemplateInheritedTableSync::Search(env, _templateScenario->getKey());
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
				ActionResultHTMLTable t(h, HTMLForm(string(), string()), ActionResultHTMLTable::RequestParameters(), ActionResultHTMLTable::ResultParameters(), addRequest.getHTMLForm("add"));

				stream << t.open();

				BOOST_FOREACH(shared_ptr<Alarm> alarm, v)
				{
					messRequest.setObjectId(alarm->getKey());
					deleteRequest.getAction()->setAlarmId(alarm->getKey());

					stream << t.row(Conversion::ToString(alarm->getKey()));
					stream << t.col() << alarm->getShortMessage();
					stream << t.col() << ""; // Emplacement
					stream << t.col() << HTMLModule::getLinkButton(messRequest.getURL(), "Modifier");
					stream << t.col() << HTMLModule::getLinkButton(deleteRequest.getURL(), "Supprimer", "Etes-vous s�r de vouloir supprimer le message du sc�nario ?");
				}

				stream << t.row();
				stream << t.col(2) << "(s�lectionnez un&nbsp;message existant pour cr�er une copie)";
				stream << t.col() << t.getActionForm().getSubmitButton("Ajouter");
				stream << t.col();
				stream << t.close();
			}

			////////////////////////////////////////////////////////////////////
			// TAB VARIABLES
			if (openTabContent(stream, TAB_VARIABLES))
			{
				stream << "<h1>R�capitulatif des variables du sc�nario</h1>";

				const ScenarioTemplate::VariablesMap& variables(_templateScenario->getVariables());

				if(variables.empty())
				{
					stream << "<p>Aucune variable d�finie.</p>";
				}
				else
				{
					HTMLTable::ColsVector h;
					h.push_back("Code");
					h.push_back("Info");
					h.push_back("Obligatoire");
					HTMLTable t(h, ResultHTMLTable::CSS_CLASS);
					stream << t.open();
					BOOST_FOREACH(const ScenarioTemplate::VariablesMap::value_type variable, variables)
					{
						string value;
						stream << t.row();
						stream << t.col() << variable.second.code;
						stream << t.col() << variable.second.helpMessage;
						stream << t.col() << (variable.second.compulsory ? "OUI" : "NON");
					}
					stream << t.close();
				}
			}
			
			////////////////////////////////////////////////////////////////////
			// TAB LOG
			if (openTabContent(stream, TAB_LOG))
			{
				// Log search
				FunctionRequest<AdminRequest> searchRequest(_request);
				searchRequest.getFunction()->setSamePage(this);

				_generalLogView.display(
					stream,
					searchRequest
				);

			}

			closeTabContent(stream);
		}

		bool MessagesScenarioAdmin::isAuthorized() const
		{
			if (_scenario.get() == NULL) return false;
			if (dynamic_pointer_cast<const SentScenario, const Scenario>(_scenario).get() != NULL) return _request->isAuthorized<MessagesRight>(READ);
			return _request->isAuthorized<MessagesLibraryRight>(READ);
		}

		MessagesScenarioAdmin::MessagesScenarioAdmin(
		):	AdminInterfaceElementTemplate<MessagesScenarioAdmin>(),
			_generalLogView("g")
		{

		}



		AdminInterfaceElement::PageLinks MessagesScenarioAdmin::getSubPagesOfParent(
			const PageLink& parentLink,
			const AdminInterfaceElement& currentPage
		) const	{
			AdminInterfaceElement::PageLinks links;
			return links;
		}



		AdminInterfaceElement::PageLinks MessagesScenarioAdmin::getSubPages( const AdminInterfaceElement& currentPage) const
		{
			AdminInterfaceElement::PageLinks links;
			Env env;
			if (_sentScenario.get())
			{
				ScenarioSentAlarmInheritedTableSync::Search(
					env, _sentScenario->getKey(), 0, 0, false, false, false, false, UP_LINKS_LOAD_LEVEL
				);
				BOOST_FOREACH(shared_ptr<SentAlarm> alarm, env.getRegistry<SentAlarm>())
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
				AlarmTemplateInheritedTableSync::Search(
					env, _templateScenario->getKey(), 0, 0, false, false, UP_LINKS_LOAD_LEVEL
				);
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

			_tabs.push_back(Tab("Param�tres", TAB_PARAMETERS, true, "table.png"));
			_tabs.push_back(Tab("Messages", TAB_MESSAGES, true, "note.png"));
			if(_templateScenario.get())
			{
				_tabs.push_back(Tab("Variables", TAB_VARIABLES, true));
			}
			_tabs.push_back(Tab("Journal", TAB_LOG, true, "book.png"));

			_tabBuilded = true;
		}



		boost::shared_ptr<const Scenario> MessagesScenarioAdmin::getScenario() const
		{
			return _scenario;
		}
	}
}

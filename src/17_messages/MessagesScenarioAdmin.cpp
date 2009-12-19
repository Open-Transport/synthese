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
#include "AdminFunctionRequest.hpp"
#include "AdminActionFunctionRequest.hpp"
#include "AdminParametersException.h"
#include "AdminInterfaceElement.h"
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
		template<> const string AdminInterfaceElementTemplate<MessagesScenarioAdmin>::DEFAULT_TITLE("(pas de nom)");
	}

	namespace messages
	{
		const string MessagesScenarioAdmin::TAB_MESSAGES("m");
		const string MessagesScenarioAdmin::TAB_PARAMETERS("p");
		const string MessagesScenarioAdmin::TAB_VARIABLES("v");
		const string MessagesScenarioAdmin::TAB_LOG("l");

		void MessagesScenarioAdmin::setFromParametersMap(
			const ParametersMap& map,
			bool objectWillBeCreatedLater
		){
			if(objectWillBeCreatedLater) return;
			
			try
			{
				_scenario = ScenarioTableSync::Get(
					map.get<RegistryKeyType>(Request::PARAMETER_OBJECT_ID),
					_getEnv(),
					UP_LINKS_LOAD_LEVEL
				);
			}
			catch(...)
			{
				throw AdminParametersException("Specified scenario not found");
			}
			
			if(dynamic_cast<const SentScenario*>(_scenario.get()))
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
				m.insert(Request::PARAMETER_OBJECT_ID, _scenario->getKey());
			return m;
		}



		void MessagesScenarioAdmin::display(
			ostream& stream,
			interfaces::VariablesMap& variables,
					const server::FunctionRequest<admin::AdminRequest>& _request
		) const	{

			const SentScenario* _sentScenario = dynamic_cast<const SentScenario*>(_scenario.get());
			const ScenarioTemplate* _templateScenario = dynamic_cast<const ScenarioTemplate*>(_scenario.get());


			////////////////////////////////////////////////////////////////////
			// TAB PARAMETERS
			if (openTabContent(stream, TAB_PARAMETERS))
			{
				AdminActionFunctionRequest<ScenarioUpdateDatesAction, MessagesScenarioAdmin> updateDatesRequest(_request);
				updateDatesRequest.getAction()->setScenarioId(_scenario->getKey());

				stream << "<h1>Paramètres</h1>";
				PropertiesHTMLTable udt(updateDatesRequest.getHTMLForm("update_dates"));

				stream << udt.open();
				stream << udt.title("Propriétés");
				stream << udt.cell("Nom", udt.getForm().getTextInput(ScenarioUpdateDatesAction::PARAMETER_NAME, _scenario->getName()));
				if (_templateScenario)
				{
					stream << udt.cell("Répertoire", udt.getForm().getSelectInput(ScenarioUpdateDatesAction::PARAMETER_FOLDER_ID, MessagesModule::GetScenarioFoldersLabels(), _templateScenario->getFolder() ? _templateScenario->getFolder()->getKey() : 0));
				}
				if(_sentScenario)
				{
					stream << udt.title("Diffusion");
					stream << udt.cell("Début diffusion", udt.getForm().getCalendarInput(ScenarioUpdateDatesAction::PARAMETER_START_DATE, _sentScenario->getPeriodStart().toPosixTime()));
					stream << udt.cell("Fin diffusion", udt.getForm().getCalendarInput(ScenarioUpdateDatesAction::PARAMETER_END_DATE, _sentScenario->getPeriodEnd().toPosixTime()));

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
								variable.second.code + (variable.second.compulsory ? "*" : "") + (variable.second.helpMessage.empty() ? string() : (" ("+ HTMLModule::getHTMLImage("information.png", "Info : ") + variable.second.helpMessage + ")"))
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
				AdminActionFunctionRequest<DeleteAlarmAction,MessagesScenarioAdmin> deleteRequest(_request);

				AdminActionFunctionRequest<NewMessageAction,MessageAdmin> addRequest(_request);
				addRequest.setActionWillCreateObject();
				addRequest.getAction()->setScenarioId(_scenario->getKey());

				stream << "<h1>Messages</h1>";

				vector<shared_ptr<Alarm> > v;
				
				if (_sentScenario)
				{
					ScenarioSentAlarmInheritedTableSync::SearchResult alarms(
						ScenarioSentAlarmInheritedTableSync::Search(*_env, _sentScenario->getKey())
					);
					BOOST_FOREACH(shared_ptr<SentAlarm> alarm, alarms)
					{
						v.push_back(static_pointer_cast<Alarm, SentAlarm>(alarm));
					}
				}
				else
				{
					AlarmTemplateInheritedTableSync::SearchResult alarms(
						AlarmTemplateInheritedTableSync::Search(*_env, _templateScenario->getKey())
					);
					BOOST_FOREACH(shared_ptr<AlarmTemplate> alarm, alarms)
					{
						v.push_back(static_pointer_cast<Alarm, AlarmTemplate>(alarm));
					}
				}

				ActionResultHTMLTable::HeaderVector h;
				h.push_back(make_pair(string(), "Message"));
				h.push_back(make_pair(string(), "Emplacement"));
				h.push_back(make_pair(string(), "Actions"));
				h.push_back(make_pair(string(), "Actions"));
				ActionResultHTMLTable t(
					h,
					HTMLForm(string(), string()),
					ActionResultHTMLTable::RequestParameters(),
					v,
					addRequest.getHTMLForm("add")
				);

				stream << t.open();

				AdminFunctionRequest<MessageAdmin> messRequest(_request);
				BOOST_FOREACH(shared_ptr<Alarm> alarm, v)
				{
					messRequest.getPage()->setMessage(alarm);
					deleteRequest.getAction()->setAlarmId(alarm->getKey());

					stream << t.row(Conversion::ToString(alarm->getKey()));
					stream << t.col() << alarm->getShortMessage();
					stream << t.col() << ""; // Emplacement
					stream << t.col() << HTMLModule::getLinkButton(messRequest.getURL(), "Ouvrir");
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
				stream << "<h1>Récapitulatif des variables du scénario</h1>";

				const ScenarioTemplate::VariablesMap& variables(_templateScenario->getVariables());

				if(variables.empty())
				{
					stream << "<p>Aucune variable définie.</p>";
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
				_generalLogView.display(stream, FunctionRequest<AdminRequest>(_request));
			}

			closeTabContent(stream);
		}

		bool MessagesScenarioAdmin::isAuthorized(
				const server::FunctionRequest<admin::AdminRequest>& _request
			) const
		{
			if(_request.getActionWillCreateObject()) return true;
			if (_scenario.get() == NULL) return false;
			if (dynamic_pointer_cast<const SentScenario, const Scenario>(_scenario).get() != NULL)
				return _request.isAuthorized<MessagesRight>(READ);
			return _request.isAuthorized<MessagesLibraryRight>(READ);
		}

		MessagesScenarioAdmin::MessagesScenarioAdmin(
		):	AdminInterfaceElementTemplate<MessagesScenarioAdmin>(),
			_generalLogView("g")
		{

		}





		AdminInterfaceElement::PageLinks MessagesScenarioAdmin::getSubPages(
			const AdminInterfaceElement& currentPage,
			const server::FunctionRequest<admin::AdminRequest>& request
		) const {
			AdminInterfaceElement::PageLinks links;
			
			if (dynamic_cast<const SentScenario*>(_scenario.get()))
			{
				ScenarioSentAlarmInheritedTableSync::SearchResult alarms(
					ScenarioSentAlarmInheritedTableSync::Search(
						*_env,
						_scenario->getKey(), 0, optional<size_t>(), false, false, false, false, UP_LINKS_LOAD_LEVEL
				)	);
				BOOST_FOREACH(shared_ptr<SentAlarm> alarm, alarms)
				{
					shared_ptr<MessageAdmin> p(
						getNewOtherPage<MessageAdmin>()
					);
					p->setMessage(alarm);
					links.push_back(p);
				}
			}
			else if (dynamic_cast<const ScenarioTemplate*>(_scenario.get()))
			{
				AlarmTemplateInheritedTableSync::SearchResult alarms(
					AlarmTemplateInheritedTableSync::Search(
						*_env,
						_scenario->getKey(), 0, optional<size_t>(), false, false, UP_LINKS_LOAD_LEVEL
				)	);
				BOOST_FOREACH(shared_ptr<AlarmTemplate> alarm, alarms)
				{
					shared_ptr<MessageAdmin> p(
						getNewOtherPage<MessageAdmin>()
					);
					p->setMessage(alarm);
					links.push_back(p);
				}
			}

			return links;
		}



		std::string MessagesScenarioAdmin::getTitle() const
		{
			return _scenario.get() && !_scenario->getName().empty() ? _scenario->getName() : DEFAULT_TITLE;
		}




		void MessagesScenarioAdmin::_buildTabs(
			const server::FunctionRequest<admin::AdminRequest>& _request
		) const {
			_tabs.clear();

			_tabs.push_back(Tab("Paramètres", TAB_PARAMETERS, true, "table.png"));
			_tabs.push_back(Tab("Messages", TAB_MESSAGES, true, "note.png"));
			
			if(dynamic_cast<const ScenarioTemplate*>(_scenario.get()))
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
		
		void MessagesScenarioAdmin::setScenario(boost::shared_ptr<Scenario> value)
		{
			_scenario = const_pointer_cast<const Scenario>(value);
		}
	
		bool MessagesScenarioAdmin::_hasSameContent(const AdminInterfaceElement& other) const
		{
			const MessagesScenarioAdmin& mother(static_cast<const MessagesScenarioAdmin&>(other));
			return _scenario && mother._scenario && _scenario->getKey() == mother._scenario->getKey();
		}
	}
}

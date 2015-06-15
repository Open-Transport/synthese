////////////////////////////////////////////////////////////////////////////////
/// MessagesScenarioAdmin class implementation.
///	@file MessagesScenarioAdmin.cpp
///	@author Hugues Romain
///
///	This file belongs to the SYNTHESE project (public transportation specialized
///	software)
///	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>
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

#include "MessagesScenarioAdmin.h"

#include "ActionResultHTMLTable.h"
#include "Profile.h"
#include "PropertiesHTMLTable.h"
#include "User.h"
#include "MessagesAdmin.h"
#include "MessageAdmin.h"
#include "Scenario.h"
#include "ScenarioTemplate.h"
#include "SentScenario.h"
#include "SentScenarioTableSync.h"
#include "ScenarioTemplateTableSync.h"
#include "AlarmTableSync.h"
#include "ScenarioSaveAction.h"
#include "NewMessageAction.h"
#include "MessagesLibraryAdmin.h"
#include "MessagesModule.h"
#include "MessagesRight.h"
#include "MessagesLibraryRight.h"
#include "ScenarioTableSync.h"
#include "StaticActionFunctionRequest.h"
#include "AdminFunctionRequest.hpp"
#include "AdminActionFunctionRequest.hpp"
#include "AdminParametersException.h"
#include "AdminInterfaceElement.h"
#include "ActionException.h"
#include "MessagesLog.h"
#include "MessagesLibraryLog.h"
#include "ScenarioFolder.h"
#include "RemoveObjectAction.hpp"
#include "ImportableAdmin.hpp"
#include "StaticActionRequest.h"

#include <boost/foreach.hpp>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace admin;
	using namespace server;
	using namespace util;
	using namespace html;
	using namespace messages;
	using namespace security;
	using namespace db;
	using namespace impex;

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
		const string MessagesScenarioAdmin::TAB_LOG("l");
		const string MessagesScenarioAdmin::TAB_DATASOURCES("s");

		void MessagesScenarioAdmin::setFromParametersMap(
			const ParametersMap& map
		){
			util::RegistryKeyType id(map.get<RegistryKeyType>(Request::PARAMETER_OBJECT_ID));
			util::RegistryTableType tableId(util::decodeTableId(id));
			try
			{
				if (tableId == ScenarioTemplateTableSync::TABLE.ID)
				{
					_scenario = ScenarioTemplateTableSync::Get(
						id,
						_getEnv(),
						UP_LINKS_LOAD_LEVEL
						);
					_generalLogView.set(map, MessagesLibraryLog::FACTORY_KEY, _scenario->getKey());
				}
				else if (tableId == SentScenarioTableSync::TABLE.ID)
				{
					_scenario = SentScenarioTableSync::Get(
						id,
						_getEnv(),
						UP_LINKS_LOAD_LEVEL
						);
					_generalLogView.set(map, MessagesLog::FACTORY_KEY, _scenario->getKey());
				}
			}
			catch(...)
			{
				throw AdminParametersException("Specified scenario not found");
			}
		}



		util::ParametersMap MessagesScenarioAdmin::getParametersMap() const
		{
			ParametersMap m(_generalLogView.getParametersMap());
			if(_scenario.get())
				m.insert(Request::PARAMETER_OBJECT_ID, _scenario->getKey());
			return m;
		}



		void MessagesScenarioAdmin::display(
			ostream& stream,
			const server::Request& _request
		) const	{

			boost::shared_ptr<const SentScenario> _sentScenario = dynamic_pointer_cast<const SentScenario>(_scenario);
			const ScenarioTemplate* _templateScenario = dynamic_cast<const ScenarioTemplate*>(_scenario.get());


			////////////////////////////////////////////////////////////////////
			// TAB PARAMETERS
			if (openTabContent(stream, TAB_PARAMETERS))
			{
				AdminActionFunctionRequest<ScenarioSaveAction, MessagesScenarioAdmin> updateDatesRequest(_request, *this);
				updateDatesRequest.getAction()->setScenarioId(_scenario->getKey());

				stream << "<h1>Paramètres</h1>";
				PropertiesHTMLTable udt(updateDatesRequest.getHTMLForm("update_dates"));

				stream << udt.open();
				stream << udt.title("Propriétés");
				stream << udt.cell("Nom", udt.getForm().getTextInput(ScenarioSaveAction::PARAMETER_NAME, _scenario->getName()));
				if (_templateScenario)
				{
					stream << udt.cell(
						"Répertoire",
						udt.getForm().getSelectInput(
							ScenarioSaveAction::PARAMETER_FOLDER_ID,
							MessagesModule::GetScenarioFoldersLabels(),
							optional<RegistryKeyType>(_templateScenario->getFolder() ? _templateScenario->getFolder()->getKey() : 0)
					)	);
				}
				if(_sentScenario.get())
				{
					stream << udt.title("Diffusion");
					stream << udt.cell("Début diffusion", udt.getForm().getCalendarInput(ScenarioSaveAction::PARAMETER_START_DATE, _sentScenario->getPeriodStart()));
					stream << udt.cell("Fin diffusion", udt.getForm().getCalendarInput(ScenarioSaveAction::PARAMETER_END_DATE, _sentScenario->getPeriodEnd()));

					stream << udt.cell("Actif", udt.getForm().getOuiNonRadioInput(ScenarioSaveAction::PARAMETER_ENABLED, _sentScenario->getIsEnabled()));

				}
				stream << udt.close();
			}


			////////////////////////////////////////////////////////////////////
			// TAB MESSAGES
			if (openTabContent(stream, TAB_MESSAGES))
			{
				AdminActionFunctionRequest<RemoveObjectAction,MessagesScenarioAdmin> deleteRequest(_request, *this);

				AdminActionFunctionRequest<NewMessageAction,MessageAdmin> addRequest(_request);
				addRequest.setActionWillCreateObject();
				addRequest.getAction()->setScenarioId(_scenario->getKey());

				stream << "<h1>Messages</h1>";


				Alarms v;
				if (_sentScenario.get())
				{
					AlarmTableSync::Search(*_env, std::back_inserter(v), _sentScenario->getKey());
				}
				else
				{
					AlarmTableSync::Search(*_env, std::back_inserter(v), _templateScenario->getKey());
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
				BOOST_FOREACH(const boost::shared_ptr<Alarm>& alarm, v)
				{
					messRequest.getPage()->setMessage(alarm);
					deleteRequest.getAction()->setObjectId(alarm->getKey());

					stream << t.row(lexical_cast<string>(alarm->getKey()));
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
			// TAB LOG
			if (openTabContent(stream, TAB_LOG))
			{
				// Log search
				_generalLogView.display(stream, _request);
			}

			////////////////////////////////////////////////////////////////////
			// TAB DATASOURCES
			if (openTabContent(stream, TAB_DATASOURCES))
			{
				// Source id
				StaticActionRequest<ScenarioSaveAction> updateOnlyRequest(_request);
				updateOnlyRequest.getAction()->setScenarioId(_scenario->getKey());
				ImportableAdmin::DisplayDataSourcesTab(stream, static_cast<const SentScenario&>(*_scenario), updateOnlyRequest);
			}

			closeTabContent(stream);
		}

		bool MessagesScenarioAdmin::isAuthorized(
			const security::User& user
		) const	{
			if (_scenario.get() == NULL) return false;
			if (dynamic_pointer_cast<const SentScenario, const Scenario>(_scenario).get() != NULL)
				return user.getProfile()->isAuthorized<MessagesRight>(READ);
			return user.getProfile()->isAuthorized<MessagesLibraryRight>(READ);
		}



		MessagesScenarioAdmin::MessagesScenarioAdmin(
		):	AdminInterfaceElementTemplate<MessagesScenarioAdmin>(),
			_generalLogView("g")
		{
		}




		AdminInterfaceElement::PageLinks MessagesScenarioAdmin::getSubPages(
			const AdminInterfaceElement& currentPage,
			const server::Request& request
		) const {
			AdminInterfaceElement::PageLinks links;

			Alarms alarms;
			AlarmTableSync::Search(
					*_env,
					std::back_inserter(alarms),
					_scenario->getKey());
			BOOST_FOREACH(const boost::shared_ptr<Alarm>& alarm, alarms)
			{
				boost::shared_ptr<MessageAdmin> p(
					getNewPage<MessageAdmin>()
				);
				p->setMessage(alarm);
				links.push_back(p);
			}

			return links;
		}


		std::string MessagesScenarioAdmin::getTitle() const
		{
			return _scenario.get() && !_scenario->getName().empty() ? _scenario->getName() : DEFAULT_TITLE;
		}




		void MessagesScenarioAdmin::_buildTabs(
			const security::Profile& profile
		) const {
			_tabs.clear();

			_tabs.push_back(Tab("Paramètres", TAB_PARAMETERS, true, "table.png"));
			_tabs.push_back(Tab("Messages", TAB_MESSAGES, true, "note.png"));
			_tabs.push_back(Tab("Journal", TAB_LOG, true, "book.png"));

			if(dynamic_cast<const SentScenario*>(_scenario.get()))
			{
				_tabs.push_back(Tab("Sources de données", TAB_DATASOURCES, true));
			}

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
			return _scenario.get() && mother._scenario.get() && _scenario->getKey() == mother._scenario->getKey();
		}
	}
}

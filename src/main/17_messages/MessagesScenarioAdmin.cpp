
/** MessagesScenarioAdmin class implementation.
	@file MessagesScenarioAdmin.cpp

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

#include "05_html/ActionResultHTMLTable.h"
#include "05_html/HTMLForm.h"

#include "17_messages/MessagesScenarioAdmin.h"
#include "17_messages/MessageAdmin.h"
#include "17_messages/Scenario.h"
#include "17_messages/ScenarioTemplate.h"
#include "17_messages/SentScenario.h"
#include "17_messages/ScenarioTableSync.h"
#include "17_messages/AlarmTableSync.h"
#include "17_messages/ScenarioNameUpdateAction.h"
#include "17_messages/ScenarioUpdateDatesAction.h"
#include "17_messages/DeleteAlarmAction.h"
#include "17_messages/NewMessageAction.h"
#include "17_messages/MessagesLibraryAdmin.h"

#include "30_server/ActionFunctionRequest.h"
#include "30_server/QueryString.h"

#include "32_admin/AdminParametersException.h"
#include "32_admin/AdminRequest.h"

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

	namespace util
	{
		template<> const string FactorableTemplate<AdminInterfaceElement,MessagesScenarioAdmin>::FACTORY_KEY("messagesscenario");
	}

	namespace admin
	{
		template<> const string AdminInterfaceElementTemplate<MessagesScenarioAdmin>::ICON("cog.png");
		template<> const AdminInterfaceElement::DisplayMode AdminInterfaceElementTemplate<MessagesScenarioAdmin>::DISPLAY_MODE(AdminInterfaceElement::DISPLAYED_IF_CURRENT);
		template<> string AdminInterfaceElementTemplate<MessagesScenarioAdmin>::getSuperior()
		{
//			if (_sentScenario.get())
//				_setSuperior("messages");
			return MessagesLibraryAdmin::FACTORY_KEY;
		}
	}

	namespace messages
	{
		void MessagesScenarioAdmin::setFromParametersMap(const ParametersMap& map)
		{
			uid id(map.getUid(QueryString::PARAMETER_OBJECT_ID, true, FACTORY_KEY));
			if (id == QueryString::UID_WILL_BE_GENERATED_BY_THE_ACTION)
				return;

			shared_ptr<const Scenario> scenario;
			try
			{
				scenario = ScenarioTableSync::getScenario(id);
			}
			catch(...)
			{
				throw AdminParametersException("Specified scenario not found");
			}

			_sentScenario = dynamic_pointer_cast<const SentScenario, const Scenario>(scenario);
			_templateScenario = dynamic_pointer_cast<const ScenarioTemplate, const Scenario>(scenario);
			_scenario = scenario.get();
		}

		string MessagesScenarioAdmin::getTitle() const
		{
			return _scenario->getName();
		}

		void MessagesScenarioAdmin::display(ostream& stream, interfaces::VariablesMap& variables, const server::FunctionRequest<admin::AdminRequest>* request) const
		{
			ActionFunctionRequest<ScenarioNameUpdateAction,AdminRequest> updateRequest(request);
			updateRequest.getFunction()->setPage<MessagesScenarioAdmin>();
			updateRequest.setObjectId(_scenario->getId());

			FunctionRequest<AdminRequest> messRequest(request);
			messRequest.getFunction()->setPage<MessageAdmin>();

			ActionFunctionRequest<DeleteAlarmAction,AdminRequest> deleteRequest(request);
			deleteRequest.getFunction()->setPage<MessagesScenarioAdmin>();
			deleteRequest.setObjectId(_scenario->getId());

			ActionFunctionRequest<NewMessageAction,AdminRequest> addRequest(request);
			addRequest.getFunction()->setPage<MessageAdmin>();
			addRequest.getAction()->setScenarioId(_scenario->getId());
			addRequest.getAction()->setIsTemplate(_templateScenario.get() != NULL);

			stream << "<h1>Propriété</h1>";
			HTMLForm uf(updateRequest.getHTMLForm("update"));
			stream << uf.open();
			stream << "<p>Nom : " << uf.getTextInput(ScenarioNameUpdateAction::PARAMETER_NAME, _scenario->getName()) << uf.getSubmitButton("Modifier") << "</p>";
			stream << uf.close();

			if (_sentScenario.get())
			{
				ActionFunctionRequest<ScenarioUpdateDatesAction, AdminRequest> updateDatesRequest(request);
				updateDatesRequest.getFunction()->setPage<MessagesScenarioAdmin>();
				updateDatesRequest.setObjectId(_scenario->getId());

				stream << "<h1>Diffusion</h1>";
				HTMLForm udf(updateDatesRequest.getHTMLForm("update_dates"));
				HTMLTable udt;

				stream << udf.open() << udt.open();

				stream << udt.row();
				stream << udt.col() << "Début diffusion";
				stream << udt.col() << udf.getCalendarInput(ScenarioUpdateDatesAction::PARAMETER_START_DATE, _sentScenario->getPeriodStart());
				stream << udt.row();
				stream << udt.col() << "Fin diffusion";
				stream << udt.col() << udf.getCalendarInput(ScenarioUpdateDatesAction::PARAMETER_END_DATE, _sentScenario->getPeriodEnd());
				stream << udt.row();
				stream << udt.col() << "Actif";
				stream << udt.col() << udf.getOuiNonRadioInput(ScenarioUpdateDatesAction::PARAMETER_ENABLED, _sentScenario->getIsEnabled());

				stream << udt.row();
				stream << udt.col(2) << udf.getSubmitButton("Enregistrer");

				stream << udt.close() << udf.close();
			}

			stream << "<h1>Messages</h1>";

			vector<shared_ptr<Alarm> > v;
			
			if (_sentScenario.get())
			{
				vector<shared_ptr<ScenarioSentAlarm> > vs = AlarmTableSync::searchScenarioSent(_sentScenario.get());
				for (vector<shared_ptr<ScenarioSentAlarm> >::const_iterator it = vs.begin(); it != vs.end(); ++it)
					v.push_back(static_pointer_cast<Alarm, ScenarioSentAlarm>(*it));
			}
			else
			{
				vector<shared_ptr<AlarmTemplate> > vs = AlarmTableSync::searchTemplates(_templateScenario.get());
				for (vector<shared_ptr<AlarmTemplate> >::const_iterator it = vs.begin(); it != vs.end(); ++it)
					v.push_back(static_pointer_cast<Alarm, AlarmTemplate>(*it));
			}

			ActionResultHTMLTable::HeaderVector h;
			h.push_back(make_pair(string(), "Message"));
			h.push_back(make_pair(string(), "Emplacement"));
			h.push_back(make_pair(string(), "Actions"));
			ActionResultHTMLTable t(h, HTMLForm(string(), string()), ActionResultHTMLTable::RequestParameters(), ActionResultHTMLTable::ResultParameters(), addRequest.getHTMLForm("add"), NewMessageAction::PARAMETER_IS_TEMPLATE);

			stream << t.open();

			for(vector<shared_ptr<Alarm> >::const_iterator it = v.begin(); it != v.end(); ++it)
			{
				shared_ptr<const Alarm> alarm = *it;
				messRequest.setObjectId(alarm->getId());
				deleteRequest.getFunction()->setParameter(DeleteAlarmAction::PARAMETER_ALARM, Conversion::ToString(alarm->getId()));
				stream << t.row(Conversion::ToString(alarm->getId()));
				stream << t.col() << alarm->getShortMessage();
				stream << t.col() << ""; // Emplacement
				stream << t.col() << HTMLModule::getLinkButton(messRequest.getURL(), "Modifier")
					<< "&nbsp;" << HTMLModule::getLinkButton(deleteRequest.getURL(), "Supprimer", "Etes-vous sûr de vouloir supprimer le message du scénario ?");
			}

			stream << t.row();
			stream << t.col(2) << "(sélectionnez un&nbsp;message existant pour créer une copie)";
			stream << t.col() << t.getActionForm().getSubmitButton("Ajouter");
			stream << t.close();
		}

		bool MessagesScenarioAdmin::isAuthorized( const server::FunctionRequest<admin::AdminRequest>* request ) const
		{
			return true;
		}

		MessagesScenarioAdmin::MessagesScenarioAdmin()
			: AdminInterfaceElementTemplate<MessagesScenarioAdmin>()
		{

		}
	}
}

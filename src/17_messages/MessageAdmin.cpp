
/** MessageAdmin class implementation.
	@file MessageAdmin.cpp

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

#include "05_html/PropertiesHTMLTable.h"
#include "05_html/HTMLTable.h"

#include "17_messages/MessageAdmin.h"
#include "17_messages/MessagesModule.h"
#include "17_messages/Types.h"
#include "17_messages/AlarmRecipient.h"
#include "17_messages/UpdateAlarmAction.h"
#include "17_messages/UpdateAlarmMessagesFromTemplateAction.h"
#include "17_messages/UpdateAlarmMessagesAction.h"
#include "17_messages/AlarmAddLinkAction.h"
#include "17_messages/AlarmRemoveLinkAction.h"
#include "17_messages/SingleSentAlarm.h"
#include "17_messages/ScenarioSentAlarm.h"
#include "17_messages/ScenarioTemplate.h"
#include "17_messages/SentScenario.h"
#include "17_messages/AlarmTemplate.h"
#include "17_messages/AlarmTableSync.h"
#include "17_messages/MessagesAdmin.h"
#include "17_messages/MessagesLibraryAdmin.h"
#include "17_messages/MessagesScenarioAdmin.h"

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
		template<> const string FactorableTemplate<AdminInterfaceElement,MessageAdmin>::FACTORY_KEY("message");
	}

	namespace admin
	{
		template<> const string AdminInterfaceElementTemplate<MessageAdmin>::ICON("note.png");
		template<> const string AdminInterfaceElementTemplate<MessageAdmin>::DEFAULT_TITLE("Message inconnu");
	}

	namespace messages
	{
		void MessageAdmin::setFromParametersMap( const ParametersMap& map )
		{
			uid id(map.getUid(QueryString::PARAMETER_OBJECT_ID, true, FACTORY_KEY));

			if (id == QueryString::UID_WILL_BE_GENERATED_BY_THE_ACTION)
				return;

			try
			{
				_alarm.reset(AlarmTableSync::Get(id, true));
			}
			catch(...)
			{
				throw AdminParametersException("Specified alarm not found");
			}

			_parameters = map;
		}

		void MessageAdmin::display(ostream& stream, interfaces::VariablesMap& variables, const server::FunctionRequest<admin::AdminRequest>* request) const
		{
			ActionFunctionRequest<UpdateAlarmAction,AdminRequest> updateRequest(request);
			updateRequest.getFunction()->setPage<MessageAdmin>();
			updateRequest.setObjectId(request->getObjectId());

			shared_ptr<const SingleSentAlarm> salarm = dynamic_pointer_cast<const SingleSentAlarm, const Alarm>(_alarm);
			
			stream << "<h1>Paramètres</h1>";
			PropertiesHTMLTable t(updateRequest.getHTMLForm("update"));
			
			stream << t.open();
			stream << t.cell("Type", t.getForm().getRadioInput(UpdateAlarmAction::PARAMETER_TYPE, MessagesModule::getLevelLabels(), _alarm->getLevel()));

			if (salarm.get())
			{
				stream << t.cell("Début diffusion", t.getForm().getCalendarInput(UpdateAlarmAction::PARAMETER_START_DATE, salarm->getPeriodStart()));
				stream << t.cell("Fin diffusion", t.getForm().getCalendarInput(UpdateAlarmAction::PARAMETER_END_DATE, salarm->getPeriodEnd()));
				stream << t.cell("Actif", t.getForm().getOuiNonRadioInput(UpdateAlarmAction::PARAMETER_ENABLED, salarm->getIsEnabled()));
			}
			stream << t.close();

			if (_alarm->getLevel() != ALARM_LEVEL_UNKNOWN)
			{
				stream << "<h1>Contenu</h1>";

				ActionFunctionRequest<UpdateAlarmMessagesFromTemplateAction,AdminRequest> templateRequest(request);
				templateRequest.getFunction()->setPage<MessageAdmin>();
				templateRequest.setObjectId(request->getObjectId());

				HTMLForm fc(templateRequest.getHTMLForm("template"));
				stream << fc.open() << "<p>";
				stream << "Modèle : ";
				stream << fc.getSelectInput(UpdateAlarmMessagesFromTemplateAction::PARAMETER_TEMPLATE_ID, MessagesModule::getTextTemplateLabels(_alarm->getLevel()), uid());
				stream << fc.getSubmitButton("Copier contenu");
				stream << "</p>" << fc.close();

				ActionFunctionRequest<UpdateAlarmMessagesAction,AdminRequest> updateMessagesRequest(request);
				updateMessagesRequest.getFunction()->setPage<MessageAdmin>();
				updateMessagesRequest.setObjectId(request->getObjectId());

				PropertiesHTMLTable tu(updateMessagesRequest.getHTMLForm("messages"));
				stream << tu.open();
				stream << tu.cell("Message court", tu.getForm().getTextAreaInput(UpdateAlarmMessagesAction::PARAMETER_SHORT_MESSAGE, _alarm->getShortMessage(), 2, 60));
				stream << tu.cell("Message long", tu.getForm().getTextAreaInput(UpdateAlarmMessagesAction::PARAMETER_LONG_MESSAGE, _alarm->getLongMessage(), 6, 60));
				stream << tu.close();

				FunctionRequest<AdminRequest> searchRequest(request);
				searchRequest.getFunction()->setPage<MessageAdmin>();
				searchRequest.setObjectId(request->getObjectId());

				ActionFunctionRequest<AlarmAddLinkAction,AdminRequest> addRequest(request);
				addRequest.getFunction()->setPage<MessageAdmin>();
				addRequest.setObjectId(request->getObjectId());
				addRequest.getAction()->setAlarm(_alarm);

				ActionFunctionRequest<AlarmRemoveLinkAction,AdminRequest> removeRequest(request);
				removeRequest.getFunction()->setPage<MessageAdmin>();
				removeRequest.setObjectId(request->getObjectId());
				removeRequest.getAction()->setAlarmId(_alarm->getId());
				
				// Alarm messages destinations loop
				for (Factory<AlarmRecipient>::Iterator arit = Factory<AlarmRecipient>::begin(); arit != Factory<AlarmRecipient>::end(); ++arit)
				{
					addRequest.getAction()->setRecipientKey(arit.getKey());
				
					stream << "<h1>Diffusion sur " << arit->getTitle() << "</h1>";

					arit->displayBroadcastListEditor(stream, _alarm.get(), _parameters, searchRequest, addRequest, removeRequest);
				}
			}
		}

		bool MessageAdmin::isAuthorized( const server::FunctionRequest<admin::AdminRequest>* request ) const
		{
			return true;
		}

		MessageAdmin::MessageAdmin()
			: AdminInterfaceElementTemplate<MessageAdmin>()
		{
		
		}

		AdminInterfaceElement::PageLinks MessageAdmin::getSubPagesOfParent( const PageLink& parentLink , const AdminInterfaceElement& currentPage 		, const server::FunctionRequest<admin::AdminRequest>* request
			) const
		{
			AdminInterfaceElement::PageLinks links;
			if (currentPage.getFactoryKey() == FACTORY_KEY)
			{
				const MessageAdmin& currentSPage(static_cast<const MessageAdmin&>(currentPage));
				shared_ptr<const SingleSentAlarm> alarm(dynamic_pointer_cast<const SingleSentAlarm, const Alarm>(currentSPage._alarm));
				if (alarm.get())
				{
					if (parentLink.factoryKey == MessagesAdmin::FACTORY_KEY)
						links.push_back(currentPage.getPageLink());
				}
			}
			return links;
		}

		AdminInterfaceElement::PageLinks MessageAdmin::getSubPages( const AdminInterfaceElement& currentPage, const server::FunctionRequest<admin::AdminRequest>* request ) const
		{
			return AdminInterfaceElement::PageLinks();
		}

		boost::shared_ptr<const Alarm> MessageAdmin::getAlarm() const
		{
			return _alarm;
		}

		std::string MessageAdmin::getTitle() const
		{
			return _alarm.get() ? _alarm->getShortMessage() : DEFAULT_TITLE;
		}

		std::string MessageAdmin::getParameterName() const
		{
			return _alarm.get() ? QueryString::PARAMETER_OBJECT_ID : string();
		}

		std::string MessageAdmin::getParameterValue() const
		{
			return _alarm.get() ? Conversion::ToString(_alarm->getId()) : string();
		}

		bool MessageAdmin::isPageVisibleInTree( const AdminInterfaceElement& currentPage ) const
		{
			if (currentPage.getFactoryKey() != MessagesScenarioAdmin::FACTORY_KEY)
				return false;

			const shared_ptr<const ScenarioSentAlarm> salarm(dynamic_pointer_cast<const ScenarioSentAlarm, const Alarm>(_alarm));
			if (salarm.get())
				return salarm->getScenario()->getKey() == Conversion::ToLongLong(currentPage.getPageLink().parameterValue);

			const shared_ptr<const AlarmTemplate> talarm(dynamic_pointer_cast<const AlarmTemplate, const Alarm>(_alarm));
			if (talarm.get())
				return talarm->getScenario()->getKey() == Conversion::ToLongLong(currentPage.getPageLink().parameterValue);

			return false;
		}
	}
}

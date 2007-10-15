
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

#include "05_html/HTMLForm.h"
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
#include "17_messages/AlarmTemplate.h"
#include "17_messages/AlarmTableSync.h"
#include "17_messages/MessagesAdmin.h"
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
		template<> const string FactorableTemplate<AdminInterfaceElement,MessageAdmin>::FACTORY_KEY("message");
	}

	namespace admin
	{
		template<> const AdminInterfaceElement::DisplayMode AdminInterfaceElementTemplate<MessageAdmin>::DISPLAY_MODE(AdminInterfaceElement::DISPLAYED_IF_CURRENT);
		template<> const string AdminInterfaceElementTemplate<MessageAdmin>::ICON("note.png");
		template<> string AdminInterfaceElementTemplate<MessageAdmin>::getSuperior()
		{
//			if (dynamic_pointer_cast<const AlarmTemplate, const Alarm>(_alarm).get())
//				return MessagesLibraryAdmin::FACTORY_KEY;
			return MessagesAdmin::FACTORY_KEY;
		}
	}

	namespace messages
	{
		string MessageAdmin::getTitle() const
		{
			return _alarm->getShortMessage();
		}

		void MessageAdmin::setFromParametersMap( const ParametersMap& map )
		{
			uid id(map.getUid(QueryString::PARAMETER_OBJECT_ID, true, FACTORY_KEY));

			if (id == QueryString::UID_WILL_BE_GENERATED_BY_THE_ACTION)
				return;

			try
			{
				_alarm = AlarmTableSync::getAlarm(id);
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
			HTMLForm f(updateRequest.getHTMLForm("update"));
			HTMLTable t;

			stream << f.open() << t.open();
			stream << t.row();
			stream << t.col() << "Type";
			stream << t.col() << f.getRadioInput(UpdateAlarmAction::PARAMETER_TYPE, MessagesModule::getLevelLabels(), _alarm->getLevel());

			if (salarm.get())
			{
				stream << t.row();
				stream << t.col() << "Début diffusion";
				stream << t.col() << f.getCalendarInput(UpdateAlarmAction::PARAMETER_START_DATE, salarm->getPeriodStart());
				stream << t.row();
				stream << t.col() << "Fin diffusion";
				stream << t.col() << f.getCalendarInput(UpdateAlarmAction::PARAMETER_END_DATE, salarm->getPeriodEnd());
				stream << t.row();
				stream << t.col() << "Actif";
				stream << t.col() << f.getOuiNonRadioInput(UpdateAlarmAction::PARAMETER_ENABLED, salarm->getIsEnabled());
			}
			stream << t.row();
			stream << t.col(2) << f.getSubmitButton("Enregistrer");
			stream << t.close() << f.close();

			if (_alarm->getLevel() != ALARM_LEVEL_UNKNOWN)
			{
				stream << "<h1>Contenu</h1>";

				ActionFunctionRequest<UpdateAlarmMessagesFromTemplateAction,AdminRequest> templateRequest(request);
				templateRequest.getFunction()->setPage(Factory<AdminInterfaceElement>::createSharedPtr<MessageAdmin>());
				templateRequest.setObjectId(request->getObjectId());

				HTMLForm fc(templateRequest.getHTMLForm("template"));
				HTMLTable tc;
				stream << fc.open() << tc.open();
				stream << tc.row();
				stream << tc.col() << "Modèle";
				stream << tc.col() << fc.getSelectInput(UpdateAlarmMessagesFromTemplateAction::PARAMETER_TEMPLATE_ID, MessagesModule::getTextTemplateLabels(_alarm->getLevel()), uid());
				stream << fc.getSubmitButton("Copier contenu");
				stream << tc.close() << fc.close();

				ActionFunctionRequest<UpdateAlarmMessagesAction,AdminRequest> updateMessagesRequest(request);
				updateMessagesRequest.getFunction()->setPage(Factory<AdminInterfaceElement>::createSharedPtr<MessageAdmin>());
				updateMessagesRequest.setObjectId(request->getObjectId());

				HTMLForm fu(updateMessagesRequest.getHTMLForm("messages"));
				HTMLTable tu;
				stream << fu.open() << tu.open();
				stream << tu.row();
				stream << tu.col() << "Message court";
				stream << tu.col() << fu.getTextAreaInput(UpdateAlarmMessagesAction::PARAMETER_SHORT_MESSAGE, _alarm->getShortMessage(), 2, 20);
				stream << tu.row();
				stream << tu.col() << "Message long";
				stream << tu.col() << fu.getTextAreaInput(UpdateAlarmMessagesAction::PARAMETER_LONG_MESSAGE, _alarm->getLongMessage(), 4, 30);
				stream << tu.row();
				stream << tu.col(2) << fu.getSubmitButton("Enregistrer");
				stream << tu.close() << fu.close();

				FunctionRequest<AdminRequest> searchRequest(request);
				searchRequest.getFunction()->setPage(Factory<AdminInterfaceElement>::createSharedPtr<MessageAdmin>());
				searchRequest.setObjectId(request->getObjectId());

				ActionFunctionRequest<AlarmAddLinkAction,AdminRequest> addRequest(request);
				addRequest.getFunction()->setPage(Factory<AdminInterfaceElement>::createSharedPtr<MessageAdmin>());
				addRequest.setObjectId(request->getObjectId());
				addRequest.getAction()->setAlarm(_alarm);

				ActionFunctionRequest<AlarmRemoveLinkAction,AdminRequest> removeRequest(request);
				removeRequest.getFunction()->setPage(Factory<AdminInterfaceElement>::createSharedPtr<MessageAdmin>());
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
	}
}

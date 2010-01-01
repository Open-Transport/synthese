////////////////////////////////////////////////////////////////////////////////
/// MessageAdmin class implementation.
///	@file MessageAdmin.cpp
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

#include "PropertiesHTMLTable.h"
#include "HTMLTable.h"
#include "MessageAdmin.h"
#include "MessagesModule.h"
#include "17_messages/Types.h"
#include "AlarmRecipient.h"
#include "UpdateAlarmMessagesFromTemplateAction.h"
#include "UpdateAlarmMessagesAction.h"
#include "AlarmAddLinkAction.h"
#include "AlarmRemoveLinkAction.h"
#include "AlarmTemplate.h"
#include "SentAlarm.h"
#include "ScenarioTemplate.h"
#include "SentScenario.h"
#include "AlarmTableSync.h"
#include "MessagesAdmin.h"
#include "MessagesLibraryAdmin.h"
#include "MessagesScenarioAdmin.h"
#include "MessagesRight.h"
#include "MessagesLibraryRight.h"
#include "AdminActionFunctionRequest.hpp"
#include "AdminFunctionRequest.hpp"
#include "AdminParametersException.h"
#include "AdminInterfaceElement.h"
#include "ActionException.h"

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
		template<> const string FactorableTemplate<AdminInterfaceElement,MessageAdmin>::FACTORY_KEY("message");
	}

	namespace admin
	{
		template<> const string AdminInterfaceElementTemplate<MessageAdmin>::ICON("note.png");
		template<> const string AdminInterfaceElementTemplate<MessageAdmin>::DEFAULT_TITLE("(pas de texte)");
	}

	namespace messages
	{
		const string MessageAdmin::TAB_PARAMS("tp");

		void MessageAdmin::setFromParametersMap(
			const ParametersMap& map
		){
			try
			{
				_alarm = AlarmTableSync::Get(
					map.get<RegistryKeyType>(Request::PARAMETER_OBJECT_ID),
					_getEnv(),
					UP_LINKS_LOAD_LEVEL
				);
			}
			catch(...)
			{
				throw AdminParametersException("Specified alarm not found");
			}

			_parameters = map;
		}
		
		


		server::ParametersMap MessageAdmin::getParametersMap() const
		{
			ParametersMap m;
			if(_alarm.get())
				m.insert(Request::PARAMETER_OBJECT_ID, _alarm->getKey());
			return m;
		}



		void MessageAdmin::display(ostream& stream, interfaces::VariablesMap& variables,
					const admin::AdminRequest& _request) const
		{
			////////////////////////////////////////////////////////////////////
			// TAB PARAMETERS
			if (openTabContent(stream, TAB_PARAMS))
			{
				stream << "<h1>Contenu</h1>";

				AdminActionFunctionRequest<UpdateAlarmMessagesFromTemplateAction,MessageAdmin> templateRequest(_request);
				templateRequest.getAction()->setAlarmId(_alarm->getKey());

				vector<pair<uid, string> > tl(MessagesModule::getTextTemplateLabels(_alarm->getLevel()));
				if(!tl.empty())
				{
					HTMLForm fc(templateRequest.getHTMLForm("template"));
					stream << fc.open() << "<p>";
					stream << "Modèle : ";
					stream << fc.getSelectInput(UpdateAlarmMessagesFromTemplateAction::PARAMETER_TEMPLATE_ID, tl, uid());
					stream << fc.getSubmitButton("Copier contenu");
					stream << "</p>" << fc.close();
				}

				AdminActionFunctionRequest<UpdateAlarmMessagesAction,MessageAdmin> updateMessagesRequest(_request);
				updateMessagesRequest.getAction()->setAlarmId(_alarm->getKey());

				PropertiesHTMLTable tu(updateMessagesRequest.getHTMLForm("messages"));
				stream << tu.open();
				stream << tu.cell("Type", tu.getForm().getRadioInputCollection(UpdateAlarmMessagesAction::PARAMETER_TYPE, MessagesModule::getLevelLabels(), _alarm->getLevel()));
				stream << tu.cell("Message court", tu.getForm().getTextAreaInput(UpdateAlarmMessagesAction::PARAMETER_SHORT_MESSAGE, _alarm->getShortMessage(), 2, 60));
				stream << tu.cell("Message long", tu.getForm().getTextAreaInput(UpdateAlarmMessagesAction::PARAMETER_LONG_MESSAGE, _alarm->getLongMessage(), 6, 60));
				stream << tu.close();
			}

			// Alarm messages destinations loop
			vector<shared_ptr<AlarmRecipient> > recipients(Factory<AlarmRecipient>::GetNewCollection());
			BOOST_FOREACH(shared_ptr<AlarmRecipient> recipient, recipients)
			{
				////////////////////////////////////////////////////////////////////
				// TAB STOPS
				if (openTabContent(stream, recipient->getFactoryKey()))
				{
					AdminActionFunctionRequest<AlarmAddLinkAction,MessageAdmin> addRequest(_request);
					addRequest.getAction()->setAlarmId(_alarm->getKey());
					addRequest.getAction()->setRecipientKey(recipient->getFactoryKey());

					AdminActionFunctionRequest<AlarmRemoveLinkAction,MessageAdmin> removeRequest(_request);
					removeRequest.getAction()->setAlarmId(_alarm->getKey());

					AdminFunctionRequest<MessageAdmin> searchRequest(_request);
					recipient->displayBroadcastListEditor(stream, _alarm.get(), _parameters, searchRequest, addRequest, removeRequest);
				}
			}
			

			////////////////////////////////////////////////////////////////////
			// END TABS
			closeTabContent(stream);
		}



		void MessageAdmin::_buildTabs(
			const admin::AdminRequest& _request
		) const {
			_tabs.clear();

			_tabs.push_back(Tab("Contenu", TAB_PARAMS, true));

			// Alarm messages destinations loop
			vector<shared_ptr<AlarmRecipient> > recipients(Factory<AlarmRecipient>::GetNewCollection());
			BOOST_FOREACH(shared_ptr<AlarmRecipient> recipient, recipients)
			{
				_tabs.push_back(Tab(recipient->getTitle(), recipient->getFactoryKey(), true));
			}

			_tabBuilded = true;
		}

		bool MessageAdmin::isAuthorized(
			const security::Profile& profile
 		) const {
			if (_alarm.get() == NULL) return false;
			if (dynamic_pointer_cast<const AlarmTemplate, const Alarm>(_alarm).get() == NULL)
				return profile.isAuthorized<MessagesRight>(READ);
			return profile.isAuthorized<MessagesLibraryRight>(READ);
		}

		MessageAdmin::MessageAdmin()
			: AdminInterfaceElementTemplate<MessageAdmin>()
		{
		
		}


		boost::shared_ptr<const Alarm> MessageAdmin::getAlarm() const
		{
			return _alarm;
		}

		std::string MessageAdmin::getTitle() const
		{
			return _alarm.get() && !_alarm->getShortMessage().empty() ? _alarm->getShortMessage() : DEFAULT_TITLE;
		}


		bool MessageAdmin::isPageVisibleInTree(
			const AdminInterfaceElement& currentPage,
			const admin::AdminRequest& request
		) const {
			const MessagesScenarioAdmin* ma(
				dynamic_cast<const MessagesScenarioAdmin*>(&currentPage)
			);
			
			if(	!ma) return false;

			assert(_alarm->getScenario());
			if(!_alarm->getScenario())
				return false;

			return _alarm->getScenario()->getKey() == ma->getScenario()->getKey();
		}
		
		void MessageAdmin::setMessage(boost::shared_ptr<const Alarm> value)
		{
			_alarm = value;
		}
		
		
		bool MessageAdmin::_hasSameContent(const AdminInterfaceElement& other) const
		{
			const MessageAdmin& mother(static_cast<const MessageAdmin&>(other));
			return _alarm && mother._alarm && _alarm->getKey() == mother._alarm->getKey();
		}



	}
}

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
#include "ActionFunctionRequest.h"
#include "Request.h"
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
		template<> const string AdminInterfaceElementTemplate<MessageAdmin>::DEFAULT_TITLE("Message inconnu");
	}

	namespace messages
	{
		const string MessageAdmin::TAB_PARAMS("tp");

		void MessageAdmin::setFromParametersMap(
			const ParametersMap& map,
			bool doDisplayPreparationActions
		){
			uid id(map.getUid(Request::PARAMETER_OBJECT_ID, false, FACTORY_KEY));
			if (id == Request::UID_WILL_BE_GENERATED_BY_THE_ACTION)
				return;

			try
			{
				_alarm = AlarmTableSync::Get(id, _getEnv(), UP_LINKS_LOAD_LEVEL);
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



		void MessageAdmin::display(ostream& stream, interfaces::VariablesMap& variables) const
		{
			////////////////////////////////////////////////////////////////////
			// TAB PARAMETERS
			if (openTabContent(stream, TAB_PARAMS))
			{
				stream << "<h1>Contenu</h1>";

				ActionFunctionRequest<UpdateAlarmMessagesFromTemplateAction,AdminRequest> templateRequest(_request);
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

				ActionFunctionRequest<UpdateAlarmMessagesAction,AdminRequest> updateMessagesRequest(_request);
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
					ActionFunctionRequest<AlarmAddLinkAction,AdminRequest> addRequest(_request);
					addRequest.getAction()->setAlarmId(_alarm->getKey());
					addRequest.getAction()->setRecipientKey(recipient->getFactoryKey());

					ActionFunctionRequest<AlarmRemoveLinkAction,AdminRequest> removeRequest(_request);
					removeRequest.getAction()->setAlarmId(_alarm->getKey());

					recipient->displayBroadcastListEditor(stream, _alarm.get(), _parameters, FunctionRequest<AdminRequest>(_request), addRequest, removeRequest);
				}
			}
			

			////////////////////////////////////////////////////////////////////
			// END TABS
			closeTabContent(stream);
		}



		void MessageAdmin::_buildTabs(
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

		bool MessageAdmin::isAuthorized() const
		{
			if (_alarm.get() == NULL) return false;
			if (dynamic_pointer_cast<const AlarmTemplate, const Alarm>(_alarm).get() == NULL) return _request->isAuthorized<MessagesRight>(READ);
			return _request->isAuthorized<MessagesLibraryRight>(READ);
		}

		MessageAdmin::MessageAdmin()
			: AdminInterfaceElementTemplate<MessageAdmin>()
		{
		
		}

		AdminInterfaceElement::PageLinks MessageAdmin::getSubPagesOfParent( const PageLink& parentLink , const AdminInterfaceElement& currentPage
		) const	{
			AdminInterfaceElement::PageLinks links;
			return links;
		}

		AdminInterfaceElement::PageLinks MessageAdmin::getSubPages( const AdminInterfaceElement& currentPage
		) const	{
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
			return _alarm.get() ? Request::PARAMETER_OBJECT_ID : string();
		}

		std::string MessageAdmin::getParameterValue() const
		{
			return _alarm.get() ? Conversion::ToString(_alarm->getKey()) : string();
		}

		bool MessageAdmin::isPageVisibleInTree( const AdminInterfaceElement& currentPage ) const
		{
			if (currentPage.getFactoryKey() != MessagesScenarioAdmin::FACTORY_KEY)
				return false;

			assert(_alarm->getScenario());
			if(!_alarm->getScenario())
				return false;

			return _alarm->getScenario()->getKey() == Conversion::ToLongLong(currentPage.getPageLink().parameterValue);
		}
	}
}

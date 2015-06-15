////////////////////////////////////////////////////////////////////////////////
/// MessageAdmin class implementation.
///	@file MessageAdmin.cpp
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

#include "MessageAdmin.h"

#include "Profile.h"
#include "PropertiesHTMLTable.h"
#include "User.h"
#include "HTMLTable.h"
#include "MessagesModule.h"
#include "MessagesTypes.h"
#include "AlarmRecipient.h"
#include "UpdateAlarmMessagesFromTemplateAction.h"
#include "UpdateAlarmMessagesAction.h"
#include "AlarmAddLinkAction.h"
#include "AlarmRemoveLinkAction.h"
#include "Alarm.h"
#include "ScenarioTemplate.h"
#include "SentScenario.h"
#include "DBDirectTableSync.hpp"
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
#include "TinyMCE.hpp"
#include "ImportableAdmin.hpp"
#include "StaticActionRequest.h"

#include <boost/foreach.hpp>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace admin;
	using namespace server;
	using namespace impex;
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
		const string MessageAdmin::TAB_DATASOURCES("s");

		void MessageAdmin::setFromParametersMap(
			const ParametersMap& map
		){
			try
			{
				RegistryKeyType objectId(map.get<RegistryKeyType>(Request::PARAMETER_OBJECT_ID));
				boost::shared_ptr<db::DBDirectTableSync> tableSync =
					dynamic_pointer_cast<db::DBDirectTableSync, db::DBTableSync>(
						db::DBModule::GetTableSync(util::decodeTableId(objectId)));
				
				_alarm = dynamic_pointer_cast<const Alarm, const Registrable>(tableSync->getRegistrable(
																	  map.get<RegistryKeyType>(Request::PARAMETER_OBJECT_ID),
																	  _getEnv(),
																	  UP_LINKS_LOAD_LEVEL));
			}
			catch(...)
			{
				throw AdminParametersException("Specified alarm not found");
			}

			_parameters = map;
		}



		util::ParametersMap MessageAdmin::getParametersMap() const
		{
			ParametersMap m;
			if(_alarm.get())
				m.insert(Request::PARAMETER_OBJECT_ID, _alarm->getKey());
			return m;
		}



		void MessageAdmin::display(
			ostream& stream,
			const server::Request& _request
		) const	{

			////////////////////////////////////////////////////////////////////
			// TAB PARAMETERS
			if (openTabContent(stream, TAB_PARAMS))
			{
				stream << "<h1>Contenu</h1>";

				AdminActionFunctionRequest<UpdateAlarmMessagesFromTemplateAction,MessageAdmin> templateRequest(_request, *this);
				templateRequest.getAction()->setAlarmId(_alarm->getKey());

				MessagesModule::Labels tl(MessagesModule::getTextTemplateLabels(_alarm->getLevel()));
				if(!tl.empty())
				{
					HTMLForm fc(templateRequest.getHTMLForm("template"));
					stream << fc.open() << "<p>";
					stream << "Modèle : ";
					stream << fc.getSelectInput(UpdateAlarmMessagesFromTemplateAction::PARAMETER_TEMPLATE_ID, tl, optional<RegistryKeyType>());
					stream << fc.getSubmitButton("Copier contenu");
					stream << "</p>" << fc.close();
				}

				AdminActionFunctionRequest<UpdateAlarmMessagesAction,MessageAdmin> updateMessagesRequest(_request, *this);
				updateMessagesRequest.getAction()->setAlarmId(_alarm->getKey());

				if(_alarm->getRawEditor())
				{
					PropertiesHTMLTable tu(updateMessagesRequest.getHTMLForm("messages"));
					stream << tu.open();
					stream << tu.cell(
						"Type",
						tu.getForm().getRadioInputCollection(
							UpdateAlarmMessagesAction::PARAMETER_TYPE,
							MessagesModule::getLevelLabels(),
							optional<AlarmLevel>(_alarm->getLevel())
					)	);
					stream << tu.cell("Titre", tu.getForm().getTextInput(UpdateAlarmMessagesAction::PARAMETER_SHORT_MESSAGE, _alarm->getShortMessage()));
					stream << tu.cell("Contenu", tu.getForm().getTextAreaInput(UpdateAlarmMessagesAction::PARAMETER_LONG_MESSAGE, _alarm->getLongMessage(), 6, 60, false));
					stream << tu.close();
				}
				else
				{
					PropertiesHTMLTable tu(updateMessagesRequest.getHTMLForm("messages"));
					stream << tu.open();
					stream << tu.cell(
						"Type",
						tu.getForm().getRadioInputCollection(
							UpdateAlarmMessagesAction::PARAMETER_TYPE,
							MessagesModule::getLevelLabels(),
							optional<AlarmLevel>(_alarm->getLevel())
					)	);
					stream << tu.close();

					StaticActionRequest<UpdateAlarmMessagesAction> contentUpdateRequest(_request);
					contentUpdateRequest.getAction()->setAlarm(const_pointer_cast<Alarm>(_alarm));

					TinyMCE tinyMCE;
					tinyMCE.setAjaxSaveURL(contentUpdateRequest.getURL());
					stream << tinyMCE.open();

					stream << TinyMCE::GetFakeFormWithInput(UpdateAlarmMessagesAction::PARAMETER_SHORT_MESSAGE, _alarm->getShortMessage());
					stream << TinyMCE::GetFakeFormWithInput(UpdateAlarmMessagesAction::PARAMETER_LONG_MESSAGE, _alarm->getLongMessage());
				}

				AdminActionFunctionRequest<UpdateAlarmMessagesAction, MessageAdmin> rawEditorUpdateRequest(_request, *this);
				rawEditorUpdateRequest.getAction()->setAlarm(const_pointer_cast<Alarm>(_alarm));
				rawEditorUpdateRequest.getAction()->setRawEditor(!_alarm->getRawEditor());
				stream <<
					HTMLModule::getLinkButton(
					rawEditorUpdateRequest.getHTMLForm().getURL(),
					"Passer à l'éditeur "+ string(_alarm->getRawEditor() ? "WYSIWYG" : "technique")
				);
			}

			// Alarm messages destinations loop
			vector<boost::shared_ptr<AlarmRecipient> > recipients(Factory<AlarmRecipient>::GetNewCollection());
			BOOST_FOREACH(const boost::shared_ptr<AlarmRecipient>& recipient, recipients)
			{
				////////////////////////////////////////////////////////////////////
				// TAB STOPS
				if (openTabContent(stream, recipient->getFactoryKey()))
				{
					AdminActionFunctionRequest<AlarmAddLinkAction,MessageAdmin> addRequest(_request, *this);
					addRequest.getAction()->setAlarmId(_alarm->getKey());
					addRequest.getAction()->setRecipientKey(recipient->getFactoryKey());

					AdminActionFunctionRequest<AlarmRemoveLinkAction,MessageAdmin> removeRequest(_request, *this);
					removeRequest.getAction()->setAlarmId(_alarm->getKey());

					AdminFunctionRequest<MessageAdmin> searchRequest(_request, *this);
					recipient->displayBroadcastListEditor(stream, _alarm.get(), _parameters, searchRequest, addRequest, removeRequest);
				}
			}

			// TAB DATASOURCES
			if (openTabContent(stream, TAB_DATASOURCES))
			{
				// Source id
				StaticActionRequest<UpdateAlarmMessagesAction> updateOnlyRequest(_request);
				updateOnlyRequest.getAction()->setAlarmId(_alarm->getKey());
				
				ImportableAdmin::DisplayDataSourcesTab(stream, *_alarm, updateOnlyRequest);
			}


			////////////////////////////////////////////////////////////////////
			// END TABS
			closeTabContent(stream);
		}



		void MessageAdmin::_buildTabs(
			const security::Profile& profile
		) const {
			_tabs.clear();

			_tabs.push_back(Tab("Contenu", TAB_PARAMS, true));

			// Alarm messages destinations loop
			vector<boost::shared_ptr<AlarmRecipient> > recipients(Factory<AlarmRecipient>::GetNewCollection());
			BOOST_FOREACH(const boost::shared_ptr<AlarmRecipient>& recipient, recipients)
			{
				_tabs.push_back(Tab(recipient->getTitle(), recipient->getFactoryKey(), true));
			}

			_tabs.push_back(Tab("Source de données", TAB_DATASOURCES, true));

			_tabBuilded = true;
		}



		bool MessageAdmin::isAuthorized(
			const security::User& user
		) const {
			if (_alarm.get() == NULL) return false;
			if (_alarm->belongsToTemplate())
				return user.getProfile()->isAuthorized<MessagesRight>(READ);
			return user.getProfile()->isAuthorized<MessagesLibraryRight>(READ);
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
			const server::Request& request
		) const {
			const MessagesScenarioAdmin* ma(
				dynamic_cast<const MessagesScenarioAdmin*>(&currentPage)
			);

			if(	!ma) return false;

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

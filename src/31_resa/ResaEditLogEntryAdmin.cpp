
/** ResaEditLogEntryAdmin class implementation.
	@file ResaEditLogEntryAdmin.cpp
	@author Hugues Romain
	@date 2008

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

#include "ResaEditLogEntryAdmin.h"

#include "DateTime.h"

#include "User.h"
#include "UserTableSync.h"

#include "ResaLogAdmin.h"
#include "ResaRight.h"
#include "ResaModule.h"
#include "ResaLogEntryUpdateAction.h"

#include "DBLog.h"
#include "DBLogEntry.h"
#include "DBLogEntryTableSync.h"

#include "Request.h"
#include "ActionFunctionRequest.h"

#include "AdminParametersException.h"
#include "AdminRequest.h"

#include "PropertiesHTMLTable.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace admin;
	using namespace interfaces;
	using namespace server;
	using namespace util;
	using namespace resa;
	using namespace dblog;
	using namespace html;
	using namespace security;
	using namespace time;

	namespace util
	{
		template<> const string FactorableTemplate<AdminInterfaceElement, ResaEditLogEntryAdmin>::FACTORY_KEY("ResaEditLogEntryAdmin");
	}

	namespace admin
	{
		template<> const string AdminInterfaceElementTemplate<ResaEditLogEntryAdmin>::ICON("book_open.png");
		template<> const string AdminInterfaceElementTemplate<ResaEditLogEntryAdmin>::DEFAULT_TITLE("Evénement inconnu");
	}

	namespace resa
	{
		ResaEditLogEntryAdmin::ResaEditLogEntryAdmin()
			: AdminInterfaceElementTemplate<ResaEditLogEntryAdmin>(),
			_log("log")
		{ }
		
		void ResaEditLogEntryAdmin::setFromParametersMap(
			const ParametersMap& map,
			bool doDisplayPreparationActions
		){
			try
			{
				_entry = DBLogEntryTableSync::Get(_request->getObjectId(), _env);
			}
			catch (...)
			{
				throw AdminParametersException("No such log entry");
			}

			_log.set(
				map,
				ResaDBLog::FACTORY_KEY,
				UNKNOWN_VALUE,
				UNKNOWN_VALUE,
				UNKNOWN_VALUE,
				DBLogEntry::DB_LOG_UNKNOWN,
				TIME_UNKNOWN,
				TIME_UNKNOWN,
				lexical_cast<string>(_entry->getKey()) + "|"
			);
		}
		
		
		
		server::ParametersMap ResaEditLogEntryAdmin::getParametersMap() const
		{
			ParametersMap m;
			return m;
		}



		void ResaEditLogEntryAdmin::display(ostream& stream, VariablesMap& variables
		) const	{
			// Requests
			ActionFunctionRequest<ResaLogEntryUpdateAction,AdminRequest> updateRequest(_request);
			updateRequest.getFunction()->setSamePage(this);
			updateRequest.getAction()->setEntryId(_entry->getKey());

			FunctionRequest<AdminRequest> searchRequest(_request);
			searchRequest.getFunction()->setSamePage(this);

			// Display
			DBLogEntry::Content content(_entry->getContent());

			vector<pair<ResaDBLog::_EntryType,string> > choices;
			choices.push_back(make_pair(ResaDBLog::CALL_ENTRY, ResaDBLog::GetIcon(ResaDBLog::CALL_ENTRY) +" "+ ResaDBLog::GetText(ResaDBLog::CALL_ENTRY)));
			choices.push_back(make_pair(ResaDBLog::OUTGOING_CALL, ResaDBLog::GetIcon(ResaDBLog::OUTGOING_CALL) +" "+ ResaDBLog::GetText(ResaDBLog::OUTGOING_CALL)));
			choices.push_back(make_pair(ResaDBLog::FAKE_CALL,ResaDBLog::GetIcon(ResaDBLog::FAKE_CALL) +" "+ ResaDBLog::GetText(ResaDBLog::FAKE_CALL)));
			choices.push_back(make_pair(ResaDBLog::RADIO_CALL,ResaDBLog::GetIcon(ResaDBLog::RADIO_CALL) +" "+ResaDBLog::GetText(ResaDBLog::RADIO_CALL)));

			vector<pair<ResaDBLog::_EntryType,string> > addChoices;
			addChoices.push_back(make_pair(ResaDBLog::CUSTOMER_COMMENT_ENTRY,ResaDBLog::GetIcon(ResaDBLog::CUSTOMER_COMMENT_ENTRY) +" "+ResaDBLog::GetText(ResaDBLog::CUSTOMER_COMMENT_ENTRY)));
			addChoices.push_back(make_pair(ResaDBLog::INFORMATION_ENTRY,ResaDBLog::GetIcon(ResaDBLog::INFORMATION_ENTRY) +" "+ResaDBLog::GetText(ResaDBLog::INFORMATION_ENTRY)));
			addChoices.push_back(make_pair(ResaDBLog::REDIRECTION_ENTRY,ResaDBLog::GetIcon(ResaDBLog::REDIRECTION_ENTRY) +" "+ResaDBLog::GetText(ResaDBLog::REDIRECTION_ENTRY)));
			addChoices.push_back(make_pair(ResaDBLog::TECHNICAL_SUPPORT_ENTRY,ResaDBLog::GetIcon(ResaDBLog::TECHNICAL_SUPPORT_ENTRY) +" "+ResaDBLog::GetText(ResaDBLog::TECHNICAL_SUPPORT_ENTRY)));
			addChoices.push_back(make_pair(ResaDBLog::OTHER,ResaDBLog::GetIcon(ResaDBLog::OTHER) +" "+ResaDBLog::GetText(ResaDBLog::OTHER)));

			PropertiesHTMLTable t(updateRequest.getHTMLForm());
			stream << "<h1>Propriétés</h1>";
			stream << t.open();
			stream << t.title("Appel");
			stream << t.cell("Date début", _entry->getDate().toString(true));
			DateTime d(DateTime::FromSQLTimestamp(content[ResaDBLog::COL_DATE2]));
			stream << t.cell("Date fin", d.toString(true));
			stream << t.cell("Durée", Conversion::ToString(d.getSecondsDifference(_entry->getDate())) + " s");
			shared_ptr<const User> customer;
			shared_ptr<const User> op;
			if (_entry->getObjectId() > 0)
			{
				try
				{
					customer = UserTableSync::Get(_entry->getObjectId(), _env);
				}
				catch(...)
				{

				}
			}
			if(_entry->getUserId() > 0)
			{
				try
				{
					op = UserTableSync::Get(_entry->getUserId(), _env);
				}
				catch(...)
				{

				}
			}
			stream << t.cell("Client", customer.get() ? customer->getFullName() : "inconnu");
 			stream << t.cell("Opérateur", op.get() ? op->getFullName() : "inconnu");
			stream << t.cell("Type d'appel", t.getForm().getRadioInputCollection(ResaLogEntryUpdateAction::PARAMETER_CALL_TYPE, choices, static_cast<ResaDBLog::_EntryType>(Conversion::ToInt(content[ResaDBLog::COL_TYPE]))));
			stream << t.title("Ajout d'information sur l'appel");
			stream << t.cell("Type d'ajout", t.getForm().getRadioInputCollection(ResaLogEntryUpdateAction::PARAMETER_TYPE, addChoices, ResaDBLog::CALL_ENTRY));
			stream << t.cell("Texte", t.getForm().getTextAreaInput(ResaLogEntryUpdateAction::PARAMETER_TEXT, string(), 4, 50));
			stream << t.getForm().setFocus(ResaLogEntryUpdateAction::PARAMETER_TYPE);
			stream << t.close();

			stream << "<h1>Evénements liés à l'appel</h1>";
			
			_log.display(stream, searchRequest);
		}

		bool ResaEditLogEntryAdmin::isAuthorized() const
		{
			return _request->isAuthorized<ResaRight>(READ, UNKNOWN_RIGHT_LEVEL);
		}
		
		AdminInterfaceElement::PageLinks ResaEditLogEntryAdmin::getSubPagesOfParent(
			const PageLink& parentLink
			, const AdminInterfaceElement& currentPage
		) const	{
			AdminInterfaceElement::PageLinks links;
			if(parentLink.factoryKey == ResaLogAdmin::FACTORY_KEY && currentPage.getFactoryKey() == FACTORY_KEY)
				links.push_back(currentPage.getPageLink());
			return links;
		}
		
		AdminInterfaceElement::PageLinks ResaEditLogEntryAdmin::getSubPages(
			const PageLink& parentLink
			, const AdminInterfaceElement& currentPage
		) const {
			AdminInterfaceElement::PageLinks links;
			return links;
		}


		std::string ResaEditLogEntryAdmin::getTitle() const
		{
			return _entry.get() ? _entry->getDate().toString() : DEFAULT_TITLE;
		}

		std::string ResaEditLogEntryAdmin::getParameterName() const
		{
			return _entry.get() ? Request::PARAMETER_OBJECT_ID : string();
		}

		std::string ResaEditLogEntryAdmin::getParameterValue() const
		{
			return _entry.get() ? Conversion::ToString(_entry->getKey()) : string();
		}
	}
}

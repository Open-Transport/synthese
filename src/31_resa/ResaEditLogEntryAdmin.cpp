
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
#include "Profile.h"
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

#include "AdminFunctionRequest.hpp"
#include "AdminActionFunctionRequest.hpp"

#include "AdminParametersException.h"
#include "AdminInterfaceElement.h"
#include "ResaCustomerAdmin.h"
#include "PropertiesHTMLTable.h"

#include <boost/lexical_cast.hpp>

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
		template<> const string AdminInterfaceElementTemplate<ResaEditLogEntryAdmin>::DEFAULT_TITLE("Ev�nement inconnu");
	}

	namespace resa
	{
		ResaEditLogEntryAdmin::ResaEditLogEntryAdmin()
			: AdminInterfaceElementTemplate<ResaEditLogEntryAdmin>(),
			_log("log")
		{ }
		
		void ResaEditLogEntryAdmin::setFromParametersMap(
			const ParametersMap& map
		){
			try
			{
				_entry = DBLogEntryTableSync::Get(
					map.get<RegistryKeyType>(Request::PARAMETER_OBJECT_ID),
					_getEnv()
				);
			}
			catch (...)
			{
				throw AdminParametersException("No such log entry");
			}

			_log.set(
				map,
				ResaDBLog::FACTORY_KEY,
				UNKNOWN_VALUE,
				_entry->getKey()
			);
		}
		
		
		
		server::ParametersMap ResaEditLogEntryAdmin::getParametersMap() const
		{
			ParametersMap m;
			if(_entry.get()) m.insert(Request::PARAMETER_OBJECT_ID, _entry->getKey());
			return m;
		}



		void ResaEditLogEntryAdmin::display(ostream& stream, VariablesMap& variables,
					const admin::AdminRequest& _request
		) const	{
			// Requests
			AdminActionFunctionRequest<ResaLogEntryUpdateAction,ResaEditLogEntryAdmin> updateRequest(_request);
			updateRequest.getAction()->setEntryId(_entry->getKey());

			AdminFunctionRequest<ResaEditLogEntryAdmin> searchRequest(_request);

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
			addChoices.push_back(make_pair(ResaDBLog::RESERVATIONS_READ_ENTRY,ResaDBLog::GetIcon(ResaDBLog::RESERVATIONS_READ_ENTRY) +" "+ResaDBLog::GetText(ResaDBLog::RESERVATIONS_READ_ENTRY)));
			addChoices.push_back(make_pair(ResaDBLog::OTHER,ResaDBLog::GetIcon(ResaDBLog::OTHER) +" "+ResaDBLog::GetText(ResaDBLog::OTHER)));

			PropertiesHTMLTable t(updateRequest.getHTMLForm());
			stream << "<h1>Propri�t�s</h1>";
			stream << t.open();
			stream << t.title("Appel");
			stream << t.cell("Date d�but", _entry->getDate().toString(true));
			DateTime d(DateTime::FromSQLTimestamp(content[ResaDBLog::COL_DATE2]));
			stream << t.cell("Date fin", d.isUnknown() ? "inconnu" : d.toString(true));

			// Duration
			if(!d.isUnknown())
			{
				stream << t.cell("Dur�e", lexical_cast<string>(d.getSecondsDifference(_entry->getDate()).total_seconds()) + " s");
			}

			// Customer
			shared_ptr<const User> customer;
			if (_entry->getObjectId() > 0)
			{
				try
				{
					customer = UserTableSync::Get(_entry->getObjectId(), _getEnv());
				}
				catch(...)
				{

				}
			}
			if(customer.get())
			{
				AdminFunctionRequest<ResaCustomerAdmin> openUserRequest(_request);
				openUserRequest.getPage()->setUser(customer);
				stream << t.cell("Client", HTMLModule::getHTMLLink(openUserRequest.getURL(), customer->getFullName()));
			}
			else
				stream << t.cell("Client", "inconnu");

			// Operator
			shared_ptr<const User> op;
			if(_entry->getUserId() > 0)
			{
				try
				{
					op = UserTableSync::Get(_entry->getUserId(), _getEnv());
				}
				catch(...)
				{

				}
			}
			stream << t.cell("Op�rateur", op.get() ? op->getFullName() : "inconnu");
			stream << t.cell("Type d'appel", t.getForm().getRadioInputCollection(ResaLogEntryUpdateAction::PARAMETER_CALL_TYPE, choices, static_cast<ResaDBLog::_EntryType>(lexical_cast<int>(content[ResaDBLog::COL_TYPE]))));
			stream << t.title("Ajout d'information sur l'appel");
			stream << t.cell("Type d'ajout", t.getForm().getRadioInputCollection(ResaLogEntryUpdateAction::PARAMETER_TYPE, addChoices, ResaDBLog::CALL_ENTRY));
			stream << t.cell("Texte", t.getForm().getTextAreaInput(ResaLogEntryUpdateAction::PARAMETER_TEXT, string(), 4, 50));
			stream << t.getForm().setFocus(ResaLogEntryUpdateAction::PARAMETER_TYPE);
			stream << t.close();

			stream << "<h1>Ev�nements li�s � l'appel</h1>";
			
			_log.display(stream, searchRequest);
		}

		bool ResaEditLogEntryAdmin::isAuthorized(
			const security::User& user
		) const	{
			return user.getProfile()->isAuthorized<ResaRight>(READ, UNKNOWN_RIGHT_LEVEL);
		}
		


		std::string ResaEditLogEntryAdmin::getTitle() const
		{
			return _entry.get() ? _entry->getDate().toString() : DEFAULT_TITLE;
		}

	
		void ResaEditLogEntryAdmin::setEntry(boost::shared_ptr<const dblog::DBLogEntry> value)
		{
			_entry = value;
		}
		
		
		boost::shared_ptr<const dblog::DBLogEntry> ResaEditLogEntryAdmin::getEntry() const
		{
			return _entry;
		}
		
		bool ResaEditLogEntryAdmin::_hasSameContent(const AdminInterfaceElement& other) const
		{
			return _entry == static_cast<const ResaEditLogEntryAdmin&>(other)._entry;
		}
			
	}
}

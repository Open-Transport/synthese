////////////////////////////////////////////////////////////////////////////////
///	DBLogDynamicViewFunction class implementation.
///	@file DBLogDynamicViewFunction.cpp
///	@date 04/06/2014
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

#include "DBLogDynamicViewFunction.hpp"
#include "AdminParametersException.h"
#include "HTMLTable.h"
#include "Session.h"
#include "Exception.h"
#include "User.h"
#include "UserTableSync.h"
#include "DBLog.h"
#include "DBLogModule.h"
#include "DBLogEntryTableSync.h"
#include "DBLogRight.h"
#include "ResaDBLog.h"
#include "Request.h"
#include "Profile.h"
#include "SecurityModule.h"

using namespace std;
using namespace boost;
using namespace boost::posix_time;


namespace synthese
{
	using namespace admin;
	using namespace dblog;
	using namespace util;
	using namespace security;
	using namespace server;
	using namespace html;

	namespace util
	{
		template<> const string FactorableTemplate<Function, resa::DBLogDynamicViewFunction>::FACTORY_KEY("DBLogDynamicViewFunction");
	}

	namespace resa
	{
		const string DBLogDynamicViewFunction::PARAMETER_USER = "user";
		const string DBLogDynamicViewFunction::PARAMETER_ENTRYTYPE = "entryType";
		const string DBLogDynamicViewFunction::PARAMETER_GETCOUNT = "count";
		const string DBLogDynamicViewFunction::PARAMETER_FIRST = "first";

		const string DBLogDynamicViewFunction::FILTER_ALL = "all";
		const string DBLogDynamicViewFunction::FILTER_RESERVATION = "Réservation";
		const string DBLogDynamicViewFunction::FILTER_MISSING = "Absence";
		const string DBLogDynamicViewFunction::FILTER_CANCEL = "Annulation";
		const string DBLogDynamicViewFunction::FILTER_CANCEL_DEADLINE = "Annulation hors délai";



		ParametersMap DBLogDynamicViewFunction::_getParametersMap() const
		{
			ParametersMap map;
			return map;
		}



		void DBLogDynamicViewFunction::_setFromParametersMap(const ParametersMap& map)
		{
			if (map.getDefault<RegistryKeyType>(PARAMETER_USER))
			{
				try
				{
					_user = UserTableSync::Get(
					map.get<RegistryKeyType>(PARAMETER_USER),
						Env::GetOfficialEnv()
					);
				}
				catch (...)
				{
					throw AdminParametersException("Bad user id");
				}
			}

			_entryType = map.getDefault<string>(PARAMETER_ENTRYTYPE,string());
			_count = map.getDefault<bool>(PARAMETER_GETCOUNT,false);
			_first = map.getDefault<int>(PARAMETER_FIRST,0);
			_dbLog.reset(Factory<DBLog>::create("ResaDBLog"));
		}



		util::ParametersMap DBLogDynamicViewFunction::run(
			std::ostream& stream,
			const Request& request
		) const {
			ParametersMap map;

			if (!_user)
				return map;

			// Just send back occurences of each entryType to display them in the admin interface tabs
			if (_count) {
				DBLogEntryTableSync::SearchResult entries = DBLogEntryTableSync::SearchByUser(
					Env::GetOfficialEnv(),
					_user->getKey(),
					"ResaDBLog"
				);

				int resa = 0, abs = 0, cancel = 0, cancel_d = 0;
				BOOST_FOREACH(const boost::shared_ptr<DBLogEntry>& dbe, entries)
				{
					try {
						const DBLogEntry::Content& content(dbe->getContent());

						const resa::ResaDBLog::_EntryType entryType(static_cast<resa::ResaDBLog::_EntryType>(lexical_cast<int>(content[0])));
						const string entryText(lexical_cast<string>(content[1]));
						switch(entryType)
						{
							case ResaDBLog::RESERVATION_ENTRY:
								if (!entryText.empty())
									resa++;
								break;
							case ResaDBLog::CANCELLATION_ENTRY:
								cancel++;
								break;
							case ResaDBLog::DELAYED_CANCELLATION_ENTRY:
								cancel_d++;
								break;
							case ResaDBLog::NO_SHOW_ENTRY:
								abs++;
								break;
							default:
								continue;
						}
					}
					catch(bad_lexical_cast)
					{
					}
				}
				stream << resa << ";" << cancel << ";" << cancel_d << ";" << abs;
			}
			// Send back 100 entries of same entryType
			else if (
				// Check entryType is filled and correct
				!_entryType.empty() &&
				(
					_entryType == FILTER_ALL ||
					_entryType == FILTER_RESERVATION ||
					_entryType == FILTER_CANCEL ||
					_entryType == FILTER_CANCEL_DEADLINE ||
					_entryType == FILTER_MISSING
				)
			){
				// Get related entries
				DBLogEntryTableSync::SearchResult entries = DBLogEntryTableSync::SearchByUser(
					Env::GetOfficialEnv(),
					_user->getKey(),
					"ResaDBLog",
					_first,
					boost::optional<size_t>(100),
					(_entryType != "all") ? _entryType : std::string()
				);

				if (!entries.empty())
				{
					// Build table header
					HTMLTable::ColsVector columns;
					columns.push_back("Type");
					columns.push_back("Date");
					columns.push_back("Utilisateur");
					dblog::DBLog::ColumnsVector dblogColumns = _dbLog->getColumnNames();
					columns.insert(columns.end(),dblogColumns.begin(),dblogColumns.end());
					HTMLTable table(columns,"adminresults");

					stream << table.open();

					// Display each row using dbLog relation to parse data
					BOOST_FOREACH(const boost::shared_ptr<DBLogEntry>& dbe, entries)
					{
						try {
								boost::shared_ptr<const User> user;
								try
								{
									user = UserTableSync::Get(dbe->getUserId(), Env::GetOfficialEnv());
								}
								catch (...)
								{
								}

								stream << table.row();
								stream <<
										  table.col() <<
										  HTMLModule::getHTMLImage(
											  "/admin/img/" + DBLogModule::getEntryIcon(dbe->getLevel()),
											  DBLogModule::getEntryLevelLabel(dbe->getLevel())
											  )
										  ;
								stream << table.col() << to_simple_string(dbe->getDate());
								stream << table.col() <<
									(	user.get() ?
										user->getLogin() :
										(dbe->getUserId() > 0) ? "(supprimé)" : "(robot)"
									);
								DBLog::ColumnsVector cols = _dbLog->parse(*dbe, request);
								BOOST_FOREACH(const DBLog::ColumnsVector::value_type& col, cols)
								{
									stream << table.col() << col;
								}

						}
						catch(bad_lexical_cast)
						{
						}
					}

					// Display table footer ( PREVIOUS / NEXT buttons if needed and data number)
					stream << table.row();
					size_t columnsSize(columns.size());
					stream << table.col(columnsSize,string(),true);
					int size(entries.size());
					stream << (_first == 0 ? "" : "<button id='previous'><<</button>&nbsp;&nbsp;");
					stream << "<span id='number'>";
					stream << ((_first == 0) ? 0 : _first) << " - " << (size + _first);
					stream << "</span>";
					stream << (size == 100 ? "&nbsp;&nbsp;<button id='next'>>></button>" : "");
					stream << table.close();
				}
			}
			return map;
		}



		std::string DBLogDynamicViewFunction::getOutputMimeType() const
		{
			return "text/html";
		}



		bool DBLogDynamicViewFunction::isAuthorized(const Session* session
		) const {
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<DBLogRight>(WRITE);
		}
	}
}

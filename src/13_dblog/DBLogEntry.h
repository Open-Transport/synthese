
/** DBLogEntry class header.
	@file DBLogEntry.h

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

#ifndef SYNTHESE_DBLogEntry_h__
#define SYNTHESE_DBLogEntry_h__

#include <vector>
#include <string>

#include "Registrable.h"
#include "Registry.h"
#include "DateTime.h"

namespace synthese
{
	namespace security
	{
		class User;
	}

	namespace dblog
	{
		/** DBLogEntry class.
			@ingroup m13
		*/
		class DBLogEntry
		: public virtual util::Registrable
		{
		public:

			/// Chosen registry class.
			typedef util::Registry<DBLogEntry>	Registry;

			typedef enum 
			{
				DB_LOG_UNKNOWN = 0
				, DB_LOG_OK = 5
				, DB_LOG_INFO = 10
				, DB_LOG_WARNING = 50
				, DB_LOG_ERROR = 99
			}	Level;
			typedef std::vector<std::string>	Content;

		private:
			std::string				_logKey;
			time::DateTime			_date;
			const security::User*	_user;
			Content					_content;
			Level					_level;
			uid						_objectId;

		public:
			DBLogEntry(util::RegistryKeyType key = UNKNOWN_VALUE);

			void					setLogKey(const std::string& key);
			void					setDate(const time::DateTime& date);
			void					setUser(const security::User* user);
			void					setContent(const Content& content);
			void					setLevel(Level level);
			void					setObjectId(uid id);

			const std::string&		getLogKey()			const;
			time::DateTime			getDate()			const;
			const security::User*	getUser()			const;
			const Content&			getContent()		const;
			std::string				getStringContent()	const;
			Level					getLevel()			const;
			uid						getObjectId()		const;
		};
	}
}

#endif // SYNTHESE_DBLogEntry_h__

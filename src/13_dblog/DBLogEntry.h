
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
#include <boost/date_time/posix_time/posix_time.hpp>

#include "Registrable.h"
#include "Registry.h"

namespace synthese
{
	namespace dblog
	{
		/** DBLogEntry class.
			@ingroup m13
		*/
		class DBLogEntry
		:	public virtual util::Registrable
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
			boost::posix_time::ptime	_date;
			util::RegistryKeyType	_userId;
			Content					_content;
			Level					_level;
			util::RegistryKeyType	_objectId;
			util::RegistryKeyType	_objectId2;

		public:
			DBLogEntry(util::RegistryKeyType key = UNKNOWN_VALUE);

			void					setLogKey(const std::string& key);
			void					setDate(const boost::posix_time::ptime& date);
			void					setUserId(util::RegistryKeyType value);
			void					setContent(const Content& content);
			void					setLevel(Level level);
			void					setObjectId(util::RegistryKeyType id);
			void					setObjectId2(util::RegistryKeyType id);

			const std::string&		getLogKey()			const;
			boost::posix_time::ptime	getDate()			const;
			util::RegistryKeyType	getUserId()			const;
			const Content&			getContent()		const;
			std::string				getStringContent()	const;
			Level					getLevel()			const;
			util::RegistryKeyType	getObjectId()		const;
			util::RegistryKeyType	getObjectId2()		const;
		};
	}
}

#endif // SYNTHESE_DBLogEntry_h__

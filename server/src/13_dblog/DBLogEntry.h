
/** DBLogEntry class header.
	@file DBLogEntry.h

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>

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

#include "Object.hpp"

#include "EnumObjectField.hpp"
#include "PtimeField.hpp"
#include "StringField.hpp"

namespace synthese
{
	namespace dblog
	{
		typedef enum
		{
			DB_LOG_UNKNOWN = 0
			, DB_LOG_OK = 5
			, DB_LOG_INFO = 10
			, DB_LOG_WARNING = 50
			, DB_LOG_ERROR = 99
		}	Level;

		FIELD_STRING(LogKey)
		FIELD_PTIME(LogDate)
		FIELD_ID(LogUser)
		FIELD_ENUM(LogLevel, Level)
		FIELD_STRING(LogContent)
		FIELD_ID(ObjectId)
		FIELD_ID(Object2Id)

		typedef boost::fusion::map<
			FIELD(Key),
			FIELD(LogKey),
			FIELD(LogDate),
			FIELD(LogUser),
			FIELD(LogLevel),
			FIELD(LogContent),
			FIELD(ObjectId),
			FIELD(Object2Id)
		> DBLogEntrySchema;

		/** DBLogEntry class.
			@ingroup m13
		*/
		class DBLogEntry
		:	public Object<DBLogEntry, DBLogEntrySchema>
		{
		public:
			static const std::string CONTENT_SEPARATOR;

			/// Chosen registry class.
			typedef util::Registry<DBLogEntry>	Registry;
			typedef std::vector<std::string>	Content;

		private:
			Content					_content;

		public:
			DBLogEntry(util::RegistryKeyType key = 0);

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

			virtual void link(util::Env& env, bool withAlgorithmOptimizations = false);

			virtual bool allowUpdate(const server::Session* session) const;
			virtual bool allowCreate(const server::Session* session) const;
			virtual bool allowDelete(const server::Session* session) const;
		};
	}
}

#endif // SYNTHESE_DBLogEntry_h__

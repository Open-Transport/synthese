
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

#include <boost/shared_ptr.hpp>

#include "01_util/Registrable.h"
#include "01_util/UId.h"

#include "04_time/DateTime.h"

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
		class DBLogEntry : public util::Registrable<uid, DBLogEntry>
		{
		public:
			typedef enum 
			{ DB_LOG_UNKNOWN = 0, DB_LOG_INFO = 10, DB_LOG_WARNING = 50, DB_LOG_ERROR = 99 }	Level;
			typedef std::vector<std::string>											Content;

		private:
			std::string				_logKey;
			time::DateTime			_date;
			boost::shared_ptr<const security::User>	_user;
			Content					_content;
			Level					_level;

		public:
			void					setLogKey(const std::string& key);
			void					setDate(const time::DateTime& date);
			void					setUser(boost::shared_ptr<const security::User> user);
			void					setContent(const Content& content);
			void					setLevel(Level level);
			const std::string&		getLogKey()		const;
			time::DateTime			getDate()		const;
			boost::shared_ptr<const security::User>	getUser()		const;
			const Content&			getContent()	const;
			Level					getLevel()		const;
		};
	}
}

#endif // SYNTHESE_DBLogEntry_h__

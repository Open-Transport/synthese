
/** DBLogTemplate class header.
	@file DBLogTemplate.h

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

#ifndef SYNTHESE_dblog_DBLogTemplate_h__
#define SYNTHESE_dblog_DBLogTemplate_h__

#include "DBLogRight.h"
#include "DBLog.h"
#include "FactorableTemplate.h"
#include "Profile.h"

namespace synthese
{
	namespace dblog
	{
		/** DBLogTemplate class.
			@ingroup m13
		*/
		template<class T>
		class DBLogTemplate:
			public util::FactorableTemplate<DBLog,T>
		{
		public:
			static bool IsAuthorized(
				const security::Profile& profile,
				const security::RightLevel& level
			){
				return profile.isAuthorized<DBLogRight>(level, security::UNKNOWN_RIGHT_LEVEL, T::FACTORY_KEY);
			}



			virtual bool isAuthorized(
				const security::Profile& profile,
				const security::RightLevel& level
			){
				return IsAuthorized(profile, level);
			};

		};
	}
}

#endif // SYNTHESE_dblog_DBLogTemplate_h__

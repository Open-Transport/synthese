
/** DBInterSYNTHESEContent class header.
	@file DBInterSYNTHESEContent.hpp

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

#ifndef SYNTHESE_db_DBReplaceInterSYNTHESEContent_hpp__
#define SYNTHESE_db_DBReplaceInterSYNTHESEContent_hpp__

#include "InterSYNTHESEContent.hpp"

namespace synthese
{
	namespace db
	{
		class DBRecord;

		/** DBInterSYNTHESEContent class.
			@ingroup m10
		*/
		class DBReplaceInterSYNTHESEContent:
			public inter_synthese::InterSYNTHESEContent
		{
			const DBRecord& _record;

		public:
			DBReplaceInterSYNTHESEContent(const DBRecord& r);

			virtual std::string getPerimeter() const;
			virtual std::string getContent() const;
		};
	}
}

#endif // SYNTHESE_db_DBInterSYNTHESEContent_hpp__

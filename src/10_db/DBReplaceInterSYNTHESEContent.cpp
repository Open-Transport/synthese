
/** DBReplaceInterSYNTHESEContent class implementation.
	@file DBReplaceInterSYNTHESEContent.cpp

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

#include "DBReplaceInterSYNTHESEContent.hpp"

#include "DBInterSYNTHESE.hpp"
#include "DBRecord.hpp"
#include "DBTableSync.hpp"

#include <boost/lexical_cast.hpp>

using namespace boost;
using namespace std;
using namespace boost::posix_time;

namespace synthese
{
	namespace db
	{
		DBReplaceInterSYNTHESEContent::DBReplaceInterSYNTHESEContent( const DBRecord& r ):
			InterSYNTHESEContent(DBInterSYNTHESE::FACTORY_KEY),
			_record(r)
		{}



		std::string DBReplaceInterSYNTHESEContent::getContent() const
		{
			stringstream content;
			DBInterSYNTHESE::RequestEnqueue visitor(content);
			visitor(_record);
			return content.str();
		}



		std::string DBReplaceInterSYNTHESEContent::getPerimeter() const
		{
			return lexical_cast<string>(_record.getTable()->getFormat().ID);
		}



		boost::posix_time::ptime DBReplaceInterSYNTHESEContent::getExpirationTime() const
		{
			return ptime(not_a_date_time);
		}
}	}


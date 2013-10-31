
/** DBDeleteInterSYNTHESEContent class implementation.
	@file DBDeleteInterSYNTHESEContent.cpp

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

#include "DBDeleteInterSYNTHESEContent.hpp"

#include "DBInterSYNTHESE.hpp"

#include <boost/lexical_cast.hpp>

using namespace boost;
using namespace std;

namespace synthese
{
	using namespace util;

	namespace db
	{
		DBDeleteInterSYNTHESEContent::DBDeleteInterSYNTHESEContent( util::RegistryKeyType id ):
			_id(id),
			InterSYNTHESEContent(DBInterSYNTHESE::FACTORY_KEY)
		{
		}
	


		std::string DBDeleteInterSYNTHESEContent::getPerimeter() const
		{
			return lexical_cast<string>(decodeTableId(_id));
		}



		std::string DBDeleteInterSYNTHESEContent::getContent() const
		{
			stringstream content;
			DBInterSYNTHESE::RequestEnqueue visitor(content);
			visitor(_id);
			return content.str();
		}
}	}

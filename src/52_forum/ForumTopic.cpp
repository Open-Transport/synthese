
/** ForumTopic class implementation.
	@file ForumTopic.cpp

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

#include "ForumTopic.hpp"

#include <boost/date_time/posix_time/posix_time.hpp>

using namespace std;
using namespace boost;
using namespace boost::posix_time;

namespace synthese
{
	using namespace util;

	namespace util
	{
		template<> const string Registry<forum::ForumTopic>::KEY("ForumTopic");
	}

	namespace forum
	{
		ForumTopic::ForumTopic(RegistryKeyType id):
			Registrable(id),
			Named(),
			_node(0),
			_user(NULL),
			_date(second_clock::local_time()),
			_published(true),
			_closed(false)
		{
		}
	}
}


/** Clock class header.
	@file Clock.hpp

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

#ifndef SYNTHESE_UTIL_Clock_HPP
#define SYNTHESE_UTIL_Clock_HPP


#include <boost/noncopyable.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

namespace synthese
{
	namespace util
	{

		/** Basic clock wrapper
		 *
			@ingroup m01
		*/
		class Clock
		{
		public:

			static Clock DEFAULT_CLOCK;

			Clock();
			virtual ~Clock();

			virtual boost::posix_time::ptime getLocalTime() const;

		};
	}
}

#endif

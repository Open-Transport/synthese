
/** Named class header.
	@file Named.h

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

#ifndef SYNTHESE_util_Named_h__
#define SYNTHESE_util_Named_h__

#include <string>

namespace synthese
{
	namespace util
	{
		/** Named class.
			@ingroup m01
		*/
		class Named
		{
		private:
			std::string _name;

		public:
			Named() : _name() {}
			Named(const std::string& value) : _name(value) {}
			const std::string& getName() const { return _name; }
			void setName(const std::string& value) { _name = value; }

		};
	}
}

#endif // SYNTHESE_util_Named_h__

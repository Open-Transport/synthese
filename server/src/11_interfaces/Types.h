
/** Interface module types definitions file.
	@file 11_interfaces/Types.h

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

#ifndef SYNTHESE_Types_h__
#define SYNTHESE_Types_h__

#include <string>
#include <deque>
#include <map>

namespace synthese
{
	namespace interfaces
	{
		/**
			@addtogroup m11
			@{
			*/
		typedef std::deque<std::string> ParametersVector;
		typedef std::map<std::string, std::string> VariablesMap;

		/** @} */
	}
}

#endif // SYNTHESE_Types_h__

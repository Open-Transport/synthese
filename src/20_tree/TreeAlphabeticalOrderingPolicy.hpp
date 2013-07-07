
/** TreeAlphabeticalOrderingPolicy class header.
	@file TreeAlphabeticalOrderingPolicy.hpp

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

#ifndef SYNTHESE_util_AlphabeticalOrderingPolicy_hpp__
#define SYNTHESE_util_AlphabeticalOrderingPolicy_hpp__

namespace synthese
{
	namespace tree
	{
		/** AlphabeticalOrderingPolicy class.
			@ingroup m20
		*/
		template<class T>
		class TreeAlphabeticalOrderingPolicy
		{
		public:
			typedef std::string OrderingKeyType;

			const OrderingKeyType getTreeOrderingKey() const { return static_cast<const T*>(this)->getName(); }

		};
	}
}

#endif // SYNTHESE_util_AlphabeticalOrderingPolicy_hpp__

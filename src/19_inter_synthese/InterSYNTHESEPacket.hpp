
/** InterSYNTHESEPacket class header.
	@file InterSYNTHESEPacket.hpp

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

#ifndef SYNTHESE_inter_synthese_InterSYNTHESEPacket_hpp__
#define SYNTHESE_inter_synthese_InterSYNTHESEPacket_hpp__

#include "Exception.h"

#include "UtilTypes.h"

#include <map>

namespace synthese
{
	namespace inter_synthese
	{
		class InterSYNTHESEIdFilter;



		/** InterSYNTHESEPacket class.
			@ingroup m19
		*/
		class InterSYNTHESEPacket
		{
		public:
			static const std::string FIELDS_SEPARATOR;
			static const std::string SYNCS_SEPARATOR;

			typedef std::map<
				util::RegistryKeyType,	// id of the content
					std::pair<
						std::string,		// synchronizer
						std::string			// message
			>	> Data;

			class BadPacketException:
				public synthese::Exception
			{
			public:
				BadPacketException();
			};

		private:
			Data _data;

		public:
			InterSYNTHESEPacket();
			InterSYNTHESEPacket(const std::string& content);

			typedef std::pair<util::RegistryKeyType, util::RegistryKeyType> IdRange;
			bool empty() const;
			size_t size() const;
			IdRange getIdRange() const;

			void load(InterSYNTHESEIdFilter* idFilter) const;
		};
	}
}

#endif // SYNTHESE_inter_synthese_InterSYNTHESEPacket_hpp__

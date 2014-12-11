
//////////////////////////////////////////////////////////////////////////
///	AlertTableSync class header.
///	@file AlertTableSync.hpp
///	@author Camille Hue
///	@date 2014
///
///	This file belongs to the SYNTHESE project (public transportation specialized software)
///	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>
///
///	This program is free software; you can redistribute it and/or
///	modify it under the terms of the GNU General Public License
///	as published by the Free Software Foundation; either version 2
///	of the License, or (at your option) any later version.
///
///	This program is distributed in the hope that it will be useful,
///	but WITHOUT ANY WARRANTY; without even the implied warranty of
///	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
///	GNU General Public License for more details.
///
///	You should have received a copy of the GNU General Public License
///	along with this program; if not, write to the Free Software
///	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#ifndef SYNTHESE_AlertTableSync_hpp__
#define SYNTHESE_AlertTableSync_hpp__

#include "Alert.hpp"
#include "DBDirectTableSyncTemplate.hpp"

namespace synthese
{
	namespace regulation
	{
		//////////////////////////////////////////////////////////////////////////
		///	Descent table synchronizer.
		///	@ingroup m38LS refLS
		///	@author Camille Hue
		///	@date 2013
		/// @since 3.8.0
		class AlertTableSync:
			public db::DBDirectTableSyncTemplate<
				AlertTableSync,
				Alert
			>
		{
		public:

			//! @name Services
			//@{
				//////////////////////////////////////////////////////////////////////////
				///	Descent search.
				///	@param env Environment to populate
				///	@param serviceId optional ID of a foreign key to filter on (deactivated if undefined)
				///	@param first First  object to answer
				///	@param number Number of  objects to answer (0 = all) The size of the vector is less or equal to number.
				///	@param linkLevel Level of links to build when reading foreign keys
				///	@return Found objects.
				///	@author Camille Hue
				///	@date 2014
				/// @since 3.8.0
            /*
				static SearchResult Search(
					util::Env& env,
					boost::optional<util::RegistryKeyType> serviceId,
					const boost::gregorian::date& minDate,
					const boost::gregorian::date& maxDate,
					bool displayCancelled = false,
					std::size_t first = 0,
					boost::optional<std::size_t> number = boost::optional<std::size_t>(),
					util::LinkLevel linkLevel = util::UP_LINKS_LOAD_LEVEL
				);
            */
			//@}
		};
	}
}

#endif // SYNTHESE_AlertTableSync_hpp__

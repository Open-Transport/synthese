
//////////////////////////////////////////////////////////////////////////
///	ForumTopicTableSync class header.
///	@file ForumTopicTableSync.hpp
///	@author Hugues Romain
///	@date 2010
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

#ifndef SYNTHESE_ForumTopicTableSync_hpp__
#define SYNTHESE_ForumTopicTableSync_hpp__

#include "ForumTopic.hpp"
#include "DBDirectTableSyncTemplate.hpp"
#include "NoSynchronizationPolicy.hpp"
#include "OldLoadSavePolicy.hpp"

#include <boost/logic/tribool.hpp>
#include <boost/optional/optional.hpp>

namespace synthese
{
	namespace forum
	{
		//////////////////////////////////////////////////////////////////////////
		///	52.10 Table : Forum topic.
		///	@ingroup m52LS refLS
		///	@author Hugues Romain
		///	@date 2010
		/// @since 3.1.18
		class ForumTopicTableSync:
			public db::DBDirectTableSyncTemplate<
				ForumTopicTableSync,
				ForumTopic,
				db::NoSynchronizationPolicy,
				db::OldLoadSavePolicy
			>
		{
		public:
			//! @name Field names
			//@{
				static const std::string COL_NAME;
				static const std::string COL_NODE_ID;
				static const std::string COL_NODE_EXTENSION;
				static const std::string COL_USER_EMAIL;
				static const std::string COL_USER_NAME;
				static const std::string COL_USER_ID;
				static const std::string COL_DATE;
				static const std::string COL_PUBLISHED;
				static const std::string COL_CLOSED;
				static const std::string COL_IP;
			//@}


			//! @name Services
			//@{
				//////////////////////////////////////////////////////////////////////////
				///	ForumTopic search.
				///	@param env Environment to populate
				///	@param nodeId (optional) identifier of the node which the topic must belong to
				///	@param nodeExtension (optional) extension key of the node (must be exactly identical)
				///	@param userName (optional) name of the user (compared with LIKE operator)
				///	@param userId (optional) id of the user
				///	@param published topic published attribute
				///	@param first First  object to answer
				///	@param number Number of  objects to answer (0 = all) The size of the vector is less or equal to number, then all users were returned despite of the number limit. If the size is greater than number (actually equal to number + 1) then there is others accounts to show. Test it to know if the situation needs a "click for more" button.
				///	@param orderByDate The result records must be ordered by their publication date
				///	@param raisingOrder The result records must be sorted ascendantly
				///	@param linkLevel Level of links to build when reading foreign keys
				///	@return Found objects.
				///	@author Hugues Romain
				///	@date 2010
				/// @since 3.1.18
				static SearchResult Search(
					util::Env& env,
					boost::optional<util::RegistryKeyType> nodeId = boost::optional<util::RegistryKeyType>(),
					boost::optional<std::string> nodeExtension = boost::optional<std::string>(),
					boost::optional<std::string> userName = boost::optional<std::string>(),
					boost::optional<util::RegistryKeyType> userId = boost::optional<util::RegistryKeyType>(),
					boost::logic::tribool published = boost::logic::indeterminate,
					std::size_t first = 0,
					boost::optional<std::size_t> number = boost::optional<std::size_t>(),
					bool orderByDate = true,
					bool raisingOrder = true,
					util::LinkLevel linkLevel = util::UP_LINKS_LOAD_LEVEL
				);
			//@}
		};
	}
}

#endif // SYNTHESE_ForumTopicTableSync_hpp__

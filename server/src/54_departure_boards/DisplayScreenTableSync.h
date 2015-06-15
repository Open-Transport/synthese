////////////////////////////////////////////////////////////////////////////////
/// DisplayScreenTableSync class header.
///	@file DisplayScreenTableSync.h
///	@author Hugues Romain
///	@date 2008-12-21 21:17
///
///	This file belongs to the SYNTHESE project (public transportation specialized
///	software)
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
///	along with this program; if not, write to the Free Software Foundation,
///	Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
////////////////////////////////////////////////////////////////////////////////

#ifndef SYNTHESE_DisplayScreenTableSync_H__
#define SYNTHESE_DisplayScreenTableSync_H__

#include "DisplayScreen.h"
#include "DBDirectTableSyncTemplate.hpp"
#include "SecurityTypes.hpp"

#include <boost/optional.hpp>

namespace synthese
{
	namespace messages
	{
		class Alarm;
	}

	namespace departure_boards
	{
		/** DisplayScreen table synchronizer.
			@ingroup m54LS refLS
		*/
		class DisplayScreenTableSync:
			public db::DBDirectTableSyncTemplate<
				DisplayScreenTableSync,
				DisplayScreen
			>
		{
		public:



			/** DisplayScreen search.
				(other search parameters)
				@param first First DisplayScreen object to answer
				@param number Number of DisplayScreen objects to answer (0 = all) The size of the vector is less or equal to number, then all users were returned despite of the number limit. If the size is greater than number (actually equal to number + 1) then there is others accounts to show. Test it to know if the situation needs a "click for more" button.
				@return Found DisplayScreen objects.
				@author Hugues Romain
				@date 2006
			*/
			static SearchResult Search(
				util::Env& env,
				boost::optional<const security::RightsOfSameClassMap&> rights = boost::optional<const security::RightsOfSameClassMap&>(),
				bool totalControl = true,
				security::RightLevel neededLevel = security::FORBIDDEN,
				boost::optional<util::RegistryKeyType> duid = boost::optional<util::RegistryKeyType>(),
				boost::optional<util::RegistryKeyType> localizationid = boost::optional<util::RegistryKeyType>(),
				boost::optional<util::RegistryKeyType> lineid = boost::optional<util::RegistryKeyType>(),
				boost::optional<util::RegistryKeyType> typeuid = boost::optional<util::RegistryKeyType>(),
				std::string cityName = std::string(),
				std::string stopName = std::string(),
				std::string name = std::string(),
				boost::optional<int> state = boost::optional<int>(),
				boost::optional<int> message = boost::optional<int>(),
				size_t first = 0,
				boost::optional<std::size_t> number = boost::optional<std::size_t>(),
				bool orderByUid = false,
				bool orderByCity = true,
				bool orderByStopName = false,
				bool orderByName = false,
				bool orderByType = false,
				bool orderByStatus = false,
				bool orderByMessage = false,
				bool raisingOrder = true,
				util::LinkLevel linkLevel = util::UP_LINKS_LOAD_LEVEL
			);



			/** DisplayScreen search.
				(other search parameters)
				@param first First DisplayScreen object to answer
				@param number Number of DisplayScreen objects to answer (0 = all) The size of the vector is less or equal to number, then all users were returned despite of the number limit. If the size is greater than number (actually equal to number + 1) then there is others accounts to show. Test it to know if the situation needs a "click for more" button.
				@return Found DisplayScreen objects.
				@author Hugues Romain
				@date 2006
			*/
			static SearchResult SearchFromCPU(
				util::Env& env,
				util::RegistryKeyType cpuId,
				util::LinkLevel linkLevel = util::UP_LINKS_LOAD_LEVEL
			);



			/** DisplayScreen search.
				(other search parameters)
				@param first First DisplayScreen object to answer
				@param number Number of DisplayScreen objects to answer (0 = all) The size of the vector is less or equal to number, then all users were returned despite of the number limit. If the size is greater than number (actually equal to number + 1) then there is others accounts to show. Test it to know if the situation needs a "click for more" button.
				@return Found DisplayScreen objects.
				@author Hugues Romain
				@date 2006
			*/
			static boost::shared_ptr<DisplayScreen> GetByMACAddress(
				util::Env& env,
				const std::string& macAddress,
				util::LinkLevel linkLevel = util::UP_LINKS_LOAD_LEVEL
			);



			////////////////////////////////////////////////////////////////////
			///	Gets the message currently displayed on a screen.
			/// @param env Environment to populate
			///	@param screenId id of the screen
			/// @param number Number of results (optional)
			///	@return pointer to the message
			///	@author Hugues Romain
			///	@date 2008
			/// @warning The message is returned even if the screen is deactivated.
			/// If there is no message, a null pointer is returned.
			////////////////////////////////////////////////////////////////////
			static std::vector<boost::shared_ptr<messages::Alarm> > GetCurrentDisplayedMessage(
				util::Env& env,
				util::RegistryKeyType screenId,
				boost::optional<int> number = boost::optional<int>()
			);



			////////////////////////////////////////////////////////////////////
			///	Gets the message later displayed on a screen.
			/// @param env Environment to populate
			///	@param screenId id of the screen
			/// @param number Number of results (<=0 : unlimited)
			///	@return pointer to the message
			///	@author Hugues Romain
			///	@date 2008
			/// @warning The message is returned even if the screen is deactivated.
			/// If there is no message, a null pointer is returned.
			////////////////////////////////////////////////////////////////////
			static std::vector<boost::shared_ptr<messages::Alarm> > GetFutureDisplayedMessages(
				util::Env& env,
				util::RegistryKeyType screenId,
				boost::optional<int> number = boost::optional<int>()
			);



			////////////////////////////////////////////////////////////////////
			///	Tests if at least a line is displayed on the screen.
			///	@param screenId id of the screen
			///	@return bool true if at least a line is displayed on the screen
			///	@author Hugues Romain
			///	@date 2008
			/// @warning This function does not verify if the line has any
			/// active service
			/// @todo work only for departures table : rewrite inverted query
			////////////////////////////////////////////////////////////////////
			static bool GetIsAtLeastALineDisplayed(
				util::RegistryKeyType screenId
			);




			//////////////////////////////////////////////////////////////////////////
			/// Transform a serialized string into a line filter.
			/// @param text the serialized string
			/// @return the corresponding LineFilter
			/// @author Hugues Romain
			/// @date 2011
			/// @since 3.3.0
			static LineFilter UnserializeLineFilter(
				const std::string& text,
				util::Env& env,
				util::LinkLevel linkLevel = util::UP_LINKS_LOAD_LEVEL
			);



			//////////////////////////////////////////////////////////////////////////
			/// Transform a line filter into a serialized string.
			/// @param value the LineFilter
			/// @return the serialized string
			/// @author Hugues Romain
			/// @date 2011
			/// @since 3.3.0
			static std::string SerializeLineFilter(const LineFilter& value);
		};
	}
}

#endif // SYNTHESE_DisplayScreenTableSync_H__

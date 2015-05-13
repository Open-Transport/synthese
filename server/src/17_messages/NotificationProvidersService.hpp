/** NotificationProvidersService class header.
	@file NotificationProvidersService.hpp
	@author yves.martin
	@date 2015

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2015 Hugues Romain - RCSmobility <contact@rcsmobility.com>

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

#ifndef SYNTHESE_messages_NotificationProvidersService_hpp__
#define SYNTHESE_messages_NotificationProvidersService_hpp__

#include <NotificationProvider.hpp>
#include <FactorableTemplate.h>
#include <Function.h>
#include <ParametersMap.h>

#include <iostream>
#include <string>


namespace synthese
{
	namespace server {
		class Request;
		class Session;
	}

	namespace messages
	{
		/**
			17.15 Function : NotificationProvidersService.
			TODO See https://projects.open-transport.org/projects/terminus/wiki/NotificationProviders
			@ingroup m17Functions refFunctions
			@author yves.martin
			@date 2015
			@since TODO

			This "notification_providers" service provides support to
			- list channels aka provider keys / protocols
			- list notification provider registered instances
			- create or update a notification provider instance
		*/
		class NotificationProvidersService:
			public util::FactorableTemplate<server::Function,NotificationProvidersService>
		{
		public:
			/// Notification provider service tag
			static const std::string TAG_NOTIFICATION_PROVIDER;

			/// Service parameter to list channels,
			/// aka notification provider key (previously known as protocol)
			static const std::string PARAMETER_LIST_CHANNELS;

			/// Channel tag with provider instances to list their key
			static const std::string TAG_CHANNEL;


		protected:
			//! \name Page parameters
			//@{
				bool _listChannels;
				const NotificationProvider* _notificationProvider;
			//@}


			//////////////////////////////////////////////////////////////////////////
			/// Conversion from attributes to generic parameter maps.
			/// See https://extranet.rcsmobility.com/projects/synthese/wiki/NotificationProviders#Request
			//////////////////////////////////////////////////////////////////////////
			///	@return Generated parameters map
			/// @author yves.martin
			/// @date 2015
			util::ParametersMap _getParametersMap() const;



			//////////////////////////////////////////////////////////////////////////
			/// Conversion from generic parameters map to attributes.
			/// See https://extranet.rcsmobility.com/projects/synthese/wiki/NotificationProviders#Request
			//////////////////////////////////////////////////////////////////////////
			///	@param map Parameters map to interpret
			/// @author yves.martin
			/// @date 2015
			virtual void _setFromParametersMap(
				const util::ParametersMap& map
			);


		public:
			NotificationProvidersService();

			//! @name Setters
			//@{
			//@}



			//////////////////////////////////////////////////////////////////////////
			/// Display of the content generated by the function.
			/// @param stream Stream to display the content on.
			/// @param request the current request
			/// @author yves.martin
			/// @date 2015
			virtual util::ParametersMap run(std::ostream& stream, const server::Request& request) const;



			//////////////////////////////////////////////////////////////////////////
			/// Gets if the function can be run according to the user of the session.
			/// @param session the current session
			/// @return true if the function can be run
			/// @author yves.martin
			/// @date 2015
			virtual bool isAuthorized(const server::Session* session) const;



			//////////////////////////////////////////////////////////////////////////
			/// Gets the Mime type of the content generated by the function.
			/// @return the Mime type of the content generated by the function
			/// @author yves.martin
			/// @date 2015
			virtual std::string getOutputMimeType() const;
		};
}	}



#endif // SYNTHESE_messages_NotificationProvidersService_hpp__

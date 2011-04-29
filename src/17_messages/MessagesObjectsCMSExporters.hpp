
/** MessagesObjectsCMSExporters class header.
	@file MessagesObjectsCMSExporters.hpp

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

#ifndef SYNTHESE_messages_MessagesObjectsCMSExporters_hpp__
#define SYNTHESE_messages_MessagesObjectsCMSExporters_hpp__

#include <string>
#include <ostream>
#include <boost/shared_ptr.hpp>

namespace synthese
{
	namespace server
	{
		class Request;
	}

	namespace cms
	{
		class Webpage;
	}

	namespace messages
	{
		class Alarm;

		/** MessagesObjectsCMSExporters class.
			@ingroup m17
		*/
		class MessagesObjectsCMSExporters
		{
		private:

			static const std::string DATA_CONTENT;
			static const std::string DATA_PRIORITY;
			static const std::string DATA_SCENARIO_ID;
			static const std::string DATA_SCENARIO_NAME;
			static const std::string DATA_TITLE;

		public:

			//////////////////////////////////////////////////////////////////////////
			/// Message display.
			/// See https://extranet-rcsmobility.com/projects/synthese/wiki/Messages_in_CMS
			//////////////////////////////////////////////////////////////////////////
			/// @param stream stream to write the result on
			/// @param request current request
			/// @param page CMS template to use for the display
			/// @param message message to display
			/// @author Hugues Romain
			/// @date 2010
			/// @since 3.2.0
			static void DisplayMessage(
				std::ostream& stream,
				const server::Request& request,
				boost::shared_ptr<const cms::Webpage> page,
				const Alarm& message
			);
		};
}	}

#endif // SYNTHESE_messages_MessagesObjectsCMSExporters_hpp__


/** JourneyBoardJunctionCellInterfacePage class header.
	@file JourneyBoardJunctionCellInterfacePage.h

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

#ifndef SYNTHESE_JourneyBoardJunctionCellInterfacePage_H__
#define SYNTHESE_JourneyBoardJunctionCellInterfacePage_H__


#include "11_interfaces/InterfacePage.h"

namespace synthese
{
	namespace env
	{
		class ConnectionPlace;
	}

	namespace messages
	{
		class SentAlarm;
	}

	namespace routeplanner
	{
		/** Journey board cell for use of a junction.
			@code journey_board_junction_cell @endcode
			@ingroup m33Pages refPages
		*/
		class JourneyBoardJunctionCellInterfacePage : public interfaces::InterfacePage
		{
		public:
			/** Display.
				@param place (0) Arr?t ? rejoindre ? pied</th><th>Index de l'arr?t
				@param alarm (1/2) Message d'alerte de l'arr?t ? rejoindre ? pied (RIEN = Pas d'alerte)
				@param site Displayed site
			*/
			void display( std::ostream& stream
				, const synthese::env::ConnectionPlace* place
				, const messages::SentAlarm* alarm
				, bool color
				, const server::Request* request = NULL ) const;

		};
	}
}

#endif // SYNTHESE_JourneyBoardJunctionCellInterfacePage_H__

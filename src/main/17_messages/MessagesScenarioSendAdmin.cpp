
/** MessagesScenarioSendAdmin class implementation.
	@file MessagesScenarioSendAdmin.cpp

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

#include "MessagesScenarioSendAdmin.h"

using namespace std;

namespace synthese
{
	using namespace admin;
	using namespace interfaces;
	using namespace server;

	namespace messages
	{
		/// @todo Verify the parent constructor parameters
		MessagesScenarioSendAdmin::MessagesScenarioSendAdmin()
			: AdminInterfaceElement("superior", AdminInterfaceElement::EVER_DISPLAYED) {}

		void MessagesScenarioSendAdmin::setFromParametersMap(const AdminRequest::ParametersMap& map)
		{
			/// @todo Initialize internal attributes from the map
		}

		string MessagesScenarioSendAdmin::getTitle() const
		{
			/// @todo Change the title of the page
			return "title";
		}

		void MessagesScenarioSendAdmin::display(ostream& stream, const AdminRequest* request) const
		{
			/// @todo Implement the display by streaming the output to the stream variable
		}
	}
}

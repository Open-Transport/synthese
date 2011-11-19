
/** HTMLList class header.
	@file HTMLList.h

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

#ifndef SYNTHESE_html_HTMLList_h__
#define SYNTHESE_html_HTMLList_h__

#include <string>

namespace synthese
{
	namespace html
	{
		/** HTMLList class.
			@ingroup m05
		*/
		class HTMLList
		{
		protected:
			bool _element;

		public:
			HTMLList();

			std::string open();
			std::string closeElement();
			std::string element(std::string className = std::string());
			std::string close();

		};
	}
}

#endif // SYNTHESE_html_HTMLList_h__

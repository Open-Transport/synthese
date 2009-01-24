
/** CalendarTemplate class implementation.
	@file CalendarTemplate.cpp

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

#include "CalendarTemplate.h"

using namespace std;

namespace synthese
{
	using namespace env;

	namespace timetables
	{
		Calendar CalendarTemplate::getCalendar( const env::Calendar& mask ) const
		{
			Calendar result;
			for (vector<CalendarTemplateElement>::const_iterator it(_elements.begin()); it != _elements.end(); ++it)
			{
				if (it->getPositive())
					result |= it->getCalendar(mask);
				else
					result.subDates(it->getCalendar(mask));
			}
			return result;
		}



		std::string CalendarTemplate::getText() const
		{
			return _text;
		}



		void CalendarTemplate::setText( const std::string& text )
		{
			_text = text;
		}



		CalendarTemplate::CalendarTemplate()
			: util::Registrable<uid,CalendarTemplate>()
		{

		}



		void CalendarTemplate::addElement( const CalendarTemplateElement& element )
		{
			_elements.insert(_elements.begin() + element.getRank(), element);
		}
	}
}


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

#include <boost/foreach.hpp>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace time;
	using namespace util;

	namespace util
	{
		template<> const std::string Registry<timetables::CalendarTemplate>::KEY("CalendarTemplate");
	}

	namespace timetables
	{
		Calendar CalendarTemplate::getCalendar(
			const Calendar& mask
		) const {
			Calendar result;
			BOOST_FOREACH(const CalendarTemplateElement& element, _elements)
			{
				if (element.getPositive())
				{
					result |= element.getCalendar(mask);
				} else {
					result.subDates(element.getCalendar(mask));
				}
			}
			return result;
		}



		string CalendarTemplate::getText() const
		{
			return _text;
		}



		void CalendarTemplate::setText(
			const string& text
		){
			_text = text;
		}



		CalendarTemplate::CalendarTemplate(
			RegistryKeyType id
		):	util::Registrable(id)
		{

		}



		void CalendarTemplate::addElement( const CalendarTemplateElement& element )
		{
			_elements.insert(_elements.begin() + element.getRank(), element);
		}
	}
}

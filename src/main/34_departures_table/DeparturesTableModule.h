
/** DeparturesTableModule class header.
	@file DeparturesTableModule.h

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

#ifndef SYNTHESE_DeparturesTableModule_H__
#define SYNTHESE_DeparturesTableModule_H__

#include <functional>
#include <utility>
#include <vector>
#include <map>
#include <set>

#include "01_util/ModuleClass.h"

#include "04_time/DateTime.h"

/** @defgroup m34 Arrival/departures tables service module.
@{
*/

namespace synthese
{
	namespace env
	{
		class LineStop;
		class ConnectionPlace;
		class PhysicalStop;
		class Line;
	}
	namespace departurestable
	{
		class DeparturesTableModule : public util::ModuleClass
		{
		public:
			typedef std::set<const env::ConnectionPlace*> DisplayedPlacesList;
			typedef std::set<const env::PhysicalStop*> PhysicalStopsList;
			typedef std::set<const env::Line*> LineFilter;
			typedef std::set<const env::ConnectionPlace*> ForbiddenPlacesList;
			typedef enum { DISPLAY_ARRIVALS, DISPLAY_DEPARTURES } Direction;
			typedef enum { ENDS_ONLY, WITH_PASSING } EndFilter;
			struct Element { const env::LineStop* linestop; int serviceNumber; time::DateTime realDepartureTime; };
			struct ElementLess : public std::binary_function<Element, Element, bool>
			{
				bool operator()(const Element& _Left, const Element& _Right) const
				{
					return (_Left.realDepartureTime < _Right.realDepartureTime
						|| _Left.realDepartureTime == _Right.realDepartureTime 
						&& _Left.linestop != _Right.linestop
						);
				}
			};
			typedef std::vector<const env::ConnectionPlace*> ActualDisplayedArrivalsList;
			typedef std::pair<Element, ActualDisplayedArrivalsList> ArrivalDepartureRow;
			typedef std::map<Element, ActualDisplayedArrivalsList, ElementLess> ArrivalDepartureList;
		};
	}
}

/** @} */

#endif // SYNTHESE_DeparturesTableModule_H__

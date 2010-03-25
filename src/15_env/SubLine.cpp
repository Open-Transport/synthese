

/** SubLine class implementation.
	@file SubLine.cpp

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

#include "SubLine.h"
#include "Conversion.h"
#include "Edge.h"
#include "LineStop.h"
#include "PathGroup.h"

using namespace std;

namespace synthese
{
	using namespace util;
	using namespace graph;
	using namespace pt;

	namespace env
	{


		SubLine::SubLine( Line* line )
			: Line()
			, _mainLine(line),
			Registrable(UNKNOWN_VALUE)
		{
			// Registration
			int rank(line->addSubLine(this));

			// Copy of the properties
			setCommercialLine(const_cast<CommercialLine*>(line->getCommercialLine()));
			setRollingStock(line->getRollingStock());
			setName(line->getName() + " (subline " + Conversion::ToString(rank) + ")");
			setTimetableName(line->getTimetableName());
			setDirection(line->getDirection());
			setWalkingLine(line->getWalkingLine());
			setUseInDepartureBoards(line->getUseInDepartureBoards());
			setUseInRoutePlanning(line->getUseInRoutePlanning());
			setUseInTimetables(line->getUseInTimetables());

			// Copy of the line-stops
			const Path::Edges edges(line->getEdges());
			for (Path::Edges::const_iterator it(edges.begin()); it != edges.end(); ++it)
			{
				Edge* newEdge(new LineStop(
						UNKNOWN_VALUE
						, this
						, (*it)->getRankInPath()
						, (*it)->isDeparture()
						, (*it)->isArrival()
						, (*it)->getMetricOffset()
						, const_cast<PhysicalStop*>(static_cast<const LineStop*>(*it)->getPhysicalStop())
				)	);
				addEdge(newEdge);
			}

			_pathGroup->addPath(this);
		}

		bool SubLine::addServiceIfCompatible( Service* service )
		{
			if (!respectsLineTheory(false, *service))
				return false;

			Path::addService(service, false);
			
			return true;
		}

		SubLine::~SubLine()
		{
			for (Path::Edges::iterator it(_edges.begin()); it != _edges.end(); ++it)
				delete *it;
			assert(_pathGroup);
			_pathGroup->removePath(this);
		}

		Line* SubLine::getMainLine() const
		{
			return _mainLine;
		}
	}
}


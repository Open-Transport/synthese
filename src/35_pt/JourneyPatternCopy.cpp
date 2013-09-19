
/** JourneyPatternCopy class implementation.
	@file JourneyPatternCopy.cpp

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

#include "JourneyPatternCopy.hpp"
#include "DesignatedLinePhysicalStop.hpp"
#include "LineArea.hpp"
#include "PathGroup.h"
#include "Service.h"

#include <boost/lexical_cast.hpp>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace util;
	using namespace graph;
	using namespace pt;

	namespace pt
	{
		JourneyPatternCopy::JourneyPatternCopy(
			JourneyPattern& line
		):	Registrable(line.getKey()),
			JourneyPattern(line.getKey()),
			_mainLine(&line)
		{
			// Registration
			int rank(line.addSubLine(this));

			// Copy of the properties
			setCommercialLine(const_cast<CommercialLine*>(line.getCommercialLine()));
			setNetwork(line.getNetwork());
			setRollingStock(line.getRollingStock());
			setName(line.getName() + " (subline " + lexical_cast<string>(rank) + ")");
			setTimetableName(line.getTimetableName());
			setDirection(line.getDirection());
			setWalkingLine(line.getWalkingLine());
			setWayBack(line.getWayBack());
			setRules(line.getRules());
			setDirectionObj(line.getDirectionObj());

			// Copy of the line-stops
			const Path::Edges edges(line.getEdges());
			for (Path::Edges::const_iterator it(edges.begin()); it != edges.end(); ++it)
			{
				if(dynamic_cast<const DesignatedLinePhysicalStop*>(*it))
				{
					const DesignatedLinePhysicalStop& lineStop(static_cast<const DesignatedLinePhysicalStop&>(**it));
					DesignatedLinePhysicalStop* newEdge(
						new DesignatedLinePhysicalStop(
							0,
							this,
							lineStop.getRankInPath(),
							lineStop.isDeparture(),
							lineStop.isArrival(),
							lineStop.getMetricOffset(),
							lineStop.getPhysicalStop(),
							lineStop.getScheduleInput()
					)	);
					newEdge->setGeometry(lineStop.getGeometry());
					addEdge(*newEdge);
				}
				if(dynamic_cast<const LineArea*>(*it))
				{
					const LineArea& lineStop(static_cast<const LineArea&>(**it));
					Edge* newEdge(
						new LineArea(
							0,
							this,
							lineStop.getRankInPath(),
							lineStop.isDeparture(),
							lineStop.isArrival(),
							lineStop.getMetricOffset(),
							lineStop.getArea(),
							lineStop.getInternalService()
					)	);
					addEdge(*newEdge);
				}
			}

			if(_pathGroup)
			{
				_pathGroup->addPath(this);
			}
		}



		bool JourneyPatternCopy::addServiceIfCompatible(
			Service& service
		){
			if (!respectsLineTheory(service))
			{
				return false;
			}

			Path::addService(service, false);
			service.setPath(this);

			return true;
		}



		JourneyPatternCopy::~JourneyPatternCopy()
		{
			for (Path::Edges::iterator it(_edges.begin()); it != _edges.end(); ++it)
			{
				delete *it;
			}
			assert(_pathGroup);
			_pathGroup->removePath(this);
		}
}	}

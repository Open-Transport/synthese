
/** PhysicalStopsCSVExportFunction class implementation.
	@file PhysicalStopsCSVExportFunction.cpp
	@author Hugues
	@date 2010

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

#include "RequestException.h"
#include "Request.h"
#include "PhysicalStopsCSVExportFunction.h"
#include "StopPoint.hpp"
#include "StopArea.hpp"
#include "City.h"
#include "CoordinatesSystem.hpp"

#include <geos/geom/Envelope.h>
#include <sstream>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

using namespace std;
using namespace boost;
using namespace geos::geom;

namespace synthese
{
	using namespace util;
	using namespace server;
	using namespace security;
	using namespace pt;

	template<> const string util::FactorableTemplate<Function,pt::PhysicalStopsCSVExportFunction>::FACTORY_KEY("PhysicalStopsCSVExportFunction");

	namespace pt
	{
		const string PhysicalStopsCSVExportFunction::PARAMETER_BBOX("bbox");
		const string PhysicalStopsCSVExportFunction::PARAMETER_SRID("srid");

		ParametersMap PhysicalStopsCSVExportFunction::_getParametersMap() const
		{
			ParametersMap map;
			if(_bbox)
			{
				stringstream s;
				s << _bbox->getMinX() << "," << _bbox->getMinY() << "," <<
					_bbox->getMaxX() << "," << _bbox->getMaxY();
				map.insert(PARAMETER_BBOX, s.str());
			}
			if(_coordinatesSystem)
			{
				map.insert(PARAMETER_SRID, static_cast<int>(_coordinatesSystem->getSRID()));
			}
			return map;
		}

		void PhysicalStopsCSVExportFunction::_setFromParametersMap(const ParametersMap& map)
		{
			CoordinatesSystem::SRID srid(map.getDefault<CoordinatesSystem::SRID>(PARAMETER_SRID, CoordinatesSystem::GetInstanceCoordinatesSystem().getSRID()));
			_coordinatesSystem = &CoordinatesSystem::GetCoordinatesSystem(srid);

			string bbox(map.getDefault<string>(PARAMETER_BBOX));
			if(!bbox.empty())
			{
				vector< string > parsed_bbox;
				split(parsed_bbox, bbox, is_any_of(",; ") );

				if(parsed_bbox.size() != 4)
				{
					throw RequestException("Malformed bbox.");
				}

				boost::shared_ptr<Point> pt1(
					_coordinatesSystem->createPoint(lexical_cast<double>(parsed_bbox[0]), lexical_cast<double>(parsed_bbox[1]))
				);
				boost::shared_ptr<Point> pt2(
					_coordinatesSystem->createPoint(lexical_cast<double>(parsed_bbox[2]), lexical_cast<double>(parsed_bbox[3]))
				);
				pt1 = CoordinatesSystem::GetInstanceCoordinatesSystem().convertPoint(*pt1);
				pt2 = CoordinatesSystem::GetInstanceCoordinatesSystem().convertPoint(*pt2);

				_bbox = Envelope(
					pt1->getX(),
					pt2->getX(),
					pt1->getY(),
					pt2->getY()
				);
			}
		}



		util::ParametersMap PhysicalStopsCSVExportFunction::run(
			std::ostream& stream,
			const Request& request
		) const	{

			util::ParametersMap pm;

			stream << fixed;
			BOOST_FOREACH(Registry<StopPoint>::value_type itps, Env::GetOfficialEnv().getRegistry<StopPoint>())
			{
				if(!itps.second.get()) continue;

				const StopPoint& ps(*itps.second);

				if(	_bbox &&
					(	!ps.hasGeometry() ||
						!_bbox->contains(*ps.getGeometry()->getCoordinate())
				)	){
					continue;
				}

				boost::shared_ptr<Point> pts(
					_coordinatesSystem->convertPoint(*ps.getGeometry())
				);

				stream <<
					ps.getKey() << ";" <<
					ps.getCodeBySources() << ";" <<
					pts->getX() << ";" <<
					pts->getY() << ";" <<
					"\"" << ps.getConnectionPlace()->getCity()->getName() << "\";" <<
					"\"" << ps.getConnectionPlace()->getName() << "\";" <<
					"\"" << ps.getName() << "\";" <<
					"\"" << ps.getConnectionPlace()->getName26OrName() << "\"" <<
					endl
				;
			}

			return pm;
		}



		bool PhysicalStopsCSVExportFunction::isAuthorized(const Session* session) const
		{
			return true;
		}



		std::string PhysicalStopsCSVExportFunction::getOutputMimeType() const
		{
			return "text/csv";
		}



		std::string PhysicalStopsCSVExportFunction::getFileName() const
		{
			stringstream result;
			posix_time::ptime now(posix_time::second_clock::local_time());
			result << "physical_stops_" << posix_time::to_iso_string(now) << ".csv";
			return result.str();
		}
	}
}

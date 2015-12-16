
/** PublicBikeStationsListFunction class implementation.
	@file PublicBikeStationsListFunction.cpp
	@author Camille Hue
	@date 2015

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

#include "PublicBikeStationsList.hpp"

#include "CoordinatesSystem.hpp"
#include "DataSource.h"
#include "PublicBikeStation.hpp"
#include "RequestException.h"

#include <sstream>
#include <boost/algorithm/string/split.hpp>
#include <geos/geom/Point.h>

using namespace std;
using namespace geos::geom;
using namespace boost;
using namespace boost::algorithm;

namespace synthese
{
	template<> const string util::FactorableTemplate<server::Function,public_biking::PublicBikeStationsListFunction>::FACTORY_KEY("PublicBikeStationsListFunction");

	namespace public_biking
	{
		const string PublicBikeStationsListFunction::PARAMETER_BBOX = "bbox";
		const string PublicBikeStationsListFunction::PARAMETER_DATA_SOURCE_FILTER = "data_source_filter";
		const string PublicBikeStationsListFunction::PARAMETER_MAX_SOLUTIONS_NUMBER = "max_solution_number";
		const string PublicBikeStationsListFunction::PARAMETER_SORT_BY_DISTANCE_TO_BBOX_CENTER = "sort_by_distance";
		const string PublicBikeStationsListFunction::PARAMETER_SRID = "srid";

		const string PublicBikeStationsListFunction::TAG_PUBLIC_BIKE_STATION = "public_bike_station";
		const string PublicBikeStationsListFunction::TAG_PUBLIC_BIKE_STATIONS = "public_bike_stations";
		const string PublicBikeStationsListFunction::DATA_DISTANCE_TO_BBOX_CENTER = "distanceToBboxCenter";

		server::FunctionAPI PublicBikeStationsListFunction::getAPI() const
		{
			server::FunctionAPI api(
				"Public Biking",
				"Returns the list of public bike stations for a bounding box",
				"");
			api.openParamGroup("Filter");
			api.addParams(PARAMETER_BBOX,
				"A bounding box of the form 'x1,y1,x2,y2'", false);
			api.addParams(PARAMETER_SRID, "", false);
			api.addParams(PARAMETER_SORT_BY_DISTANCE_TO_BBOX_CENTER, "", false);
			api.addParams(PARAMETER_MAX_SOLUTIONS_NUMBER,
				"Max number of solution", false);
			return api;
		}


		util::ParametersMap PublicBikeStationsListFunction::_getParametersMap() const
		{
			util::ParametersMap map;
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

			// Max solutions number
			if(_maxSolutionsNumber)
			{
				map.insert(PARAMETER_MAX_SOLUTIONS_NUMBER, *_maxSolutionsNumber);
			}
			
			// dataSourceFilter
			if(_dataSourceFilter)
			{
				map.insert(PARAMETER_DATA_SOURCE_FILTER, _dataSourceFilter->getKey());
			}

			return map;
		}



		void PublicBikeStationsListFunction::_setFromParametersMap(const util::ParametersMap& map)
		{
			// Max solutions number
			_maxSolutionsNumber = map.getOptional<size_t>(PARAMETER_MAX_SOLUTIONS_NUMBER);

			// Coordinate system
			CoordinatesSystem::SRID srid(
				map.getDefault<CoordinatesSystem::SRID>(
					PARAMETER_SRID,
					CoordinatesSystem::GetInstanceCoordinatesSystem().getSRID()
			)	);
			_coordinatesSystem = &CoordinatesSystem::GetCoordinatesSystem(srid);

			// BBox selection request
			string bbox(map.getDefault<string>(PARAMETER_BBOX));
			if(!bbox.empty())
			{
				vector< string > parsed_bbox;
				split(parsed_bbox, bbox, is_any_of(",; ") );

				if(parsed_bbox.size() != 4)
				{
					throw server::RequestException("Malformed bbox.");
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

			Function::setOutputFormatFromMap(map,string());

			// Sort by distance to bbox center
			_isSortByDistanceToBboxCenter = false;
			optional<string> sortValueStr(map.getOptional<string>(PARAMETER_SORT_BY_DISTANCE_TO_BBOX_CENTER));
			if(sortValueStr && (*sortValueStr) == "1" && _bbox)
			{
				_isSortByDistanceToBboxCenter = true;
			}

			if(map.getOptional<util::RegistryKeyType>(PARAMETER_DATA_SOURCE_FILTER)) try
			{
				_dataSourceFilter = util::Env::GetOfficialEnv().get<impex::DataSource>(map.get<util::RegistryKeyType>(PARAMETER_DATA_SOURCE_FILTER));
			}
			catch (util::ObjectNotFoundException<impex::DataSource>&)
			{
				throw server::RequestException("No such data source");
			}
		}



		util::ParametersMap PublicBikeStationsListFunction::run(
			std::ostream& stream,
			const server::Request& request
		) const {

			PublicBikeStationSetType publicBikeStationSetType;
			BOOST_FOREACH(const util::Registry<PublicBikeStation>::value_type& publicBikeStation, util::Env::GetOfficialEnv().getRegistry<PublicBikeStation>())
			{
				if((_bbox &&
					(!publicBikeStation.second->getGeometry() ||
					!_bbox->contains(*publicBikeStation.second->getGeometry()->getCoordinate()))) ||
					(_dataSourceFilter && !publicBikeStation.second->hasLinkWithSource(*_dataSourceFilter))
				){
					continue;
				}

				int distanceToBboxCenter = CalcDistanceToBboxCenter(*publicBikeStation.second);
				SortablePublicBikeStation sortablePublicBikeStation(&(*publicBikeStation.second),distanceToBboxCenter,_isSortByDistanceToBboxCenter);

				publicBikeStationSetType.insert(sortablePublicBikeStation);
			}

			// Filling in the result parameters map
			util::ParametersMap pm;
			size_t nbStations = 0;
			BOOST_FOREACH(const PublicBikeStationSetType::value_type& pbs, publicBikeStationSetType)
			{
				// Declarations
				boost::shared_ptr<util::ParametersMap> publicBikeStationPM(new util::ParametersMap);

				// Main attributes
				pbs.getPublicBikeStation()->toParametersMap(
					*publicBikeStationPM,
					true,
					*_coordinatesSystem
				);

				// Distance to bbox center
				if (_isSortByDistanceToBboxCenter)
				{
					int distanceToBboxCenter = pbs.getDistanceToBboxCenter();
					publicBikeStationPM->insert(DATA_DISTANCE_TO_BBOX_CENTER, distanceToBboxCenter);
				}
				nbStations++;

				if (_maxSolutionsNumber && nbStations >= *_maxSolutionsNumber)
				{
					break;
				}
			}

			if(_outputFormat == util::MimeTypes::XML)
			{
				pm.outputXML(
					stream,
					TAG_PUBLIC_BIKE_STATIONS,
					true
				);
			}
			else if(_outputFormat == util::MimeTypes::JSON)
			{
				pm.outputJSON(stream, TAG_PUBLIC_BIKE_STATIONS);
			}

			return pm;
		}



		//Sort stopPoint by distance to bbox,or by code operator.Sort by code operator is applied by default. 
		PublicBikeStationsListFunction::SortablePublicBikeStation::SortablePublicBikeStation(const PublicBikeStation * pbs, int distanceToBboxCenter, bool isSortByDistanceToBboxCenter):
			_pbs(pbs),
			_distanceToBboxCenter(distanceToBboxCenter),
			_isSortByDistanceToBboxCenter(isSortByDistanceToBboxCenter)
		{
		}



		bool PublicBikeStationsListFunction::SortablePublicBikeStation::operator<(SortablePublicBikeStation const &otherPublicBikeStation) const
		{
			if(	_isSortByDistanceToBboxCenter &&
				_distanceToBboxCenter != otherPublicBikeStation.getDistanceToBboxCenter()
			){
				return _distanceToBboxCenter < otherPublicBikeStation.getDistanceToBboxCenter();
			}

			return _pbs < otherPublicBikeStation._pbs;
		}



		int PublicBikeStationsListFunction::SortablePublicBikeStation::getDistanceToBboxCenter() const
		{
			return _distanceToBboxCenter;
		}



		const PublicBikeStation* PublicBikeStationsListFunction::SortablePublicBikeStation::getPublicBikeStation() const
		{
			return _pbs;
		}



		bool PublicBikeStationsListFunction::isAuthorized(
			const server::Session* session
		) const {
			return true;
		}



		int PublicBikeStationsListFunction::CalcDistanceToBboxCenter(const PublicBikeStation & publicBikeStation) const
		{
			//return value
			int distanceToBboxCenter = 0;

			if(_bbox)
			{
				boost::shared_ptr<Point> gp = publicBikeStation.getGeometry();

				// Coordinates of bbox center
				double xCenter = (_bbox->getMaxX() + _bbox->getMinX()) / 2.0; 
				double yCenter = (_bbox->getMaxY() + _bbox->getMinY()) / 2.0; 

				if(gp.get())
				{
					distanceToBboxCenter = sqrt((gp->getX() - xCenter) * (gp->getX() - xCenter)+(gp->getY() - yCenter) * (gp->getY() - yCenter));
				}
			}

			//return value
			return static_cast<int>(round(distanceToBboxCenter));
		}

		std::string PublicBikeStationsListFunction::getOutputMimeType() const
		{
			return _outputFormat;
		}
}	}

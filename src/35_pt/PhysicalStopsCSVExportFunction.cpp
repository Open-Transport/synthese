
/** PhysicalStopsCSVExportFunction class implementation.
	@file PhysicalStopsCSVExportFunction.cpp
	@author Hugues
	@date 2010

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

#include "RequestException.h"
#include "Request.h"
#include "PhysicalStopsCSVExportFunction.h"
#include "PhysicalStop.h"
#include "PublicTransportStopZoneConnectionPlace.h"
#include "City.h"

#include <sstream>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace util;
	using namespace server;
	using namespace security;
	using namespace geometry;
	using namespace pt;

	template<> const string util::FactorableTemplate<Function,pt::PhysicalStopsCSVExportFunction>::FACTORY_KEY("PhysicalStopsCSVExportFunction");
	
	namespace pt
	{
		const string PhysicalStopsCSVExportFunction::PARAMETER_BBOX("bbox");
		
		ParametersMap PhysicalStopsCSVExportFunction::_getParametersMap() const
		{
			ParametersMap map;
			if(_bbox)
			{
				stringstream s;
				s << _bbox->first.getX() << "," << _bbox->first.getY() << "," <<
					_bbox->second.getX() << "," << _bbox->second.getY();
				map.insert(PARAMETER_BBOX, s.str());
			}
			return map;
		}

		void PhysicalStopsCSVExportFunction::_setFromParametersMap(const ParametersMap& map)
		{
			string bbox(map.getDefault<string>(PARAMETER_BBOX));
			if(!bbox.empty())
			{
				vector< string > parsed_bbox;
				split(parsed_bbox, bbox, is_any_of(",; ") );
				
				if(parsed_bbox.size() != 4)
				{
					throw RequestException("Malformed bbox.");
				}

				_bbox = make_pair(
					Point2D(lexical_cast<double>(parsed_bbox[0]), lexical_cast<double>(parsed_bbox[1])),
					Point2D(lexical_cast<double>(parsed_bbox[2]), lexical_cast<double>(parsed_bbox[3]))
				);
			}
		}

		void PhysicalStopsCSVExportFunction::run(
			std::ostream& stream,
			const Request& request
		) const	{
			
			stream << fixed;
			BOOST_FOREACH(Registry<PhysicalStop>::value_type itps, Env::GetOfficialEnv().getRegistry<PhysicalStop>())
			{
				if(!itps.second.get()) continue;

				const PhysicalStop& ps(*itps.second);

				if(	_bbox &&
					(	ps.getX() < _bbox->first.getX() ||
						ps.getY() < _bbox->first.getY() ||
						ps.getX() > _bbox->second.getX() ||
						ps.getY() > _bbox->second.getY()
				)	){
					continue;
				}

				stream <<
					ps.getKey() << ";" <<
					ps.getCodeBySource() << ";" <<
					ps.getX() << ";" <<
					ps.getY() << ";" <<
					"\"" << ps.getConnectionPlace()->getCity()->getName() << "\";" <<
					"\"" << ps.getConnectionPlace()->getName() << "\";" <<
					"\"" << ps.getName() << "\";" <<
					"\"" << ps.getConnectionPlace()->getName26OrName() << "\"" <<
					endl
				;
			}
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


/** PTFileFormat class header.
	@file PTFileFormat.hpp

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

#ifndef SYNTHESE_pt_PTFileFormat_hpp__
#define SYNTHESE_pt_PTFileFormat_hpp__

#include "JourneyPattern.hpp"
#include "ImportableTableSync.hpp"
#include "StopPoint.hpp"

namespace synthese
{
	namespace util
	{
		class Env;
		struct RGBColor;
	}

	namespace geography
	{
		class City;
	}

	namespace pt
	{
		class TransportNetwork;
		class TransportNetworkTableSync;
		class StopArea;
		class StopAreaTableSync;
		class CommercialLine;
		class CommercialLineTableSync;
		class StopPointTableSync;


		/** PTFileFormat class.
			@ingroup m35
		*/
		class PTFileFormat
		{
		public:
			/// The created object is owned by the environment (it is not required to 
			/// maintain the returned shared pointer)
			static boost::shared_ptr<JourneyPattern> CreateJourneyPattern(
				const JourneyPattern::StopsWithDepartureArrivalAuthorization& stops,
				CommercialLine& line,
				const impex::DataSource& source,
				util::Env& env,
				std::ostream& logStream
			);


			//////////////////////////////////////////////////////////////////////////
			/// @return the created network object.
			/// The created object is owned by the environment (it is not required to 
			/// maintain the returned shared pointer)
			static TransportNetwork* CreateOrUpdateNetwork(
				impex::ImportableTableSync::ObjectBySource<TransportNetworkTableSync>& networks,
				const std::string& id,
				const std::string& name,
				const impex::DataSource& source,
				util::Env& env,
				std::ostream& logStream
			);



			//////////////////////////////////////////////////////////////////////////
			/// @return the created network object.
			/// The created object is owned by the environment (it is not required to 
			/// maintain the returned shared pointer)
			static CommercialLine* CreateOrUpdateLine(
				impex::ImportableTableSync::ObjectBySource<CommercialLineTableSync>& lines,
				const std::string& id,
				const std::string& name,
				const std::string& shortName,
				boost::optional<util::RGBColor> color,
				const TransportNetwork& network,
				const impex::DataSource& source,
				util::Env& env,
				std::ostream& logStream
			);



			static StopArea* CreateOrUpdateStopArea(
				impex::ImportableTableSync::ObjectBySource<StopAreaTableSync>& stopAreas,
				const std::string& id,
				const std::string& name,
				const geography::City& city,
				boost::posix_time::time_duration defaultTransferDuration,
				const impex::DataSource& source,
				util::Env& env,
				std::ostream& logStream
			);



			static StopPoint* CreateOrUpdateStopPoint(
				impex::ImportableTableSync::ObjectBySource<StopPointTableSync>& stopPoints,
				const std::string& id,
				const std::string& name,
				const StopArea& stopArea,
				const StopPoint::Geometry& geometry,
				const impex::DataSource& source,
				util::Env& env,
				std::ostream& logStream
			);
		};
	}
}

#endif // SYNTHESE_pt_PTFileFormat_hpp__

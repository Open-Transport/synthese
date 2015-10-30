
/** StopArea class header.
	@file StopArea.hpp

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

#ifndef SYNTHESE_pt_StopArea_hpp__
#define SYNTHESE_pt_StopArea_hpp__

#include "Object.hpp"

#include "City.h"
#include "CommercialLine.h"
#include "CoordinatesSystem.hpp"
#include "DataSourceLinksField.hpp"
#include "GeometryField.hpp"
#include "Hub.h"
#include "ImportableTemplate.hpp"
#include "NamedPlaceTemplate.h"
#include "PointerField.hpp"
#include "Registry.h"
#include "PointersVectorField.hpp"

#include <map>
#include <utility>
#include <boost/optional.hpp>

namespace synthese
{
	namespace util
	{
		class ParametersMap;
	}

	namespace road
	{
		class Address;
	}

	namespace graph
	{
		class Vertex;
	}

	namespace pt
	{
		class FreeDRTArea;
		class StopPoint;
		class DRTArea;

		FIELD_ID(CityId)
		FIELD_BOOL(ConnectionType)
		FIELD_BOOL(IsCityMainConnection)
		FIELD_INT(DefaultTransferDelay)
		FIELD_STRING(TransferDelays)
		FIELD_BOOL(IsRelayPark)
		FIELD_STRING(ShortDisplayName)
		FIELD_STRING(LongDisplayName)
		FIELD_DATASOURCE_LINKS(StopAreaDataSource)
		FIELD_STRING(StopAreaTimetableName)
		FIELD_DOUBLE(X)
		FIELD_DOUBLE(Y)

		typedef boost::fusion::map<
			FIELD(Key),
			FIELD(Name),
			FIELD(CityId),
			FIELD(ConnectionType),
			FIELD(IsCityMainConnection),
			FIELD(DefaultTransferDelay),
			FIELD(TransferDelays),
			FIELD(IsRelayPark),
			FIELD(ShortDisplayName),
			FIELD(LongDisplayName),
			FIELD(StopAreaDataSource),
			FIELD(StopAreaTimetableName),
			FIELD(HandicappedComplianceId),
			FIELD(X),
			FIELD(Y),
			FIELD(PointGeometry)
		> StopAreaSchema;

		//////////////////////////////////////////////////////////////////////////
		/// Stop area.
		///	@ingroup m35
		/// @author Hugues Romain
		//////////////////////////////////////////////////////////////////////////
		/// <h2>Use cases</h2>
		/// <h3>Routing</h3>
		/// The following example is a stop area with 2 railway platforms and 2 bus stops :
		/// @image html station-example1-0.png
		/// The railway platforms can be accessed from the road part by 3 points : the building main entrance, and two stairs located on each side of the street and giving access to a footbridge linked to the platforms by two ther stairs.
		///
		/// The next figure show how such a stop area can be modeled :
		/// @image html station-example1-1.png
		///	The target of the model is the routing algorithm, with 3 use cases :
		/// <ul>
		///	<li>find the entrance of the stop area from the roads (road to pt)</li>
		///	<li>go out of the stop area to the road (pt to road)</li>
		/// <li>transfers inside the stop area (pt to pt)</li>
		/// </ul>
		///
		///	The stop area can be reached from the roads by one of the A1 to A5 points (A=address) :
		/// <table class="table">
		/// <tr><th>code</th><th>description</th><th>class</th></tr>
		/// <tr><td>A1</td><td>Entrance of the station building</td><td>StationEntrance</td></tr>
		/// <tr><td>A2</td><td>Stairs 3</td><td>StationEntrance</td></tr>
		/// <tr><td>A3</td><td>@ref projection "Projection" of the bus stop</td><td>@ref road::Address "Address" embedded in StopPoint</td></tr>
		/// <tr><td>A4</td><td>@ref projection "Projection" of the bus stop 2</td><td>@ref road::Address "Address" embedded in StopPoint</td></tr>
		/// <tr><td>A5</td><td>Stairs 4</td><td>StationEntrance</td></tr>
		/// </table>
		///
		/// There are 4 S1 to S4 boarding positions (S=stop) :
		/// <ul>
		/// <li>railway platform 2</li>
		/// <li>railway platform 1</li>
		/// <li>bus stop 1</li>
		/// <li>bus stop 2</li>
		/// </ul>
		///
		/// Notice : the ways inside the stop area are not modeled (footbridge, aisle in the building, stairs 1 and 2, pavement...) : the stop area is described on a service point of view and not a geographical point of vue. Such a detail level is useless for routing.
		/// The stop area object must be able to give the possible links between two A or S points, and for each possible link the minimal duration to make the transfer.
		///
		/// The 3 routing use cases are handled by a transfer matrix :
		///
		///	1: Access to the stop area from the roads (from A1 to A5 points to S1 to S4 points)
		/// The following table shows an example corresponding to the previous topology example.
		/// <table class="table">
		///	<tr><th></th><th>S1</th><th>S2</th><th>S3</th><th>S4</th></tr>
		/// <tr><th>A1</th><td>3 min</td><td>1 min</td><td>forbidden</td><td>forbidden</td></tr>
		/// <tr><th>A2</th><td>3 min</td><td>2 min</td><td>forbidden</td><td>forbidden</td></tr>
		/// <tr><th>A3</th><td>forbidden</td><td>forbidden</td><td>0 min</td><td>forbidden</td></tr>
		/// <tr><th>A4</th><td>forbidden</td><td>forbidden</td><td>forbidden</td><td>0 min</td></tr>
		/// <tr><th>A5</th><td>3 min</td><td>3 min</td><td>forbidden</td><td>forbidden</td></tr>
		/// </table>
		///
		/// Notes :
		/// <ul>
		/// <li>The 0 minutes durations correspond to projections of physical stops on the nearest road.</li>
		/// <li>The other durations correspond to the needed time to reach the S points from the A points by the infrastructures of the stop area (stairs, footbridge...)</li>
		/// </ul>
		///
		/// 2: Access to the road from the stop area (from S1 to S4 points to A1 to A5 points)
		/// The following table corresponds to the reverse view of the previous one.
		/// <table class="table">
		/// <tr><th></th><th>A1</th><th>A2</th><th>A3</th><th>A4</th><th>A5</th></tr>
		/// <tr><th>S1</th><td>3 min</td><td>3 min</td><td>forbidden</td><td>forbidden</td><td>3 min</td></tr>
		/// <tr><th>S2</th><td>1 min</td><td>2 min</td><td>forbidden</td><td>forbidden</td><td>3 min</td></tr>
		/// <tr><th>S3</th><td>forbidden</td><td>forbidden</td><td>0 min</td><td>forbidden</td><td>forbidden</td></tr>
		/// <tr><th>S4</th><td>forbidden</td><td>forbidden</td><td>forbidden</td><td>0 min</td><td>forbidden</td></tr>
		/// </table>
		///
		/// 3: Transfer between physical stops (between S1 to S4 points)
		/// <table class="table">
		///	<tr><th></th><th>S1</th><th>S2</th><th>S3</th><th>S4</th></tr>
		/// <tr><th>S1</th><td>2 min</td><td>4 min</td><td>5 min</td><td>6 min</td></tr>
		/// <tr><th>S2</th><td>4 min</td><td>2 min</td><td>4 min</td><td>5 min</td></tr>
		/// <tr><th>S3</th><td>8 min</td><td>7 min</td><td>5 min</td><td>6 min</td></tr>
		/// <tr><th>S4</th><td>9 min</td><td>8 min</td><td>6 min</td><td>5 min</td></tr>
		/// </table>
		///
		/// The minimal durations of transfer are qualified by pair of stop, considering the time needed by the walk, but also other informations like probabilities of delay.
		/// This duration must be seen as a contractual data instead of a geographical data : the transport network is responsible of the transfer conditions, contrary to the access from the departure place to the first stop of a journey, which is under the customers responsibility.
		/// In most of cases, the durations include a security time defined by the transport mode arriving in the stop area, that is added to the time neeeded to walk between the stops. In the example, 2 min are added if the customer comes from the train line, 5 min for the bus line).
		///
		/// <h2>Conceptual data model</h2>
		///	@image html uml_stop_area.png
		///
		/// <h2>Physical data model</h2>
		/// @ref StopAreaTableSync
		///
		/// <h2>Object model</h2>
		/// For performance purpose, object model differs from conceptual model by
		/// the following issues :
		/// <ul>
		///	<li>StopPoint and StationEntrance lists are separated and are hosted
		/// by StopArea and not by Hub</li>
		/// <li>There is no Transfer class : a TransferDelaysMap is hosted by
		/// StopArea.</li>
		/// </ul>
		class StopArea:
			public Object<StopArea, StopAreaSchema>,
			public graph::Hub,
			public geography::NamedPlaceTemplate<StopArea>,
			public impex::ImportableTemplate<StopArea>
		{
		public:
			static const std::string DATA_STOP_ID;
			static const std::string DATA_STOP_NAME;
			static const std::string DATA_CITY_NAME;
			static const std::string DATA_STOP_NAME_13;
			static const std::string DATA_STOP_NAME_26;
			static const std::string DATA_STOP_NAME_FOR_TIMETABLES;

			struct Vector:
				public PointersVectorField<Vector, StopArea>
			{

			};

			typedef std::map<util::RegistryKeyType,const pt::StopPoint*> PhysicalStops;
			typedef std::vector<std::pair<boost::optional<util::RegistryKeyType>, std::string> > PhysicalStopsLabels;

			typedef std::map<
				std::pair<
					util::RegistryKeyType,
					util::RegistryKeyType
				>, boost::posix_time::time_duration
			> TransferDelaysMap;

			typedef std::set<DRTArea*> DRTAreas;

		private:

			//! @name Content
			//@{
				PhysicalStops			_physicalStops;
			//@}

			//! @name Transfer parameters
			//@{
				TransferDelaysMap		_transferDelays; //!< Transfer delays between vertices (in minutes)
				boost::posix_time::time_duration	_defaultTransferDelay;
			//@}

			//! @name Caching
			//@{
				mutable boost::optional<graph::HubScore> _score;
				mutable boost::posix_time::time_duration _minTransferDelay;
				mutable DRTAreas _drtAreas;
			//@}

			//! @Location
			//@{
				boost::shared_ptr<geos::geom::Point> _location;
			//@}


		public:

			StopArea(
				util::RegistryKeyType id = 0,
				bool allowedConnection = false,
				boost::posix_time::time_duration defaultTransferDelay = boost::posix_time::time_duration()
			);

			~StopArea();

			//! @name Getters
			//@{
				const PhysicalStops& getPhysicalStops() const { return _physicalStops; }
				bool getAllowedConnection() const { return get<ConnectionType>(); }
				boost::posix_time::time_duration	getDefaultTransferDelay() const { return _defaultTransferDelay; }
				const TransferDelaysMap& getTransferDelays() const { return _transferDelays; }
				const std::string& getTimetableName() const { return get<StopAreaTimetableName>(); }
				const boost::shared_ptr<geos::geom::Point>& getLocation() const { return _location; }
				const DRTAreas& getDRTAreas() const { return _drtAreas; }
				bool getIsRelayPark() const { return _isRelayPark; }
				virtual std::string getName() const;
			//@}

			//! @name Setters
			//@{
				void setDefaultTransferDelay(
					boost::posix_time::time_duration defaultTransferDelay
				);
				void setTimetableName(const std::string& value){ set<StopAreaTimetableName>(value); }
				void setAllowedConnection(bool value) { set<ConnectionType>(value); }
				void setLocation(const boost::shared_ptr<geos::geom::Point>& value);
				void setTransferDelaysMatrix(const TransferDelaysMap& value);
				void setIsRelayPark(bool value) { set<IsRelayPark>(value); }
				virtual void setName(const std::string& value);
				virtual void setName13(const std::string& value);
				virtual void setName26(const std::string& value);
				virtual void setCity(geography::City* value);
				virtual void setRules(const Rules& value);
			//@}

			//! @name Update methods.
			//@{
				void addPhysicalStop(const pt::StopPoint& physicalStop);
				void addDRTArea(const DRTArea& area);
				void removeDRTArea(const DRTArea& area);

				static void _addTransferDelay(
					TransferDelaysMap& map,
					TransferDelaysMap::key_type::first_type departure,
					TransferDelaysMap::key_type::second_type arrival,
					boost::posix_time::time_duration transferDelay
				);

				static void _addForbiddenTransferDelay(
					TransferDelaysMap& map,
					TransferDelaysMap::key_type::first_type departure,
					TransferDelaysMap::key_type::second_type arrival
				);

				void removePhysicalStop(const pt::StopPoint& physicalStop);
			//@}


			//! @name Virtual queries for Hub interface
			//@{
				virtual Vertices getVertices(
					graph::GraphIdType graphId
				) const;

				virtual boost::posix_time::time_duration	getMinTransferDelay() const;

				virtual bool isConnectionAllowed(
					const graph::Vertex& fromVertex,
					const graph::Vertex& toVertex
				) const;

				virtual boost::posix_time::time_duration getTransferDelay(
					const graph::Vertex& fromVertex,
					const graph::Vertex& toVertex
				) const;

				virtual bool isConnectionAllowed(
					util::RegistryKeyType fromId,
					util::RegistryKeyType toId
				) const;

				virtual boost::posix_time::time_duration getTransferDelay(
					util::RegistryKeyType fromId,
					util::RegistryKeyType toId
				) const;


				/** Score getter.
					@return int the score of the place
					@author Hugues Romain
					@date 2007

					The vertex score is calculated by the following way :
						- each commercial line gives some points, depending of the number of services which belongs to the line :
							- 1 to 10 services lines gives 2 point
							- 10 to 50 services lines gives 3 points
							- 50 to 100 services lines gives 4 points
							- much than 100 services lines gives 5 points
						- if the score is bigger than 100 points, then the score is 100
				*/
				virtual graph::HubScore getScore() const;

				virtual const boost::shared_ptr<geos::geom::Point>& getPoint() const;

				virtual bool isConnectionPossible() const;

				virtual std::string getNameForAllPlacesMatcher(
					std::string text = std::string()
				) const;

				virtual void getVertexAccessMap(
					graph::VertexAccessMap& result,
					graph::GraphIdType whatToSearch,
					const graph::Vertex& origin,
					bool vertexIsOrigin
				) const;



				virtual bool containsAnyVertex(graph::GraphIdType graphType) const;
			//@}



			//! @name Virtual queries for Place interface
			//@{
				virtual void getVertexAccessMap(
					graph::VertexAccessMap& result,
					const graph::AccessParameters& accessParameters,
					const geography::Place::GraphTypes& whatToSearch
				) const;
			//@}


			//! @name Services
			//@{
				virtual std::string getRuleUserName() const;

				/** getPhysicalStopLabels.
					@param withAll displays an item corresponding to all stops
					@param noStopLabel displays an item corresponding to no stop (if empty, no empty item)
					@return Physical stops labels
					@author Hugues Romain
					@date 2007
				*/
				PhysicalStopsLabels	getPhysicalStopLabels(
					bool withAll = false,
					std::string noStopLabel = std::string()
				) const;

				/** Labels list for select field containing physical stops, with exclusion list.
					@param noDisplay Physical stops to exclude
					@return Labels list for select field containing physical stops
					@author Hugues Romain
					@date 2007
				*/
				PhysicalStopsLabels	getPhysicalStopLabels(const PhysicalStops& noDisplay) const;


				typedef std::set<CommercialLine*, CommercialLine::PointerComparator> Lines;

				//////////////////////////////////////////////////////////////////////////
				/// List of the lines which call at a stop of the stop area.
				/// @param includeArrivals if true, the line with only arrival at a stop are also returned
				/// @return the list of the lines which call at a stop of the stop area
				/// @author Hugues Romain
				/// @since 3.3.0
				/// @date 2011
				Lines getLines(
					bool includeArrivals
				) const;



				//////////////////////////////////////////////////////////////////////////
				/// Exports the object into a parameters map.
				/// See https://extranet.rcsmobility.com/projects/synthese/wiki/Stop_Areas_in_CMS
				//////////////////////////////////////////////////////////////////////////
				/// @retval the parameters map to populate
				/// @param prefix prefix to add to the field names
				/// @author Hugues Romain
				/// @since 3.3.0
				/// @date 2011
				void toParametersMap(
					util::ParametersMap& pm,
					const CoordinatesSystem* coordinatesSystem,
					std::string prefix = std::string()
				) const;

				virtual void toParametersMap(
					util::ParametersMap& pm,
					bool withAdditionalParameters,
					boost::logic::tribool withFiles = boost::logic::indeterminate,
					std::string prefix = std::string()
				) const;


				typedef std::set<FreeDRTArea*> FreeDRTs;

				//////////////////////////////////////////////////////////////////////////
				/// Free DRT calling at the stop.
				FreeDRTs getFreeDRTs() const;

				//////////////////////////////////////////////////////////////////////////
				/// Return true if StopArea is contained in a DRTArea
				bool isInDRT() const;

				virtual LinkedObjectsIds getLinkedObjectsIds(
					const Record& record
				) const;

				virtual void link(util::Env& env, bool withAlgorithmOptimizations = false);

				virtual void unlink();

				virtual SubObjects getSubObjects() const;

				static std::string SerializeTransferDelaysMatrix(const TransferDelaysMap& matrix);
			//@}

			virtual bool allowUpdate(const server::Session* session) const;
			virtual bool allowCreate(const server::Session* session) const;
			virtual bool allowDelete(const server::Session* session) const;
		};
}	}

#endif // SYNTHESE_pt_StopArea_hpp__

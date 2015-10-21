
/** StopPointTableSync class header.
	@file StopPointTableSync.hpp

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

#ifndef SYNTHESE_ENVLSSQL_PHYSICALSTOPTABLESYNC_H
#define SYNTHESE_ENVLSSQL_PHYSICALSTOPTABLESYNC_H

#include "StopPoint.hpp"

#include <string>
#include <iostream>

#include "DBDirectTableSyncTemplate.hpp"
#include "FetcherTemplate.h"

namespace synthese
{
	namespace pt
	{
		//////////////////////////////////////////////////////////////////////////
		/// StopPoint table synchronizer.
		///	@ingroup m35LS refLS
		/// @author Marc Jambert, Hugues Romain
		//////////////////////////////////////////////////////////////////////////
		///	Corresponding class : StopPoint
		///	Table number : 12
		///
		///	<h2>Fields</h2>
		///
		///	<table class="table">
		///	<tr><th>field</th><th>Description</th><th>Comments</th><th>Possible values</th><th>Corresponding attribute</th></tr>
		///	<tr><th>id</th><td>Unique identifier</td><td>Primary key</td><td>Table #12 : 3377699720527872 to 3659174697238527</td><td>StopPoint::_key</td></tr>
		///	<tr><th>name</th><td>Name of the stop point</td><td></td><td>String. Avoid " characters</td><td>StopPoint::_name</td></tr>
		///	<tr><th>x, y</th><td>Coordinates in the default projection of the instance</td><td>If lon/lat are not null, x and y are ignored</td><td>Positive numbers</td><td>StopPoint::x, StopPoint::y</td></tr>
		///	<tr><th>operator_code</th><td>Identifier of the stop in the source database</td><td>To be used when the stop is imported from an other system</td><td></td><td>StopPoint::_operator_code</td></tr>
		///	<tr><th>longitude, latitude</th><td>Longitude and latitude in degrees</td><td>Can be null if x,y are defined. If lon/lat are not null, x and y are ignored</td><td>between -180 and 180</td><td>StopPoint::_logitude, StopPoint::_latitude</td></tr>
		///	</table>
		///
		///	<h2>Up links</h2>
		///
		///	<dl>
		///	<dt>place_id</dt><dd>id of @ref StopAreaTableSync "stop area" which the stop point belongs to</dd>
		///	<dt>projected_road_chunk_id</dt><dd>id of @ref road::RoadChunkTableSync "road chunk" on which the stop point is projected</dd>
		///	<dt>projected_metric_offset</dt><dd>metric offset of the projected point on the road chunk</dd>
		///	</dl>
		class StopPointTableSync:
			public db::DBDirectTableSyncTemplate<
				StopPointTableSync,
				StopPoint>,
			public db::FetcherTemplate<graph::Vertex, StopPointTableSync>
		{
		public:
			virtual bool allowList( const server::Session* session ) const;

			//////////////////////////////////////////////////////////////////////////
			/// Stop points search.
			/// @param env environment to populate when loading objects
			/// @param placeId id of stop area which returned stops must belong to
			/// @param operatorCode LIKE expression operator code must correspond to
			/// @param orderByCityAndStopName results are ordered by city name, then stop area name, then stop point name
			/// @param raisingOrder results are ordered ascendantly
			/// @param first first element to return
			///	@param number Number of objects to return (undefined = all) The size of the vector is less or equal to number, then all users were returned despite of the number limit. If the size is greater than number (actually equal to number + 1) then there is others accounts to show. Test it to know if the situation needs a "click for more" button.
			/// @param linkLevel level of link
			/// @return stop points corresponding to search criteria
			static SearchResult Search(
				util::Env& env,
				boost::optional<util::RegistryKeyType> placeId = boost::optional<util::RegistryKeyType>(),
				boost::optional<std::string> operatorCode = boost::optional<std::string>(),
				bool orderByCityAndStopName = true,
				bool raisingOrder = true,
				int first = 0,
				boost::optional<std::size_t> number = boost::optional<std::size_t>(),
				util::LinkLevel linkLevel = util::UP_LINKS_LOAD_LEVEL
			);



			static SearchResult SearchDistance(
				StopPoint const& point,
				util::Env& env,
				bool insideSameStopArea = false,
				double const& distance = 300,
				bool maxDistance = true, 
				util::LinkLevel linkLevel = util::UP_LINKS_LOAD_LEVEL
			);



			static bool SearchDistance(
				StopPoint const& point1,
				StopPoint const& point2,
				double const& distance = 300,
				bool maxDistance = true 
			);



			/** StopPoint search for AutoComplete.
			@param prefix Part of the name of the StopPoint
			@param limit Number of results to answer. The size of the vector is less
			or equal to number
			@return Found stopoints (object id and name).
			@author Gael Sauvanet
			@date 2012
			*/
			db::RowsList SearchForAutoComplete(
				const boost::optional<std::string> prefix,
				const boost::optional<std::size_t> limit,
				const boost::optional<std::string> optionalParameter) const;

		};

	}
}

#endif

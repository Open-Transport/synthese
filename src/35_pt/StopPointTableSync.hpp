
/** StopPointTableSync class header.
	@file StopPointTableSync.hpp

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

#ifndef SYNTHESE_ENVLSSQL_PHYSICALSTOPTABLESYNC_H
#define SYNTHESE_ENVLSSQL_PHYSICALSTOPTABLESYNC_H

#include "StopPoint.hpp"

#include <string>
#include <iostream>

#include "SQLiteRegistryTableSyncTemplate.h"
#include "FetcherTemplate.h"

namespace synthese
{
	namespace pt
	{
		/** StopPoint SQLite table synchronizer.
			@ingroup m35LS refLS
			
			Physical stops table :
				- on insert : 
				- on update : 
				- on delete : X

			Corresponding class : StopPoint

			<h2>Fields</h2>

			<table class="table">
			<tr><th>field</th><th>Description</th><th>Comments</th><th>Possible values</th><th>Corresponding attribute</th></tr>
			<tr><th>id</th><td>Unique identifier</td><td>Primary key</td><td>Table #12 : 3377699720527872 to 3659174697238527</td><td>StopPoint::_key</td></tr>
			<tr><th>name</th><td>Name of the stop point</td><td></td><td>String. Avoid " characters</td><td>StopPoint::_name</td></tr>
			<tr><th>x, y</th><td>Coordinates in the default projection of the instance</td><td>If lon/lat are not null, x and y are ignored</td><td>Positive numbers</td><td>StopPoint::x, StopPoint::y</td></tr>
			<tr><th>operator_code</th><td>Identifier of the stop in the source database</td><td>To be used when the stop is imported from an other system</td><td></td><td>StopPoint::_operator_code</td></tr>
			<tr><th>longitude, latitude</th><td>Longitude and latitude in degrees</td><td>Can be null if x,y are defined. If lon/lat are not null, x and y are ignored</td><td>between -180 and 180</td><td>StopPoint::_logitude, StopPoint::_latitude</td></tr>
			</table>
		*/
		class StopPointTableSync:
			public db::SQLiteRegistryTableSyncTemplate<StopPointTableSync,StopPoint>,
			public db::FetcherTemplate<graph::Vertex, StopPointTableSync>
		{
		public:
			static const std::string COL_NAME;
			static const std::string COL_PLACEID;
			static const std::string COL_X;
			static const std::string COL_Y;
			static const std::string COL_OPERATOR_CODE;
			static const std::string COL_LONGITUDE;
			static const std::string COL_LATITUDE;



			static SearchResult Search(
				util::Env& env,
				boost::optional<util::RegistryKeyType> placeId = boost::optional<util::RegistryKeyType>(),
				boost::optional<std::string> operatorCode = boost::optional<std::string>(),
				int first = 0,
				boost::optional<std::size_t> number = boost::optional<std::size_t>(),
				util::LinkLevel linkLevel = util::UP_LINKS_LOAD_LEVEL
			);
		};
	}
}

#endif

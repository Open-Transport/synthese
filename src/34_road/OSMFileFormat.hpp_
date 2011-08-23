
/** OSMFileFormat class header.
	@file OSMFileFormat.hpp

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

#ifndef SYNTHESE_road_OSMFileFormat_hpp__
#define SYNTHESE_road_OSMFileFormat_hpp__

#include "FileFormatTemplate.h"
#include "Road.h"
#include "OSMElements.h"
#include <iostream>
#include <map>
#include <string>
#include <vector>

namespace synthese
{
	namespace util
	{
		class Registrable;
	}
	namespace road
	{
		class Address;
	}
	namespace geography
	{
		class City;
	}
	namespace geometry
	{
		class Point2D;
	}


	namespace osm
	{

		//////////////////////////////////////////////////////////////////////////
		/// OSM file format.
		/// @ingroup m35
		//////////////////////////////////////////////////////////////////////////
		class OSMFileFormat
		:	public impex::FileFormatTemplate<OSMFileFormat>
		{

		private:

			typedef std::map<int, boost::shared_ptr<road::Address> > _AddressesMap;
			typedef std::map<int, boost::shared_ptr<geography::City> > _CitiesMap;

			_AddressesMap _addressesMap;

		protected:

			virtual void _parse(
				const boost::filesystem::path& filePath,
				std::ostream& os,
				std::string key
			);

			boost::shared_ptr<road::Address> lookupOrCreateAddress(NodePtr &node);
			void insertRoadChunk(boost::shared_ptr<road::Address> &start,
					boost::shared_ptr<road::Address> &end,
					double length,
					boost::shared_ptr<road::RoadPlace> &roadPlace,
					std::list<geometry::Point2D> &via_points,
					road::Road::RoadType roadType);

			std::string utf8ToLatin1(std::string &text);
			projPJ dstProjection;
		public:

			OSMFileFormat(
				util::Env* env = NULL
			);
			~OSMFileFormat();

			virtual void build(std::ostream& os);

			virtual void save(std::ostream& os) const;

		private:

		};
	}
}

#endif // SYNTHESE_road_OSMFileFormat_hpp__


/** Place class header.
	@file Place.h

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

#ifndef SYNTHESE_ENV_PLACE_H
#define SYNTHESE_ENV_PLACE_H

#include "Registrable.h"
#include "GraphTypes.h"

#include <map>
#include <string>
#include <vector>
#include <set>
#include <boost/shared_ptr.hpp>

namespace geos
{
	namespace geom
	{
		class Point;
}	}

namespace synthese
{
	namespace graph
	{
		class AccessParameters;
		class VertexAccessMap;
		class Vertex;
		struct VertexAccess;
	}

	namespace geography
	{
		//////////////////////////////////////////////////////////////////////////
		/// Place base class.
		///
		///	@ingroup m32
		//////////////////////////////////////////////////////////////////////////
		class Place
		{
		public:
			typedef std::set<graph::GraphIdType> GraphTypes;

		protected:
			mutable boost::shared_ptr<geos::geom::Point> _isoBarycentre;

		protected:

			Place();

		public:

			virtual ~Place ();


			//! @name Query methods
			//@{
				virtual void getVertexAccessMap(
					graph::VertexAccessMap& result,
					const graph::AccessParameters& accessParameters,
					const GraphTypes& whatToSearch
				) const = 0;



				graph::VertexAccessMap getVertexAccessMap(
					const graph::AccessParameters& accessParameters,
					GraphTypes::value_type whatToSearch,
					...
				) const;

				virtual const boost::shared_ptr<geos::geom::Point>& getPoint() const = 0;

				virtual bool includes(const Place* place) const;
			//@}

		};
}	}

#endif

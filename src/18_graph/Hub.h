////////////////////////////////////////////////////////////////////////////////
/// Hub class header.
///	@file Hub.h
///	@author Hugues Romain (RCS)
///	@date dim jan 25 2009
///
///	This file belongs to the SYNTHESE project (public transportation specialized
///	software)
///	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>
///
///	This program is free software; you can redistribute it and/or
///	modify it under the terms of the GNU General Public License
///	as published by the Free Software Foundation; either version 2
///	of the License, or (at your option) any later version.
///
///	This program is distributed in the hope that it will be useful,
///	but WITHOUT ANY WARRANTY; without even the implied warranty of
///	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
///	GNU General Public License for more details.
///
///	You should have received a copy of the GNU General Public License
///	along with this program; if not, write to the Free Software Foundation,
///	Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
////////////////////////////////////////////////////////////////////////////////

#ifndef SYNTHESE_HUB_H
#define SYNTHESE_HUB_H

#include "GraphTypes.h"

#include <boost/date_time/posix_time/posix_time_duration.hpp>

namespace synthese
{
	namespace geometry
	{
		class Point2D;
	}

	namespace graph
	{
		class Vertex;
		class VertexAccessMap;
		
		/** Generic hub interface.
			@ingroup m18
		*/
		class Hub
		{
		public:
			//! @name Interface for query methods
			//@{
				virtual boost::posix_time::time_duration getMinTransferDelay(
				) const = 0;
				

				virtual void getVertexAccessMap(
					VertexAccessMap& result,
					const AccessDirection& accessDirection,
					GraphIdType whatToSearch,
					const Vertex& vertex
				) const = 0;

				virtual bool isConnectionAllowed(
					const Vertex& origin
					, const Vertex& destination
				) const = 0;

				virtual boost::posix_time::time_duration getTransferDelay(
					const Vertex& origin,
					const Vertex& destination
				) const = 0;



				///////////////////////////////////////////////////////////////
				/// Score getter.
				/// The score of a hub represents its capacity to provide some
				/// connections.
				/// The score range is from 0 to 100.
				/// Several special values are :
				///  - 0 : connection is never possible, jump over the hub in
				///		a routing procedure, except if the hub is near the goal
				///  - 1 : connection is possible, lowest score. This is the
				///		default score of a hub
				///  - 100 : maximum value for a score
				/// @return the score of the hub				
				virtual HubScore getScore(
				) const = 0;

				virtual const geometry::Point2D& getPoint() const = 0;

				virtual bool containsAnyVertex(GraphIdType graphType) const = 0;

				bool isConnectionPossible() const {	return getScore() > 0; }
			//@}
		};
	}
}

#endif

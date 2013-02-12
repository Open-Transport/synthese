////////////////////////////////////////////////////////////////////////////////
/// Hub class header.
///	@file Hub.h
///	@author Hugues Romain (RCS)
///	@date dim jan 25 2009
///
///	This file belongs to the SYNTHESE project (public transportation specialized
///	software)
///	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>
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
#include "RuleUser.h"

#include <boost/date_time/posix_time/posix_time_duration.hpp>
#include <boost/logic/tribool.hpp>

namespace geos
{
	namespace geom
	{
		class Point;
	}
}

namespace synthese
{
	namespace graph
	{
		class Vertex;
		class VertexAccessMap;

		//////////////////////////////////////////////////////////////////////////
		/// Generic hub interface.
		///	@ingroup m18
		class Hub:
			public RuleUser
		{
		private:
			typedef std::vector<boost::logic::tribool> UsefulnessTransferCache;
			mutable UsefulnessTransferCache _usefulnessTransferCache;



			//////////////////////////////////////////////////////////////////////////
			/// Arrange the _previousConnectionDeparture and 
			/// _followingConnectionArrival pointers of the edges accessing
			/// the vertices of the current hub according to the usefulness of transfers
			/// in the hub.
			/// @param graphId the cache to clear
			/// @pre the transfer usefulness cache must have been updated before the call
			void _linkEdgesAccordingToTransferUsefulness(
				GraphIdType graphType
			) const;		
		
		public:
			//! @name Interface for query methods
			//@{
				virtual boost::posix_time::time_duration getMinTransferDelay(
				) const = 0;


				virtual void getVertexAccessMap(
					VertexAccessMap& result,
					GraphIdType whatToSearch,
					const Vertex& vertex,
					bool vertexIsOrigin
				) const = 0;



				typedef std::vector<const Vertex*> Vertices;



				//////////////////////////////////////////////////////////////////////////
				/// Lists the vertices contained in the hub.
				/// @param graphId the graph the vertices must belong to
				virtual Vertices getVertices(
					GraphIdType graphId
				) const = 0;



				virtual bool isConnectionAllowed(
					const Vertex& origin,
					const Vertex& destination
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



				//////////////////////////////////////////////////////////////////////////
				///	Virtual centroid getter.
				virtual const boost::shared_ptr<geos::geom::Point>& getPoint() const = 0;

				virtual bool containsAnyVertex(GraphIdType graphType) const = 0;

				virtual bool isConnectionPossible() const {	return getScore() > 0; }

				virtual const RuleUser* _getParentRuleUser() const { return NULL; }



				//////////////////////////////////////////////////////////////////////////
				/// Gets if the hub is useful as a transfer point according to the graph
				/// structure.
				/// The value is stored in a cache to avoid useless identical algorithm runs.
				/// In case of cache rewriting, all the pointers of edges are regenerated.
				/// @param graphId the cache to clear
				/// @return true if the hub is useful for a transfer
				bool isUsefulTransfer(
					GraphIdType graphId
				) const;



				//////////////////////////////////////////////////////////////////////////
				/// Clears the cache on transfer usefulness for the current hub.
				/// @param graphId the cache to clear
				void clearUsefulForTransfer(
					GraphIdType graphId
				) const;




				//////////////////////////////////////////////////////////////////////////
				/// Clears the cache on transfer usefulness for the current hub and its
				/// influence area.
				/// @param graphId the cache to clear
				void clearAndPropagateUsefulTransfer(
					GraphIdType graphId
				) const;
			//@}
		};
	}
}

#endif

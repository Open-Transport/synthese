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
#include "Point2D.h"

namespace synthese
{
	namespace env
	{
		class AccessParameters;
	}
	
	namespace graph
	{
		class Vertex;
		
		/** Generic hub class.
			The default behaviour is forbidden connection between vertexes.
			@ingroup m18
		*/
		class Hub
		{
		public:
			//! @name Interface for query methods
			//@{
				virtual MinutesDuration getMinTransferDelay(
				) const;
				
				virtual bool isConnectionAllowed(
					const Vertex* fromVertex
					, const Vertex* toVertex
				) const;

				virtual MinutesDuration getTransferDelay(
					const Vertex* fromVertex
					, const Vertex* toVertex
				) const;

				static const MinutesDuration FORBIDDEN_TRANSFER_DELAY;


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
				) const;

				bool isConnectionPossible() const;
				
				virtual const geometry::Point2D& getPoint() const = 0;
			//@}
		};
	}
}

#endif


/** WFSType class header.
	@file WFSType.hpp

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

#ifndef SYNTHESE_map_WFSType_hpp__
#define SYNTHESE_map_WFSType_hpp__

#include "FactoryBase.h"

#include <ostream>

namespace geos
{
	namespace geom
	{
		class Envelope;
	}
}

namespace synthese
{
	class CoordinatesSystem;

	namespace map
	{
		//////////////////////////////////////////////////////////////////////////
		/// WFS Type (can bx returned by a GetFeature WFS request)
		/// @ingroup m39
		/// @author Hugues Romain
		/// @since 3.2.1
		/// @date 2010
		class WFSType:
			public util::FactoryBase<WFSType>
		{
		public:
			WFSType() {}
			virtual ~WFSType() {}

			//////////////////////////////////////////////////////////////////////////
			/// Writes on the stream the schema part corresponding to the type.
			/// @param stream the stream to write the schema on
			virtual void getSchema(
				std::ostream& stream
			) const = 0;


			//////////////////////////////////////////////////////////////////////////
			/// Writes on the stream the list of the features selected by an envelope.
			/// @param stream the stream to write the result on
			/// @param envelope the envelope where the features must be in
			/// @param sr the spatial reference of the output
			virtual void getFeatures(
				std::ostream& stream,
				const geos::geom::Envelope& envelope,
				const CoordinatesSystem& sr
			) const = 0;
		};
	}
}

#endif // SYNTHESE_map_WFSType_hpp__

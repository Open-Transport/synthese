
/** Road class header.
	@file Road.h

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

#ifndef SYNTHESE_ENV_ROAD_H
#define SYNTHESE_ENV_ROAD_H

#include "Object.hpp"

#include "EnumObjectField.hpp"
#include "NumericField.hpp"
#include "RoadPlace.h"
#include "RoadTypes.hpp"
#include "SchemaMacros.hpp"

namespace synthese
{
	FIELD_ENUM(RoadTypeField, road::RoadType)

	typedef boost::fusion::map<
		FIELD(Key),
		FIELD(RoadTypeField),
		FIELD(road::RoadPlace)
	> RoadSchema;

	namespace road
	{
		class RoadChunk;
		class RoadPart;

		//////////////////////////////////////////////////////////////////////////
		/// Contiguous part of a named road.
		/// Road class.
		///	@ingroup m34
		///	@author Marc Jambert, Hugues Romain
		//////////////////////////////////////////////////////////////////////////
		/// A road object designates only one direction of the road part, called
		/// main direction.
		/// The other direction is an other Road object, which is auto-generated
		/// by the constructor.
		///
		/// A physical road part is modeled by two Road objects, one per direction.
		/// The _side attribute tells if the direction corresponds to the right or
		///	left side of the road.
		///
		/// A physical road is modeled by two Road objects : one for each direction.
		/// A Road objects corresponds to a side of the road too.
		/// The side attribute indicates which side is modeled by the instance.
		/// The relation between side and direction must correspond to the driving
		/// side of the country which the road belongs to.
		/// By default, the side of the road is right, and its reversed road is on
		/// left.
		class Road:
			public Object<Road, RoadSchema>,
			virtual public util::Registrable
		{
		public:


		private:
			boost::shared_ptr<RoadPath> _forwardPath;
			boost::shared_ptr<RoadPath> _reversePath;

		public:

			Road(
				util::RegistryKeyType key = 0,
				RoadType type = ROAD_TYPE_UNKNOWN
			);
			virtual ~Road();


		//! @name Getters
		//@{
			RoadPath& getForwardPath() const { return *_forwardPath; }
			RoadPath& getReversePath() const { return *_reversePath; }
		//@}

		//! @name Setters
		//@{
		//@}

		//! @name Services
		//@{
		//@}

		//! @name Update methods.
		//@{
			//////////////////////////////////////////////////////////////////////////
			/// Inserts a road chunk at the beginning of the road
			/// @param chunk the chunk to add
			/// @param length length of the chunk
			/// @param rankShift
			/// @author Hugues Romain
			/// @date 2010
			/// @since 3.1.18
			/// @pre the road must contain at least one chunk
			/// @pre the rank and the metric offset of the chunk to insert must be less
			/// than the rank of the first edge raised by the rankShift.
			/// @pre same condition on the metric offset
			/// @post do not use the generated graph directly : only for saving purpose
			void insertRoadChunk(
				RoadChunk& chunk,
				double length,
				size_t rankShift
			);



			void merge(
				Road& other
			);
		//@}

		//! @name Modifiers
		//@{
			virtual bool allowUpdate(const server::Session* session) const;
			virtual bool allowCreate(const server::Session* session) const;
			virtual bool allowDelete(const server::Session* session) const;

			virtual void link(util::Env& env, bool withAlgorithmOptimizations = false);
			virtual void unlink();
		//@}
		};
	}
}

#endif

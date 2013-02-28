
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

#include <string>

#include "Path.h"

namespace synthese
{
	namespace road
	{
		class RoadPlace;
		class RoadChunk;
		class Address;

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
			public graph::Path
		{
		public:

		  typedef enum {
			ROAD_TYPE_UNKNOWN,
			ROAD_TYPE_MOTORWAY, /* autoroute */
			ROAD_TYPE_MEDIANSTRIPPEDROAD,  /* route a chaussees separees (terre plein) */
			ROAD_TYPE_PRINCIPLEAXIS, /* axe principal (au sens rue) */
			ROAD_TYPE_SECONDARYAXIS, /* axe principal (au sens rue) */
			ROAD_TYPE_BRIDGE, /* pont */
			ROAD_TYPE_STREET, /* rue */
			ROAD_TYPE_PEDESTRIANSTREET, /* rue pietonne */
			ROAD_TYPE_ACCESSROAD, /* bretelle */
			ROAD_TYPE_PRIVATEWAY, /* voie privee */
			ROAD_TYPE_PEDESTRIANPATH, /* chemin pieton */
			ROAD_TYPE_TUNNEL, /* tunnel */
			ROAD_TYPE_HIGHWAY, /* route secondaire */
			ROAD_TYPE_STEPS, /* steps */
			ROAD_TYPE_SERVICE
		} RoadType;


		protected:
			RoadType _type;

			Road(
				util::RegistryKeyType key = 0,
				RoadType type = ROAD_TYPE_UNKNOWN
			);

			virtual ~Road();

			void _setRoadPlace( RoadPlace& value );

			void _insertRoadChunk(
				RoadChunk& chunk,
				double length,
				std::size_t rankShift
			);

		public:
		//! @name Getters
		//@{
			const RoadType& getType () const { return _type; }
		//@}

		//! @name Setters
		//@{
			void setType (const RoadType& type);
		//@}

		//! @name Services
		//@{
			RoadPlace* getRoadPlace() const;
			virtual bool isPedestrianMode() const;
			virtual bool isRoad() const;
			virtual std::string getRuleUserName() const;
			virtual bool isActive(const boost::gregorian::date& date) const;
			virtual bool isReversed() const = 0;
		//@}
		};
	}
}

#endif

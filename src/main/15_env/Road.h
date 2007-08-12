
/** Road class header.
	@file Road.h

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

#ifndef SYNTHESE_ENV_ROAD_H
#define SYNTHESE_ENV_ROAD_H

#include <string>

#include "01_util/Registrable.h"
#include "01_util/UId.h"

#include "AddressablePlace.h"
#include "Path.h"

namespace synthese
{
	namespace env
	{

		class City;
		class RoadChunk;

		/** Road class.
			@ingroup m15
		*/
		class Road : 
			public synthese::util::Registrable<uid,Road>, 
			public AddressablePlace,
			public Path
		    

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
			ROAD_TYPE_HIGHWAY /* route secondaire */
		  } RoadType;
		    

		private:
		  
		  RoadType _type;

		public:

		  Road (
			  uid id = UNKNOWN_VALUE
			  , std::string name = std::string()
			  , const City* city = NULL
			  ,	RoadType type = ROAD_TYPE_UNKNOWN
			  );

		  virtual ~Road();


		  //! @name Getters/Setters
		  //@{
		  virtual uid getId () const;

		  bool hasReservationRule () const;
		  const ReservationRule* getReservationRule () const;

		  const Axis* getAxis () const;

		  const RoadType& getType () const;
		  void setType (const RoadType& type);
		  //@}
		    

		  //! @name Query methods.
		  //@{

		  bool isRoad () const;
		  bool isLine () const;
		  virtual bool isPedestrianMode() const;
		  
		  /** Find closest address of this road, before a given metric offset.
			  @param metricOffset The reference point.
			  @return Closest address before reference, or 0 if none.
		  */
		  const Address* findClosestAddressBefore (double metricOffset) const;

		  
		  /** Find closest address of this road, after a given metric offset.
			  @param metricOffset The reference point.
			  @return Closest address after reference, or 0 if none.
		  */
		  const Address* findClosestAddressAfter (double metricOffset) const;
		  
		  //@}

		  //! @name Update methods.
		  //@{

		  //@}

		};


	}
}

#endif 


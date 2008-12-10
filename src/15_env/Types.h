
/** Environment module related types definitions.
	@file Types.h

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

#ifndef SYNTHESE_Env_Types_H__
#define SYNTHESE_Env_Types_H__

#include <set>
#include <map>
#include <vector>
#include <boost/shared_ptr.hpp>

#include "01_util/Constants.h"
#include "01_util/UId.h"

namespace synthese
{
	namespace env
	{
		class PhysicalStop;
		class City;
		class Line;
		class Service;
		class Address;
		class Place;

		/** @addtogroup m35
		@{
		*/
		typedef std::map<uid,const PhysicalStop*> PhysicalStops;
		typedef std::vector<const Address*> Addresses;

		typedef std::vector<boost::shared_ptr<const City> > CityList;
		typedef std::set<boost::shared_ptr<const Line> > LineSet;

		typedef std::set<const Place*> PlacesSet;
		/** @} */
	}

	/** @addtogroup m35
		@{
	*/
	typedef enum { SEARCH_ADDRESSES, DO_NOT_SEARCH_ADDRESSES } SearchAddresses;
	typedef enum { SEARCH_PHYSICALSTOPS, DO_NOT_SEARCH_PHYSICALSTOPS } SearchPhysicalStops;
	
	
	/** Service determination method.
		- DEPARTURE_TO_ARRIVAL = the service is chosen from a presence time before a departure. The arrival will be chosen in the following edges.
		- ARRIVAL_TO_DEPARTURE = the service is chosen from a presence time after an arrival. The departure will be chosen in the preceding edges.
	*/
	typedef enum { ARRIVAL_TO_DEPARTURE, DEPARTURE_TO_ARRIVAL } AccessDirection ;


	/** Reservation rule type.
		- FORBIDDEN : it is impossible to book a seat on the service
		- COMPULSORY : it is impossible to use the service without having booked a seat
		- OPTIONAL : is is possible to book a place on the service, but it is possible to use the service without having booked a seat
		- MIXED_BY_DEPARTURE_PLACE : it is impossible to use the service without having booked a place, except for journeys beginning at several places, defined in the commercial line.
	*/
	typedef enum {
		RESERVATION_FORBIDDEN = 0,
		RESERVATION_COMPULSORY = 1,
		RESERVATION_OPTIONAL = 2,
		RESERVATION_MIXED_BY_DEPARTURE_PLACE = 3
	} ReservationRuleType;
	/** @} */

}

#endif // SYNTHESE_Env_Types_H__

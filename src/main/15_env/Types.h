
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
#include <vector>
#include <boost/shared_ptr.hpp>

#include "15_env/Complyer.h"

#include "01_util/Constants.h"

namespace synthese
{
	namespace env
	{
		class PhysicalStop;
		class City;
		class Line;
		class Service;
		class Address;

		/** @addtogroup m15
		@{
		*/

		typedef std::set<const PhysicalStop*> PhysicalStops;
		typedef std::vector<const Address*> Addresses;

		typedef std::vector<boost::shared_ptr<const City> > CityList;
		typedef std::set<boost::shared_ptr<const Line> > LineSet;

		/** @} */
	}

	/** @addtogroup m15
		@{
	*/
	typedef enum { FROM_ORIGIN, TO_DESTINATION } AccessDirection ;

	struct AccessParameters {
		double maxApproachDistance;
		double maxApproachTime;
		double approachSpeed;
		int maxTransportConnectionCount;

		env::Complyer	complyer;

	};

	/** @} */

}

#endif // SYNTHESE_Env_Types_H__

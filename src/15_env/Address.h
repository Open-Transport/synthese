
/** Address class header.
	@file Address.h

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

#ifndef SYNTHESE_ENV_ADDRESS_H
#define SYNTHESE_ENV_ADDRESS_H

#include "Vertex.h"
#include "Registry.h"
#include "01_util/Constants.h"

#include <vector>
#include <set>

namespace synthese
{
	namespace env
	{
		class LogicalPlace;
		class Road;

		/** Address (road + metric offset)
			 An address is a position on a road given a metric offset from the start of the road.

			An address may be associated with a connection place in the following cases :
			  - The address corresponds to a crossing between two roads
			  - The address belongs to a logical stop
			@ingroup m35
		*/
		class Address
		:	 public Vertex
		{
		public:

			/// Chosen registry class.
			typedef util::Registry<Address>	Registry;

		private:

			const Road* _road;    //!< Address road.
			double _metricOffset;  //!< Metric offset (meters)

		public:

			Address(
				util::RegistryKeyType id = UNKNOWN_VALUE,
				 const AddressablePlace* place = NULL,
				 const Road* road = NULL, 
				 double metricOffset = UNKNOWN_VALUE,
				 double x = UNKNOWN_VALUE,
				 double y = UNKNOWN_VALUE);

			~Address();


			//! @name Getters
			//@{
				const Road* getRoad() const;
				double getMetricOffset () const;
			//@}

			//! @name Setters
			//@{
				void setRoad(const Road* road);
				void setMetricOffset(double value);
			//@}

			//! @name Query methods
			//@{
				bool isAddress () const;
				bool isPhysicalStop () const;
				bool isConnectionAllowed() const;
			//@}
		};
	}
}

#endif

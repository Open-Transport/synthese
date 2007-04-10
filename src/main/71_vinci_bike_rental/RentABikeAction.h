
/** RentABikeAction class header.
	@file RentABikeAction.h

	This file belongs to the VINCI BIKE RENTAL SYNTHESE module
	Copyright (C) 2006 Vinci Park 
	Contact : Rapha�l Murat - Vinci Park <rmurat@vincipark.com>

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

#ifndef SYNTHESE_RentABikeAction_H__
#define SYNTHESE_RentABikeAction_H__

 #include <boost/shared_ptr.hpp>

#include "04_time/DateTime.h"

#include "30_server/Action.h"

namespace synthese
{
	namespace vinci
	{
		class VinciRate;
		class VinciBike;
		class VinciContract;
		class VinciAntivol;

		/** Starting a bike rent Action Class.
			@ingroup m71Actions refActions
		*/
		class RentABikeAction : public server::Action
		{
		public:
			static const std::string PARAMETER_RATE_ID;
			static const std::string PARAMETER_BIKE_ID;
			static const std::string PARAMETER_CONTRACT_ID;
			static const std::string PARAMETER_LOCK_ID;
			static const std::string PARAMETER_DATE;

		private:
			time::DateTime	_date;
			boost::shared_ptr<VinciRate>		_rate;
			boost::shared_ptr<VinciBike>		_bike;
			boost::shared_ptr<VinciContract>	_contract;
			boost::shared_ptr<VinciAntivol>		_lock;
			std::string		_lockMarkedNumber;
			double			_amount;

		protected:
			/** Conversion from attributes to generic parameter maps.
			*/
			server::ParametersMap getParametersMap() const;

			/** Conversion from generic parameters map to attributes.
			Removes the used parameters from the map.
			*/
			void _setFromParametersMap(const server::ParametersMap& map);

		public:
			/** Action execution.
			*/
			void run();
			
		};
	}
}

#endif 

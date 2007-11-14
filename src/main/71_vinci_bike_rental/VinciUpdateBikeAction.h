
/** VinciUpdateBikeAction class header.
	@file VinciUpdateBikeAction.h

	This file belongs to the VINCI BIKE RENTAL SYNTHESE module
	Copyright (C) 2006 Vinci Park 

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

#ifndef SYNTHESE_VinciUpdateBikeAction_H__
#define SYNTHESE_VinciUpdateBikeAction_H__

#include <boost/shared_ptr.hpp>

#include "30_server/Action.h"

#include "01_util/FactorableTemplate.h"

namespace synthese
{
	namespace vinci
	{
		class VinciBike;

		/** VinciUpdateBikeAction action class.
			@ingroup m71Action refAction
		*/
		class VinciUpdateBikeAction : public util::FactorableTemplate<server::Action, VinciUpdateBikeAction>
		{
		public:
			static const std::string PARAMETER_BIKE_ID;
			static const std::string PARAMETER_NUMBER;
			static const std::string PARAMETER_MARKED_NUMBER;

		private:
			boost::shared_ptr<VinciBike>	_bike;
			std::string	_number;
			std::string	_markedNumber;

		protected:
			/** Conversion from attributes to generic parameter maps.
			*/
			server::ParametersMap getParametersMap() const;

			/** Conversion from generic parameters map to attributes.
				Removes the used parameters from the map.
				@exception ActionException Occurs when some parameters are missing or incorrect.
			*/
			void _setFromParametersMap(const server::ParametersMap& map);

		public:
			/** Action to run, defined by each subclass.
			*/
			void run();
		};
	}
}

#endif // SYNTHESE_VinciUpdateBikeAction_H__
 

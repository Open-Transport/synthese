
/** AccessParameters class header.
	@file AccessParameters.h

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

#ifndef SYNTHESE_env_AccessParameters_h__
#define SYNTHESE_env_AccessParameters_h__

#include "GraphTypes.h"
#include "GraphConstants.h"

#include <boost/optional.hpp>

namespace synthese
{
	namespace graph
	{
		class RuleUser;

		/** Network access parameters class.
			@ingroup m18

			This class stores two types of information about how to use the transportation network :
				- numeric parameters : speeds, maximums, etc.
				- filters 
		*/
		class AccessParameters
		{
			double		_maxApproachDistance;
			double		_maxApproachTime;
			double		_approachSpeed;
			boost::optional<size_t>		_maxTransportConnectionCount;
			bool		_drtOnly;
			bool		_withoutDrt;
//			Fare*		_fare;
			UserClassCode	_userClass;

		public:


			/** AccessParameters constructor.
				@param bikeCompliance (default = no constraint)
				@param fare (default = no constraint)
				@param handicappedCompliance (default = no constraint)
				@param pedestrianCompliance (default = no constraint)
				@param drtOnly if true, only demand responsive transport lines are used (default = no constraint)
				@param withoutDrt if true, the demand responsive transport lines with compulsory reservation are not used (default = no constraint)
				@param maxApproachDistance (default = 1000 m)
				@param maxApproachTime (default = 23 min)
				@param approachSpeed Approach speed (default = 67 m/min = 4 km/h)
				@param maxTransportConnectionCount (default = 10 connections)
				@date 2008
			*/
			AccessParameters(
				UserClassCode	userClass = USER_PEDESTRIAN
				, bool			drtOnly = false
				, bool			withoutDrt = false
				, double		maxApproachDistance = 1000
				, double		maxApproachTime = 23
				, double		approachSpeed = 67
				, boost::optional<size_t>	maxTransportConnectionCount = boost::optional<size_t>()
			);


			//! @name Controls
			//@{
				/** Approach compatibility control.
					@param distance length of the approach
					@param duration duration of the approach
					@return bool true if the approach is compatible with the current rules
					@author Hugues Romain
					@date 2008				
				*/
				bool isCompatibleWithApproach(double distance, double duration) const;
			//@}


			//! @name Getters
			//@{
				graph::UserClassCode	getUserClass()	const;
				double	getApproachSpeed()		const;
				boost::optional<size_t>	getMaxtransportConnectionsCount() const;
				bool	getDRTOnly()	const;
				bool	getWithoutDRT()	const;
			//@}
		};
	}
}

#endif // SYNTHESE_env_AccessParameters_h__

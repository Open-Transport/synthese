
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

namespace synthese
{
	namespace env
	{
		class Fare;
		class Complyer;

		/** Network access parameters class.
			@ingroup m35

			This class stores two types of information about how to use the transportation network :
				- numeric parameters : speeds, maximums, etc.
				- filters 
		*/
		class AccessParameters
		{
			double		_maxApproachDistance;
			double		_maxApproachTime;
			double		_approachSpeed;
			int			_maxTransportConnectionCount;
			bool		_disabledUser;
			bool		_drtOnly;
			bool		_withoutDrt;
			bool		_withBike;
			bool		_pedestrian;
			Fare*		_fare;

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
				bool					bikeCompliance = false
				, Fare*					fare = NULL
				, bool					handicappedCompliance = false
				, bool					pedestrianCompliance = false
				, bool					drtOnly = false
				, bool					withoutDrt = false
				, double				maxApproachDistance = 1000
				, double				maxApproachTime = 23
				, double				approachSpeed = 67
				, int					maxTransportConnectionCount = 10
			);


			//! @name Controls
			//@{
				/** Controls if a complyer object respects the filter rules contained in the current object.
					@param complyer object to test
					@return bool true if the object can be used according to the current rules
					@author Hugues Romain
					@date 2008				
				*/
				bool isCompatibleWith(
					const Complyer& complyer
				) const;



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
				bool	getBikeFilter()			const;
				bool	getHandicappedFilter()	const;
				double	getApproachSpeed()		const;
			//@}
		};
	}
}

#endif // SYNTHESE_env_AccessParameters_h__

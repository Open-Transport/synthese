
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
#include "RuleUser.h"
#include "UseRule.h"
#include "PathClass.h"

#include <string>
#include <set>
#include <vector>
#include <boost/optional.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#include <boost/date_time/posix_time/posix_time_duration.hpp>
#include <boost/lexical_cast.hpp>

namespace synthese
{
	namespace graph
	{
		class PathClass;

		static const std::string ACCESSPARAMETERS_SERIALIZATION_SEPARATOR("|");
		static const std::string ACCESSPARAMETERS_LIST_SEPARATOR(",");

		/** Network access parameters class.
			@ingroup m18

			This class stores two types of information about how to use the transportation network :
				- numeric parameters : speeds, maximums, etc.
				- filters 
		*/
		class AccessParameters
		{
		public:
			typedef std::set<PathClass::Identifier> AllowedPathClasses;

		private:
			double		_maxApproachDistance;
			boost::posix_time::time_duration	_maxApproachTime;
			double		_approachSpeed;
			boost::optional<size_t>		_maxTransportConnectionCount;
			bool		_drtOnly;
			bool		_withoutDrt;
			//const Fare*		_fare;
			UserClassCode	_userClass;
			AllowedPathClasses	_allowedPathClasses;

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
				, boost::posix_time::time_duration maxApproachTime = boost::posix_time::minutes(23)
				, double		approachSpeed = 67
				, boost::optional<size_t>	maxTransportConnectionCount = boost::optional<size_t>(),
				AllowedPathClasses allowedPathClasses = AllowedPathClasses()
			):	_maxApproachDistance(maxApproachDistance)
				, _maxApproachTime(maxApproachTime)
				, _approachSpeed(approachSpeed)
				, _maxTransportConnectionCount(maxTransportConnectionCount)
				, _drtOnly(drtOnly)
				, _withoutDrt(withoutDrt),
				_userClass(userClass)
			{

			}


			AccessParameters& operator=(
				const std::string& serialized
			){
				std::vector<std::string> elements;
				boost::algorithm::split(elements, serialized, boost::algorithm::is_any_of(ACCESSPARAMETERS_SERIALIZATION_SEPARATOR));
				std::vector<std::string>::const_iterator it(elements.begin());

				_maxApproachDistance = boost::lexical_cast<double>(*it);
				++it;
				_maxApproachTime = boost::posix_time::seconds(boost::lexical_cast<int>(*it));
				++it;
				_approachSpeed = boost::lexical_cast<double>(*it);
				++it;
				if(boost::lexical_cast<std::size_t>(*it))
				{
					_maxTransportConnectionCount =  boost::lexical_cast<std::size_t>(*it);
				}
				++it;
				_drtOnly = boost::lexical_cast<bool>(*it);
				++it;
				_withoutDrt = boost::lexical_cast<bool>(*it);
				++it;
				_userClass = boost::lexical_cast<UserClassCode>(*it);
				++it;

				std::vector<std::string> elements2;
				boost::algorithm::split(elements2, *it, boost::algorithm::is_any_of(ACCESSPARAMETERS_LIST_SEPARATOR));
				BOOST_FOREACH(const std::string& element, elements2)
				{
					_allowedPathClasses.insert(boost::lexical_cast<PathClass::Identifier>(element));
				}

				return *this;
			}

			//! @name Queries
			//@{
				/** Approach compatibility control.
					@param distance length of the approach
					@param duration duration of the approach
					@return bool true if the approach is compatible with the current rules
					@author Hugues Romain
					@date 2008				
				*/
				bool isCompatibleWithApproach(
					double distance,
					boost::posix_time::time_duration duration
				) const {
					return distance < _maxApproachDistance && duration < _maxApproachTime;
				}

				bool isAllowedPathClass(PathClass::Identifier value) const {	return _allowedPathClasses.empty() || _allowedPathClasses.find(value) != _allowedPathClasses.end();	}

				std::string serialize() const
				{
					std::stringstream stream;
					stream <<
						getMaxApproachDistance() << ACCESSPARAMETERS_SERIALIZATION_SEPARATOR <<
						_maxApproachTime.total_seconds() << ACCESSPARAMETERS_SERIALIZATION_SEPARATOR <<
						getApproachSpeed() << ACCESSPARAMETERS_SERIALIZATION_SEPARATOR <<
						(getMaxtransportConnectionsCount() ? *getMaxtransportConnectionsCount() : double(0))  << ACCESSPARAMETERS_SERIALIZATION_SEPARATOR <<
						getDRTOnly() << ACCESSPARAMETERS_SERIALIZATION_SEPARATOR <<
						getWithoutDRT() << ACCESSPARAMETERS_SERIALIZATION_SEPARATOR <<
						getUserClass() << ACCESSPARAMETERS_SERIALIZATION_SEPARATOR;
					bool first(true);
					BOOST_FOREACH(const AllowedPathClasses::value_type& element, getAllowedPathClasses())
					{
						stream << (first ? std::string() : ACCESSPARAMETERS_LIST_SEPARATOR) << element;
					}
					return stream.str();
				}
			//@}


			//! @name Getters
			//@{
				graph::UserClassCode	getUserClass()	const {	return _userClass;	}
				double	getApproachSpeed()	const {	return _approachSpeed;	}
				boost::optional<size_t>	getMaxtransportConnectionsCount() const { return _maxTransportConnectionCount;}
				bool getDRTOnly() const {	return _drtOnly;}
				bool getWithoutDRT()	const { return _withoutDrt;	}
				boost::posix_time::time_duration getMaxApproachTime() const {	return _maxApproachTime; }
				double getMaxApproachDistance() const { return _maxApproachDistance; }
				const AllowedPathClasses& getAllowedPathClasses() const { return _allowedPathClasses; } 
			//@}
		};
	}
}

#endif // SYNTHESE_getEnv()_AccessParameters_h__

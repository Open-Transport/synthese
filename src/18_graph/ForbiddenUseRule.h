////////////////////////////////////////////////////////////////////////////////
/// ForbiddenUseRule class header.
///	@file ForbiddenUseRule.h
///	@author Hugues Romain (RCS)
///	@date dim jan 25 2009
///
///	This file belongs to the SYNTHESE project (public transportation specialized
///	software)
///	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>
///
///	This program is free software; you can redistribute it and/or
///	modify it under the terms of the GNU General Public License
///	as published by the Free Software Foundation; either version 2
///	of the License, or (at your option) any later version.
///
///	This program is distributed in the hope that it will be useful,
///	but WITHOUT ANY WARRANTY; without even the implied warranty of
///	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
///	GNU General Public License for more details.
///
///	You should have received a copy of the GNU General Public License
///	along with this program; if not, write to the Free Software Foundation,
///	Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
////////////////////////////////////////////////////////////////////////////////

#ifndef SYNTHESE_ForbiddenUseRule_h__
#define SYNTHESE_ForbiddenUseRule_h__

#include "UseRule.h"

#include <boost/shared_ptr.hpp>

namespace synthese
{
	namespace graph
	{
		//////////////////////////////////////////////////////////////////////////
		/// Forbidden Use rule class.
		///
		/// The use of the service is ever Forbidden.
		/// @ingroup m18
		class ForbiddenUseRule:
			public UseRule
		{
		public:
			static boost::shared_ptr<const ForbiddenUseRule> INSTANCE;

			virtual boost::posix_time::ptime getReservationDeadLine (
				const boost::posix_time::ptime& originTime,
				const boost::posix_time::ptime& departureTime
			) const;


			virtual AccessCapacity getAccessCapacity(
				) const;


			virtual boost::posix_time::ptime getReservationOpeningTime (
				const ServicePointer& servicePointer
			) const;



			virtual RunPossibilityType isRunPossible (
				const ServicePointer& servicePointer,
				bool ignoreReservation
			) const;



			virtual ReservationAvailabilityType getReservationAvailability(
				const ServicePointer& servicePointer,
				bool ignoreReservationDeadline
			) const;


			virtual bool isCompatibleWith(
				const AccessParameters& accessParameters
			) const;

			virtual std::string getUseRuleName() const;
		};
	}
}

#endif

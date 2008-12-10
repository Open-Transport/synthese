
/** Complyer class template header.
	@file Complyer.h

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

#ifndef SYNTHESE_ENV_COMPLYER_H
#define SYNTHESE_ENV_COMPLYER_H

#include <vector>
#include <boost/shared_ptr.hpp>

namespace synthese
{
	namespace env
	{
		class BikeCompliance;
		class Fare;
		class PedestrianCompliance;
		class ReservationRule;
		class HandicappedCompliance;

		/** 
			Base class for an entity providing a regulation.
			If no regulation is provided, the parent complyer is called.
			@todo Merge Complyer and Calendar : put a CalendarDays set in Complyer
			and delegate to the tablesync the parent calendar update. Add the useinrouteplannoing and departuretable attributes.
		    
			@ingroup m35
		*/
		class Complyer
		{

		private:

			boost::shared_ptr<const BikeCompliance>			_bikeCompliance;
			boost::shared_ptr<const Fare>					_fare;
			boost::shared_ptr<const HandicappedCompliance>	_handicappedCompliance;
			boost::shared_ptr<const PedestrianCompliance>	_pedestrianCompliance;
			
			const Complyer*					_complianceParent;

		public:

			Complyer ();
			~Complyer ();

			//! @name Getters
			//@{
				boost::shared_ptr<const BikeCompliance>			getBikeCompliance ()		const;
				boost::shared_ptr<const Fare>					getFare()					const;
				boost::shared_ptr<const HandicappedCompliance>	getHandicappedCompliance()	const;
				boost::shared_ptr<const PedestrianCompliance>	getPedestrianCompliance()	const;
				
				/** Temporary pedestrian reservation rule getter. */
				boost::shared_ptr<const ReservationRule>		getReservationRule()		const;
			//@}

			//! @name Setters
			//@{
				void	setHandicappedCompliance(boost::shared_ptr<const HandicappedCompliance> compliance);
				void	setPedestrianCompliance(boost::shared_ptr<const PedestrianCompliance> compliance);
				void	setBikeCompliance(boost::shared_ptr<const BikeCompliance> compliance);
				void	setFare(boost::shared_ptr<const Fare> fare);
				
				/** Temporary pedestrian reservation rule setter. */
				void	setReservationRule(boost::shared_ptr<const ReservationRule> rule);
				void	setComplianceParent(const Complyer* parent);
			//@}

			//! @name Update methods.
			//@{
			//@}
		};
	}
}

#endif 	    

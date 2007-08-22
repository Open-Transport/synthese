
/** Complyer class implementation.
	@file Complyer.cpp

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

#include "Complyer.h"

#include "15_env/BikeComplyer.h"
#include "15_env/BikeCompliance.h"
#include "15_env/HandicappedComplyer.h"
#include "15_env/PedestrianComplyer.h"
#include "15_env/PedestrianCompliance.h"
#include "15_env/ReservationRuleComplyer.h"
#include "15_env/ReservationRule.h"
#include "15_env/Fare.h"
#include "15_env/HandicappedCompliance.h"

namespace synthese
{
	namespace env
	{
		Complyer::Complyer()
			: _bikeCompliance(BikeCompliance::Get(0).get())
			, _fare(Fare::Get(0).get())
			, _pedestrianCompliance(PedestrianCompliance::Get(0).get())
			, _reservationRule(ReservationRule::Get(0).get())
			, _handicappedCompliance(HandicappedCompliance::Get(0).get())
			, _complianceParent(NULL)
		{

		}

		const BikeCompliance* Complyer::getBikeCompliance() const
		{
			return _bikeCompliance;
		}

		const Fare* Complyer::getFare() const
		{
			return _fare;
		}

		const HandicappedCompliance* Complyer::getHandicappedCompliance() const
		{
			return _handicappedCompliance;
		}

		const PedestrianCompliance* Complyer::getPedestrianCompliance() const
		{
			return _pedestrianCompliance;
		}

		const ReservationRule* Complyer::getReservationRule() const
		{
			if ((_reservationRule == NULL || _reservationRule->isCompliant() == false) && _complianceParent != NULL)
				return _complianceParent->getReservationRule();
			return _reservationRule;
		}

		void Complyer::setHandicappedCompliance( const HandicappedCompliance* compliance )
		{
			_handicappedCompliance = compliance;
		}

		void Complyer::setPedestrianCompliance( const PedestrianCompliance* compliance )
		{
			_pedestrianCompliance = compliance;
		}

		void Complyer::setBikeCompliance( const BikeCompliance* compliance )
		{
			_bikeCompliance = compliance;
		}

		void Complyer::setFare( const Fare* fare )
		{
			_fare = fare;
		}

		void Complyer::setReservationRule( const ReservationRule* rule )
		{
			_reservationRule = rule;
		}

		bool Complyer::isCompatibleWith( const Complyer& complyer ) const
		{
			return
				getBikeCompliance()->isCompatibleWith(*complyer.getBikeCompliance())
				&& getFare()->isCompatibleWith(*complyer.getFare())
				&& getHandicappedCompliance()->isCompatibleWith(*complyer.getHandicappedCompliance())
				&& getPedestrianCompliance()->isCompatibleWith(*complyer.getPedestrianCompliance())
				&& getReservationRule()->isCompatibleWith(*complyer.getReservationRule())
				;
		}

		Complyer::~Complyer()
		{

		}

		void Complyer::setComplianceParent( const Complyer* parent )
		{
			_complianceParent = parent;
		}
	}
}

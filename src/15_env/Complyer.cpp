
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
#include "Env.h"
#include "BikeComplyer.h"
#include "BikeCompliance.h"
#include "HandicappedComplyer.h"
#include "PedestrianComplyer.h"
#include "PedestrianCompliance.h"
#include "ReservationRuleComplyer.h"
#include "ReservationRule.h"
#include "Fare.h"
#include "HandicappedCompliance.h"

using namespace boost;

namespace synthese
{
	using namespace util;

	namespace env
	{
		Complyer::Complyer()
			: _bikeCompliance(Env::GetOfficialEnv()->getEditableRegistry<BikeCompliance>().getWithAutoCreation(0))
			, _fare(Env::GetOfficialEnv()->getEditableRegistry<Fare>().getWithAutoCreation(0))
			, _pedestrianCompliance(Env::GetOfficialEnv()->getEditableRegistry<PedestrianCompliance>().getWithAutoCreation(0))
			, _handicappedCompliance(Env::GetOfficialEnv()->getEditableRegistry<HandicappedCompliance>().getWithAutoCreation(0))
			, _complianceParent(NULL)
		{

		}

		shared_ptr<const BikeCompliance> Complyer::getBikeCompliance() const
		{
			return _bikeCompliance;
		}

		shared_ptr<const Fare> Complyer::getFare() const
		{
			return _fare;
		}

		shared_ptr<const HandicappedCompliance> Complyer::getHandicappedCompliance() const
		{
			return _handicappedCompliance;
		}

		shared_ptr<const PedestrianCompliance> Complyer::getPedestrianCompliance() const
		{
			return _pedestrianCompliance;
		}

		shared_ptr<const ReservationRule> Complyer::getReservationRule() const
		{
			return _pedestrianCompliance->getReservationRule();
		}

		void Complyer::setHandicappedCompliance(shared_ptr<const HandicappedCompliance> compliance)
		{
			_handicappedCompliance = compliance;
		}

		void Complyer::setPedestrianCompliance(shared_ptr<const PedestrianCompliance> compliance )
		{
			_pedestrianCompliance = compliance;
		}

		void Complyer::setBikeCompliance(shared_ptr<const BikeCompliance> compliance )
		{
			_bikeCompliance = compliance;
		}

		void Complyer::setFare(shared_ptr<const Fare> fare )
		{
			_fare = fare;
		}

		void Complyer::setReservationRule(shared_ptr<const ReservationRule> rule )
		{
			const_pointer_cast<PedestrianCompliance, const PedestrianCompliance>(_pedestrianCompliance)->setReservationRule(rule);
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

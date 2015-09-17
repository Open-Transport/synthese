
//////////////////////////////////////////////////////////////////////////
/// FareUpdateAction class implementation.
/// @file FareUpdateAction.cpp
/// @author Gaël Sauvanet
/// @date 2011
///
///	This file belongs to the SYNTHESE project (public transportation specialized software)
///	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>
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
///	along with this program; if not, write to the Free Software
///	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#include "FareUpdateAction.hpp"

#include "ActionException.h"
#include "ParametersMap.h"
#include "Profile.h"
#include "Session.h"
#include "User.h"
#include "Request.h"
#include "FareTableSync.hpp"

using namespace std;

namespace synthese
{
	using namespace server;
	using namespace security;
	using namespace util;

	namespace util
	{
		template<> const string FactorableTemplate<Action, fare::FareUpdateAction>::FACTORY_KEY("FareUpdateAction");
	}

	namespace fare
	{
		const string FareUpdateAction::PARAMETER_FARE_ID = Action_PARAMETER_PREFIX + "id";
		const string FareUpdateAction::PARAMETER_NAME = Action_PARAMETER_PREFIX + "na";
		const string FareUpdateAction::PARAMETER_TYPE = Action_PARAMETER_PREFIX + "ty";
		const string FareUpdateAction::PARAMETER_CURRENCY = Action_PARAMETER_PREFIX + "cu";
		const string FareUpdateAction::PARAMETER_PERMITTED_CONNECTIONS_NUMBER = Action_PARAMETER_PREFIX + "pc";
		const string FareUpdateAction::PARAMETER_REQUIRED_CONTINUITY = Action_PARAMETER_PREFIX + "rc";
		const string FareUpdateAction::PARAMETER_VALIDITY_PERIOD = Action_PARAMETER_PREFIX + "vp";

		const string FareUpdateAction::PARAMETER_ACCESS_PRICE = Action_PARAMETER_PREFIX + "ap";
		const string FareUpdateAction::PARAMETER_SLICES = Action_PARAMETER_PREFIX + "sl";
		const string FareUpdateAction::PARAMETER_UNIT_PRICE = Action_PARAMETER_PREFIX + "up";
		const string FareUpdateAction::PARAMETER_MATRIX = Action_PARAMETER_PREFIX + "ma";
		const string FareUpdateAction::PARAMETER_SUB_FARES = Action_PARAMETER_PREFIX + "sf";

		ParametersMap FareUpdateAction::getParametersMap() const
		{
			ParametersMap map;
			if(_fare.get())
			{
				map.insert(PARAMETER_FARE_ID, _fare->getKey());
			}
			if(_name)
			{
				map.insert(PARAMETER_NAME, *_name);
			}
			if(_type)
			{
				map.insert(PARAMETER_TYPE, *_type);
			}
			if(_currency)
			{
				map.insert(PARAMETER_CURRENCY, *_currency);
			}
			if(_permittedConnectionsNumber)
			{
				map.insert(PARAMETER_PERMITTED_CONNECTIONS_NUMBER, *_permittedConnectionsNumber);
			}
			if(_requiredContinuity)
			{
				map.insert(PARAMETER_REQUIRED_CONTINUITY, *_requiredContinuity);
			}
			if(_validityPeriod)
			{
				map.insert(PARAMETER_VALIDITY_PERIOD, *_validityPeriod);
			}
			if(_accessPrice)
			{
				map.insert(PARAMETER_ACCESS_PRICE, *_accessPrice);
			}
			if(_slices)
			{
				map.insert(PARAMETER_SLICES, Fare::SerializeSlices(*_slices));
			}
			if(_isUnitPrice)
			{
				map.insert(PARAMETER_UNIT_PRICE, *_isUnitPrice);
			}
			if(_matrix)
			{
				map.insert(PARAMETER_MATRIX, *_matrix);
			}
			if(_subFares)
			{
				map.insert(PARAMETER_SUB_FARES, *_subFares);
			}

			return map;
		}



		void FareUpdateAction::_setFromParametersMap(const ParametersMap& map)
		{
			if(map.getOptional<RegistryKeyType>(PARAMETER_FARE_ID)) try
			{
				_fare = FareTableSync::GetEditable(map.get<RegistryKeyType>(PARAMETER_FARE_ID), *_env);
			}
			catch (ObjectNotFoundException<Fare>&)
			{
				throw ActionException("No such fare");
			}
			else
			{
				_fare.reset(new Fare);
			}

			if(map.isDefined(PARAMETER_NAME))
			{
				_name = map.get<string>(PARAMETER_NAME);
			}
			if(map.isDefined(PARAMETER_TYPE))
			{
				_type = (FareType::FareTypeNumber) map.get<int>(PARAMETER_TYPE);
			}
			if(map.isDefined(PARAMETER_CURRENCY))
			{
				_currency = map.get<string>(PARAMETER_CURRENCY);
			}
			if(map.isDefined(PARAMETER_PERMITTED_CONNECTIONS_NUMBER))
			{
				_permittedConnectionsNumber = map.getOptional<int>(PARAMETER_PERMITTED_CONNECTIONS_NUMBER);
			}
			if(map.isDefined(PARAMETER_REQUIRED_CONTINUITY))
			{
				_requiredContinuity = map.get<bool>(PARAMETER_REQUIRED_CONTINUITY);
			}
			_validityPeriod = map.getDefault<int>(PARAMETER_VALIDITY_PERIOD,0);
			if(map.isDefined(PARAMETER_ACCESS_PRICE))
			{
				_accessPrice = map.get<double>(PARAMETER_ACCESS_PRICE);
			}
			if(map.isDefined(PARAMETER_SLICES))
			{
				_slices = Fare::GetSlicesFromSerializedString(
					map.get<string>(PARAMETER_SLICES)
				);
			}
			if(map.isDefined(PARAMETER_UNIT_PRICE))
			{
				_isUnitPrice = map.get<bool>(PARAMETER_UNIT_PRICE);
			}
			if(map.isDefined(PARAMETER_MATRIX))
			{
				_matrix = map.get<string>(PARAMETER_MATRIX);
			}
			if(map.isDefined(PARAMETER_SUB_FARES))
			{
				_subFares = map.get<string>(PARAMETER_SUB_FARES);
			}
		}



		void FareUpdateAction::run(
			Request& request
		){
			if(_name)
			{
				_fare->set<Name>(*_name);
			}
			if(_type)
			{
				_fare->set<FareTypeEnum>(*_type);
			}
			if(_currency)
			{
				_fare->set<Currency>(*_currency);
			}
			if(_permittedConnectionsNumber)
			{
				_fare->set<PermittedConnectionsNumber>(*_permittedConnectionsNumber);
			}
			else
			{
				_fare->set<PermittedConnectionsNumber>(0);
			}
			if(_requiredContinuity)
			{
				_fare->set<RequiredContinuity>(*_requiredContinuity);
			}
			if(_validityPeriod)
			{
				_fare->set<ValidityPeriod>(*_validityPeriod);
			}
			if(_fare->getType())
			{
				if(_accessPrice)
				{
					_fare->getType()->setAccessPrice(*_accessPrice);
					// Update Access price in Fare object for DB record
					_fare->set<Access>(*_accessPrice);
				}
				if(_slices)
				{
					_fare->getType()->setSlices(*_slices);
					// Update Slices in Fare object for DB record
					_fare->set<Slices>(Fare::SerializeSlices(*_slices));
				}
				if(_isUnitPrice)
				{
					_fare->getType()->setIsUnitPrice(*_isUnitPrice);
					// Update Unit price in Fare object for DB record
					_fare->set<UnitPrice>(*_isUnitPrice);
				}
			}

			FareTableSync::Save(_fare.get());

			if(request.getActionWillCreateObject())
			{
				request.setActionCreatedId(_fare->getKey());
			}
		}



		bool FareUpdateAction::isAuthorized(
			const Session* session
		) const {
			return true; // TODO create fare right
		}
	}
}

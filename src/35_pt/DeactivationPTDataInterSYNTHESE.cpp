
/** DeactivationPTDataInterSYNTHESE class implementation.
	@file DeactivationPTDataInterSYNTHESE.cpp

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>

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

#include "DeactivationPTDataInterSYNTHESE.hpp"

#include "CommercialLine.h"
#include "ContinuousServiceTableSync.h"
#include "ScheduledServiceTableSync.h"
#include "TransportNetwork.h"

#include <boost/algorithm/string/split.hpp>
#include <boost/lexical_cast.hpp>
#include <sstream>

using namespace boost;
using namespace boost::algorithm;
using namespace boost::posix_time;
using namespace std;

namespace synthese
{
	using namespace graph;
	using namespace inter_synthese;
	using namespace util;
	
	
	template<>
	const string FactorableTemplate<InterSYNTHESESyncTypeFactory, pt::DeactivationPTDataInterSYNTHESE>::FACTORY_KEY = "deactrt";



	namespace pt
	{
		const std::string DeactivationPTDataInterSYNTHESE::FIELD_SEPARATOR = ",";


		DeactivationPTDataInterSYNTHESE::DeactivationPTDataInterSYNTHESE():
			FactorableTemplate<inter_synthese::InterSYNTHESESyncTypeFactory, DeactivationPTDataInterSYNTHESE>()
		{
		}



		/// Config perimeter = network id
		/// Message perimeter = line id
		bool DeactivationPTDataInterSYNTHESE::mustBeEnqueued(
			const std::string& configPerimeter,
			const std::string& messagePerimeter
		) const	{

			RegistryKeyType lineId(lexical_cast<RegistryKeyType>(messagePerimeter));
			boost::shared_ptr<const CommercialLine> line(Env::GetOfficialEnv().get<CommercialLine>(lineId));
			RegistryKeyType networkId(lexical_cast<RegistryKeyType>(configPerimeter));
			return line->getNetwork()->getKey() == networkId;

		}



		void DeactivationPTDataInterSYNTHESE::initSync() const
		{

		}



		bool DeactivationPTDataInterSYNTHESE::sync(
			const string& parameter
		) const	{

			if(parameter.empty())
			{
				return false;
			}

			vector<string> fields;
			split(fields, parameter, is_any_of(FIELD_SEPARATOR));

			if(fields.size() < 1)
			{
				return false;
			}

			// Load of the service
			boost::shared_ptr<SchedulesBasedService> service;
			try
			{
				RegistryKeyType serviceId(
					lexical_cast<RegistryKeyType>(fields[0])
				);
				if(decodeTableId(serviceId) == ScheduledServiceTableSync::TABLE.ID)
				{
					service = Env::GetOfficialEnv().getCastEditable<SchedulesBasedService, ScheduledService>(serviceId);
				}
				else if(decodeTableId(serviceId) == ContinuousServiceTableSync::TABLE.ID)
				{
					service = Env::GetOfficialEnv().getCastEditable<SchedulesBasedService, ContinuousService>(serviceId);
				}
			}
			catch (bad_lexical_cast&)
			{
				return false;
			}
			catch(ObjectNotFoundException<ScheduledService>&)
			{
				return false;
			}

			// Deactivation of the service
			boost::gregorian::date today(boost::gregorian::day_clock::local_day());
			service->setInactive(today);

			return true;
		}



		void DeactivationPTDataInterSYNTHESE::closeSync() const
		{

		}



		void DeactivationPTDataInterSYNTHESE::initQueue(
			const inter_synthese::InterSYNTHESESlave& slave,
			const std::string& perimeter
		) const	{

		}



		DeactivationPTDataInterSYNTHESE::SortedItems DeactivationPTDataInterSYNTHESE::sort( const RandomItems& randItems ) const
		{
			return randItems;
		}



		DeactivationPTDataInterSYNTHESE::Content::Content(
			const SchedulesBasedService& service
		):	InterSYNTHESEContent(DeactivationPTDataInterSYNTHESE::FACTORY_KEY),
			_service(service)
		{
		}



		std::string DeactivationPTDataInterSYNTHESE::Content::getPerimeter() const
		{
			return lexical_cast<string>(_service.getRoute()->getCommercialLine()->getKey());
		}



		std::string DeactivationPTDataInterSYNTHESE::Content::getContent() const
		{
			stringstream result;
			result << _service.getKey();
			return result.str();
		}



		ptime DeactivationPTDataInterSYNTHESE::Content::getExpirationTime() const
		{
			return _service.getNextRTUpdate();
		}
}	}

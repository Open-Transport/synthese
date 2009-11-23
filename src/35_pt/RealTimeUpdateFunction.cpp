
/** RealTimeUpdateFunction class implementation.
	@file RealTimeUpdateFunction.cpp
	@author Hugues
	@date 2009

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

#include "RequestException.h"
#include "Request.h"
#include "TransportNetworkRight.h"
#include "RealTimeUpdateFunction.h"
#include "PublicTransportStopZoneConnectionPlace.h"
#include "LineStop.h"
#include "ScheduledService.h"
#include "RealTimeUpdateScreenServiceInterfacePage.h"
#include "Interface.h"
#include "Line.h"

using namespace std;

namespace synthese
{
	using namespace util;
	using namespace server;
	using namespace security;
	using namespace env;
	using namespace interfaces;

	template<> const string util::FactorableTemplate<RequestWithInterface,pt::RealTimeUpdateFunction>::FACTORY_KEY("rtu");
	
	namespace pt
	{
		const string RealTimeUpdateFunction::PARAMETER_LINE_STOP_RANK("ls");
		const string RealTimeUpdateFunction::PARAMETER_SERVICE_ID("se");

		RealTimeUpdateFunction::RealTimeUpdateFunction():
			FactorableTemplate<RequestWithInterface, RealTimeUpdateFunction>()
		{

		}

		ParametersMap RealTimeUpdateFunction::_getParametersMap() const
		{
			ParametersMap map(RequestWithInterface::_getParametersMap());
			if(_service.get())
			{
				map.insert(PARAMETER_SERVICE_ID, _service->getKey());
			}
			map.insert(PARAMETER_LINE_STOP_RANK, static_cast<int>(_lineStopRank));
			return map;
		}

		void RealTimeUpdateFunction::_setFromParametersMap(const ParametersMap& map)
		{
			try
			{
				// Interface
				RequestWithInterface::_setFromParametersMap(map);
				if(getInterface() == NULL)
				{
					throw RequestException("An interface must be specified");
				}
				if(!getInterface()->hasPage<RealTimeUpdateScreenServiceInterfacePage>())
				{
					throw RequestException("The interface does not implement the Real Time Update Screen");
				}

				_service = Env::GetOfficialEnv().getRegistry<ScheduledService>().get(
					map.get<RegistryKeyType>(PARAMETER_SERVICE_ID)
					);
				_lineStopRank = map.get<RegistryKeyType>(PARAMETER_LINE_STOP_RANK);

				if(_lineStopRank >= _service->getArrivalSchedules(false).size())
				{
					throw RequestException("Inconsistent line stop number");
				}
			}
			catch(ObjectNotFoundException<ScheduledService>)
			{
				throw RequestException("No such service");
			}
		}

		void RealTimeUpdateFunction::_run( std::ostream& stream ) const
		{
			VariablesMap vm;
			const RealTimeUpdateScreenServiceInterfacePage* page(
				getInterface()->getPage<RealTimeUpdateScreenServiceInterfacePage>()
			);
			page->display(
				stream,
				*_service,
				*_service->getRoute()->getLineStop(_lineStopRank),
				vm,
				_request
			);
		}
		
		
		
		bool RealTimeUpdateFunction::_isAuthorized() const
		{
			return true;
		}



		std::string RealTimeUpdateFunction::getOutputMimeType() const
		{
			return
				getInterface() ?
				getInterface()->getPage<RealTimeUpdateScreenServiceInterfacePage>()->getMimeType() :
				"text/plain";
		}



		void RealTimeUpdateFunction::setService( boost::shared_ptr<const env::ScheduledService> value )
		{
			_service = value;
		}



		void RealTimeUpdateFunction::setLineStopRank( std::size_t value )
		{
			_lineStopRank = value;
		}
	}
}

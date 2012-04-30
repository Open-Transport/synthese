
/** ServiceInformationsFunction class implementation.
	@file ServiceInformationsFunction.cpp
	@author Hugues Romain
	@date 2010

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

#include "RequestException.h"
#include "Request.h"
#include "ServiceInformationsFunction.hpp"
#include "JourneyPattern.hpp"
#include "ScheduledService.h"
#include "StopArea.hpp"
#include "StopPoint.hpp"
#include "Webpage.h"
#include "AccessParameters.h"

using namespace std;
using namespace boost;
using namespace boost::posix_time;

namespace synthese
{
	using namespace util;
	using namespace server;
	using namespace security;
	using namespace pt;
	using namespace cms;
	using namespace graph;

	template<> const string util::FactorableTemplate<Function,pt_website::ServiceInformationsFunction>::FACTORY_KEY("ServiceInformationsFunction");

	namespace pt_website
	{
		const string ServiceInformationsFunction::PARAMETER_STOP_PAGE_ID("stop_page_id");
		const string ServiceInformationsFunction::PARAMETER_DATE("date");

		const string ServiceInformationsFunction::PAGE_PARAMETER_RANK("rank");
		const string ServiceInformationsFunction::PAGE_PARAMETER_TIME("time");
		const string ServiceInformationsFunction::PAGE_PARAMETER_DURATION("duration");
		const string ServiceInformationsFunction::PAGE_PARAMETER_STOP_POINT_ID("stop_point_id");
		const string ServiceInformationsFunction::PAGE_PARAMETER_STOP_AREA_ID("stop_area_id");

		ParametersMap ServiceInformationsFunction::_getParametersMap() const
		{
			ParametersMap map;
			if(_service.get())
			{
				map.insert(Request::PARAMETER_OBJECT_ID, _service->getKey());
			}
			if(_stopPage.get())
			{
				map.insert(PARAMETER_STOP_PAGE_ID, _stopPage->getKey());
			}
			return map;
		}

		void ServiceInformationsFunction::_setFromParametersMap(const ParametersMap& map)
		{
			// Service
			try
			{
				RegistryKeyType id(map.get<RegistryKeyType>(Request::PARAMETER_OBJECT_ID));
				_service = Env::GetOfficialEnv().get<ScheduledService>(id);
			}
			catch (ObjectNotFoundException<ScheduledService>& e)
			{
				throw RequestException("No such service : "+ e.getMessage());
			}
			catch(ParametersMap::MissingParameterException&)
			{
				throw RequestException("Service to display not specified");
			}

			// Stop page
			try
			{
				optional<RegistryKeyType> id(map.getOptional<RegistryKeyType>(PARAMETER_STOP_PAGE_ID));
				if(id)
				{
					_stopPage = Env::GetOfficialEnv().get<Webpage>(*id);
				}
			}
			catch (ObjectNotFoundException<Webpage>& e)
			{
				throw RequestException("No such stop page : "+ e.getMessage());
			}

			// Date
			if(!map.getDefault<string>(PARAMETER_DATE).empty())
			{
				_date = time_from_string(map.get<string>(PARAMETER_DATE));
			}
		}



		util::ParametersMap ServiceInformationsFunction::run(
			std::ostream& stream,
			const Request& request
		) const {

			AccessParameters ap;
			ServicePointer ptr(_service->getDeparturePosition(true, ap, _date));

			if(_stopPage.get())
			{
				_displayStop(stream, true, ptr, request);
			}
			else
			{
				_displaySIRI(stream);
			}

			for(const Edge* edge(ptr.getDepartureEdge()->getFollowingArrivalForFineSteppingOnly());
				edge;
				edge = edge->getFollowingArrivalForFineSteppingOnly()
			){
				ServicePointer iptr(ptr);
				_service->completeServicePointer(iptr, *edge, ap);
				if(_stopPage.get())
				{
					_displayStop(stream, false, iptr, request);
				}
				else
				{
					_displaySIRI(stream);
				}
			}

			return util::ParametersMap();
		}



		bool ServiceInformationsFunction::isAuthorized(
			const Session* session
		) const {
			return true;
		}



		std::string ServiceInformationsFunction::getOutputMimeType() const
		{
			return _stopPage.get() ? _stopPage->getMimeType() : "text/xml";
		}



		ServiceInformationsFunction::ServiceInformationsFunction():
			_date(boost::posix_time::second_clock::local_time())
		{

		}



		void ServiceInformationsFunction::_displayStop(
			std::ostream& stream,
			bool departureTime,
			const graph::ServicePointer& ptr,
			const Request& request
		) const	{
			ParametersMap pm(getTemplateParameters());

			const Edge& edge(departureTime ? *ptr.getDepartureEdge() : *ptr.getArrivalEdge());
			const ptime& time(departureTime ? ptr.getDepartureDateTime() : ptr.getArrivalDateTime());

			pm.insert(PAGE_PARAMETER_RANK, edge.getRankInPath());

			pm.insert(PAGE_PARAMETER_DURATION, departureTime ? 0 : ((time - _date).total_seconds() / 60));

			pm.insert(PAGE_PARAMETER_STOP_AREA_ID, static_cast<StopPoint*>(edge.getFromVertex())->getConnectionPlace()->getKey());

			pm.insert(PAGE_PARAMETER_STOP_POINT_ID, edge.getFromVertex()->getKey());

			pm.insert(PAGE_PARAMETER_TIME, time);

			_stopPage->display(stream, request, pm);
		}



		void ServiceInformationsFunction::_displaySIRI( std::ostream& stream ) const
		{
			/// @todo to be implemented
		}
}	}

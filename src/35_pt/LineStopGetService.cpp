
/** LineStopGetService class implementation.
	@file LineStopGetService.cpp
	@author Hugues Romain
	@date 2011

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
#include "LineStopGetService.hpp"
#include "DesignatedLinePhysicalStop.hpp"
#include "StopPointTableSync.hpp"
#include "StopAreaTableSync.hpp"
#include "Webpage.h"
#include "JourneyPattern.hpp"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace util;
	using namespace server;
	using namespace security;
	using namespace cms;
	using namespace graph;

	template<> const string util::FactorableTemplate<Function,pt::LineStopGetService>::FACTORY_KEY("LineStopGet");

	namespace pt
	{
		const string LineStopGetService::PARAMETER_ROUTE_ID("route_id");
		const string LineStopGetService::PARAMETER_STOP_ID("stop_id");
		const string LineStopGetService::PARAMETER_PAGE_ID("page_id");

		const string LineStopGetService::DATA_LINE_STOP_ID("line_stop_id");
		const string LineStopGetService::DATA_RANK_IN_PATH("rank_in_path");
		const string LineStopGetService::DATA_ROUTE_ID("route_id");
		const string LineStopGetService::DATA_STOP_ID("stop_id");

		ParametersMap LineStopGetService::_getParametersMap() const
		{
			ParametersMap map;
			if(_lineStop.get())
			{
				map.insert(Request::PARAMETER_OBJECT_ID, _lineStop->getKey());
			}
			if(_page.get())
			{
				map.insert(PARAMETER_PAGE_ID, _page->getKey());
			}
			return map;
		}



		void LineStopGetService::_setFromParametersMap(const ParametersMap& map)
		{
			// Identification of the line stop
			if(map.getDefault<RegistryKeyType>(Request::PARAMETER_OBJECT_ID, 0))
			{	// Load by id
				try
				{
					_lineStop = Env::GetOfficialEnv().getCast<DesignatedLinePhysicalStop, LineStop>(
						map.get<RegistryKeyType>(Request::PARAMETER_OBJECT_ID)
					);
				}
				catch(ObjectNotFoundException<DesignatedLinePhysicalStop>&)
				{
					throw RequestException("No such line stop");
				}
			}
			else
			{	// Load by route and stop

				// Load of the route
				try
				{
					boost::shared_ptr<const JourneyPattern> journeyPattern(
						Env::GetOfficialEnv().get<JourneyPattern>(
							map.get<RegistryKeyType>(PARAMETER_ROUTE_ID)
					)	);

					RegistryKeyType stopId(map.get<RegistryKeyType>(PARAMETER_STOP_ID));

					// Search by stop
					if(decodeTableId(stopId) == StopPointTableSync::TABLE.ID)
					{
						try
						{
							boost::shared_ptr<const StopPoint> stop(
								Env::GetOfficialEnv().get<StopPoint>(stopId)
							);

							const Edge& edge(journeyPattern->findEdgeByVertex(stop.get()));
							_lineStop = static_pointer_cast<const DesignatedLinePhysicalStop, const LineStop>(
								Env::GetOfficialEnv().getSPtr(static_cast<const LineStop*>(&edge))
							);
						}
						catch (ObjectNotFoundException<StopPoint>&)
						{
							throw RequestException("No such stop");
						}
						catch (Path::VertexNotFoundException&)
						{
						}
					}
					else if(decodeTableId(stopId) == StopAreaTableSync::TABLE.ID)
					{
						try
						{
							boost::shared_ptr<const StopArea> stopArea(
								Env::GetOfficialEnv().get<StopArea>(stopId)
							);
							BOOST_FOREACH(const StopArea::PhysicalStops::value_type& itStop, stopArea->getPhysicalStops())
							{
								try
								{
									const Edge& edge(journeyPattern->findEdgeByVertex(itStop.second));
									_lineStop = static_pointer_cast<const DesignatedLinePhysicalStop, const LineStop>(
										Env::GetOfficialEnv().getSPtr(static_cast<const LineStop*>(&edge))
									);
									break;
								}
								catch (Path::VertexNotFoundException&)
								{
								}
							}
						}
						catch (ObjectNotFoundException<StopArea>&)
						{
							throw RequestException("No such stop area");
						}
					}
					else
					{
						throw RequestException("Inconsistent stop id");
					}

					// Throws an exception if no line stop were found
					if(!_lineStop.get())
					{
						throw RequestException("The specified stop is not served by the specified route");
					}
				}
				catch(ObjectNotFoundException<JourneyPattern>&)
				{
					throw RequestException("No such route");
				}
			}

			// CMS template
			if(map.getDefault<RegistryKeyType>(PARAMETER_PAGE_ID, 0)) try
			{
				_page = Env::GetOfficialEnv().get<Webpage>(map.get<RegistryKeyType>(PARAMETER_PAGE_ID));
			}
			catch (ObjectNotFoundException<Webpage>&)
			{
				throw RequestException("No such page");
			}
		}



		util::ParametersMap LineStopGetService::run(
			std::ostream& stream,
			const Request& request
		) const {

			ParametersMap pm;

			pm.insert(DATA_LINE_STOP_ID, _lineStop->getKey());
			pm.insert(DATA_RANK_IN_PATH, _lineStop->getRankInPath());
			pm.insert(DATA_ROUTE_ID, _lineStop->getParentPath()->getKey());
			pm.insert(DATA_STOP_ID, _lineStop->getPhysicalStop()->getKey());

			if(_page.get())
			{	// CMS output
				pm.merge(getTemplateParameters());

				_page->display(stream, request, pm);
			}

			return pm;
		}



		bool LineStopGetService::isAuthorized(
			const Session* session
		) const {
			return true;
		}



		std::string LineStopGetService::getOutputMimeType() const
		{
			return _page.get() ? _page->getMimeType() : "text/xml";
		}
	}
}

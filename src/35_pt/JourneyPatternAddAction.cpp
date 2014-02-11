
//////////////////////////////////////////////////////////////////////////
/// JourneyPatternAddAction class implementation.
/// @file JourneyPatternAddAction.cpp
/// @author Hugues
/// @date 2010
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

#include "JourneyPatternAddAction.hpp"

#include "ActionException.h"
#include "ParametersMap.h"
#include "Profile.h"
#include "Session.h"
#include "TransportNetworkRight.h"
#include "User.h"
#include "Request.h"
#include "CommercialLineTableSync.h"
#include "JourneyPatternTableSync.hpp"
#include "LineStopTableSync.h"
#include "DBTransaction.hpp"
#include "LineArea.hpp"
#include "DesignatedLinePhysicalStop.hpp"

#include <geos/geom/LineString.h>

using namespace std;
using namespace boost;
using namespace geos::geom;

namespace synthese
{
	using namespace server;
	using namespace security;
	using namespace util;
	using namespace pt;
	using namespace graph;
	using namespace db;

	namespace util
	{
		template<> const string FactorableTemplate<Action, pt::JourneyPatternAddAction>::FACTORY_KEY("JourneyPatternAddAction");
	}

	namespace pt
	{
		const string JourneyPatternAddAction::PARAMETER_COMMERCIAL_LINE_ID = Action_PARAMETER_PREFIX + "cl";
		const string JourneyPatternAddAction::PARAMETER_NAME = Action_PARAMETER_PREFIX + "na";
		const string JourneyPatternAddAction::PARAMETER_TEMPLATE_ID = Action_PARAMETER_PREFIX + "te";
		const string JourneyPatternAddAction::PARAMETER_REVERSE_COPY = Action_PARAMETER_PREFIX + "re";



		ParametersMap JourneyPatternAddAction::getParametersMap() const
		{
			ParametersMap map;
			if(_template.get())
			{
				map.insert(PARAMETER_TEMPLATE_ID, _template->getKey());
				map.insert(PARAMETER_REVERSE_COPY, _reverse);
			}
			if(_commercialLine.get())
			{
				map.insert(PARAMETER_COMMERCIAL_LINE_ID, _commercialLine->getKey());
			}
			map.insert(PARAMETER_NAME, _name);
			return map;
		}



		void JourneyPatternAddAction::_setFromParametersMap(const ParametersMap& map)
		{
			RegistryKeyType tid(map.getDefault<RegistryKeyType>(PARAMETER_TEMPLATE_ID));

			if(tid > 0)
			{
				try
				{
					_template = JourneyPatternTableSync::Get(map.get<RegistryKeyType>(PARAMETER_TEMPLATE_ID), *_env);
				}
				catch(ObjectNotFoundException<JourneyPattern>&)
				{
					throw ActionException("No such template route");
				}
				_reverse = map.getDefault<bool>(PARAMETER_REVERSE_COPY, false);

				LineStopTableSync::Search(*_env, _template->getKey());
			}
			else
			{
				try
				{
					_commercialLine = CommercialLineTableSync::GetEditable(map.get<RegistryKeyType>(PARAMETER_COMMERCIAL_LINE_ID), *_env);
				}
				catch(ObjectNotFoundException<CommercialLine>&)
				{
					throw ActionException("No such commercial line");
				}
			}
			_name = map.getDefault<string>(PARAMETER_NAME);
		}



		void JourneyPatternAddAction::run(
			Request& request
		){
			DBTransaction transaction;
			JourneyPattern object;

			if(_template.get())
			{
				object.setCommercialLine(_template->getCommercialLine());
				object.setNetwork(_template->getCommercialLine()->getNetwork());
				object.setRollingStock(_template->getRollingStock());
				if(_reverse)
				{
					object.setWayBack(!_template->getWayBack());
				}
				else
				{
					object.setWayBack(_template->getWayBack());
					object.setDirection(_template->getDirection());
				}
			}
			else
			{
				object.setCommercialLine(_commercialLine.get());
			}
			object.setName(_name);

			JourneyPatternTableSync::Save(&object, transaction);

			// Stops copy
			if(_template.get())
			{
				if(_reverse)
				{
					size_t rank(0);
					const double maxMetricOffset(_template->getLastEdge()->getMetricOffset());
					for(Path::Edges::const_reverse_iterator it(_template->getEdges().rbegin()); it != _template->getEdges().rend(); ++it)
					{
						if(dynamic_cast<const DesignatedLinePhysicalStop*>(*it))
						{
							const DesignatedLinePhysicalStop& other(static_cast<const DesignatedLinePhysicalStop&>(**it));
							DesignatedLinePhysicalStop ls(
								0,
								&object,
								rank++,
								other.getIsArrival(),
								other.getIsDeparture(),
								maxMetricOffset - other.getMetricOffset(),
								other.getPhysicalStop(),
								other.getScheduleInput(),
								other.getReservationNeeded()
							);
							if((it+1) != _template->getEdges().rend())
							{
								const DesignatedLinePhysicalStop& prevOther(
									static_cast<const DesignatedLinePhysicalStop&>(**(it+1))
								);
								if(prevOther.getGeometry().get())
								{
									ls.setGeometry(
										boost::shared_ptr<LineString>(
											dynamic_cast<LineString*>(prevOther.getGeometry()->reverse())
									)	);
								}
							}
							LineStopTableSync::Save(&ls, transaction);
						}
						if(dynamic_cast<const LineArea*>(*it))
						{
							const LineArea& other(static_cast<const LineArea&>(**it));
							LineArea ls(
								0,
								&object,
								rank++,
								other.getIsArrival(),
								other.getIsDeparture(),
								maxMetricOffset - other.getMetricOffset(),
								other.getArea(),
								other.getInternalService()
							);
							if(other.getGeometry().get())
							{
								ls.setGeometry(
									boost::shared_ptr<LineString>(
										dynamic_cast<LineString*>(other.getGeometry()->reverse())
								)	);
							}
							LineStopTableSync::Save(&ls, transaction);
						}
					}
				}
				else
				{
					for(Path::Edges::const_iterator it(_template->getEdges().begin()); it != _template->getEdges().end(); ++it)
					{
						if(dynamic_cast<const DesignatedLinePhysicalStop*>(*it))
						{
							const DesignatedLinePhysicalStop& other(static_cast<const DesignatedLinePhysicalStop&>(**it));
							DesignatedLinePhysicalStop ls(
								0,
								&object,
								other.getRankInPath(),
								other.getIsDeparture(),
								other.getIsArrival(),
								other.getMetricOffset(),
								other.getPhysicalStop(),
								other.getScheduleInput(),
								other.getReservationNeeded()
							);
							if(other.getGeometry().get())
							{
								ls.setGeometry(
									boost::shared_ptr<LineString>(
										dynamic_cast<LineString*>(other.getGeometry()->clone())
								)	);
							}
							LineStopTableSync::Save(&ls, transaction);
						}
						if(dynamic_cast<const LineArea*>(*it))
						{
							const LineArea& other(static_cast<const LineArea&>(**it));
							LineArea ls(
								0,
								&object,
								other.getRankInPath(),
								other.getIsDeparture(),
								other.getIsArrival(),
								other.getMetricOffset(),
								other.getArea(),
								other.getInternalService()
							);
							if(other.getGeometry().get())
							{
								ls.setGeometry(
									boost::shared_ptr<LineString>(
										dynamic_cast<LineString*>(other.getGeometry()->clone())
								)	);
							}
							LineStopTableSync::Save(&ls, transaction);
						}
					}
				}
			}

			transaction.run();

			//::AddCreationEntry(object, request.getUser().get());
			request.setActionCreatedId(object.getKey());
		}



		bool JourneyPatternAddAction::isAuthorized(
			const Session* session
		) const {
			//TODO test if the user has sufficient right level for this journey pattern
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<TransportNetworkRight>(WRITE, UNKNOWN_RIGHT_LEVEL, string());
		}



		void JourneyPatternAddAction::setCommercialLine( boost::shared_ptr<pt::CommercialLine> value )
		{
			_commercialLine = value;
		}
}	}

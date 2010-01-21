
/** ServicePointer class implementation.
	@file ServicePointer.cpp

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

#include "ServicePointer.h"
#include "Service.h"
#include "UseRule.h"
#include "Edge.h"

#include "04_time/module.h"

#include "01_util/Constants.h"

using namespace boost;

namespace synthese
{
	using namespace time;

	namespace graph
	{

		ServicePointer::ServicePointer(
			bool RTData,
			AccessDirection method,
			UserClassCode userClassCode,
			const Edge* edge
		):	_RTData(RTData),
			_service(NULL)
			, _originDateTime(TIME_UNKNOWN)
			, _determinationMethod(method)
			, _actualTime(TIME_UNKNOWN),
			_theoreticalTime(TIME_UNKNOWN)
			, _range(posix_time::seconds(0))
			, _edge(edge),
			_userClass(userClassCode),
			_useRule(NULL),
			_RTVertex(edge ? edge->getFromVertex() : NULL)
		{}



		ServicePointer::ServicePointer():
			_RTData(false),
			_service(NULL),
			_edge(NULL),
			_RTVertex(NULL),
			_determinationMethod(UNDEFINED_DIRECTION),
			_actualTime(TIME_UNKNOWN),
			_theoreticalTime(TIME_UNKNOWN),
			_useRule(NULL),
			_originDateTime(TIME_UNKNOWN),
			_range(posix_time::seconds(0))
		{}



		const UseRule* ServicePointer::getUseRule(
		) const {
			return _useRule;
		}
		
		

		void ServicePointer::setActualTime( const time::DateTime& dateTime )
		{
			_actualTime = dateTime;
		}

		void ServicePointer::setService( const Service* service )
		{
			_service = service;
			_useRule = &service->getUseRule(_userClass);
		}

		void ServicePointer::setOriginDateTime( const time::DateTime& dateTime )
		{
			_originDateTime = dateTime;
		}



		UseRule::RunPossibilityType ServicePointer::isUseRuleCompliant(
		) const	{
			return _useRule->isRunPossible(
				*this
			);
		}

		const Service* ServicePointer::getService() const
		{
			return _service;
		}

		const time::DateTime& ServicePointer::getActualDateTime() const
		{
			return _actualTime;
		}

		const time::DateTime& ServicePointer::getOriginDateTime() const
		{
			return _originDateTime;
		}

		AccessDirection ServicePointer::getMethod() const
		{
			return _determinationMethod;
		}

		const Edge* ServicePointer::getEdge() const
		{
			return _edge;
		}

		

		void ServicePointer::setServiceRange(posix_time::time_duration duration)
		{
			_range = duration;
		}



		posix_time::time_duration ServicePointer::getServiceRange() const
		{
			return _range;
		}
		
		UserClassCode ServicePointer::getUserClass() const
		{
			return _userClass;
		}



		bool ServicePointer::getRTData() const
		{
			return _RTData;
		}



		const time::DateTime& ServicePointer::getTheoreticalDateTime() const
		{
			return _theoreticalTime;
		}



		void ServicePointer::setTheoreticalTime( const time::DateTime& dateTime )
		{
			_theoreticalTime = dateTime;
		}



		void ServicePointer::setRealTimeVertex( const Vertex* value )
		{
			_RTVertex = value;
		}



		const Vertex* ServicePointer::getRealTimeVertex() const
		{
			return _RTVertex;
		}
	}
}

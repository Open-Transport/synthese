
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

using namespace boost;
using namespace boost::posix_time;

namespace synthese
{
	namespace graph
	{
		ServicePointer::ServicePointer(
			bool RTData,
			AccessDirection method,
			size_t userClassRank,
			const Edge* edge
		):	_RTData(RTData),
			_service(NULL),
			_determinationMethod(method),
			_range(posix_time::seconds(0)),
			_edge(edge),
			_userClassRank(userClassRank),
			_RTVertex(edge ? edge->getFromVertex() : NULL)
		{}



		ServicePointer::ServicePointer():
			_RTData(false),
			_service(NULL),
			_edge(NULL),
			_RTVertex(NULL),
			_determinationMethod(UNDEFINED_DIRECTION),
			_range(posix_time::seconds(0)),
			_userClassRank(0)
		{}



		const UseRule& ServicePointer::getUseRule() const
		{
			return _service->getUseRule(_userClassRank);
		}

		

		UseRule::RunPossibilityType ServicePointer::isUseRuleCompliant(
		) const	{
			return getUseRule().isRunPossible(*this);
		}
	}
}

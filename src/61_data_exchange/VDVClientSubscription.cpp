
/** VDVClientSubscription class implementation.
	@file VDVClientSubscription.cpp

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

#include "VDVClientSubscription.hpp"

using namespace boost;
using namespace std;
using namespace boost::posix_time;

namespace synthese
{
	using namespace departure_boards;
	

	namespace data_exchange
	{
		void VDVClientSubscription::buildGenerator() const
		{
			ArrivalDepartureTableGenerator::PhysicalStops ps(_stopArea->getPhysicalStops());
			DeparturesTableDirection di(DISPLAY_DEPARTURES);
			EndFilter ef(WITH_PASSING);
			LineFilter lf;
			lf.insert(make_pair(_line.get(), optional<bool>()));
			DisplayedPlacesList dp;
			ForbiddenPlacesList fp;
			ptime now(second_clock::local_time());
			ptime end(now + _timeSpan);

			_generator.reset(
				new StandardArrivalDepartureTableGenerator(
					ps,
					di,
					ef,
					lf,
					dp,
					fp,
					now,
					end,
					false
			)	);
		}



		bool VDVClientSubscription::checkUpdate() const
		{
			if(!_generator.get())
			{
				buildGenerator();
			}

			const ArrivalDepartureList& result(_generator->generate());

			if(result.size() == _lastResult.size())
			{
				ArrivalDepartureList::const_iterator it1(result.begin());
				ArrivalDepartureList::const_iterator it2(_lastResult.begin());
				bool identical(true);
				for(; it1 != result.end(); ++it1, ++it2)
				{
					if(it1->first != it2->first)
					{
						identical = false;
						break;
					}
				}
				if(identical)
				{
					return false;
				}
			}

			_lastResult = result;
			return true;
		}


		VDVClientSubscription::VDVClientSubscription()
		{

		}
}	}



/** TransferPlaceCheck class header.
	@file TransferPlaceCheck.hpp

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

#ifndef SYNTHESE_algorithm_TransferHubCheck_hpp__
#define SYNTHESE_algorithm_TransferHubCheck_hpp__

#include "GraphTypes.h"

namespace synthese
{
	namespace graph
	{
		class Hub;
	}

	namespace algorithm
	{
		/** Checks if a place is a useful transfer hub.
			@ingroup m33

			Routes schema (bidirectional) :

			A-B-C-D-E-F-G-H-I-J-K      r1 l1
			A---------------I-J-K      r2 l2
					E-------------L    r3 l3
								K---M  r4 l4

			Transfer stops :
			A, E, I, K

			Expression :
			A stop is a transfer stop if at least one pair of routes 

			Check of the stop x :
			for each incoming route at x = i :
			 - pi = the previous stop before x on route i
			 - ni = the next stop after x on route i (can be null)
			 - For each outgoing route at x = o, with o != i :
			   - no is the next stop after x on route o
			   - if no != pi and no != ni :
			     return true
			return false
		*/
		class TransferPlaceCheck
		{
		private:
			const graph::Hub& _checkedPlace;
			graph::GraphIdType _graph;

		public:
			TransferPlaceCheck(
				const graph::Hub& checkedPlace,
				graph::GraphIdType graph
			);

			bool operator()() const;
		};
	}
}

#endif // SYNTHESE_algorithm_TransferHubCheck_hpp__


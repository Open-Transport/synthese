
/** SentScenarioInheritedTableSync class header.
	@file SentScenarioInheritedTableSync.h
	@author Hugues Romain
	@date 2008

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

#ifndef SYNTHESE_SentScenarioInheritedTableSync_H__
#define SYNTHESE_SentScenarioInheritedTableSync_H__


#include <vector>
#include <string>
#include <iostream>

#include "02_db/SQLiteInheritedRegistryTableSync.h"

#include "17_messages/ScenarioTableSync.h"
#include "17_messages/SentScenario.h"

namespace synthese
{
	namespace messages
	{
		/** SentScenario table synchronizer.
			@ingroup m17LS refLS
		*/
		class SentScenarioInheritedTableSync
			: public db::SQLiteInheritedRegistryTableSync<ScenarioTableSync,SentScenarioInheritedTableSync,SentScenario>
		{
		public:
			
			SentScenarioInheritedTableSync();


			

			/** SentScenario search.
				(other search parameters)
				@param first First SentScenario object to answer
				@param number Number of SentScenario objects to answer (0 = all) The size of the vector is less or equal to number, then all users were returned despite of the number limit. If the size is greater than number (actually equal to number + 1) then there is others accounts to show. Test it to know if the situation needs a "click for more" button.
				@return vector<SentScenario> Founded SentScenario objects.
				@author Hugues Romain
				@date 2006
			*/
/*			static std::vector<boost::shared_ptr<SentScenario> > search(
				// other search parameters ,
				int first = 0, int number = 0);
*/

		protected:

		};
	}
}

#endif // SYNTHESE_SentScenarioInheritedTableSync_H__

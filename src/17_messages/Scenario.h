
/** Scenario class header.
	@file Scenario.h

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

#ifndef SYNTHESE_Scenario_h__
#define SYNTHESE_Scenario_h__

#include "Registrable.h"

#include <string>
#include <set>

namespace synthese
{
	namespace messages
	{
		/** Scenario of alarms diffusion.
			@ingroup m17

			A scenario is a collection of alarms.
			The type of the contained alarms defines two categories of scenario :
				- the template scenario
				- the sent scenario
		*/
		class Scenario
			:	public virtual util::Registrable
		{
		private:
			std::string					_name;

		protected:
			Scenario(const std::string name = std::string());

		public:
			virtual ~Scenario();

			/** Name getter.
				@return const std::string& The name of the scenario
				@author Hugues Romain
				@date 2007
			*/
			const std::string& getName() const;


			/** Name setter.
				@param name Name of the scenario
				@author Hugues Romain
				@date 2007
			*/
			void setName(const std::string& name);
		};
	}
}

#endif // SYNTHESE_Scenario_h__

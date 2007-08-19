
/** LogoutAction class header.
	@file LogoutAction.h

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

#ifndef SYNTHESE_LogoutAction_H__
#define SYNTHESE_LogoutAction_H__

#include "30_server/Action.h"

namespace synthese
{
	namespace server
	{
		/** Logout Action Class
			@ingroup m18Action refActions
		*/
		class LogoutAction : public server::Action
		{
		protected:
			/** Conversion from attributes to generic parameter maps.
			*/
			server::ParametersMap getParametersMap() const;

			/** Conversion from generic parameters map to attributes.
			Removes the used parameters from the map.
			*/
			void _setFromParametersMap(const server::ParametersMap& map);

			/** Activates the action before the session control.
				@return TRUE
				@author Hugues Romain
				@date 2007				
			*/
			bool _beforeSessionControl() const;
		
		public:

			/** Action to run, defined by each subclass.
			*/
			void run();
		};
	}
}

#endif // SYNTHESE_LogoutAction_H__


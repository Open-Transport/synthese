
/** Action class header.
	@file Action.h

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

#ifndef SYNTHESE_Action_H__
#define SYNTHESE_Action_H__

#include "01_util/Factorable.h"

#include "30_server/Request.h"

#define Action_PARAMETER_PREFIX std::string("actionParam")


namespace synthese
{
	namespace server
	{
		/** Action to run before the display of a function result.

			A factory of actions is handled by 30_server module.

		*/
		class Action : public util::Factorable
		{
		public:
			static const std::string PARAMETER_ACTION;
			
		protected:
			Request* _request;

			/** Conversion from generic parameters map to attributes.
				Removes the used parameters from the map.
			*/
			virtual void setFromParametersMap(Request::ParametersMap& map) = 0;

		public:
			/** Conversion from attributes to generic parameter maps.
			*/
			virtual Request::ParametersMap getParametersMap() const = 0;

			/** Action to run, defined by each subclass.
			*/
			virtual void run() = 0;

			/** Action creation from a request.
			*/
			static Action* create(Request*request, Request::ParametersMap& params);
		};
	}
}

#endif // SYNTHESE_Action_H__


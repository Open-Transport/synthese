
/** RequestWithInterface class header.
	@file RequestWithInterface.h

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

#ifndef SYNTHESE_RequestWithInterface_h__
#define SYNTHESE_RequestWithInterface_h__

#include "30_server/Function.h"

namespace synthese
{
	namespace interfaces
	{
		class Interface;

		/** RequestWithInterface class.
			@ingroup m11-functions
		*/
		class RequestWithInterface : public server::Function
		{
		public:
			static const std::string PARAMETER_INTERFACE;

		protected:
			boost::shared_ptr<const Interface>	_interface;

			/** Conversion from generic parameters map to attributes.
			*/
			virtual void _setFromParametersMap(const server::ParametersMap& map);

			/** Conversion from attributes to generic parameter maps.
			*/
			virtual server::ParametersMap _getParametersMap() const;


			/** Redirection to the same request without the action.
			@return true : interrupts the display of the request with the action.
			*/
			virtual bool _runAfterSucceededAction(std::ostream& stream);

			virtual void _copy(boost::shared_ptr<const Function> function);

		public:
			/** Interface getter.
				@return Requested interface (can be NULL)
				@author Hugues Romain
				@date 2007				
			*/
			boost::shared_ptr<const Interface> getInterface() const;
		};
	}
}

#endif // SYNTHESE_RequestWithInterface_h__

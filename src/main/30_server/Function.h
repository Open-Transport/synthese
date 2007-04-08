
/** Function class header.
	@file Function.h

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

#ifndef SYNTHESE_Function_h__
#define SYNTHESE_Function_h__

#include "01_util/Factorable.h"

#include "30_server/Types.h"

namespace synthese
{
	namespace server
	{
		class Request;

		/** Function abstract class to use in a Request.
			@ingroup m30
		*/
		class Function : public util::Factorable
		{
		protected:
			Request* _request;

		private:
			
			/** Conversion from attributes to generic parameter maps.
				@return The generated parameters map
			*/
			virtual ParametersMap _getParametersMap() const = 0;

			/** Conversion from generic parameters map to attributes.
				@param map The map to interpret (comes from _parseString())
				@throw RequestException if a parameter is missing or has corrupted value
			*/
			virtual void _setFromParametersMap(const ParametersMap& map) = 0;

			/** Method to run after action execution.
				@return true if the current execution must be stopped

				This method has to be overloaded.
				The default behaviour is to do nothing and continue the execution after the action run.
			*/
			virtual bool _runAfterSucceededAction(std::ostream& stream) { return false; }

			/** Method to run before display when no session is detected.
				@return true if the current execution must be stopped

				This method has to be overloaded.
				The default behaviour is to do nothing and continue the execution without any session.
			*/
			virtual bool _runBeforeDisplayIfNoSession(std::ostream& stream) { return true; }


			/** Authorization control.
				@return True if the action run is authorized
				@author Hugues Romain
				@date 2007
			*/
			virtual bool _isAuthorized() const { return true; }

			/** Copy of the function parameters.
				@param function
				@return void
				@author Hugues Romain
				@date 2007				
			*/
			virtual void _copy(const Function* function) {}

			/** Function to display, defined by each subclass.
			@param stream Stream to write the output on.
			*/
			virtual void _run(std::ostream& stream) const = 0;

			friend Request;
		};
	}
}

#endif // SYNTHESE_Function_h__

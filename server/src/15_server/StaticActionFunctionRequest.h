
/** StaticActionFunctionRequest class header.
	@file StaticActionFunctionRequest.h

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

#ifndef SYNTHESE_server_StaticActionFunctionRequest_h__
#define SYNTHESE_server_StaticActionFunctionRequest_h__

#include "StaticActionRequest.h"
#include "StaticFunctionRequest.h"

namespace synthese
{
	namespace server
	{
		//////////////////////////////////////////////////////////////////////////
		/// Request which calls statically defined action and function.
		/// @ingroup m15
		template<
			class A,
			class F
		>	class StaticActionFunctionRequest:
			public StaticActionRequest<A>,
			public StaticFunctionRequest<F>
		{
		public:

			//! \name Constructor and destructor
			//@{
				/** Construction of an empty request.
					@author Hugues Romain
					@date 2007
					Use the public setters to fill the request.
				*/
				StaticActionFunctionRequest();



				/** Construction of a request from an other one.
					@param request Request to copy (default/NULL = no copy)
					@author Hugues Romain
					@date 2009
					Use the public setters to fill the request.
				*/
				explicit StaticActionFunctionRequest(const Request& request, bool copyFunction);
			//@}
		};




		template<class A, class F>
		StaticActionFunctionRequest<A,F>::StaticActionFunctionRequest(
			const Request& request, bool copyFunction
		):	Request(request),
			StaticActionRequest<A>(request),
			StaticFunctionRequest<F>(request, copyFunction)
		{
			this->_redirectAfterAction = true;
		}



		template<class A, class F>
		StaticActionFunctionRequest<A,F>::StaticActionFunctionRequest():
			Request(),
			StaticActionRequest<A>(),
			StaticFunctionRequest<F>()
		{
			this->redirectAfterAction = true;
		}
	}
}

#endif // SYNTHESE_server_StaticActionFunctionRequest_h__
